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
        func_(std::forward<F>(f)), active_(true) {}
    ~ScopeGuardT() noexcept
    {
        if (active_) func_();
    }
    ScopeGuardT(const ScopeGuardT&)            = delete;
    ScopeGuardT& operator=(const ScopeGuardT&) = delete;
    ScopeGuardT(ScopeGuardT&& other) noexcept
        :
        func_(std::move(other.func_)), active_(other.active_) { other.active_ = false; }

    void dismiss() noexcept { active_ = false; }

private:
    F    func_;
    bool active_;
};

template <class F>
[[nodiscard]] auto make_scope_guard(F&& f) noexcept
{
    return ScopeGuardT<std::decay_t<F>>(std::forward<F>(f));
}
} // namespace Scope
} // namespace Voxol::Base
#endif
