#pragma once

#include <gst/gst.h>

namespace wolf::gst {

/**
 * @brief a gstreamer application.
 */
class w_application
{
public:
    w_application() = delete;

    static bool init(int* argc, char*** argv)
    {
        gst_init(argc, argv);
        return true;
    }
};

}  // namespace wolf::gst
