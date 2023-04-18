#pragma once

#include "media/gst/core/w_element.hpp"
#include "media/gst/core/w_caps.hpp"

namespace wolf::media::gst {

/**
 * @brief wrappper of capsfilter gstreamer element.
 */
class w_element_capsfilter : public w_element
{
    constexpr static const char* factory_name = "capsfilter";

public:
    [[nodiscard]] static auto make() -> boost::leaf::result<w_element_capsfilter>
    {
        BOOST_LEAF_AUTO(base_element, w_element::make(factory_name));
        return w_element_capsfilter(std::move(base_element));
    }

    void set_caps(w_caps& p_caps)
    {
        g_object_set(raw(), "caps", internal::w_raw_access::raw(p_caps), nullptr);
    }

private:
    explicit w_element_capsfilter(w_element&& p_base) noexcept
        : w_element(std::move(p_base))
    {}
};

}  // namespace wolf::media::gst
