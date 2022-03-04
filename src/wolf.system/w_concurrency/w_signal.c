#ifndef _WIN32

#include "w_signal.h"
#include <apr_signal.h>
#include <w_log/w_log.h>

int w_signal_init()
{
    return 0;
}

const char* w_signal_get_desc(_In_ int pSigno)
{
    return NULL;
}

w_signal_handler* w_signal_set_handler(_In_ int pSigno, _In_ w_signal_handler* pHandler)
{
    if (!pHandler)
    {
        W_ASSERT(false, "w_signal_handler is NULL!. trace info: w_signal_set_handler");
        return NULL;
    }
    return apr_signal(pSigno, (apr_sigfunc_t*)pHandler);
}

W_RESULT w_signal_block(_In_ int pSigno)
{
	apr_status_t _ret = apr_signal_block(pSigno);
	return _ret == APR_SUCCESS ? W_SUCCESS : W_FAILURE;
}

W_RESULT w_signal_unblock(_In_ int pSigno)
{
	apr_status_t _ret = apr_signal_unblock(pSigno);
	return _ret == APR_SUCCESS ? W_SUCCESS : W_FAILURE;
}

#endif
