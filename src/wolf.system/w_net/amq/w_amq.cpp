#include "w_amq.h"

#ifdef WOLF_ENABLE_ACTIVEMQ

#include <activemq/library/ActiveMQCPP.h>
#include <w_log/w_log.h>
#include "amq_consumer.h"
#include "amq_producer.h"

void	w_amq_init()
{
	activemq::library::ActiveMQCPP::initializeLibrary();
}

W_RESULT	w_amq_producer_open(
	_Inout_ w_mem_pool pMemPool,
	_Inout_ w_amq_object* pAMQObject,
	_In_z_ const char* pBrokerURI,
	_In_z_ const char* pUsername,
	_In_z_ const char* pPassword,
	_In_z_ const char* pQueueOrTopicName,
	_In_ amq_connection_type pConnectionType,
	_In_ amq_delivery_mode pDeliveryMode,
	_In_ bool pSessionTransacted)
{
	const char* _trace_info = "w_amq_producer_open";

	if (!pMemPool)
	{
		W_ASSERT_P(false, "memory pool is invalid! trace info: %s",
			_trace_info);
		return W_BAD_ARG;
	}

	auto _obj = (w_amq_object)w_malloc(pMemPool, sizeof(w_amq_object_t));
	if (!_obj)
	{
		W_ASSERT_P(false, "could not allocate memory for w_amq_object_t! trace info: %s",
			_trace_info);
		return W_FAILURE;
	}

	auto _amq = new amq_producer(
		pBrokerURI,
		pUsername,
		pPassword,
		pQueueOrTopicName,
		pConnectionType,
		pDeliveryMode,
		pSessionTransacted);
	if (!_amq)
	{
		W_ASSERT_P(false, "could not allocate memory for amq_producer! trace info: %s",
			_trace_info);
		return W_FAILURE;
	}

	_obj->object = (void*)_amq;
	*pAMQObject = _obj;

	return W_SUCCESS;
}

W_RESULT	w_amq_consumer_open(
	_Inout_ w_mem_pool pMemPool,
	_Inout_ w_amq_object* pAMQObject,
	_In_z_  const char* pBrokerURI,
	_In_z_  const char* pUsername,
	_In_z_  const char* pPassword,
	_In_z_  const char* pQueueOrTopicName,
	_In_ amq_connection_type pConnectionType,
	_In_ bool pSessionTransacted,
	_In_ w_amq_consumer_callback_fn pOnMessageCallBack)
{
	const char* _trace_info = "w_amq_consumer_open";

	if (!pMemPool)
	{
		W_ASSERT_P(false, "memory pool is invalid! trace info: %s",
			_trace_info);
		return W_BAD_ARG;
	}

	auto _obj = (w_amq_object)w_malloc(pMemPool, sizeof(w_amq_object_t));
	if (!_obj)
	{
		W_ASSERT_P(false, "could not allocate memory for w_amq_object_t! trace info: %s",
			_trace_info);
		return W_FAILURE;
	}

	auto _amq = new amq_consumer(
		pBrokerURI,
		pUsername,
		pPassword,
		pQueueOrTopicName,
		pOnMessageCallBack,
		pConnectionType,
		pSessionTransacted);
	if (!_amq)
	{
		W_ASSERT_P(false, "could not allocate memory for amq_consumer! trace info: %s",
			_trace_info);
		return W_FAILURE;
	}

	_obj->object = (void*)_amq;
	*pAMQObject = _obj;

	return W_SUCCESS;
}

W_RESULT	w_amq_producer_run(
	_In_ w_amq_object pAMQObject)
{
	const char* _trace_info = "w_amq_producer_run";

	if (!pAMQObject)
	{
		W_ASSERT_P(false, "invalid parameter! trace info: %s",
			_trace_info);
		return W_BAD_ARG;
	}

	auto _obj = (w_amq_object)pAMQObject;
	if (!_obj)
	{
		W_ASSERT_P(false, "could not cast to w_amq_object! trace info: %s",
			_trace_info);
		return W_BAD_ARG;
	}

	auto _amq = (amq_producer*)_obj->object;
	if (!_amq)
	{
		W_ASSERT_P(false, "could not cast to amq_producer! trace info: %s",
			_trace_info);
		return W_BAD_ARG;
	}
	_amq->run();

	return W_SUCCESS;
}

