
#include "amq_producer.h"
#include "amq_config.h"

using namespace cms;

amq_producer::amq_producer(
    bool pUseQueue, 
    bool pSessionTransacted) : 
    _connection(nullptr),
    _session(nullptr),
    _destination(nullptr),
    _producer(nullptr),
    _use_quque(pUseQueue),
    _session_transacted(pSessionTransacted)
{
}

amq_producer::~amq_producer()
{
    close();
}
void* w_thread_job_my(w_thread arg1, void* arg2) {
   {
        w_mem_pool _mem_pool = NULL;
        int x = w_mem_pool_init(&_mem_pool);
        amq_producer* arg2_tmp = (amq_producer *)(arg2);
            for(;;)
            {
                //wait on signal to send message
               
                 Message_ac *_q = ( Message_ac *)w_malloc(_mem_pool, sizeof( Message_ac*));

                w_concurrent_queue_pop(arg2_tmp->_messages,(void **)&_q);

                if(!_q->msg.empty())
                {
                   // send message
                    auto _text_message = arg2_tmp->_session->createTextMessage(_q->msg);
                    if (!_text_message)
                    {
                       // could not allocate memory for text message
                        //_last_error = "could not allocate memory for TextMessage";
                    }
                    else
                    {
                        //set property
                        _text_message->setIntProperty("Integer", _q->priority);
                       // send text message
                        arg2_tmp->_producer->send(_text_message);
                       
                        //release text message
                        _text_message->clearBody();
                        delete _text_message;
                        _text_message = nullptr;
                    }
               }
            }
        };
}
void amq_producer::run()
{
    try 
    {
       // create a connection factory
        auto _connection_factory = ConnectionFactory::createCMSConnectionFactory(BROKER_URI);
       // create a connection
        this->_connection = _connection_factory->createConnection(USER_NAME, PASS_WORD);
        this->_connection->start();
            
        // create a Session
        if (this->_session_transacted) 
        {
            this->_session = this->_connection->createSession(Session::SESSION_TRANSACTED);
        } 
        else 
        {
            this->_session = this->_connection->createSession(Session::AUTO_ACKNOWLEDGE);
        }

         //create the destination (Topic or Queue)
        if (this->_use_quque) 
        {
            this->_destination = this->_session->createQueue(PRODUCER_QUEUE_NAME);
        }
        else
        {
            this->_destination = this->_session->createTopic(PRODUCER_TOPIC_NAME);
        }

         //create a MessageProducer from the Session to the Topic or Queue
        this->_producer = this->_session->createProducer(this->_destination);
        this->_producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

       
            w_mem_pool _mem_pool = NULL;
        int x = w_mem_pool_init(&_mem_pool);
        w_thread_job callback=&w_thread_job_my;
        w_thread_init(_mem_pool, &_thread_handle, callback, NULL);
        
    }
    catch (CMSException& e) 
    {
        _last_error = "exception occurred: " + 
            e.getStackTraceString() + 
            " .trace info: amq_producer::run_async()";
    }
}

void amq_producer::send_message(const std::string& pMessage, const int& pPriority)
{
    w_mem_pool _mem_pool = NULL;
    int x = w_mem_pool_init(&_mem_pool);
    Message_ac* _tmp = (Message_ac*)w_malloc(_mem_pool, sizeof(Message_ac*));
    _tmp->msg = pMessage;
    _tmp->priority = pPriority;

    w_concurrent_queue_push(_messages, (void**)&_tmp);
}

void amq_producer::close()
{    
   // destroy resources
    try 
    {
        if (this->_connection) 
        {
            this->_connection->close();
            delete this->_connection;
            this->_connection = nullptr;
        }
        if(this->_destination)
        {
            delete this->_destination;
            this->_destination = nullptr;
        }
        if(this->_producer)
        {
            delete this->_producer;
            this->_producer = nullptr;
        }
        if(this->_session)
        {
            delete this->_session;
            this->_session = nullptr;
        }
        if(this->_connection)
        {
            delete this->_connection;
            this->_connection = nullptr;
        }
    }
    catch (CMSException& e) 
    {
        _last_error = "exception occurred: " + 
            e.getStackTraceString() + 
            " .trace info: amq_producer::close()";
    }
}

const std::string amq_producer::get_last_error() const
{
    return this->_last_error;
}