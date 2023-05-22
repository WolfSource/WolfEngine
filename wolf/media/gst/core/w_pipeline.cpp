#include "media/gst/core/w_pipeline.hpp"

namespace wolf::media::gst {

auto w_pipeline::make(const char *p_name)
    -> boost::leaf::result<w_pipeline>
{
    auto pipeline_raw = gst_pipeline_new(p_name);
    if (!pipeline_raw) {
        return W_FAILURE(std::errc::operation_canceled,
                         wolf::format("couldn't create pipeline: {}", p_name));
    }
    return w_pipeline(internal::w_raw_tag{}, pipeline_raw);
}

auto w_pipeline::get_bus() -> boost::leaf::result<w_bus>
{
    auto bus_raw = gst_element_get_bus(raw());
    if (!bus_raw) {
        return W_FAILURE(std::errc::operation_canceled,
                         "couldn't get pipeline's bus.");
    }
    return internal::w_raw_access::from_raw<w_bus>(bus_raw);
}

bool w_pipeline::bin(w_flow_path &p_flow)
{
    for (auto& element : p_flow) {
        if (!bin(*element)) {
            return false;
        }
    }

    return true;
}

bool w_pipeline::link(w_flow_path &p_flow)
{
    auto* last = p_flow.first().get();
    for (int i = 1; i < p_flow.size(); ++i) {
        if (!link(*last, *p_flow[i])) {
            return false;
        }

        last = p_flow[i].get();
    }

    return true;
}

}  // namespace wolf::media::gst
