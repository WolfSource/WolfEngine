#pragma once

#include "wolf.hpp"

#include "media/gst/w_flow.hpp"
#include "media/gst/core/w_element.hpp"
#include "media/gst/core/w_bus.hpp"
#include "media/gst/internal/w_wrapper.hpp"

#include <gst/gst.h>

#include <string_view>
#include <stdexcept>

namespace wolf::media::gst {

/**
 * @brief wrapper of GstElement, providing gstreamer pipeline concept.
 */
class w_pipeline : public w_wrapper<w_pipeline, GstElement, void, gst_object_unref>
{
public:
    /**
     * @brief make an instance of pipeline with given name.
     * @param p_name pipeline's name.
     * @return an instance of pipeline on success.
     */
    [[nodiscard]] static auto make(const char* p_name)
        -> boost::leaf::result<w_pipeline>;

    /**
     * @brief get pipeline's bus.
     */
    [[nodiscard]] auto get_bus() -> boost::leaf::result<w_bus>;

    /**
     * @brief add given elements to pipeline's bin.
     * @param p_args pack of elements.
     * @return boolean indicating success or failure.
     */
    template <typename ...Args>
        requires (sizeof...(Args) > 1)
    bool bin(Args&& ...p_args)
    {
        return (true && ... && bin(std::forward<Args>(p_args)));
    }

    /**
     * @brief add elements from given flow path to pipeline's bin.
     * @param p_flow elements flow path.
     * @return boolean indicating success or failure.
     */
    bool bin(w_flow_path& p_flow);

    /**
     * @brief add given single element to pipeline's bin.
     * @param p_element single element.
     * @return boolean indicating success or failure.
     */
    bool bin(w_element& p_element)
    {
        auto element_raw = internal::w_raw_access::raw(p_element);
        p_element.parented();
        return gst_bin_add(GST_BIN(raw()), element_raw);
    }

    /**
     * @brief link a pack of given linkables (element or pad) together after each other.
     * @param p_a source linkable to link with `p_b`.
     * @param p_b sink linkable to link with `p_a`.
     * @param p_rest pack of rest of linkables to be linked after `p_b` as source.
     * @return boolean indicating success or failure.
     */
    template <typename T, typename U, typename ...Rest>
    bool link(T&& p_a, U&& p_b, Rest&& ...p_rest)
    {
        auto raw_a = internal::w_raw_access::raw(p_a);
        auto raw_b = internal::w_raw_access::raw(p_b);

        if (!gst_element_link(raw_a, raw_b)) {
            return false;
        }

        if constexpr (sizeof...(Rest) > 0) {
            return link(std::forward<U>(p_b), std::forward<Rest>(p_rest)...);
        }

        return true;
    }

    /**
     * @brief link two elements.
     * @param p_src source element.
     * @param p_sink sink element.
     * @return boolean indicating success or failure.
     */
    bool link(w_element& p_src, w_element& p_sink)
    {
        return gst_element_link(
            internal::w_raw_access::raw(p_src),
            internal::w_raw_access::raw(p_sink)
        );
    }

    /**
     * @brief link a flow path's elements after each other.
     * @param p_flow flow path set of elements.
     * @return boolean indicating success or failure.
     */
    bool link(w_flow_path& p_flow);

    /**
     * @brief set the pipeline to play state.
     * @return boolean indicating success or failure.
     */
    bool play()
    {
        return static_cast<bool>(gst_element_set_state(raw(), GST_STATE_PLAYING));
    }

    /**
     * @brief set the pipeline to pause state.
     * @return boolean indicating success or failure.
     */
    bool pause()
    {
        return static_cast<bool>(gst_element_set_state(raw(), GST_STATE_PAUSED));
    }

    /**
     * @brief set the pipeline to stop state.
     * @return boolean indicating success or failure.
     */
    bool stop() {
        return static_cast<bool>(gst_element_set_state(raw(), GST_STATE_NULL));
    }

private:
    w_pipeline(internal::w_raw_tag, GstElement* pipeline_raw) noexcept
        : w_wrapper(pipeline_raw)
    {}
};

}  // namespace wolf::media::gst
