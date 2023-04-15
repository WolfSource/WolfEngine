#pragma once

#include <utility>

namespace wolf::gst::internal {

/** tag to indicate having a raw pointer in the context. */
struct w_raw_tag {};

/**
 * @brief helper utility to be common friend of wrappers,
 *        poviding access to certain api for any wrapper to another.
 *
 * this comes handy when w_audio_info wants to create a w_caps,
 * or w_pipeline wants to add w_element to its bin.
 *
 * instead of having the aformentioned types know/befriend each other,
 * they'll be friend to this class and provide private access to this,
 * so any of them can access each other with this class as a common friend.
 */
class w_raw_access
{
public:
    /**
     * @brief create an instance of `T` with given raw pointer.
     * @param p_raw_ptr raw pointer that `T` wraps or accepts.
     * @param p_args    custom extended args.
     */
    template <typename T, typename RawT, typename ...Args>
    static auto from_raw(RawT* p_raw_ptr, Args&& ...p_args)
    {
        return T(w_raw_tag{}, p_raw_ptr, std::forward<Args>(p_args)...);
    }

    /**
     * @brief get raw pointer wrapped in the given wrapper object.
     * @param p_obj wrapper object.
     */
    template <typename T>
    static auto raw(T& p_obj) noexcept
    {
        return p_obj.raw();
    }

    /**
     * @brief get the underlying raw pointer from given object,
     *        and make the object no longer point to that raw resource without releasing it.
     * @param p_obj the wrapper object to discard its underlying raw pointer.
     */
    template <typename T>
    static auto disown_raw(T&& p_obj) noexcept
    {
        return p_obj.disown_raw();
    }
};

}  // namespace wolf::gst::internal
