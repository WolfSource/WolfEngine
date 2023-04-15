#pragma once

#include "media/gst/internal/w_common.hpp"
#include "media/gst/internal/w_wrapper.hpp"

#include <gst/gst.h>

#include <utility>

namespace wolf::gst {

class w_message;

/** type of GstMessage */
enum class w_message_type : std::size_t
{
    Unknown = 0,
    EOS,
    Error,
    Warning,
    Info
};

//- message structs

/** the unknown variant of GstMessage */
struct w_message_unknown {};

/** the end-of-stream variant of GstMessage */
struct w_message_eos {};

/** the error message variant of GstMessage */
struct w_message_error
{
    friend class w_message;

public:
    w_message_error() = delete;

    w_message_error(const w_message_error&) = delete;

    ~w_message_error() noexcept
    {
        if (_error) {
            g_clear_error(&_error);
        }

        if (_debug_info) {
            g_free(_debug_info);
        }
    }

    /**
     * @brief print the error message to standard output.
     */
    void print() const
    {
        g_printerr(
            "Error received from element %s: %s\n",
            GST_OBJECT_NAME(_msg->src),
            _error->message
        );
        g_printerr(
            "Debugging information: %s\n",
            _debug_info ? _debug_info : "none"
        );
    }

private:
    /**
     * @brief parse the message to extract error info.
     */
    explicit w_message_error(internal::w_raw_tag, GstMessage* p_msg_raw)
        : _msg(p_msg_raw)
    {
        gst_message_parse_error(p_msg_raw, &_error, &_debug_info);
    }

    GstMessage* _msg = nullptr;  //< non-owning.
    GError* _error = nullptr;
    gchar* _debug_info = nullptr;
};

/**
 * @brief wrapper of GstMessage. gstreamer's ultimate message struct.
 *
 * there are so many different kinds and variants of message,
 * and all are represented by w_message.
 *
 * for ease of use, there are helper message representative classes,
 * which on visit the appropriate one will be created and passed to
 * given visitor.
 */
class w_message : public w_wrapper<w_message, GstMessage, void, gst_message_unref>
{
    friend class internal::w_raw_access;

public:
    // not supported yet.
    w_message() = delete;

//    w_message_type type() const noexcept
//    {
//        return convert_message_type(GST_MESSAGE_TYPE(msg_));
//    }

    /**
     * @brief visit the message based on its type as the helper representative type.
     * @param p_visitor visitor to visit message variant.
     */
    template <typename VisitorF>
    auto visit(VisitorF&& p_visitor)
    {
        return raw_visit(std::forward<VisitorF>(p_visitor), raw());
    }

private:
    explicit w_message(internal::w_raw_tag, GstMessage* p_msg_raw)
        : w_wrapper(p_msg_raw)
    {}

    /**
     * @brief create and visit appropriate repr variant by given raw message and visitor.
     * @param p_visitor visitor to visit message variant.
     * @param p_msg_raw raw message pointer.
     */
    template <typename VisitorF>
    static auto raw_visit(VisitorF&& p_visitor, GstMessage* p_msg_raw)
    {
        switch (GST_MESSAGE_TYPE(p_msg_raw)) {
            case GST_MESSAGE_EOS:
                return std::forward<VisitorF>(w_message_eos{});
            case GST_MESSAGE_ERROR:
                return std::forward<VisitorF>(p_visitor)(
                    w_message_error(internal::w_raw_tag{}, p_msg_raw)
                );
            default: // GST_MESSAGE_UNKNOWN
                return std::forward<VisitorF>(p_visitor)(w_message_unknown{});
        }
    }
};

}  // namespace wolf::gst
