#include "amq_producer.h"

using namespace std;
using namespace cms;

amq_producer::amq_producer(
    std::string_view pBrokerURI,
    std::string_view pUsername,
    std::string_view pPassword,
    std::string_view pQueueOrTopicName,
    amq_connection_type pConnectionType,
    amq_delivery_mode pDeliveryMode,
    bool pSessionTransacted) :
    _broker_uri(pBrokerURI),
    _connection(nullptr),
    _session(nullptr),
    _destination(nullptr),
    _producer(nullptr),
    _queue_or_topic_name(pQueueOrTopicName),
    _use_queue_or_topic(pConnectionType),
    _delivery_mode(pDeliveryMode),
    _session_transacted(pSessionTransacted)
{
}

amq_producer::~amq_producer()
{
    close();
}

void amq_producer::run()
{
    try
    {
        // Create a ConnectionFactory
        auto_ptr<ConnectionFactory> _connection_factory(
            ConnectionFactory::createCMSConnectionFactory(this->_broker_uri));

        //Create a Connection
        this->_connection = _connection_factory->createConnection();
        this->_connection->start();

        // Create a Session
        if (this->_session_transacted)
        {
            this->_session = this->_connection->createSession(Session::SESSION_TRANSACTED);
        }
        else
        {
            this->_session = this->_connection->createSession(Session::AUTO_ACKNOWLEDGE);
        }

        // Create the destination (Topic or Queue)
        if (this->_use_queue_or_topic == amq_connection_type::AMQ_QUEUE)
        {
            this->_destination = this->_session->createQueue(this->_queue_or_topic_name);
        }
        else
        {
            this->_destination = this->_session->createTopic(this->_queue_or_topic_name);
        }

        // Create a MessageProducer from the Session to the Topic or Queue
        this->_producer = this->_session->createProducer(this->_destination);
        if (this->_producer)
        {
            this->_producer->setDeliveryMode(static_cast<int>(this->_delivery_mode));
        }
        else
        {
            this->_last_error = "create producer failed";
        }
    }
    catch (const CMSException& ex)
    {
        onException(ex);
    }
}

void amq_producer::send_message(_In_z_ const char* pMessage, _In_ int pPriority)
{
    //make a copy from this message
    std::string _msg(pMessage);
    if (!_msg.empty())
    {
        std::auto_ptr<TextMessage> _message(this->_session->createTextMessage(_msg));
        _message->setIntProperty("Integer", pPriority);
        this->_producer->send(_message.get());
    }
}

void amq_producer::close() {

    try
    {
        if (this->_connection)
        {
            this->_connection->close();
            delete this->_connection;
            this->_connection = nullptr;
        }

        if (this->_destination)
        {
            delete this->_destination;
            this->_destination = NULL;
        }

        if (this->_producer)
        {
            delete this->_producer;
            this->_producer = NULL;
        }

        if (this->_session)
        {
            delete this->_session;
            this->_session = NULL;
        }

        if (this->_connection)
        {
            delete this->_connection;
            this->_connection = NULL;
        }
    }
    catch (const CMSException& ex)
    {
        onException(ex);
    }
}

void amq_producer::onException(const CMSException& ex)
{
    this->_last_error = "exception occurred: " + ex.getMessage() +
        +" trace info: " + ex.getStackTraceString();
}