/*
	Project			 : ActiveMQ-APIs
	Source			 : Please direct any bug to https://github.com/FanapSoft/ActiveMQ-APIs/issues
	Website			 : http://FanapSoft.github.io
	Name			 : w_amq.h
	Description		 : activemq manager
	Comment          :
*/

#pragma once

#ifdef WOLF_ENABLE_ACTIVEMQ

#ifdef __cplusplus
extern "C" {
#endif

#include <wolf.h>
#include "amq_connection_type.h"

	//forward declaration
	typedef struct w_amq_object_t
	{
		void* object;
	} w_amq_object_t;
	typedef w_amq_object_t* w_amq_object;

	void	w_amq_init();

	W_RESULT	w_amq_producer_open(
		_Inout_ w_mem_pool pMemPool,
		_Inout_ w_amq_object* pAMQObject,
		_In_z_ const char* pBrokerURI,
		_In_z_ const char* pUsername,
		_In_z_ const char* pPassword,
		_In_z_ const char* pQueueOrTopicName,
		_In_ amq_connection_type pConnectionType,
		_In_ amq_delivery_mode pDeliveryMode,
		_In_ bool pSessionTransacted);

	W_RESULT	w_amq_consumer_open(
		_Inout_ w_mem_pool pMemPool,
		_Inout_ w_amq_object* pAMQObject,
		_In_z_  const char* pBrokerURI,
		_In_z_  const char* pUsername,
		_In_z_  const char* pPassword,
		_In_z_  const char* pQueueOrTopicName,
		_In_ w_amq_consumer_callback_fn pOnMessageCallBack,
		_In_ amq_connection_type pConnectionType,
		_In_ bool pSessionTransacted);

	W_RESULT	w_amq_producer_run(
		_In_ w_amq_object pAMQObject);

	W_RESULT	w_amq_consumer_run(
		_In_ w_amq_object pAMQObject);

	W_RESULT	w_amq_producer_send_message(
		_In_ w_amq_object pAMQObject,
		_In_z_  const char* pMessage,
		_In_  int pPriority);

	W_RESULT	w_amq_producer_close(
		_Inout_ w_amq_object pAMQObject);
		
	W_RESULT	w_amq_consumer_close(
		_Inout_ w_amq_object pAMQObject);

	void	w_amq_fini();

#ifdef __cplusplus
}
#endif

#endif