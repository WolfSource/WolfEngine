#include "media/gst/core/w_buffer.hpp"

#include "media/gst/internal/w_common.hpp"

namespace wolf::media::gst {

auto w_buffer::make(size_t p_size) -> boost::leaf::result<w_buffer>
{
    auto buffer_raw = gst_buffer_new_and_alloc(p_size);
    if (!buffer_raw) {
        return W_FAILURE(std::errc::operation_canceled,
                         "couldn't create and allocate Buffer.");
    }

    return w_buffer(internal::w_raw_tag{}, buffer_raw);
}

}  // namespace wolf::media::gst
