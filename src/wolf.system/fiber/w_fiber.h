/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
    Name			 : w_fiber.h
    Description		 : a fiber based on boost::fiber
    Comment          :
*/
#pragma once

#ifdef WOLF_ENABLE_FIBER

#ifdef __cplusplus
extern "C" {
#endif

#include <wolf.h>

    struct w_fiber_t;
    typedef w_fiber_t* w_fiber;
    typedef void (*w_fiber_job)(void* /*pArgs*/);

    /**
     * initialize a fiber
     * @param pJob fiber job
     * @return fiber id
    */
    W_SYSTEM_EXPORT
        W_RESULT w_fiber_init(
            _In_        w_mem_pool pMemPool,
            _Inout_     w_fiber* pFiber,
            _In_        w_fiber_job pJob,
            _In_opt_    void* pArg);

    /**
     * check whether fiber is joinable
     * @param pFiber handler of fiber
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT w_fiber_is_joinable(_In_ w_fiber pFiber);

    /**
     * join a fiber
     * @param pFiber handler of fiber
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT w_fiber_join(_In_ w_fiber pFiber);

    /**
     * detach a fiber
     * @param pFiber handler of fiber
     * @return result as W_RESULT
    */
    W_SYSTEM_EXPORT
        W_RESULT w_fiber_detach(_In_ w_fiber pFiber);

    /**
     * swap two fibers
     * @param pFiber1 handler of first fiber
     * @param pFiber2 handler of second fiber
     * @return result as W_RESULT
    */
    W_SYSTEM_EXPORT
        W_RESULT w_fiber_swap(_Inout_ w_fiber pFiber1, _Inout_ w_fiber pFiber2);

    /**
     * finitialize fiber
     * @param pFiber handler of fiber
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT w_fiber_fini(_Inout_ w_fiber* pFiber);

#ifdef __cplusplus
}
#endif

#endif
