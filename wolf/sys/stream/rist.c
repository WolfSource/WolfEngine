#include "rist.h"
#include <mimalloc.h>

#ifdef _MSVC
#pragma pack(push, _ALIGNMENT_)
#endif
typedef struct w_rist_t
{
    struct rist_ctx *ctx;
} w_rist_t __attribute__((aligned(_ALIGNMENT_)));

int w_rist_init(w_rist p_rist, w_buf p_trace)
{
    w_rist _rist = (w_rist)mi_malloc(sizeof(w_rist_t));
    if (_rist == NULL)
    {
        return 1;
    }

    p_rist;
    return 0;
}
