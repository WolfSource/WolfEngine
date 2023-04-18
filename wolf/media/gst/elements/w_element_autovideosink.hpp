#pragma once

#include "media/gst/core/w_element.hpp"

#include <gst/gst.h>

namespace wolf::media::gst {

/**
 * @brief wrappper of audiovideosink gstreamer element.
 */
class w_element_autovideosink : public w_element
{
    constexpr static const char* factory_name = "autovideosink";

public:
    [[nodiscard]] static auto make() -> boost::leaf::result<w_element_autovideosink>
    {
        BOOST_LEAF_AUTO(base_element, w_element::make(factory_name));
        return w_element_autovideosink(std::move(base_element));
    }

private:
    explicit w_element_autovideosink(w_element&& p_base) noexcept
        : w_element(std::move(p_base))
    {}
};

}  // namespace wolf::media::gst
