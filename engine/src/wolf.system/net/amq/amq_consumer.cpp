#include "amq_consumer.h"
#include "amq_config.h"

amq_consumer::amq_consumer(
    bool pUseQueue, 
    bool pSessionTransacted):
   // _latch(1),
    _connection(nullptr),
    _session(nullptr),
    _destination(nullptr),
    _consumer(nullptr),
    _use_queue(pUseQueue),
    _session_transacted(pSessionTransacted)
{
}

amq_consumer::~amq_consumer()
{
    close();
}

void amq_consumer::waitUntilReady()
{
    //this->_latch.await();
}

void amq_consumer::run()
{
    try 
    {
        // create a connection factory
        auto _connection_factory = ConnectionFactory::createCMSConnectionFactory(BROKER_URI);
        // create a connection
        this->_connection = _connection_factory->createConnection(USER_NAME, PASS_WORD);
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
        if (this->_use_queue) 
        {
            this->_destination = this->_session->createQueue(CONSUMER_QUEUE_NAME);
        } 
        else 
        {
            this->_destination = this->_session->createTopic(CONSUMER_TOPIC_NAME);    
        }

        //create a MessageConsumer from the Session to the Topic or Queue
        this->_consumer = this->_session->createConsumer(this->_destination);
        this->_consumer->setMessageListener(this);
        // //indicate we are ready for messages.
        // this->_latch.countDown();
        // //wait while asynchronous messages come in.
        // this->_done_latch.await(this->_wait_in_millseconds);

    } 
    catch (CMSException& e) 
    {
        //indicate we are ready for messages.
        //this->_latch.countDown();
        this->_last_error = "exception occurred: " + 
            e.getStackTraceString() + 
        " .trace info: amq_consumer::async_run()";
    }
}

void amq_consumer::onMessage(const Message* pMessage)
{
    try 
    {
        const TextMessage* _text_message = dynamic_cast<const TextMessage*>(pMessage);        
        if (_text_message) 
        {
            if(on_message_received)
            {
                on_message_received(_text_message->getText());
            } 
        } 
        else 
        {
            _last_error = "text message recieved but it's NULL";
        }
    } 
    catch (CMSException& e) 
    {
        _last_error = "exception occurred: " + 
            e.getStackTraceString() + 
        " .trace info: amq_consumer::onMessage()";
    }

    //commit all messages
    if (this->_session_transacted) 
    {
        this->_session->commit();
    }

    //no matter what, tag the count down latch until done.
   // this->_done_latch.countDown();
}

void amq_consumer::onException(const CMSException& e)
{
    this->_last_error = "exception occurred: " + 
        e.getStackTraceString() + 
        " .trace info: amq_consumer::onException()";
}

void amq_consumer::close()
{
    //destroy resources
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
    catch (cms::CMSException& e) 
    {
        _last_error = "exception occurred: " + 
            e.getStackTraceString() + 
            " .trace info: amq_consumer::close()";
    }
}