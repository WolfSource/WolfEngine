#include "net/w_net.h"
#include <net/amq/amq_consumer.h>
#include<net/amq/amq_producer.h>
#include"concurrency/w_concurrent_queue.h"
#include<net/w_amq.h>
struct mystruct
{
    std::string message;
    int priority;
};
w_thread _thread;
w_mem_pool _mem_pool = NULL;
int x = w_mem_pool_init(&_mem_pool);

w_concurrent_queue _q = (w_concurrent_queue)w_malloc(_mem_pool, sizeof(w_concurrent_queue));
void w_net_open_producer() 
{
   
    w_thread_init(_mem_pool, &_thread, &w_thread_job_my, NULL);
}

void w_net_send_producer(std::string pMessage, int _priority) 
{
    struct mystruct pargs;
    pargs.message = pMessage;
    pargs.priority = _priority;
    w_concurrent_queue_push(_q,(void**)&pargs);
}

void* w_thread_job_my(w_thread arg1, void* pargs) {


    struct mystruct _message;
    struct mystruct *_message_tmp=& _message;
        amq_producer _producer;
        amq_consumer _consumer;
    
        _producer.run();

        for (;;)
        {
            if (!w_concurrent_queue_size(_q))
            {
                w_thread_current_sleep_for_milliseconds(50);
                continue;
            }

            w_concurrent_queue_pop(_q, (void**)&_message_tmp);
            if (_message_tmp==NULL ) continue;

            
            _producer.send_message(_message_tmp->message, _message_tmp->priority);

        }
}


void* w_thread_job_my_2(w_thread arg1, void* pargs)
{

    amq_consumer _consumer;
    w_call_amq usercallback =(w_call_amq) pargs;
    _consumer.on_message_received = [usercallback](const std::string pMessage) -> void
    {
        usercallback(pMessage);
    };
    _consumer.run();
    return NULL;
    
}

void w_net_amq_open_consumer(w_call_amq pCallback_tmp)
{
    w_thread_init(_mem_pool, &_thread, &w_thread_job_my_2, (void*)&pCallback_tmp);
    w_mem_pool_fini(&_mem_pool);
}

void w_amq_init()
{
    library::ActiveMQCPP::initializeLibrary();

}

void w_amq_fini()
{
    amq_consumer _consumer;
    amq_producer _producer;

    _consumer.close();
    _producer.close();
    activemq::library::ActiveMQCPP::shutdownLibrary();
}




