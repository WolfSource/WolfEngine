#pragma once

#include "media/gst/core/w_element.hpp"

namespace wolf::media::gst {

/**
 * @brief wrappper of h264parse gstreamer element.
 */
class w_element_h264parse : public w_element
{
    constexpr static const char* factory_name = "h264parse";

public:
    [[nodiscard]] static auto make() -> boost::leaf::result<w_element_h264parse>
    {
        BOOST_LEAF_AUTO(base_element, w_element::make(factory_name));
        return w_element_h264parse(std::move(base_element));
    }

private:
    explicit w_element_h264parse(w_element&& p_base) noexcept
        : w_element(std::move(p_base))
    {}
};

}  // namespace wolf::media::gst
