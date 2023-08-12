#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <memory>
#include <random>
//#include <malloc.h>
//#include <memory>
#include <new>

//#define NEW_POS_OK

#ifdef NEW_POS_OK
// "old" unaligned overloads
void* operator new(std::size_t size)
{
    auto ptr = malloc(size);
    std::cout << "unaligned new(" << size << ") = " << ptr << '\n';
    return ptr ? ptr : throw std::bad_alloc{};
}

#    if __cpp_aligned_new
void operator delete(void* ptr, std::size_t size)
{
    std::cout << "unaligned sized delete(" << ptr << ", " << size << ")\n";
    free(ptr);
}

void operator delete(void* ptr)
{
    std::cout << "unaligned unsized delete(" << ptr << ")\n";
    free(ptr);
}

// "new" over-aligned overloads
void* operator new(std::size_t size, std::align_val_t align)
{
    auto ptr = _aligned_malloc(size, static_cast<std::size_t>(align));
    std::cout << "aligned new(" << size << ", " << static_cast<std::size_t>(align) << ") = " << ptr << '\n';
    return ptr ? ptr : throw std::bad_alloc{};
}

void operator delete(void* ptr, std::size_t size, std::align_val_t align)
{
    std::cout << "aligned sized delete(" << ptr << ", " << size << ", " << static_cast<std::size_t>(align) << ")\n";
    _aligned_free(ptr);
}

void operator delete(void* ptr, std::align_val_t align)
{
    std::cout << "aligned unsized delete(" << ptr << ", " << static_cast<std::size_t>(align) << ")\n";
    _aligned_free(ptr);
}
struct alignas(8) OverAligned8
{
    char   c;
    int    i;
    double d;
};
struct alignas(16) OverAligned16
{
    char   c;
    int    i;
    double d;
};
struct alignas(32) OverAligned32
{
    char   c;
    int    i;
    double d;
    char   d0;
    char   d1;
};
struct alignas(256) OverAligned256
{
    char   c;
    int    i;
    double d;
    char   d0;
    char   d1;
}; // warning C4324, structure is padded
#    endif
#endif
namespace base::testMemory
{

template <class _Ty>
class enable_shared_from_this_t
{
public:    
    using _Esft_type = enable_shared_from_this_t;
    std::shared_ptr<_Ty> shared_from_this()
    {
        std::cout << "enable_shared_from_this_t::shared_from_this()...\n";
        auto ptr = _Wptr;
        return std::shared_ptr<_Ty>(_Wptr);
    }
protected:
    constexpr enable_shared_from_this_t() noexcept
    {
        std::cout << "enable_shared_from_this_t::constructor() A...\n";
    }
    ~enable_shared_from_this_t() = default;

private:
    template <class _Yty>
    friend class std::shared_ptr;

    mutable std::weak_ptr<_Ty> _Wptr;
};

namespace test_4
{

class NodeBase : public enable_shared_from_this_t<NodeBase>
//class NodeBase : public std::enable_shared_from_this<NodeBase
{
public:
    NodeBase()
    {
        std::cout << "NodeBase::constructor()...\n";
    }
    [[nodiscard]] std::shared_ptr<NodeBase>& getptr3()
    {
        auto sptr = std::shared_ptr<NodeBase>(this);
        std::weak_ptr<NodeBase> pv;
        pv = sptr;
        
        auto r = std::shared_ptr<NodeBase>(pv);
        return r;
    }
    [[nodiscard]] std::shared_ptr<NodeBase> getptr2()
    {
        return std::shared_ptr<NodeBase>(this);
    }
    [[nodiscard]] std::shared_ptr<NodeBase> getptr()
    {
        return shared_from_this();
    }
    virtual ~NodeBase()
    {
        std::cout << "NodeBase::destructor()...\n";
    }
};

class BigUnit
{
public:
    using _Esft_type = BigUnit;
    BigUnit()
    {
        std::cout << "BigUnit::constructor()...\n";
    }
    [[nodiscard]] std::shared_ptr<BigUnit> getSharedPtr()
    {
        return std::shared_ptr<BigUnit>(_Wptr);
    }
    [[nodiscard]] std::shared_ptr<const BigUnit> getSharedPtr() const
    {
        return std::shared_ptr<const BigUnit>(_Wptr);
    }

    virtual ~BigUnit()
    {
        std::cout << "BigUnit::destructor()...\n";
    }

private:
    template <class _Yty>
    friend class std::shared_ptr;

