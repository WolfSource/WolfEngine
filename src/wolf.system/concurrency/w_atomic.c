#include "w_atomic.h"
#include <apr_atomic.h>

W_ATOMIC_INT w_atomic_inc(_Inout_ volatile W_ATOMIC_INT* pVal)
{
    return (W_ATOMIC_INT)

#ifdef _WIN64
        apr_atomic_inc64((volatile apr_uint64_t*)pVal);
#else
        apr_atomic_inc32((volatile apr_uint32_t*)pVal);
#endif
}

int w_atomic_dec(_Inout_ volatile W_ATOMIC_INT* pVal)
{
    return
#ifdef _WIN64
        apr_atomic_dec64((volatile apr_uint64_t*)pVal);
#else
        apr_atomic_dec32((volatile apr_uint32_t*)pVal);
#endif
}

void w_atomic_set(_Inout_ volatile W_ATOMIC_INT* pMem, W_ATOMIC_INT pVal)
{
#ifdef _WIN64
    apr_atomic_set64((volatile apr_uint64_t*)pMem, (apr_uint64_t)pVal);
#else
    apr_atomic_set32((volatile apr_uint32_t*)pMem, (apr_uint32_t)pVal);
#endif
}

W_ATOMIC_INT w_atomic_read(_Inout_ volatile W_ATOMIC_INT* pMem)
{
    return (W_ATOMIC_INT)
#ifdef _WIN64
        apr_atomic_read64((volatile apr_uint64_t*)pMem);
#else
        apr_atomic_read32((volatile apr_uint32_t*)pMem);
#endif 
}
