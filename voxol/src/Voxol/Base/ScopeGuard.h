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
} // namespace Scope
} // namespace Voxol::Base
#endif
