#ifndef VOXOL_BASE_SCOPE_GUARD_H
#define VOXOL_BASE_SCOPE_GUARD_H

namespace Voxol::Base
{
namespace Scope
{
template <class F>
class ScopeExitGuardT
{
public:
    explicit ScopeExitGuardT(F&& f) noexcept :
        exitFunc(std::forward<F>(f)), active(true) {}
    ~ScopeExitGuardT() noexcept
    {
        if (active && isCallable()) exitFunc();
    }
    ScopeExitGuardT(const ScopeExitGuardT&)            = delete;
    ScopeExitGuardT& operator=(const ScopeExitGuardT&) = delete;
    ScopeExitGuardT(ScopeExitGuardT&& other) noexcept
        :
        exitFunc(std::move(other.exitFunc)), active(other.active) { other.active = false; }

    void exec() noexcept
    {
        active = false;
        if (isCallable())
            exitFunc();
            
    }
    void dismiss() noexcept { active = false; }

private:
    F    exitFunc;
    bool active;
    constexpr bool isCallable() const noexcept
    {
        if constexpr (requires(const F& f) { static_cast<bool>(f); })
        {
            return static_cast<bool>(exitFunc);
        }
        else if constexpr (std::is_pointer_v<F>)
        {
            return exitFunc != nullptr;
        }
        else
        {
            return true;
        }
    }
};

template <class F>
[[nodiscard]] auto make_scope_exit_guard(F&& f) noexcept
{
    return ScopeExitGuardT<std::decay_t<F>>(std::forward<F>(f));
}



template <class EnterF, class ExitF>
class ScopeEnterAndExitGuardT
{
public:
    explicit ScopeEnterAndExitGuardT(EnterF&& enter_f, ExitF&& exit_f) noexcept :
        enterFunc(std::forward<EnterF>(enter_f)), exitFunc(std::forward<ExitF>(exit_f)), active(true)
    {
        if (isEnterCallable()) {
            enterFunc();
        }
    }
    ~ScopeEnterAndExitGuardT() noexcept
    {
        if (active && isExitCallable())
        {
            exitFunc();
        }
    }
    ScopeEnterAndExitGuardT(const ScopeEnterAndExitGuardT&) = delete;
    ScopeEnterAndExitGuardT& operator=(const ScopeEnterAndExitGuardT&) = delete;
    ScopeEnterAndExitGuardT(ScopeEnterAndExitGuardT&& other) noexcept
        :
        enterFunc(std::move(other.enterFunc)), exitFunc(std::move(other.exitFunc)), active(other.active) { other.active = false; }

    void execExitFunc() noexcept
    {
        active = false;
        if (isExitCallable())
            exitFunc();
    }
    void resetExitFunc() noexcept { active = true; }
    void dismissExitFunc() noexcept { active = false; }

private:
    EnterF          enterFunc;
    ExitF          exitFunc;
    bool           active;
    constexpr bool isEnterCallable() const noexcept
    {
        if constexpr (requires(const EnterF& f) { static_cast<bool>(f); })
        {
            return static_cast<bool>(enterFunc);
        }
        else if constexpr (std::is_pointer_v<EnterF>)
        {
            return enterFunc != nullptr;
        }
        else
        {
            return true;
        }
    }
    constexpr bool isExitCallable() const noexcept
    {
        if constexpr (requires(const ExitF& f) { static_cast<bool>(f); })
        {
            return static_cast<bool>(exitFunc);
        }
        else if constexpr (std::is_pointer_v<ExitF>)
        {
            return exitFunc != nullptr;
        }
        else
        {
            return true;
        }
    }
};
template <class EnterF, class ExitF>
[[nodiscard]] auto make_scope_enter_and_exit_guard(EnterF&& enter_f, ExitF&& exit_f) noexcept
{
    return ScopeEnterAndExitGuardT<std::decay_t<EnterF>, std::decay_t<ExitF>>(std::forward<EnterF>(enter_f), std::forward<ExitF>(exit_f));
}
} // namespace Scope
} // namespace Voxol::Base
#endif
