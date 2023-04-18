#pragma once

#include "media/gst/core/w_element.hpp"

namespace wolf::media::gst {

/**
 * @brief wrappper of filesink gstreamer element.
 */
class w_element_filesink : public w_element
{
    constexpr static const char* factory_name = "filesink";

public:
    [[nodiscard]] static auto make() -> boost::leaf::result<w_element_filesink>
    {
        BOOST_LEAF_AUTO(base_element, w_element::make(factory_name));
        return w_element_filesink(std::move(base_element));
    }

    void set_location(const char* p_location)
    {
        g_object_set(raw(), "location", p_location, nullptr);
    }

private:
    explicit w_element_filesink(w_element&& p_base) noexcept
        : w_element(std::move(p_base))
    {}
};

}  // namespace wolf::media::gst
