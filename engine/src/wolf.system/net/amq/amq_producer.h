/*
	Project			 : ActiveMQ-APIs
	Source			 : Please direct any bug to https://github.com/FanapSoft/ActiveMQ-APIs/issues
	Website			 : http://FanapSoft.github.io
	Name			 : amq_producer.h
	Description		 : activemq producer class 
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_AMQ_PRODUCER_H__
#define __W_AMQ_PRODUCER_H__

#include <activemq/library/ActiveMQCPP.h>
#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/lang/Integer.h>
#include <decaf/lang/Long.h>
#include <decaf/lang/System.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/util/Config.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
#include <cms/MapMessage.h>
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include <string>
#include "concurrency/w_concurrent_queue.h"
#include "concurrency/w_thread.h"
#include"memory/w_mem_pool.h"
 struct Message_ac
{
    int             priority = 0;
    std::string     msg = "";
};
class amq_producer : public decaf::lang::Runnable
{
    friend void* w_thread_job_my(w_thread arg1, void* arg2);
public:
    //consturctor
    amq_producer( 
        bool pUseQueue = true, 
        bool pSessionTransacted = false);
    //desturctor
    virtual ~amq_producer();
    //run producer in async mode 
    virtual void run();
    //send message
    void send_message(const std::string& pMessage, const int& pPriority);
    //close and release connection and all resources 
    void close();
    //get last error 
    const std::string get_last_error() const;

private:
    //prevent private copy constructor
    amq_producer(const amq_producer&);
    amq_producer& operator=(const amq_producer&);
    
    std::string                                         _last_error;
    cms::Connection*                                    _connection;
    cms::Session*                                       _session;
    cms::Destination*                                   _destination;
    cms::MessageProducer*                               _producer;
    bool                                                _use_quque;
    bool                                                _session_transacted;
    w_thread                              _thread_handle;

    
   w_concurrent_queue           _messages;
};


#endif //__W_AMQ_PRODUCER_H__