#pragma once

#include "wolf.hpp"

#include <utility>

namespace wolf::stream::quic::internal {

/**
 * @brief Tag/Indicator of raw interaction constructor/method overload.
 */
class W_API w_raw_tag {};

/**
 * @brief A type to befriended by types which want
 *        to expose their private raw access api to
 *        other types within the library.
 *
 * This type allow for sharing a common api between
 * any library type declaring it as friend to any other library type.
 *
 * This is to avoid redundant/hardcoded/forward-declared friend access pattern.
 *
 * It also limits the access to a certain private api, unlike direct friendship.
 *
 * "Friends of friend are also friend" :)
 */
class W_API w_raw_access {
public:
    w_raw_access() = delete;

    template <typename T, typename ...Args>
    static auto from_raw(Args&& ...args)
    {
        return T(w_raw_tag{}, std::forward<Args>(args)...);
    }

    template <typename T>
    static auto raw(T&& p_object)
    {
        return std::forward<T>(p_object).raw();
    }
};

}  // namespace wolf::stream::quic::internal
