#pragma once

#include "media/gst/core/w_element.hpp"

namespace wolf::media::gst {

/**
 * @brief wrappper of flvmux gstreamer element.
 */
class w_element_flvmux : public w_element
{
    constexpr static const char* factory_name = "flvmux";

public:
    [[nodiscard]] static auto make() -> boost::leaf::result<w_element_flvmux>
    {
        BOOST_LEAF_AUTO(base_element, w_element::make(factory_name));
        return w_element_flvmux(std::move(base_element));
    }

    void set_streamable(bool p_streamable)
    {
        g_object_set(raw(), "streamable", p_streamable, nullptr);
    }

private:
    explicit w_element_flvmux(w_element&& p_base) noexcept
        : w_element(std::move(p_base))
    {}
};

}  // namespace wolf::media::gst
