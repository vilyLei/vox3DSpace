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
        if (active) targetFunc();
    }
    ScopeGuardT(const ScopeGuardT&)            = delete;
    ScopeGuardT& operator=(const ScopeGuardT&) = delete;
    ScopeGuardT(ScopeGuardT&& other) noexcept
        :
        targetFunc(std::move(other.targetFunc)), active(other.active) { other.active = false; }

    void exec() noexcept
    {
        active = false;
        targetFunc();
    }
    void dismiss() noexcept { active = false; }

private:
    F    targetFunc;
    bool active;
};

template <class F>
[[nodiscard]] auto make_scope_guard(F&& f) noexcept
{
    return ScopeGuardT<std::decay_t<F>>(std::forward<F>(f));
}
} // namespace Scope
} // namespace Voxol::Base
#endif
