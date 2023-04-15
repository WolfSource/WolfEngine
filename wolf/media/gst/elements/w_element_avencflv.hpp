#pragma once

#include "media/gst/core/w_element.hpp"

namespace wolf::gst {

/**
 * @brief wrappper of avencflv gstreamer element.
 */
class w_element_avencflv : public w_element
{
    constexpr static const char* factory_name = "avenc_flv";

public:
    [[nodiscard]] static auto make() -> boost::leaf::result<w_element_avencflv>
    {
        BOOST_LEAF_AUTO(base_element, w_element::make(factory_name));
        return w_element_avencflv(std::move(base_element));
    }

private:
    explicit w_element_avencflv(w_element&& p_base) noexcept
        : w_element(std::move(p_base))
    {}
};

}  // namespace wolf::gst
