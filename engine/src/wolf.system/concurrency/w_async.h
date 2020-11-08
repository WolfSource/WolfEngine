///*
//    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
//    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
//    Website          : https://WolfEngine.App
//    Name             : w_async.h
//    Description      : asynchronous job
//    Comment          : https://metacpan.org/pod/distribution/EV/libev/ev.pod
//*/
//
//#pragma once
//
//#ifdef __cplusplus
//extern "C" {
//#endif
//
//#include "wolf.h"
//#include "libev/ev.h"
//#include "w_thread.h"
//
//    typedef ev_async w_async_base;
//    typedef struct ev_loop w_async_loop;
//    typedef void w_async_callback(EV_P_ w_async_base*, int /*revents*/);
//
//    typedef struct w_async_t
//    {
//        w_async_base* a;
//        w_async_loop* l;
//        w_thread                    t;
//    } w_async_t;
//    typedef struct w_async_t* w_async;
//
//    /**
//     * create a async
//     * @param pAsync async
//     * @param pAsyncCallBack callback async
//     * @return result code
//    */
//    W_SYSTEM_EXPORT
//        W_RESULT w_async_init(
//            _Inout_ w_mem_pool pMemPool,
//            _Inout_ w_async* pAsync, 
//            _In_ w_async_callback pAsyncCallBack);
//
//    /**
//     * create a async
//     * @param pAsync async
//     * @return result code
//    */
//    W_SYSTEM_EXPORT
//        W_RESULT w_async_send(_In_ w_async pAsync, _In_opt_ void* pArg);
//
//    /**
//     * start async
//     * @param pAsync async
//     * @return result code
//    */
//    W_SYSTEM_EXPORT
//        W_RESULT w_async_start(_In_ w_async pAsync);
//
//    /**
//     * stop async
//     * @param pAsync async
//     * @return result code
//    */
//    W_SYSTEM_EXPORT
//        W_RESULT w_async_stop(_In_ w_async pAsync);
//
//#ifdef __cplusplus
//}
//#endif
