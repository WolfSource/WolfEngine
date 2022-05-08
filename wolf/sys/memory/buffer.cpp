// #include "buffer.h"
// #include <mimalloc.h>

// buffer *w_malloc(size_t p_size)
// {
//     // allocate memory for struct buffer
//     auto *buf = static_cast<buffer *>(mi_malloc(sizeof(buffer)));
//     if (buf == nullptr)
//     {
//         return nullptr;
//     }

//     // allocate memory for data
//     buf->data = static_cast<uint8_t *>(mi_malloc_aligned(p_size, BUFFER_ALIGNED_SIZE));
//     if (buf->data == nullptr)
//     {
//         // free memory buffer
//         mi_free(buf);
//         return nullptr;
//     }
//     buf->size = p_size;

//     // it's ok to return buffer
//     return buf;
// }

// void w_free(buffer *p_mem)
// {
//     if (p_mem != nullptr && p_mem->data != nullptr && p_mem->size > 0)
//     {
//         mi_free(p_mem->data);
//     }
// }