    mutable std::weak_ptr<BigUnit> _Wptr;
};
void testMain()
{
    //std::shared_ptr<NodeBase> p0 = std::make_shared<NodeBase>();
    //std::shared_ptr<NodeBase> q0 = p0->getptr();
    //std::cout << "(p0 == q0): " << (p0.get() == q0.get()) << std::endl;
    //std::cout << p0.use_count() << std::endl;
    //std::cout << q0.use_count() << std::endl;
    /*
    // error code:
    NodeBase node{};
    auto     ptr = node.getptr();
    */
    /*
    // error code:
    std::shared_ptr<NodeBase> p(new NodeBase());
    std::shared_ptr<NodeBase> q = p->getptr2();

    std::cout << p.use_count() << std::endl;
    std::cout << q.use_count() << std::endl;
    */
    {
        // correct code:
        std::shared_ptr<NodeBase> p0(new NodeBase());
        std::shared_ptr<NodeBase> q0 = p0->getptr();
        std::cout << "(p0 == q0): " << (p0.get() == q0.get()) << std::endl;
        std::cout << p0.use_count() << std::endl;
        std::cout << q0.use_count() << std::endl;
    }
    std::cout << "---------- -------A A------- ------------" << std::endl;
    {
        //std::weak_ptr<NodeBase>   gw;
        //std::shared_ptr<NodeBase> p(new NodeBase());
        //std::shared_ptr<NodeBase> q = p->getptr3();
        //std::shared_ptr<NodeBase> q = std::make_shared<NodeBase>(std::move(*p));
        //gw                          = p;

        std::shared_ptr<NodeBase> p = std::make_shared<NodeBase>();
        std::weak_ptr<NodeBase>   gw;
        gw                             = p;
        std::shared_ptr<NodeBase> q    = std::shared_ptr<NodeBase>(gw);
        auto                      pg_0 = p.get();
        auto                      pg_1 = q.get();
        std::cout << "(pg_0 == pg_1): " << (pg_0 == pg_1) << std::endl;
        std::cout << p.use_count() << std::endl;
        std::cout << q.use_count() << std::endl;
    }
    //std::cout << "---------- -------B B------- ------------" << std::endl;
    //{
    //    // error
    //    //std::weak_ptr<NodeBase>   gw;
    //    //std::shared_ptr<NodeBase> p(new NodeBase());
    //    //std::shared_ptr<NodeBase> q = p->getptr3();
    //    //std::shared_ptr<NodeBase> q = std::make_shared<NodeBase>(std::move(*p));
    //    //gw                          = p;

    //    std::shared_ptr<NodeBase> p = std::make_shared<NodeBase>();
    //    std::weak_ptr<NodeBase>   gw;
    //    gw = p;
    //    std::shared_ptr<NodeBase> q    = p->getptr3();
    //    auto                      pg_0 = p.get();
    //    auto                      pg_1 = q.get();
    //    std::cout << "(pg_0 == pg_1): " << (pg_0 == pg_1) << std::endl;
    //    std::cout << p.use_count() << std::endl;
    //    std::cout << q.use_count() << std::endl;
    //}
    std::cout << "---------- -------C C------- ------------" << std::endl;
    {
        std::shared_ptr<BigUnit> p = std::make_shared<BigUnit>();
        std::shared_ptr<BigUnit> q = p->getSharedPtr();
        std::cout << "(p == q): " << (p.get() == q.get()) << std::endl;
        std::cout << p.use_count() << std::endl;
        std::cout << q.use_count() << std::endl;
    }
}
}
namespace test_3
{
class GoodUnit
{
public:
    std::shared_ptr<GoodUnit> getptr()
    {
        return std::shared_ptr<GoodUnit>(this);
    }
    virtual ~GoodUnit()
    {
        std::cout << "GoodUnit::destructor()...\n";
    }
};
class Good : public std::enable_shared_from_this<Good>
{
public:
    [[nodiscard]] std::shared_ptr<Good> getptr()
    {
        return shared_from_this();
    }
    virtual ~Good()
    {
        std::cout << "Good::destructor()...\n";
    }
};

class Best : public std::enable_shared_from_this<Best>
{
public:
    std::shared_ptr<Best> getptr()
    {
        return shared_from_this();
    }
    // No public constructor, only a factory function,
    // so there's no way to have getptr return nullptr.
    [[nodiscard]] static std::shared_ptr<Best> create()
    {
        // Not using std::make_shared<Best> because the c'tor is private.
        return std::shared_ptr<Best>(new Best());
    }

private:
    Best() = default;
};


struct Bad
{
    std::shared_ptr<Bad> getptr()
    {
        return std::shared_ptr<Bad>(this);
    }
    ~Bad() { std::cout << "Bad::~Bad() called\n"; }
};

void testGoodUnit()
{
    // testGoodUnit: the two shared_ptr's share the same object
    // error operations:
    {
        std::shared_ptr<GoodUnit> good0 = std::make_shared<GoodUnit>();
        std::shared_ptr<GoodUnit> good1 = good0->getptr();
        std::cout << "testGoodUnit(), good1.use_count() = " << good1.use_count() << '\n';
    }
    using namespace std::literals;
    std::this_thread::sleep_for(1s);
}
void testGood()
{
    // Good: the two shared_ptr's share the same object
    {
        std::shared_ptr<Good> good0 = std::make_shared<Good>();
        std::shared_ptr<Good> good1 = good0->getptr();
        std::cout << "testGood(), good1.use_count() = " << good1.use_count() << '\n';
    }
    using namespace std::literals;
    std::this_thread::sleep_for(1s);
}

void misuseGood()
{
    // Bad: shared_from_this is called without having std::shared_ptr owning the caller
    try
    {
        Good                  not_so_good;
        std::shared_ptr<Good> gp1 = not_so_good.getptr();
    }
    catch (std::bad_weak_ptr& e)
    {
        // undefined behavior (until C++17) and std::bad_weak_ptr thrown (since C++17)
        std::cout << e.what() << '\n';
    }
}

void testBest()
{
    // Best: Same but can't stack-allocate it:
    std::shared_ptr<Best> best0 = Best::create();
    std::shared_ptr<Best> best1 = best0->getptr();
    std::cout << "best1.use_count() = " << best1.use_count() << '\n';

    // Best stackBest; // <- Will not compile because Best::Best() is private.
}

void testBad()
{
    // Bad, each shared_ptr thinks it's the only owner of the object
    std::shared_ptr<Bad> bad0 = std::make_shared<Bad>();
    std::shared_ptr<Bad> bad1 = bad0->getptr();
    std::cout << "bad1.use_count() = " << bad1.use_count() << '\n';
} // UB: double-delete of Bad

void testMain()
{
    //testGoodUnit();
    testGood();
    //misuseGood();
    //testBest();
    //testBad();
}
}
namespace test_2
{
std::weak_ptr<int> gw;

[[nodiscard]] int observe()
{
    std::cout << "gw.use_count() == " << gw.use_count() << "; ";
    // we have to make a copy of shared pointer before usage:
    if (std::shared_ptr<int> spt = gw.lock())
        std::cout << "*spt == " << *spt << '\n';
    else
        std::cout << "gw is expired\n";

    return 1;
}

[[noreturn]] void testMain()
{
    int flag = 0;
    //auto sp = std::make_shared<int>(42);
    {
        auto sp = std::make_shared<int>(42);
        gw      = sp;

        flag = observe();
    }
    flag = observe();
    std::cout << "flag: " << flag << "\n";
}
} // namespace test_2
namespace test_1
{
void testMain()
{
    /*
https://en.cppreference.com/w/cpp/memory/new/align_val_t
https://learn.microsoft.com/en-us/cpp/build/reference/zc-alignednew?view=msvc-170
//*/

#if __cpp_aligned_new
    // https://developercommunity.visualstudio.com/t/using-c17-new-stdalign-val-tn-syntax-results-in-er/528320
    //intptr_t* p = new (std::align_val_t(32)) intptr_t[64];//error
    //delete[] ( operator new[](sizeof(intptr_t) * 64, (std::align_val_t)(32)) );

    intptr_t* p = (intptr_t*)operator new[](sizeof(intptr_t) * 64, (std::align_val_t)(32));
    p[1]        = 12;

#ifdef NEW_POS_OK
    delete new int;
    delete new OverAligned256;
    delete new OverAligned32;
    delete new OverAligned16;
    delete new OverAligned8;

    int* p1 = (int*)std::malloc(10 * sizeof *p1);
    printf("default-aligned addr:   %p\n", (void*)p1);
    free(p1);
#        if _WIN32
    int* p2 = (int*)_aligned_malloc(1024, 1024 * sizeof *p2);
    printf("1024-byte aligned addr: %p\n", (void*)p2);
    _aligned_free(p2);
#        endif
    std::cout << "alignof(std::max_align_t): " << alignof(std::max_align_t) << '\n';
//int* plus_p1 = static_cast<int*>(std::malloc(10 * sizeof *plus_p1));
//std::printf("plus_p1 default-aligned address:   %p\n", static_cast<void*>(plus_p1));
//std::free(plus_p1);

//int* plus_p2 = static_cast<int*>(std::aligned_malloc(1024, 1024));
//std::printf("1024-byte aligned address: %p\n", static_cast<void*>(plus_p2));
//std::free(plus_p2);
#endif
#endif
}
} // namespace test_1

int testMain()
{
    std::cout << "base::testMemory::testMain() begin.\n";
    //test_1::testMain();
    //test_2::testMain();
    //test_3::testMain();
    test_4::testMain();
    std::cout << "base::testMemory::testMain() end.\n";
    return EXIT_SUCCESS;
}
} // namespace base::testMemory