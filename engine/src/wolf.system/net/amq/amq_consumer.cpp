#include "amq_consumer.h"

using namespace cms;

amq_consumer::amq_consumer(
    std::string_view pBrokerURI,
    std::string_view pUsername,
    std::string_view pPassword,
    std::string_view pQueueOrTopicName,
    w_amq_consumer_callback_fn pOnMessageCallBack,
    amq_connection_type pConnectionType,
    bool pSessionTransacted) :
    _connection(nullptr),
    _session(nullptr),
    _destination(nullptr),
    _consumer(nullptr),
    _use_queue_or_topic(pConnectionType),
    _session_transacted(pSessionTransacted),
    _broker_uri(pBrokerURI),
    _username(pUsername),
    _password(pPassword),
    _on_message_received(pOnMessageCallBack),
    _queue_or_topic_name(pQueueOrTopicName)
{
}

amq_consumer::~amq_consumer()
{
    close();
}

//void amq_consumer::wait_until_ready()
//{
//    this->_latch.await();
//}

void amq_consumer::run()
{
    try
    {
        // create a connection factory
        auto _connection_factory = ConnectionFactory::createCMSConnectionFactory(this->_broker_uri);
        // create a connection
        this->_connection = _connection_factory->createConnection(
            this->_username,
            this->_password);
        this->_connection->start();
        this->_connection->setExceptionListener(this);

        //ccreate a session
        if (this->_session_transacted)
        {
            this->_session = this->_connection->createSession(Session::SESSION_TRANSACTED);
        }
        else
        {
            this->_session = this->_connection->createSession(Session::AUTO_ACKNOWLEDGE);
        }

        //ccreate the destination topic or queue
        if (this->_use_queue_or_topic == amq_connection_type::AMQ_QUEUE)
        {
            this->_destination = this->_session->createQueue(this->_queue_or_topic_name);
        }
        else
        {
            this->_destination = this->_session->createTopic(this->_queue_or_topic_name);
        }

        //create a MessageConsumer from the Session to the Topic or Queue
        this->_consumer = this->_session->createConsumer(this->_destination);
        if (this->_consumer)
        {
            this->_consumer->setMessageListener(this);
        }
        else
        {
            this->_last_error = "create consumer failed";
        }

        // Indicate we are ready for messages.
        //this->_latch.countDown();

        //wait while asynchronous messages come in.
        //this->_done_latch.await(this->_wait_in_millis);
    }
    catch (const CMSException& ex)
    {
        //indicate we are ready for messages.
        //this->_latch.countDown();
        onException(ex);
    }
}

void amq_consumer::onMessage(const Message* pMessage)
{
    try
    {
        auto _text_message = dynamic_cast<const TextMessage*>(pMessage);
        if (_text_message)
        {
            if (this->_on_message_received)
            {
                auto _msg = _text_message->getText();
                this->_on_message_received(_msg.c_str());
            }
        }
        else
        {
            this->_last_error = "NULL message recieved";
        }
    }
    catch (const CMSException& ex)
    {
        onException(ex);
    }

    //commit all messages
    if (this->_session_transacted)
    {
        this->_session->commit();
    }

    //no matter what, tag the count down latch until done.
    //this->_done_latch.countDown();
}

void amq_consumer::close()
{
    //destroy resources
    try
    {
        this->_on_message_received = nullptr;

        if (this->_connection)
        {
            this->_connection->close();
            delete this->_connection;
            this->_connection = nullptr;
        }

        if (this->_destination)
        {
            delete this->_destination;
            this->_destination = nullptr;
        }

        if (this->_consumer)
        {
            delete this->_consumer;
            this->_consumer = nullptr;
        }

        if (this->_session)
        {
            delete this->_session;
            this->_session = nullptr;
        }
    }
    catch (const CMSException& ex)
    {
        onException(ex);
    }
}

void amq_consumer::onException(const CMSException& ex)
{
    this->_last_error = "exception occurred: " + ex.getMessage() +
        +" trace info: " + ex.getStackTraceString();
}