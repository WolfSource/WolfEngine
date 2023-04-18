#include "media/gst/core/w_structure.hpp"

namespace wolf::media::gst {

auto wolf::gst::w_structure::make(const char *p_name)
    -> boost::leaf::result<wolf::gst::w_structure>
{
    auto structure_raw = gst_structure_new_empty(p_name);
    if (!structure_raw) {
        return W_FAILURE(std::errc::operation_canceled,
                         "couldn't create structure.");
    }

    return w_structure(internal::w_raw_tag{}, structure_raw);
}

auto w_structure::make_from_str(const char *p_str) -> boost::leaf::result<w_structure>
{
    auto structure_raw = gst_structure_from_string(p_str, nullptr);
    if (!structure_raw) {
        return W_FAILURE(std::errc::operation_canceled,
                         "couldn't create structure from given string repr.");
    }

    return w_structure(internal::w_raw_tag{}, structure_raw);
}

}  // namespace wolf::media::gst
