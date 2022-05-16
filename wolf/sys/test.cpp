#include "wolf.h"
#include "stream/rist.h"

int main()
{
    w_buf_t trace;
    trace.len = 256;
    trace.data = new uint8_t[trace.len];

    w_rist_config_t config =
        {
            w_rist_mode::w_rist_mode_sender,
            0,
            10,
            0,
        };
    w_rist rist = nullptr;
    w_rist_init(&rist,
                "rist://127.0.0.1:9999",
                &config,
                &trace);

    w_rist_start(rist, &trace);
}