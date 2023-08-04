#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <cassert>
#include <list>
#include <mutex>
#include <algorithm>

namespace base
{
namespace demoMacro
{

namespace test_2
{
static inline void machine_pause(int32_t delay)
{
    while (delay-- > 0)
    {
        _mm_pause();
    }
    //#if __TBB_x86_64 || __TBB_x86_32
    //    while (delay-- > 0)
    //    {
    //        _mm_pause();
    //    }
    //#elif __ARM_ARCH_7A__ || __aarch64__
    //    while (delay-- > 0)
    //    {
    //        __asm__ __volatile__("yield" ::
    //                                 : "memory");
    //    }
    //#else /* Generic */
    //    (void)delay; // suppress without including _template_helpers.h
    //    yield();
    //#endif
}
class atomic_backoff
{
    //! Time delay, in units of "pause" instructions.
    /** Should be equal to approximately the number of "pause" instructions
      that take the same time as an context switch. Must be a power of two.*/
    static constexpr std::int32_t LOOPS_BEFORE_YIELD = 16;
    std::int32_t                  count;

public:
    // In many cases, an object of this type is initialized eagerly on hot path,
    // as in for(atomic_backoff b; ; b.pause()) { /*loop body*/ }
    // For this reason, the construction cost must be very small!
    atomic_backoff() :
        count(1) {}
    // This constructor pauses immediately; do not use on hot paths!
    atomic_backoff(bool) :
        count(1) { pause(); }

    //! No Copy
    atomic_backoff(const atomic_backoff&)            = delete;
    atomic_backoff& operator=(const atomic_backoff&) = delete;

    //! Pause for a while.
    void pause()
    {
        if (count <= LOOPS_BEFORE_YIELD)
        {
            machine_pause(count);
            // Pause twice as long the next time.
            count *= 2;
        }
        else
        {
            // Pause is so long that we might as well yield CPU to scheduler.
            std::this_thread::yield();
        }
    }

    //! Pause for a few times and return false if saturated.
    bool bounded_pause()
    {
        machine_pause(count);
        if (count < LOOPS_BEFORE_YIELD)
        {
            // Pause twice as long the next time.
            count *= 2;
            return true;
        }
        else
        {
            return false;
        }
    }

