/*
	Project			 : ActiveMQ-APIs
	Source			 : Please direct any bug to https://github.com/FanapSoft/ActiveMQ-APIs/issues
	Website			 : http://FanapSoft.github.io
	Name			 : amq_consumer.h
	Description		 : activemq consumer class 
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_AMQ_CONSUMER_H__
#define __W_AMQ_CONSUMER_H__

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
#include <functional>

    using namespace activemq; 
    using namespace activemq::core;
    using namespace decaf::util::concurrent;
    using namespace decaf::util;
    using namespace decaf::lang;
    using namespace cms;
    using namespace std;

class amq_consumer : public cms::ExceptionListener, public MessageListener, public Runnable 
{
public:
    //consturctor
    amq_consumer(
        bool pUseQueue = true, 
        bool pSessionTransacted = false);
    //destructor
    virtual ~amq_consumer();
    //close this connection
    void close();
    //wait for ready
    void waitUntilReady();
    //run consumer
    void run();
    //listen and rised on message received
    virtual void onMessage(const Message* message);
    //registered as an ExceptionListener with the connection.
    virtual void onException(const CMSException& ex AMQCPP_UNUSED);

    std::function<void(const std::string&)> on_message_received;
private:
    //private constructors for avoid copy constructor
    amq_consumer(const amq_consumer&);
    amq_consumer& operator=(const amq_consumer&);

    std::string                 _last_error;
    Connection*                 _connection;
    Session*                    _session;
    Destination*                _destination;
    MessageConsumer*            _consumer;
    bool                        _use_queue;
    bool                        _session_transacted;
};

#endif