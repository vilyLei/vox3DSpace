#ifndef VOXOL_BASE_SCOPE_GUARD_H
#define VOXOL_BASE_SCOPE_GUARD_H

namespace Voxol::Base
{
namespace Scope
{
template <class F>
class ScopeGuardT
{
public:
    explicit ScopeGuardT(F&& f) noexcept :
        targetFunc(std::forward<F>(f)), active(true) {}
    ~ScopeGuardT() noexcept
    {
        if (active && isCallable()) targetFunc();
    }
    ScopeGuardT(const ScopeGuardT&)            = delete;
    ScopeGuardT& operator=(const ScopeGuardT&) = delete;
    ScopeGuardT(ScopeGuardT&& other) noexcept
        :
        targetFunc(std::move(other.targetFunc)), active(other.active) { other.active = false; }

    void exec() noexcept
    {
        active = false;
        if (isCallable())
            targetFunc();
            
    }
    void dismiss() noexcept { active = false; }

private:
    F    targetFunc;
    bool active;
    constexpr bool isCallable() const noexcept
    {
        if constexpr (requires(const F& f) { static_cast<bool>(f); })
        {
            return static_cast<bool>(targetFunc);
        }
        else if constexpr (std::is_pointer_v<F>)
        {
            return targetFunc != nullptr;
        }
        else
        {
            return true;
        }
    }
};

template <class F>
[[nodiscard]] auto make_scope_guard(F&& f) noexcept
{
    return ScopeGuardT<std::decay_t<F>>(std::forward<F>(f));
}
} // namespace Scope
} // namespace Voxol::Base
#endif
