#pragma once

#include "media/gst/core/w_element.hpp"

namespace wolf::gst {

/**
 * @brief wrappper of videotestsrc gstreamer element.
 */
class w_element_videotestsrc : public w_element
{
    constexpr static const char* factory_name = "videotestsrc";

public:
    [[nodiscard]] static auto make() -> boost::leaf::result<w_element_videotestsrc>
    {
        BOOST_LEAF_AUTO(base_element, w_element::make(factory_name));
        return w_element_videotestsrc(std::move(base_element));
    }

private:
    explicit w_element_videotestsrc(w_element&& p_base) noexcept
        : w_element(std::move(p_base))
    {}
};

}  // namespace wolf::gst

