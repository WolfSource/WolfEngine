/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : amq_consumer.h
    Description      : activemq consumer class
    Comment          :
                        Set the URI to point to the IP Address of your broker.
                        add any optional params to the url to enable things like
                        tightMarshalling or tcp logging etc.  See the CMS web site for
                        a full list of configuration options.
                        http://activemq.apache.org/cms/
                        Wire Format Options:
                        =========================
                        Use either stomp or openwire, the default ports are different for each
                        
                        Examples:
                           tcp://127.0.0.1:61616                      default to openwire
                           tcp://127.0.0.1:61616?wireFormat=openwire  same as above
                           tcp://127.0.0.1:61613?wireFormat=stomp     use stomp instead
                        
                        SSL:
                        =========================
                        To use SSL you need to specify the location of the trusted Root CA or the
                        certificate for the broker you want to connect to.  Using the Root CA allows
                        you to use failover with multiple servers all using certificates signed by
                        the trusted root.  If using client authentication you also need to specify
                        the location of the client Certificate.
                        
                            System::setProperty( "decaf.net.ssl.keyStore", "<path>/client.pem" );
                            System::setProperty( "decaf.net.ssl.keyStorePassword", "password" );
                            System::setProperty( "decaf.net.ssl.trustStore", "<path>/rootCA.pem" );
                        
                        The you just specify the ssl transport in the URI, for example:
                        
                            ssl://localhost:61617
                        
*/

#pragma once

#include "amq_connection_type.h"
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <decaf/lang/Runnable.h>
#include <string>

class amq_consumer :
    public cms::ExceptionListener,
    public cms::MessageListener,
    public decaf::lang::Runnable
{
public:
    //consturctor
    amq_consumer(
        std::string_view pBrokerURI,
        std::string_view pUsername,
        std::string_view pPassword,
        std::string_view pQueueOrTopicName,
        w_amq_consumer_callback_fn pOnMessageCallBack,
        amq_connection_type pConnectionType = amq_connection_type::AMQ_QUEUE,
        bool pSessionTransacted = false);

    //destructor
    virtual ~amq_consumer();

    //close this connection
    void close();

    //run consumer
    void run();

    //listen and rised on message received
    virtual void onMessage(const cms::Message* message);

    const char* get_last_error() const { return this->_last_error.c_str(); }

private:
    //private constructors for avoid copy constructor
    amq_consumer(const amq_consumer&);
    amq_consumer& operator=(const amq_consumer&);

    //registered as an ExceptionListener with the connection.
    virtual void onException(const cms::CMSException& e);

    std::string                 _last_error;
    cms::Connection* _connection;
    cms::Session* _session;
    cms::Destination* _destination;
    cms::MessageConsumer* _consumer;
    amq_connection_type         _use_queue_or_topic;
    bool                        _session_transacted;
    std::string                 _broker_uri;
    std::string                 _username;
    std::string                 _password;
    std::string                 _queue_or_topic_name;
    w_amq_consumer_callback_fn _on_message_received;
    //CountDownLatch _latch;
    //CountDownLatch _done_latch;
};