    void reset()
    {
        count = 1;
    }
};

enum class do_once_state
{
    uninitialized = 0,     ///< No execution attempts have been undertaken yet
    pending,               ///< A thread is executing associated do-once routine
    executed,              ///< Do-once routine has been executed
    initialized = executed ///< Convenience alias
};
static std::atomic<do_once_state> assertion_state;
// Run the initializer which can not fail
template <typename Functor>
void run_initializer(const Functor& f, std::atomic<do_once_state>& state)
{
    f();
    state.store(do_once_state::executed, std::memory_order_release);
}


//! Spin WHILE the condition is true.
/** T and U should be comparable types. */
template <typename T, typename C>
T spin_wait_while(const std::atomic<T>& location, C comp, std::memory_order order)
{
    atomic_backoff backoff;
    T              snapshot = location.load(order);
    while (comp(snapshot))
    {
        backoff.pause();
        snapshot = location.load(order);
    }
    return snapshot;
}

//! Spin WHILE the value of the variable is equal to a given value
/** T and U should be comparable types. */
template <typename T, typename U>
T spin_wait_while_eq(const std::atomic<T>& location, const U value, std::memory_order order = std::memory_order_acquire)
{
    return spin_wait_while(
        location, [&value](T t) { return t == value; }, order);
}

//! Spin UNTIL the value of the variable is equal to a given value
/** T and U should be comparable types. */
template <typename T, typename U>
T spin_wait_until_eq(const std::atomic<T>& location, const U value, std::memory_order order = std::memory_order_acquire)
{
    return spin_wait_while(
        location, [&value](T t) { return t != value; }, order);
}

//! Spin UNTIL the condition returns true or spinning time is up.
/** Returns what the passed functor returned last time it was invoked. */
template <typename Condition>
bool timed_spin_wait_until(Condition condition)
{
    // 32 pauses + 32 yields are meausered as balanced spin time before sleep.
    bool finish = condition();
    for (int i = 1; !finish && i < 32; finish = condition(), i *= 2)
    {
        machine_pause(i);
    }
    for (int i = 32; !finish && i < 64; finish = condition(), ++i)
    {
        std::this_thread::yield();
    }
    return finish;
}

template <typename F>
void atomic_do_once(const F& initializer, std::atomic<do_once_state>& state)
{
    // The loop in the implementation is necessary to avoid race when thread T2
    // that arrived in the middle of initialization attempt by another thread T1
    // has just made initialization possible.
    // In such a case T2 has to rely on T1 to initialize, but T1 may already be past
    // the point where it can recognize the changed conditions.
    do_once_state expected_state;
    while (state.load(std::memory_order_acquire) != do_once_state::executed)
    {
        if (state.load(std::memory_order_relaxed) == do_once_state::uninitialized)
        {
            expected_state = do_once_state::uninitialized;
            if (state.compare_exchange_strong(expected_state, do_once_state::pending))
            {
                run_initializer(initializer, state);
                break;
            }
        }
        spin_wait_while_eq(state, do_once_state::pending);
    }
}
// TODO: consider extension for formatted error description string
static void assertion_failure_impl(const char* location, int line, const char* expression, const char* comment)
{

    std::fprintf(stderr, "Assertion %s failed (located in the %s function, line in file: %d)\n",
                 expression, location, line);
    if (comment)
    {
        std::fprintf(stderr, "Detailed description: %s\n", comment);
    }
#if _MSC_VER && _DEBUG
    if (1 == _CrtDbgReport(_CRT_ASSERT, location, line, "XXX.dll", "%s\r\n%s", expression, comment ? comment : ""))
    {
        std::fprintf(stderr, "_CrtDbgBreak call ...\n");
        _CrtDbgBreak();
    }
    else
#endif
    {
        std::fflush(stderr);
        std::abort();
    }
}
void assertion_failure(const char* location, int line, const char* expression, const char* comment)
{
// Workaround for erroneous "unreachable code" during assertion throwing using call_once
#pragma warning(push)
#pragma warning(disable : 4702)

    // We cannot use std::call_once because it brings a dependency on C++ runtime on some platforms
    // while assert_impl.h is reused in demo malloc that should not depend on C++ runtime
    atomic_do_once([&]() { assertion_failure_impl(location, line, expression, comment); }, assertion_state);

#pragma warning(pop)
}
#define ASSERT_RELEASE(predicate, message) ((predicate) ? ((void)0) : assertion_failure(__func__, __LINE__, #predicate, message))

#ifdef _DEBUG
#define __PROC_ASSERT(predicate, message) ASSERT_RELEASE(predicate, message)
#else
#    define __PROC_ASSERT(predicate, message) ((void)0)
#endif

void testMain()
{
    std::cout << "base::demoMacro::test_2::testMain() begin.\n";
    int flag_a = 10;
    int flag_b = 11;
    __PROC_ASSERT(flag_a == flag_b, "scoped_lock already in transaction");
    std::cout << "base::demoMacro::test_2::testMain() end.\n";
}
} // namespace test_2
namespace test_1
{
struct Command
{
    const char* name;
    void (*parseInfo)(const char* pname);
    void showInfo()
    {
        if (parseInfo)
        {
            parseInfo(name);
        }
    }
    Command(const char* pname, void (*p_parseInfo)(const char* pname))
    {
        name      = pname;
        parseInfo = p_parseInfo;
    }
};
#define COMMAND_INSTANCE(CLASS_NAME, NAME) \
    CLASS_NAME                             \
    {                                      \
#        NAME, NAME##_command              \
    }

void pf0_command(const char* pname)
{
    std::cout << "pf0_command(), pname: " << pname << ".\n";
}
void pf1_command(const char* pname)
{
    std::cout << "pf1_command(), pname: " << pname << ".\n";
}
void buildConmands()
{
    Command v("f0", pf0_command);

    Command commands[] =
        {
            COMMAND_INSTANCE(Command, pf0),
            COMMAND_INSTANCE(Command, pf1)};
    for (auto i = 0; i < 2; ++i)
    {
        commands[i].showInfo();
    }
}
void showCodeInfo()
{
    printf("source file \"%s\",line %d, in function %s\n", __FILE__, __LINE__, __FUNCTION__);
}
#define WARN_INFO_STR(EXP)                          \
    do {                                            \
        if (EXP)                                    \
        {                                           \
            fprintf(stderr, "Warning: " #EXP "\n"); \
        }                                           \
    } while (0);

void testMain()
{
    //do {
    //} while (0);
    showCodeInfo();
    WARN_INFO_STR("3!=5");
    buildConmands();
    char s;
    s = 128;
}
} // namespace test_1
void testMain()
{
    std::boolalpha(std::cout);
    std::cout << "base::demoMacro::testMain() begin.\n";
    //std::cout << std::atomic<int>::is_always_lock_free << "\n";
    test_1::testMain();
    //test_2::testMain();
    std::cout << "base::demoMacro::testMain() end.\n";
}
} // namespace demoMacro
} // namespace base