#pragma once

#include "media/gst/core/w_refptr.hpp"
#include "media/gst/core/w_element.hpp"

#include <vector>
#include <memory>
#include <utility>

namespace wolf::media::gst {

/**
 * @brief a container of w_refptr<w_element> representing a path in a pipeline flow.
 *
 * this container will always contain 2 elements at least.
 *
 * @note this container does not link
 *       or check for correct src/sink composition.
 *       it's only a storage.
 */
class w_flow_path
{
public:
    /**
     * @brief make a flow path.
     * @param elements list of elements to make a flow path from.
     */
    template <typename ...Ts>
        requires (sizeof...(Ts) >= 2)
    static auto make(Ts&& ...elements) {
        auto path = w_flow_path();

        path._vec.reserve(sizeof...(Ts));
        (..., path._vec.emplace_back(to_refptr(std::forward<Ts>(elements))));

        return path;
    }

    constexpr auto size() const noexcept -> std::size_t { return _vec.size(); }

    auto& first() { return _vec[0]; }
    const auto& first() const { return _vec[0]; }

    auto& last() { return _vec[size() - 1]; }
    const auto& last() const { return _vec[size() - 1]; }

    auto begin() noexcept { return std::begin(_vec); }
    auto begin() const noexcept { return std::begin(_vec); }

    auto end() noexcept { return std::end(_vec); }
    auto end() const noexcept { return std::end(_vec); }

    auto& operator[](std::size_t index) { return _vec[index]; }
    const auto& operator[](std::size_t index) const { return _vec[index]; }

private:
    w_flow_path() {}

    std::vector<w_refptr<w_element>> _vec;
};

}  // namespace wolf::media::gst
