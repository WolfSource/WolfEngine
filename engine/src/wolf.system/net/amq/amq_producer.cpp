
#include "amq_producer.h"
#include "amq_config.h"
#include"w_log.h"
using namespace cms;
Message_ac _tmp;

#define SLEEP_THREAD        500

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
void* w_thread_job_my(w_thread arg1, void* arg2) 
   {
    auto _arg2_tmp = (amq_producer*)(arg2);
    _arg2_tmp->Pflag = 1;
        w_mem_pool _mem_pool = NULL;
         w_mem_pool_init(&_mem_pool);
         const char* _trace_info = "w_thread_job_my";
        if (!_mem_pool)
        {
            W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
            return NULL;
        }
        
            for(;;)
            {
                //wait on signal to send message
               
                 Message_ac *_q = ( Message_ac *)w_malloc(_arg2_tmp->_mem_producer, sizeof( Message_ac*));
                 if (!w_concurrent_queue_size(_arg2_tmp->_messages))
                 {
                     w_thread_current_sleep_for_milliseconds(SLEEP_THREAD);
                     continue;
                 }
                w_concurrent_queue_pop(_arg2_tmp->_messages,(void **)&_q);
                _arg2_tmp->Pflag = 1;

                if(!_q->msg.empty())
                {
                   // send message
                    auto _text_message = _arg2_tmp->_session->createTextMessage(_q->msg);
                    if (!_text_message)
                    {
                       // could not allocate memory for text message
                        _arg2_tmp->_last_error = "could not allocate memory for TextMessage";
                    }
                    else
                    {
                        //set property
                        _text_message->setIntProperty("Integer", _q->priority);
                       // send text message
                        _arg2_tmp->_producer->send(_text_message);
                       
                        //release text message
                        _text_message->clearBody();
                        delete _text_message;
                        _text_message = nullptr;
                    }
               }
            }
          
    }
   
int amq_producer::init_producer(w_mem_pool _mem_pool)
{
   
    const char* _trace_info = "amq_producer::init_pool()";
    if (!_mem_pool)
    {
        W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
        return W_FAILURE;
    }
    _mem_producer = _mem_pool;
    return W_SUCCESS;
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

       
      
          W_RESULT _c_queue_init = w_concurrent_queue_init(_mem_producer, &_messages, 10);

          w_thread_job callback = &w_thread_job_my;
          w_thread_init(_mem_producer, &_thread_handle, callback, this);
          while (!Pflag)
          {
              w_thread_current_sleep_for_milliseconds(SLEEP_THREAD);
          }
         

      
    }
    catch (CMSException& e) 
    {
        _last_error = "exception occurred: " + 
            e.getStackTraceString() + 
            " .trace info: amq_producer::run_async()";
    }
}

int amq_producer::send_message(  const std::string& pMessage, const int& pPriority)
{
   
    _tmp.priority = pPriority;
    _tmp.msg.append(pMessage);
    Pflag = 0;
    W_RESULT _ret=w_concurrent_queue_push(_messages, (void**)&_tmp);
    while (!Pflag)
    {
        w_thread_current_sleep_for_milliseconds(SLEEP_THREAD);
    }
    return W_SUCCESS;
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