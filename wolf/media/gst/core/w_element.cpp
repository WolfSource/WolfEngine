#include "media/gst/core/w_element.hpp"

namespace wolf::media::gst {

auto w_element::make(const char *p_factory_name) -> boost::leaf::result<wolf::gst::w_element>
{
    auto element_factory_raw = gst_element_factory_find(p_factory_name);
    if (!element_factory_raw) {
        auto err_msg = wolf::format("couldn't find factory name: {}", p_factory_name);
        return W_FAILURE(std::errc::operation_canceled, err_msg);
    }

    auto element_raw = gst_element_factory_create(element_factory_raw, nullptr);
    if (!element_raw) {
        auto err_msg = wolf::format(
            "couldn't create the element with given factory name: {}",
            p_factory_name
        );
        return W_FAILURE(std::errc::operation_canceled, err_msg);
    }

    if (G_IS_INITIALLY_UNOWNED(element_raw)) {
        gst_object_ref_sink(element_raw);
    }

    return w_element(internal::w_raw_tag{}, element_raw);
}

}  // namespace wolf::media::gst
