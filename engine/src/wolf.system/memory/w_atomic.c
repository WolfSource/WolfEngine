#include "w_atomic.h"
#include <apr-1/apr_atomic.h>

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

