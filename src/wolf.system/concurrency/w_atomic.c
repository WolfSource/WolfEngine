#include "w_atomic.h"
#include <apr_atomic.h>

#if defined (_WIN64) || defined (W_PLATFORM_ANDROID) || defined (W_PLATFORM_OSX) || defined (W_PLATFORM_IOS) || defined (W_PLATFORM_LINUX)

W_ATOMIC_INT64 w_atomic_inc64(_Inout_ volatile W_ATOMIC_INT64* pVal)
{
    return (W_ATOMIC_INT64)apr_atomic_inc64((volatile apr_uint64_t*)pVal);
}

int w_atomic_dec64(_Inout_ volatile W_ATOMIC_INT64* pVal)
{
    return apr_atomic_dec64((volatile apr_uint64_t*)pVal);
}

void w_atomic_set64(_Inout_ volatile W_ATOMIC_INT64* pMem, W_ATOMIC_INT64 pVal)
{
    apr_atomic_set64((volatile apr_uint64_t*)pMem, (apr_uint64_t)pVal);
}

W_ATOMIC_INT64 w_atomic_read64(_Inout_ volatile W_ATOMIC_INT64* pMem)
{
    return (W_ATOMIC_INT64)apr_atomic_read64((volatile apr_uint64_t*)pMem);
}

#else

W_ATOMIC_INT64 w_atomic_inc32(_Inout_ volatile W_ATOMIC_INT64* pVal)
{
    return (W_ATOMIC_INT64)apr_atomic_inc32((volatile apr_uint32_t*)pVal);

}

int w_atomic_dec32(_Inout_ volatile W_ATOMIC_INT64* pVal)
{
    return apr_atomic_dec32((volatile apr_uint32_t*)pVal);
}

void w_atomic_set32(_Inout_ volatile W_ATOMIC_INT64* pMem, W_ATOMIC_INT64 pVal)
{
    apr_atomic_set32((volatile apr_uint32_t*)pMem, (apr_uint32_t)pVal);
}

W_ATOMIC_INT64 w_atomic_read32(_Inout_ volatile W_ATOMIC_INT64* pMem)
{
    return (W_ATOMIC_INT64)apr_atomic_read32((volatile apr_uint32_t*)pMem);
}

#endif