W_RESULT	w_amq_consumer_run(
	_In_ w_amq_object pAMQObject)
{
	const char* _trace_info = "w_amq_consumer_run";

	if (!pAMQObject)
	{
		W_ASSERT_P(false, "invalid parameter! trace info: %s",
			_trace_info);
		return W_BAD_ARG;
	}

	auto _obj = (w_amq_object)pAMQObject;
	if (!_obj)
	{
		W_ASSERT_P(false, "could not cast to w_amq_object! trace info: %s",
			_trace_info);
		return W_BAD_ARG;
	}

	auto _amq = (amq_consumer*)_obj->object;
	if (!_amq)
	{
		W_ASSERT_P(false, "could not cast to amq_consumer! trace info: %s",
			_trace_info);
		return W_BAD_ARG;
	}
	_amq->run();

	return W_SUCCESS;
}

W_RESULT	w_amq_producer_send_message(
	_In_ w_amq_object pAMQObject,
	_In_z_  const char* pMessage,
	_In_  int pPriority)
{
	const char* _trace_info = "w_amq_producer_send_message";

	if (!pAMQObject)
	{
		W_ASSERT_P(false, "invalid parameter! trace info: %s",
			_trace_info);
		return W_BAD_ARG;
	}

	auto _obj = (w_amq_object)pAMQObject;
	if (!_obj)
	{
		W_ASSERT_P(false, "could not cast to w_amq_object! trace info: %s",
			_trace_info);
		return W_BAD_ARG;
	}

	auto _amq = (amq_producer*)_obj->object;
	if (!_amq)
	{
		W_ASSERT_P(false, "could not cast to amq_producer! trace info: %s",
			_trace_info);
		return W_BAD_ARG;
	}
	_amq->send_message(pMessage, pPriority);

	return W_SUCCESS;
}

W_RESULT	w_amq_producer_close(
	_Inout_ w_amq_object pAMQObject)
{
	const char* _trace_info = "w_amq_producer_close";
	
	if (!pAMQObject)
	{
		W_ASSERT_P(false, "invalid parameter! trace info: %s",
			_trace_info);
		return W_BAD_ARG;
	}

	auto _obj = (w_amq_object)pAMQObject;
	if (!_obj)
	{
		W_ASSERT_P(false, "could not cast to w_amq_object! trace info: %s",
			_trace_info);
		return W_BAD_ARG;
	}

	auto _producer = (amq_producer*)_obj->object;
	if (!_producer)
	{
		W_ASSERT_P(false, "could not cast to amq_producer! trace info: %s",
			_trace_info);
		return W_BAD_ARG;
	}
	_producer->close();

	return W_SUCCESS;
}

W_RESULT	w_amq_consumer_close(
	_Inout_ w_amq_object pAMQObject)
{
	const char* _trace_info = "w_amq_consumer_close";

	if (!pAMQObject)
	{
		W_ASSERT_P(false, "invalid parameter! trace info: %s",
			_trace_info);
		return W_BAD_ARG;
	}

	auto _obj = (w_amq_object)pAMQObject;
	if (!_obj)
	{
		W_ASSERT_P(false, "could not cast to w_amq_object! trace info: %s",
			_trace_info);
		return W_BAD_ARG;
	}

	auto _amq = (amq_consumer*)_obj->object;
	if (!_amq)
	{
		W_ASSERT_P(false, "could not cast to amq_producer! trace info: %s",
			_trace_info);
		return W_BAD_ARG;
	}
	_amq->close();

	return W_SUCCESS;
}

void	w_amq_fini()
{
	activemq::library::ActiveMQCPP::shutdownLibrary();
}

#endif







