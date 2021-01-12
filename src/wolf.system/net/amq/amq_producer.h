/*
    Project			 : ActiveMQ-APIs
    Source			 : Please direct any bug to https://github.com/FanapSoft/ActiveMQ-APIs/issues
    Website			 : http://FanapSoft.github.io
    Name			 : amq_producer.h
    Description		 : activemq producer class
    Comment          :
*/

#pragma once

#ifdef WOLF_ENABLE_ACTIVEMQ

#include <wolf.h>
#include "amq_connection_type.h"
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <decaf/lang/Runnable.h>
#include <string>

class amq_producer : public decaf::lang::Runnable
{
public:
    //consturctor
    amq_producer(
        std::string_view pBrokerURI,
        std::string_view pUsername,
        std::string_view pPassword,
        std::string_view pQueueOrTopicName,
        amq_connection_type pConnectionType = amq_connection_type::AMQ_QUEUE,
        amq_delivery_mode pDeliveryMode = amq_delivery_mode::AMQ_NON_PERSISTENT,
        bool pSessionTransacted = false);

    //destructor
    virtual ~amq_producer();

    //run the producer
    virtual void run();

    void send_message(_In_z_ const char* pMessage, _In_ int pPriority);

    //close all
    void close();

    const char* get_last_error() const { return this->_last_error.c_str(); }

private:
    //private constructors for avoid copy constructor
    amq_producer(const amq_producer&);
    amq_producer& operator=(const amq_producer&);

    //registered as an ExceptionListener with the connection.
    virtual void onException(const cms::CMSException& e);

    std::string                 _last_error;
    cms::Connection* _connection;
    cms::Session* _session;
    cms::Destination* _destination;
    cms::MessageProducer* _producer;
    std::string                 _broker_uri;
    std::string                 _queue_or_topic_name;
    amq_connection_type         _use_queue_or_topic;
    amq_delivery_mode           _delivery_mode;
    bool                        _session_transacted;
};

#endif

