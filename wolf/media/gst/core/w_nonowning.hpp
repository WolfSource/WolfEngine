#pragma once

#include "media/gst/internal/w_common.hpp"

#include <utility>

namespace wolf::media::gst {

/**
 * @brief nonowning view of `T` pointing to same resource
 *        given to make nonowning view of. similar to std::string_view/std::span.
 *
 * it's for sharing things like element.clock() returning Clock instance,
 * which the instance does not own the clock's lifetime, but
 * points to same resource to keep track of.
 *
 * this class is wrapper acting as a pointer to an instance of `T`
 * with shared resource beneath.
 *
 * @tparam T underlying type of nonowning view.
 *
 * @note make sure the given object lives longer than this nonowning view of it,
 *       or any access will be undefined behavior.
 *       similar to std::string_view or std::span.
 */
template <typename T>
class w_nonowning
{
    friend class internal::w_raw_access;

    using value_type = std::remove_cvref_t<T>;

public:
    /**
     * @brief implicit constructor from a value of wrapping type.
     * @param p_value value to make nonowing view of.
     */
    w_nonowning(value_type& p_value)
        : w_nonowning(internal::w_raw_access::raw(p_value))
    {}

    w_nonowning(const w_nonowning&) = default;
    w_nonowning(w_nonowning&&) noexcept = default;

    w_nonowning& operator=(const w_nonowning&) = default;
    w_nonowning& operator=(w_nonowning&&) noexcept = default;

    ~w_nonowning() noexcept
    {
        // disown raw resource before resource
        // be released/free'ed by T's destructor.
        internal::w_raw_access::disown_raw(_value);
    }

    value_type* operator->() noexcept { return std::addressof(_value); }
    const value_type* operator->() const noexcept { return std::addressof(_value); }

    value_type& operator*() noexcept { return _value; }
    const value_type& operator*() const noexcept { return _value; }

private:
    template <typename RawT>
    w_nonowning(internal::w_raw_tag, RawT* p_rawptr) noexcept
        : _value(internal::w_raw_access::from_raw<value_type>(p_rawptr))
    {}

    value_type _value;
};


}  // namespace wolf::media::gst
