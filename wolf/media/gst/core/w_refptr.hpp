#pragma once

#include <boost/leaf.hpp>

#include <type_traits>
#include <memory>

namespace wolf::media::gst {

/**
 * shared ref-counting smart pointer of given `T`.
 *
 * @tparam T type to wrap shared reference to.
 */
template <typename T>
using w_refptr = std::shared_ptr<T>;

/**
 * @brief convert given value to a ref-counting smart pointer.
 * @param p_value value to make shared.
 * @return sharable ref-counting refptr.
 */
template <typename T>
inline w_refptr<T> to_refptr(T&& p_value)
{
    using type = std::remove_cvref_t<T>;
    return std::make_shared<type>(std::forward<T>(p_value));
}

/**
 * @brief convert given value to a ref-counting smart pointer.
 * @param p_value a refptr to increase its ref-count.
 * @return another copy of given refptr pointing to same value.
 */
template <typename T>
inline w_refptr<T> to_refptr(w_refptr<T> p_value)
{
    return p_value;
}

//template <typename T>
//inline w_refptr<T> to_refptr(boost::leaf::result<T> p_value)
//{
//    if (!p_value) {
//        return w_refptr<T>(nullptr);
//    }
//    return to_refptr<T>(std::move(*p_value));
//}

}  // namespace wolf::media::gst
