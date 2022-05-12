#include "wolf.h"

int is_buf_valid(w_buf p_buf)
{
    if (p_buf->len == 0 || p_buf->data == NULL)
    {
        return 1;
    }
    return 0;
}