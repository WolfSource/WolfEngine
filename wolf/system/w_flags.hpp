#pragma once

#include "wolf.hpp"

#include <type_traits>

namespace wolf::system {

/**
 * @brief combinable and comparable bit flags of given enum.
 *
 * Enum types, specially in C programming language, are commonly
 * used for bit flags. but in C++ an enum type requires a cast
 * and bitwise operators can't be applied to it directly.
 *
 * This class helps with avoiding repeating cast
 * and mistaken implicit numeric conversions.
 *
 * It's implicitly convertible to boolean, and supports & and | operators.
 * It's also explicitly convertible to given enum's underlying type and std::size_t.
 *
 * @tparam T  enum type to wrap.
 */
template <typename T>
    requires std::is_enum_v<T>
class W_API w_flags {
public:
    using enum_type = std::remove_cvref_t<T>;
    using underlying_type = std::underlying_type_t<enum_type>;

    constexpr w_flags(enum_type value) noexcept
        : _bits(static_cast<underlying_type>(value))
    {}

    /**
     * @brief to underlying integral type.
     */
    constexpr auto to_underlying() const noexcept { return _bits; }

    constexpr friend w_flags operator&(w_flags lhs, w_flags rhs) noexcept
    {
        return w_flags(lhs._bits | rhs._bits);
    }

    constexpr friend w_flags operator&(w_flags lhs, enum_type rhs) noexcept
    {
        return w_flags(lhs._bits & static_cast<underlying_type>(rhs));
    }

    constexpr friend w_flags operator|(w_flags lhs, w_flags rhs) noexcept
    {
        return w_flags(lhs._bits | rhs._bits);
    }

    constexpr friend w_flags operator|(w_flags lhs, enum_type rhs) noexcept
    {
        return w_flags(lhs._bits | static_cast<underlying_type>(rhs));
    }

    constexpr friend bool operator==(w_flags lhs, w_flags rhs) noexcept
    {
        return lhs._bits == rhs._bits;
    }

    constexpr friend bool operator!=(w_flags lhs, w_flags rhs) noexcept
    {
        return lhs._bits != rhs._bits;
    }

    constexpr friend bool operator==(w_flags lhs, enum_type rhs) noexcept
    {
        return lhs._bits == static_cast<underlying_type>(rhs);
    }

    constexpr friend bool operator!=(w_flags lhs, enum_type rhs) noexcept
    {
        return lhs._bits != static_cast<underlying_type>(rhs);
    }

    constexpr operator bool() const noexcept
    {
        return bool(_bits);
    }

    constexpr explicit operator underlying_type() const noexcept
    {
        return _bits;
    }

    constexpr explicit operator std::size_t() const noexcept
    {
        return std::size_t(_bits);
    }

private:
    constexpr explicit w_flags(underlying_type bits) noexcept
        : _bits(bits)
    {}

    underlying_type _bits;
};

}  // namespace wolf::system

