#pragma once

#include "media/gst/core/w_element.hpp"

namespace wolf::media::gst {

/**
 * @brief wrappper of x264enc gstreamer element.
 */
class w_element_x264enc : public w_element
{
    constexpr static const char* factory_name = "x264enc";

public:
    [[nodiscard]] static auto make() -> boost::leaf::result<w_element_x264enc>
    {
        BOOST_LEAF_AUTO(base_element, w_element::make(factory_name));
        return w_element_x264enc(std::move(base_element));
    }

    void set_bitrate(std::size_t p_bitrate)
    {
        g_object_set(raw(), "bitrate", p_bitrate, nullptr);
    }

    void set_pass(std::size_t p_pass)
    {
        g_object_set(raw(), "pass", p_pass, nullptr);
    }

    void set_quantizer(std::size_t p_quantizer)
    {
        g_object_set(raw(), "quantizer", p_quantizer, nullptr);
    }

    void set_qp_min(std::size_t p_qp_min)
    {
        g_object_set(raw(), "qp-min", p_qp_min, nullptr);
    }

    void set_qp_max(std::size_t p_qp_max)
    {
        g_object_set(raw(), "qp-max", p_qp_max, nullptr);
    }

    void set_speed_preset(std::size_t p_speed_preset)
    {
        g_object_set(raw(), "speed-preset", p_speed_preset, nullptr);
    }

    void set_key_int_max(std::size_t p_key_int_max)
    {
        g_object_set(raw(), "key-int-max", p_key_int_max, nullptr);
    }

    void set_b_adapt(bool p_b_adapt)
    {
        g_object_set(raw(), "b-adapt", p_b_adapt, nullptr);
    }

    void set_bframes(std::size_t p_bframes)
    {
        g_object_set(raw(), "bframes", p_bframes, nullptr);
    }

private:
    explicit w_element_x264enc(w_element&& p_base) noexcept
        : w_element(std::move(p_base))
    {}
};

}  // namespace wolf::media::gst
