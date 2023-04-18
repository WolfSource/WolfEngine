#pragma once

#include "media/gst/core/w_element.hpp"

namespace wolf::media::gst {

/**
 * @brief wrappper of voaacenc gstreamer element.
 */
class w_element_voaacenc : public w_element
{
    constexpr static const char* factory_name = "voaacenc";

public:
    [[nodiscard]] static auto make() -> boost::leaf::result<w_element_voaacenc>
    {
        BOOST_LEAF_AUTO(base_element, w_element::make(factory_name));
        return w_element_voaacenc(std::move(base_element));
    }

    void set_bitrate(std::size_t p_bitrate)
    {
        g_object_set(raw(), "bitrate", p_bitrate, nullptr);
    }

private:
    explicit w_element_voaacenc(w_element&& p_base) noexcept
        : w_element(std::move(p_base))
    {}
};

}  // namespace wolf::media::gst
