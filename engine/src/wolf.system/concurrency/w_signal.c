#include "w_signal.h"
#include <apr-1/apr_signal.h>

int w_signal_init()
{
#ifdef _WIN32
	return APR_ENOTIMPL;
#else
    return 0;
#endif
}

const char* w_signal_get_desc(_In_ int pSigno)
{
#ifdef _WIN32
	return "unknown signal (not supported)";
#else
    return 0;
#endif
}

w_signal_handler* w_signal_set_handler(_In_ int pSigno, _In_ w_signal_handler* pHandler)
{
#ifdef _WIN32
	//windows does not support signal
	return NULL;
#else
    if (!pHandler)
    {
        W_ASSERT(false, "w_signal_handler is NULL!. trace info: w_signal_set_handler");
        return NULL;
    }
    return apr_signal(pSigno, (apr_sigfunc_t*)pHandler);
#endif
}

W_RESULT w_signal_block(_In_ int pSigno)
{
#ifdef _WIN32
	//windows does not support signal
	return W_FAILED;
#else
	apr_status_t _ret = apr_signal_block(pSigno);
	return _ret == APR_SUCCESS ? W_PASSED : W_FAILED;
#endif
}

W_RESULT w_signal_unblock(_In_ int pSigno)
{
#ifdef _WIN32
	//windows does not support signal
	return W_FAILED;
#else
	apr_status_t _ret = apr_signal_unblock(pSigno);
	return _ret == APR_SUCCESS ? W_PASSED : W_FAILED;
#endif
}
