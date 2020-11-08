

#include "net/w_net.h"



#include <net/amq/amq_consumer.h>
#include<net/amq/amq_producer.h>
#include"concurrency/w_concurrent_queue.h"
#include<net/w_amq.h>
#include"w_log.h"
amq_consumer _consumer;
amq_producer _producer;



int  w_net_init_producer(_Inout_ w_mem_pool pMemPool)
{
    const char* _trace_info = "w_net_init_producer";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
        return W_FAILURE;
    }
    _producer.init_producer(pMemPool);
    return W_SUCCESS;
}

void w_net_open_producer()
{
    _producer.run();

}

void w_net_send_producer( char * pMessage, int _priority)
{
    std::string _tmp_message(pMessage);
    _producer.send_message(_tmp_message, _priority);

}



void* w_thread_job_my_2(w_thread arg1, void* pargs)
{
   
    
    w_call_amq _usercallback = (w_call_amq)pargs;
    _consumer.on_message_received = [_usercallback](const std::string pMessage) -> void
    {
        _usercallback(pMessage.c_str());
    };
    _consumer.run();

   while(w_net_consumer_fini)
   {
        w_thread_current_sleep_for_milliseconds(50);

   }
    return NULL;

}


int  w_net_receive_consumer(_Inout_ w_mem_pool pMemPool , w_call_amq pCallback_tmp)
{
    w_thread _thread = NULL;
    const char* _trace_info = "w_net_open_consumer";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
        return APR_BADARG;
    }
   
    w_thread_init(pMemPool, &_thread, &w_thread_job_my_2, pCallback_tmp);
}

void w_callback_pOnceJob_init()
{
    library::ActiveMQCPP::initializeLibrary();

}


int  w_amq_init(_Inout_ w_mem_pool pMemPool)
{
    
    const char* _trace_info_1 = "w_amq_init:_mem_pool";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "bad args! trace info:%s", _trace_info_1);
        return APR_BADARG;
    }
    w_thread_once_flag _ponceflag =(w_thread_once_flag)w_malloc(pMemPool,sizeof(w_thread_once_flag));
    W_RESULT _thread_init_once_flag = w_thread_init_once_flag(pMemPool,&_ponceflag);

    const char* _trace_info_2 = "w_amq_init:w_thread_init_once_flag";
    if (_thread_init_once_flag)
    {
        W_ASSERT_P(false, "bad args. trace info %s", _trace_info_2);
        return W_FAILURE;
    }
    W_RESULT _thread_once_call= w_thread_once_call(  _ponceflag, &w_callback_pOnceJob_init);

    const char* _trace_info_3 = "w_amq_init:w_thread_once_call";
    if (_thread_once_call)
    {
        W_ASSERT_P(false, "bad args. trace info %s", _trace_info_3);
        return W_FAILURE;
    }
    return W_SUCCESS;
}

void w_callback_pOnceJob_fini()
{
    activemq::library::ActiveMQCPP::shutdownLibrary();
}
int w_amq_fini(_Inout_ w_mem_pool pMemPool)
{
   
    const char* _trace_info_1 = "w_amq_fini:_mem_pool";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "bad args! trace info:%s", _trace_info_1);
        return APR_BADARG;
    }

    w_thread_once_flag _ponceflag = (w_thread_once_flag)w_malloc(pMemPool, sizeof(w_thread_once_flag));
    W_RESULT _thread_init_once_flag = w_thread_init_once_flag(pMemPool, &_ponceflag);

    const char* _trace_info_2 = "w_amq_fini:w_thread_init_once_flag";
    if (_thread_init_once_flag)
    {
        W_ASSERT_P(false, "bad args. trace info %s", _trace_info_2);
        return W_FAILURE;
    }
    W_RESULT _thread_once_call = w_thread_once_call(_ponceflag, &w_callback_pOnceJob_fini);
    const char* _trace_info_3 = "w_amq_fini:w_thread_once_call";

    if (_thread_once_call)
    {
        W_ASSERT_P(false, "bad args. trace info %s", _trace_info_3);
        return W_FAILURE;
        
    }
   
    return W_SUCCESS;
}

 void w_net_consumer_fini()
{
     _consumer.close();
}

 void w_net_producer_fini()
 {
     _producer.close();
 }











