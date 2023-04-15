#pragma once

#include "media/gst/internal/w_common.hpp"
#include "media/gst/internal/w_utils.hpp"
#include "media/gst/core/w_nonowning.hpp"

#include <memory>
#include <utility>

namespace wolf::gst {

/**
 * the w_wrapper is helper CRTP class wrapping given raw C type pointer,
 * managing its lifetime and ownership/parentship.
 *
 * @tparam DerivedT class type inheriting this w_wrapper.
 * @tparam RawT     raw type to wrap.
 * @tparam BaseT    base type of `RawT`, for GObject types. (not supported yet, use `void`)
 * @tparam FreeFV   function (pointer) to free the `RawT` pointed resource.
 * @tparam CopyFV   optional function (pointer) to copy the `RawT` pointed resource.
 *                  (default: nullptr)
 */
template <typename DerivedT,
          typename RawT,
          typename BaseT = void,
          auto FreeFV = nullptr,
          auto CopyFV = nullptr>
class w_wrapper
{
    static_assert(
        std::is_same_v<BaseT, void>,
        "currently wrapper base isn't supported. it must be void."
    );

    friend class internal::w_raw_access;

    using underlying_type = std::remove_cvref_t<RawT>;

public:
    w_wrapper(const w_wrapper& p_other)
        requires (CopyFV != nullptr)
        : _ptr(CopyFV(p_other._ptr.get()))
        , _parented(false)
    {}

    w_wrapper(w_wrapper&&) noexcept = default;

    w_wrapper& operator=(const w_wrapper& p_other)
        requires (CopyFV != nullptr)
    {
        reset(CopyFV(p_other._ptr.get()));
        _parented = false;
        return *this;
    }

    w_wrapper& operator=(w_wrapper&&) noexcept = default;

    ~w_wrapper() noexcept
    {
        // avoid being passed to FreeFV if it's parented,
        // as its parent should/will take care of its lifetime.
        if (_parented) {
            _ptr.release();
        }
    }

    /**
     * create a nonowning/shallow copy of the instance wrapping
     * underlying raw pointer.
     *
     * only for lvalues, no accidental disposal
     * of rvalues leading dangling references.
     *
     * @return a non-owning shallow copy of the instance.
     */
    auto nonowning_view() & -> w_nonowning<DerivedT>
    {
        return internal::w_raw_access::from_raw<w_nonowning<DerivedT>>(_ptr.get());
    }

    /**
     * set as parented to keep the raw pointer pointing to resource,
     * but not to try to release it at destruction as that'd be up
     * to the parent.
     *
     * only applicable to lvalues since calling it
     * on rvalues has a high chance of memory/resource leak,
     * and it's advised to call this method after
     * adoption by parent object.
     */
    void parented() &
    {
        _parented = true;
    }

protected:
    // only derived classes should be able to initialize this class
    // with thier raw pointer provided resource.
    explicit w_wrapper(underlying_type* ptr) : _ptr(ptr) {}

    [[nodiscard]] underlying_type* raw() noexcept { return _ptr.get(); }
    [[nodiscard]] const underlying_type* raw() const noexcept { return _ptr.get(); }

    void replace_raw(underlying_type* ptr = nullptr) { _ptr.reset(ptr); }

    auto disown_raw() noexcept { return _ptr.release(); }

private:
    bool _parented = false;
    std::unique_ptr<underlying_type, internal::w_functor<FreeFV>> _ptr;
};

}  // namespace wolf::gst
