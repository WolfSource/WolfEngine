#include "wolf.h"
#include <log/w_log.h>

static apr_pool_t* s_default_memory_pool = NULL;

W_RESULT wolf_initialize()
{
    apr_status_t _ret = apr_initialize();
    if(_ret != APR_SUCCESS)
    {
        return W_FAILED;
    }
    
    //create default memory pool
    apr_pool_create(&s_default_memory_pool, NULL);
    if (!s_default_memory_pool) return W_FAILED;
    
    return W_PASSED;
}

apr_pool_t* w_get_default_memory_pool(void)
{
    return s_default_memory_pool;
}

void* w_alloc(_In_ size_t pSize)
{
    apr_pool_t* _pool = w_get_default_memory_pool();
    if(!_pool)
    {
        W_ASSERT(false, "could not get default memory. trace info: w_alloc");
        return NULL;
    }
     
    return apr_palloc(_pool, pSize);
}

void wolf_terminate()
{
    apr_terminate();
}
