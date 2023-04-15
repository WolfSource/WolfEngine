#pragma once

#include "media/gst/internal/w_common.hpp"
#include "media/gst/core/w_element.hpp"

#include <gst/gst.h>

#include <optional>
#include <utility>
#include <stdexcept>

namespace wolf::gst {

/**
 * @brief gstreamer's element factory to create elements.
 */
class w_element_factory
{
public:
    w_element_factory() = delete;

    /**
     * @brief make an element from given gstreamer launch command.
     * @param p_launch_str a gstreamer lauch command.
     */
    [[nodiscard]] static auto make_from_launch_str(const char* p_launch_str)
        -> boost::leaf::result<w_element>
    {
        auto element_raw = gst_parse_launch(p_launch_str, nullptr);
        if (!element_raw) {
            return W_FAILURE(std::errc::operation_canceled,
                             "couldn't create element from launch string.");
        }

        return internal::w_raw_access::from_raw<w_element>(element_raw);
    }

    /**
     * @brief make a an element of given factory name, with given element name.
     * @param p_factory_name name of element module.
     * @param p_element_name custom name for element to reference later. (nullable)
     * @return an element on success.
     */
    [[nodiscard]] static auto make_simple(const char* p_factory_name, const char* p_element_name)
        -> boost::leaf::result<w_element>
    {
        auto element_raw = gst_element_factory_make(p_factory_name, p_element_name);
        if (!element_raw) {
            auto err_msg = wolf::format("couldn't create element with factory name: {}", p_factory_name);
            return W_FAILURE(std::errc::operation_canceled, err_msg);
        }
        return internal::w_raw_access::from_raw<w_element>(element_raw);
    }
};

}  // namespace wolf::gst
