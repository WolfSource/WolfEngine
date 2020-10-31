
#include"w_cassandra.h"
#include<cassandra.h>
#include <log/w_log.h>



/**
     * Describes the version of the connected Cassandra cluster.
     *
     * @struct CassVersion
     */

w_cass_exec_profile* w_cassandra_execution_profile_new()
    {
    return cass_execution_profile_new();
    }

   void w_cassandra_execution_profile_free(_In_ w_cass_exec_profile* pProfile)
   {
       const char* _trace_info = "w_cassandra_execution_profile_free";
       if (!pProfile)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           
       }
    
    cass_execution_profile_free(pProfile);


   }

   w_Cass_Error w_cassandra_execution_profile_set_request_timeout(_In_ w_cass_exec_profile* pProfile, _In_  uint64_t pTimeoutMs)
   {
       const char* _trace_info = "w_cassandra_execution_profile_set_request_timeout";
    if (!pProfile || !pTimeoutMs)
    {
        W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
        return -1;
    }
   return cass_execution_profile_set_request_timeout(pProfile, pTimeoutMs);

   }

   w_Cass_Error w_cassandra_execution_profile_set_consistency(w_cass_exec_profile* pProfile, w_cass_consistency pConsistency)
   {
       const char* _trace_info = "w_cassandra_execution_profile_set_consistency";

    if (!pProfile || !pConsistency)
    {
        W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
        return -1;
    }
    
       return cass_execution_profile_set_consistency(pProfile, pConsistency);
   }

   w_Cass_Error w_cassandra_execution_profile_set_serial_consistency(w_cass_exec_profile* pProfile, w_cass_consistency pPserialConsistency)
   {
       const char* _trace_info = "w_cassandra_execution_profile_set_serial_consistency";

    if (!pProfile || !pPserialConsistency)
    {
        W_ASSERT_P(false, "bad args! trace info:%s",);
        return -1;
    }
   return cass_execution_profile_set_serial_consistency(pProfile, pPserialConsistency);
   }

   w_Cass_Error  w_cassandra_execution_profile_set_load_balance_round_robin(_In_ w_cass_exec_profile* pProfile)
   {
       const char* _trace_info = "w_cassandra_execution_profile_set_load_balance_round_robin";

    if (!pProfile)
    {
        W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
        return -1;
    }
   return cass_execution_profile_set_load_balance_round_robin(pProfile);
   }

   w_Cass_Error w_cassandra_execution_profile_set_load_balance_dc_aware(_In_ w_cass_exec_profile* pProfile, _In_ const char* pLocalDc, _In_ unsigned pUsedHostsPerRemoteDc, _In_ w_bool_t pAllowRemoteCsForLocalCl)
   {
       const char* _trace_info = "w_cassandra_execution_profile_set_load_balance_dc_aware";

    if (!pProfile || !pLocalDc || !pUsedHostsPerRemoteDc)
    {
        W_ASSERT_P(false, "bad args! trace info: %s", _trace_info);

        return -1;
    }

     return cass_execution_profile_set_load_balance_dc_aware(pProfile, pLocalDc, pUsedHostsPerRemoteDc, pAllowRemoteCsForLocalCl);
   }

   w_Cass_Error w_cassandra_execution_profile_set_load_balance_dc_aware_n(_In_ w_cass_exec_profile* pProfile, const char* pLocalDc, _In_ size_t pLocalDcLength, _In_ unsigned pUsedHostsPerRemoteDc, _In_ w_bool_t pAllowRemoteDcsForLocalCl)
   {
       const char* _trace_info = "w_cassandra_execution_profile_set_load_balance_dc_aware_n";

    if (!pProfile || !pLocalDc || !pUsedHostsPerRemoteDc)
    {
        W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
        return -1;
    }
    return cass_execution_profile_set_load_balance_dc_aware_n(pProfile, pLocalDc, pLocalDcLength, pUsedHostsPerRemoteDc, pAllowRemoteDcsForLocalCl);
   }

   w_Cass_Error w_cassandra_execution_profile_set_token_aware_routing(_In_ w_cass_exec_profile* pProfile, _In_ w_bool_t pEnabled)
   {
       const char* _trace_info = "w_cassandra_execution_profile_set_token_aware_routing";

    if (!pProfile)
    {
        W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
        return -1;
    }
   
       return cass_execution_profile_set_token_aware_routing(pProfile, pEnabled);
   }

   w_Cass_Error w_cassandra_execution_profile_set_token_aware_routing_shuffle_replicas(_In_ w_cass_exec_profile* pProfile, _In_ w_bool_t pEnabled)

   {
       const char* _trace_info = "w_cassandra_execution_profile_set_token_aware_routing_shuffle_replicas";

    if (!pProfile)
    {
        W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
        return -1;
    }
    
    return cass_execution_profile_set_token_aware_routing_shuffle_replicas(pProfile, pEnabled);
   }

   w_Cass_Error w_cassandra_execution_profile_set_latency_aware_routing(_In_ w_cass_exec_profile* pProfile, _In_ w_bool_t pEnabled)
   {
       const char* _trace_info = "w_cassandra_execution_profile_set_latency_aware_routing";

    if (!pProfile)
    {
        W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
        return -1;
    }
      return   cass_execution_profile_set_latency_aware_routing(pProfile, pEnabled);
    }

    w_Cass_Error w_cassandra_execution_profile_set_latency_aware_routing_settings(_In_ w_cass_exec_profile* pProfile, double pExclusionThreshold, _In_ uint64_t pScaleMs, _In_ uint64_t pRetryPeriodMs, _In_ uint64_t pUpdateRateMs, _In_ uint64_t pMinMeasured)
    {
        const char* _trace_info = "w_cassandra_execution_profile_set_latency_aware_routing_settings";

     if (!pProfile || !pExclusionThreshold || !pScaleMs || !pRetryPeriodMs || !pUpdateRateMs || !pMinMeasured)
     {
         W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
         return -1;
      }
      return    cass_execution_profile_set_latency_aware_routing_settings(pProfile, pExclusionThreshold, pScaleMs, pRetryPeriodMs, pUpdateRateMs, pMinMeasured);
    }

   w_Cass_Error w_cassandra_execution_profile_set_whitelist_filtering(_In_ w_cass_exec_profile* pProfile, _In_ const char* pHosts)
   {
       const char* _trace_info = "w_cassandra_execution_profile_set_whitelist_filtering";

    if (!pProfile || !pHosts)
    {
        W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
        return -1;
    }
    
      return  cass_execution_profile_set_whitelist_filtering(pProfile, pHosts);
    }

    w_Cass_Error w_cassandra_execution_profile_set_whitelist_filtering_n(_In_ w_cass_exec_profile* pProfile, _In_ const char* pHosts, _In_ size_t pHostsLength)
    {
        const char* _trace_info = "w_cassandra_execution_profile_set_whitelist_filtering_n";
      if (!pProfile || !pHosts || !pHostsLength)
      {
          W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
          return -1;
      }
    return cass_execution_profile_set_whitelist_filtering_n(pProfile, pHosts, pHostsLength);
    }

    w_Cass_Error w_cassandra_execution_profile_set_blacklist_filtering(_In_ w_cass_exec_profile* pProfile,
        _In_ const char* pHosts)
    {
        const char* _trace_info = "w_cassandra_execution_profile_set_blacklist_filtering";

     if (!pProfile || !pHosts)
     {
         W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
         return -1;
     }
    
     return   cass_execution_profile_set_blacklist_filtering(pProfile, pHosts);

    }

    w_Cass_Error w_cassandra_execution_profile_set_blacklist_filtering_n(_In_ w_cass_exec_profile* pProfile, _In_ const char* pHosts, _In_ size_t pHostsLength)
    {
        const char* _trace_info = "w_cassandra_execution_profile_set_blacklist_filtering_n";
      if (!pProfile || !pHosts || !pHostsLength)
      {
          W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
          return -1;
      }
     return   cass_execution_profile_set_blacklist_filtering_n(pProfile, pHosts,
         pHostsLength);
    }

    w_Cass_Error w_cassandra_execution_profile_set_whitelist_dc_filtering(_In_ w_cass_exec_profile* pProfile, _In_  const char* pDcs)
    {
        const char* _trace_info = "w_cassandra_execution_profile_set_whitelist_dc_filtering";
      if (!pProfile || !pDcs)
      {
          W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
          return -1;
      }
    
      return  cass_execution_profile_set_whitelist_dc_filtering(pProfile, pDcs);
    }

    w_Cass_Error w_cassandra_execution_profile_set_whitelist_dc_filtering_n(_In_ w_cass_exec_profile* pProfile, _In_ const char* pDcs, _In_ size_t pDcsLength)
    {
        const char* _trace_info = "w_cassandra_execution_profile_set_whitelist_dc_filtering_n";
     if (!pProfile || !pDcs || !pDcsLength)
     {
         W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
         return -1;
     }
   
     return   cass_execution_profile_set_whitelist_dc_filtering_n(pProfile,
         pDcs, pDcsLength);
    }

    w_Cass_Error w_cassandra_execution_profile_set_blacklist_dc_filtering(_In_ w_cass_exec_profile* pProfile, _In_ const char* pDcs)
    {
        const char* _trace_info = "w_cassandra_execution_profile_set_blacklist_dc_filtering";
       if (!pProfile || !pDcs)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           return -1;
       }
        return cass_execution_profile_set_blacklist_dc_filtering(pProfile, pDcs);
    }

    w_Cass_Error  w_cassandra_execution_profile_set_blacklist_dc_filtering_n(_In_ w_cass_exec_profile* pProfile, _In_  const char* pDcs, _In_   size_t pDcsLength)
    {
        const char* _trace_info = "w_cassandra_execution_profile_set_blacklist_dc_filtering_n";
     if (!pProfile || !pDcs || !pDcsLength)
     {
         W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
         return -1;
     }
     
        return cass_execution_profile_set_blacklist_dc_filtering_n(pProfile, pDcs, pDcsLength);
    }

    w_Cass_Error w_cassandra_execution_profile_set_retry_policy(_In_ w_cass_exec_profile* pProfile, _In_  w_cass_retry_policy * pRetryPolicy)
    {
        const char* _trace_info = "w_cassandra_execution_profile_set_retry_policy";
     if (!pProfile || !pRetryPolicy)
     {
         W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
         return -1;
     }
        return cass_execution_profile_set_retry_policy(pProfile, pRetryPolicy);

    }

    w_Cass_Error w_cassandra_execution_profile_set_constant_speculative_execution_policy(_In_ w_cass_exec_profile* pProfile, _In_  int64_t pConstantDelayMs, _In_  int pMaxSpeculativeExecutions)
    {
        const char* _trace_info = "w_cassandra_execution_profile_set_constant_speculative_execution_policy";
      if (!pProfile || !pConstantDelayMs || !pMaxSpeculativeExecutions)
      {
          W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
          return -1;
      }
     
        return cass_execution_profile_set_constant_speculative_execution_policy(pProfile,
            pConstantDelayMs,
            pMaxSpeculativeExecutions);
    }

    w_Cass_Error w_cassandra_execution_profile_set_no_speculative_execution_policy(_In_ w_cass_exec_profile* pProfile)
    {
        const char* _trace_info = "w_cassandra_execution_profile_set_no_speculative_execution_policy";

     if (!pProfile)
     {
         W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
         return -1;
     }
     
         return cass_execution_profile_set_no_speculative_execution_policy(pProfile);
    }

   w_cass_cluster*w_cassandra_cluster_new()
   {

    return  cass_cluster_new();

   }

   void w_cassandra_cluster_free(_In_  w_cass_cluster* pCluster)
   {
       const char* _trace_info = "w_cassandra_cluster_free";
     if (!pCluster)
     {
         W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

     }
    cass_cluster_free(pCluster);

   }

   w_Cass_Error  w_cassandra_cluster_set_contact_points(_In_ w_cass_cluster* pCluster, _In_ const char* pContactPoints)
   {
       const char* _trace_info = "w_cassandra_cluster_set_contact_points";
     if (!pCluster || !pContactPoints)
     {
         W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
         return -1;
     }
     return cass_cluster_set_contact_points( pCluster, pContactPoints);
   }

   w_Cass_Error  w_cassandra_cluster_set_contact_points_n(_In_ w_cass_cluster* pCluster, _In_  const char* pContactPoints, _In_  size_t pContactPointsLength)
   {
       const char* _trace_info = "w_cassandra_cluster_set_contact_points_n";
     if (!pCluster || !pContactPoints || !pContactPointsLength)
     {
         W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
         return -1;
     }
     
     return cass_cluster_set_contact_points_n(pCluster, pContactPoints, pContactPointsLength);
     
   }

   w_Cass_Error w_cassandra_cluster_set_port(_In_ w_cass_cluster* pCluster, _In_  int pPort)
   {
       const char* _trace_info = "w_cassandra_cluster_set_port";

     if (!pCluster || !pPort)
     {
         W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
         return -1;
     }

     return 
         cass_cluster_set_port(pCluster, pPort);
   }

   w_Cass_Error  w_cassandra_cluster_set_local_address(_In_ w_cass_cluster* pCluster, _In_ const char* pName)
   {
       const char* _trace_info = "w_cassandra_cluster_set_local_address";
      if (!pCluster || !pName)
      {
          W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
          return -1;
      }
       
         return cass_cluster_set_local_address( pCluster, pName);
   }

   w_Cass_Error w_cassandra_cluster_set_local_address_n(_In_ w_cass_cluster* pCluster, _In_ const char* pName, _In_ size_t pNameLength)
   {
       const char* _trace_info = "w_cassandra_cluster_set_local_address_n";
      if (!pCluster || !pNameLength || !pName)
      {
          W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
          return -1;
      }
      
        return  cass_cluster_set_local_address_n( pCluster, pName, pNameLength);
   }


   void w_cassandra_cluster_set_ssl(_In_ w_cass_cluster* pCluster, _In_ w_cass_ssl * pSsl)
   {
       const char* _trace_info = "w_cassandra_cluster_set_ssl";
      if (!pCluster || !pSsl)
      {
          W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
      }
       cass_cluster_set_ssl( pCluster, pSsl);
   }

   w_Cass_Error w_cassandra_cluster_set_authenticator_callbacks(_In_ w_cass_cluster* pCluster, _In_ const w_cass_authenticator_callbacks pExchangeCallbacks, _In_ w_cass_authenticator_data_cleanup_callback pCleanupCallback, _In_  void* pData)
   {
       const char* _trace_info = "w_cassandra_cluster_set_authenticator_callbacks";
      if (!pCluster )
      {
          W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
          return -1;

      }
      return  cass_cluster_set_authenticator_callbacks(pCluster, pExchangeCallbacks, pCleanupCallback, pData);
   }

   w_Cass_Error w_cassandra_cluster_set_protocol_version(_In_ w_cass_cluster* pCluster, _In_ int pProtocolVersion)
   {
       const char* _trace_info = "w_cassandra_cluster_set_protocol_version";
      if (!pCluster ||  !pProtocolVersion)
      {
          W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

          return -1;

      }
      return cass_cluster_set_protocol_version( pCluster, pProtocolVersion);
   }


   w_Cass_Error  w_cassandra_cluster_set_use_beta_protocol_version(_In_ w_cass_cluster* pCluster, _In_  w_bool_t pEnable)
   {
       const char* _trace_info = "w_cassandra_cluster_set_use_beta_protocol_version";
      if (!pCluster )
      {
          W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
          return -1;

      }
      return     cass_cluster_set_use_beta_protocol_version( pCluster, pEnable);

   }


   w_Cass_Error  w_cassandra_cluster_set_consistency(_In_ w_cass_cluster* pCluster, _In_  w_cass_consistency pConsistency)
   {
       const char* _trace_info = "w_cassandra_cluster_set_consistency";
      if (!pCluster || !pConsistency)
      {
          W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
          return -1;

      }
      
         return  cass_cluster_set_consistency(pCluster, pConsistency);
   }

   w_Cass_Error w_cassandra_cluster_set_serial_consistency(_In_ w_cass_cluster* pCluster, _In_ w_cass_consistency pConsistency)
   {
       const char* _trace_info = "w_cassandra_cluster_set_serial_consistency";
      if (!pCluster || !pConsistency)
      {
          W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
          return -1;

      }

       return cass_cluster_set_serial_consistency(pCluster, pConsistency);

   }

   w_Cass_Error w_cassandra_cluster_set_num_threads_io(_In_ w_cass_cluster* pCluster, _In_ unsigned pNumThreads)
   {
       const char* _trace_info = "w_cassandra_cluster_set_num_threads_io";

       if (!pCluster || !pNumThreads)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           return -1;

       }
       return cass_cluster_set_num_threads_io(pCluster, pNumThreads);
   }


   w_Cass_Error w_cassandra_cluster_set_queue_size_io(_In_ w_cass_cluster* pCluster, _In_ unsigned pQueueSize)
   {
       const char* _trace_info = "w_cassandra_cluster_set_queue_size_io";
       if (!pCluster || !pQueueSize)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           return -1;

       }

     return  cass_cluster_set_queue_size_io(pCluster, pQueueSize);
   }

   /*deprecated*/
   w_Cass_Error w_cassandra_cluster_set_queue_size_event(_In_ w_cass_cluster* pCluster, _In_ unsigned pQueueSize)
   {
       const char* _trace_info = "w_cassandra_cluster_set_queue_size_event";
       if (!pCluster || !pQueueSize)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           return -1;

       }
       return cass_cluster_set_queue_size_event(pCluster, pQueueSize);
   }

   w_Cass_Error w_cassandra_cluster_set_core_connections_per_host(_In_ w_cass_cluster* pCluster, _In_  unsigned pNumConnections)
   {
       const char* _trace_info = "w_cassandra_cluster_set_core_connections_per_host";
       if (!pCluster || !pNumConnections)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           return -1;

       }
           return cass_cluster_set_core_connections_per_host( pCluster, pNumConnections);
   }

   /*deprecated*/
   w_Cass_Error w_cassandra_cluster_set_max_connections_per_host(_In_ w_cass_cluster* pCluster, _In_ unsigned pNumConnections)
   {
       const char* _trace_info = "w_cassandra_cluster_set_max_connections_per_host";
       if (!pCluster || !pNumConnections)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           return -1;

       }
       return cass_cluster_set_max_connections_per_host(pCluster, pNumConnections);
   }
   
   /*deprecated*/
   void w_cassandra_cluster_set_reconnect_wait_time(_In_ w_cass_cluster* pCluster, _In_ unsigned pWaitTime)
   {
       const char* _trace_info = "w_cassandra_cluster_set_max_connections_per_host";
       if (!pCluster || !pWaitTime)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           

       }
       cass_cluster_set_reconnect_wait_time(pCluster, pWaitTime);
   }

   void w_cassandra_cluster_set_constant_reconnect(_In_  w_cass_cluster* pCluster, _In_  uint64_t pDelayMs)
   {
       const char* _trace_info = "w_cassandra_cluster_set_constant_reconnect";
       if (!pCluster || !pDelayMs)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

       }

       
           cass_cluster_set_constant_reconnect( pCluster, pDelayMs);
   }

   w_Cass_Error  w_cassandra_cluster_set_exponential_reconnect(_In_ w_cass_cluster* pCluster, _In_ uint64_t pBaseDelayMs, _In_ uint64_t pMaxDelayMs)
   {
       const char* _trace_info = "w_cassandra_cluster_set_exponential_reconnect";
       if (!pCluster || !pBaseDelayMs || !pMaxDelayMs)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           return -1;
       }
      return  cass_cluster_set_exponential_reconnect(pCluster, pBaseDelayMs, pMaxDelayMs);
   }


   w_Cass_Error  w_cassandra_cluster_set_coalesce_delay(_In_ w_cass_cluster* pCluster, _In_ int64_t pDelayUs)
   {
       const char* _trace_info = "w_cassandra_cluster_set_coalesce_delay";
       if (!pCluster || !pDelayUs)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           return -1;
       }
         return   cass_cluster_set_coalesce_delay( pCluster, pDelayUs);
   }

   w_Cass_Error w_cassandra_cluster_set_new_request_ratio(_In_ w_cass_cluster* pCluster, _In_ int32_t pRatio)
   {
       const char* _trace_info = "w_cassandra_cluster_set_new_request_ratio";
       if (!pCluster || !pRatio)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           return -1;
       }
       return cass_cluster_set_new_request_ratio( pCluster, pRatio);

   }
 
   /*deprecated*/
   w_Cass_Error  w_cassandra_cluster_set_max_concurrent_creation(_In_ w_cass_cluster* pCluster, _In_ unsigned pNumConnections)
   {
       const char* _trace_info = "w_cassandra_cluster_set_max_concurrent_creation";
       if (!pCluster)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info); 
           return -1;
       }
       return cass_cluster_set_max_concurrent_creation(pCluster, pNumConnections);

   }

   /*deprecated*/
   w_Cass_Error  w_cassandra_cluster_set_max_concurrent_requests_threshold(_In_ w_cass_cluster* pCluster,  _In_  unsigned pNumConnections)
   {
       const char* _trace_info = "w_cassandra_cluster_set_max_concurrent_requests_threshold";
       if (!pCluster)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           return -1;
       }
       return cass_cluster_set_max_concurrent_requests_threshold(pCluster, pNumConnections);
   }

   /*deprecated*/
   w_Cass_Error  w_cassandra_cluster_set_max_requests_per_flush(_In_ w_cass_cluster* pCluster,_In_ unsigned pNumRequests)
   {
       const char* _trace_info = "w_cassandra_cluster_set_max_requests_per_flush";
       if (!pCluster)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           return -1;
       }
       return cass_cluster_set_max_requests_per_flush(pCluster, pNumRequests);
   }

   /*deprecated*/
   w_Cass_Error  w_cassandra_cluster_set_write_bytes_high_water_mark(_In_ w_cass_cluster* pCluster,  _In_  unsigned pNumBytes)
   {
       const char* _trace_info = "w_cassandra_cluster_set_write_bytes_high_water_mark";

       if (!pCluster)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           return -1;
       }
       return cass_cluster_set_write_bytes_high_water_mark(pCluster, pNumBytes);
   }

   /*deprecated*/
   w_Cass_Error w_cassandra_cluster_set_write_bytes_low_water_mark(_In_ w_cass_cluster* pCluster ,_In_ unsigned pNumBytes)
   {
       const char* _trace_info = "w_cassandra_cluster_set_write_bytes_low_water_mark";
       if (!pCluster)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           return -1;
       }
       return cass_cluster_set_write_bytes_low_water_mark(pCluster, pNumBytes);

   }

   /*deprecated*/
   w_Cass_Error   w_cassandra_cluster_set_pending_requests_high_water_mark(_In_ w_cass_cluster* pCluster,  _In_   unsigned pNumRequests)
   {
       const char* _trace_info = "w_cassandra_cluster_set_pending_requests_high_water_mark";
       if (!pCluster)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           return -1;
       }
       return cass_cluster_set_pending_requests_high_water_mark(pCluster, pNumRequests);

   }

   /*deprecated*/
   w_Cass_Error  w_cassandra_cluster_set_pending_requests_low_water_mark(_In_ w_cass_cluster* pCluster,  _In_  unsigned pNumRequests)
   {
       const char* _trace_info = "w_cassandra_cluster_set_pending_requests_low_water_mark";
       if (!pCluster)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           return -1;
       }
       return cass_cluster_set_pending_requests_low_water_mark(pCluster, pNumRequests);

   }
   void w_cassandra_cluster_set_connect_timeout(_In_ w_cass_cluster* pCluster, _In_  unsigned pTimeoutMs)
   {
       const char* _trace_info = "w_cassandra_cluster_set_connect_timeout";
       if (!pCluster )
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

       }
      cass_cluster_set_connect_timeout(pCluster, pTimeoutMs);
   }


   void w_cassandra_cluster_set_request_timeout(_In_ w_cass_cluster* pCluster, _In_ unsigned pTimeoutMs)
   {
       const char* _trace_info = "w_cassandra_cluster_set_request_timeout";
       if (!pCluster )
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
       }
           cass_cluster_set_request_timeout(pCluster, pTimeoutMs);
   }

   void w_cassandra_cluster_set_resolve_timeout(_In_ w_cass_cluster* pCluster, _In_ unsigned pTimeoutMs)
   {
       const char* _trace_info = "w_cassandra_cluster_set_resolve_timeout";
       if (!pCluster )
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
       }
       cass_cluster_set_resolve_timeout( pCluster, pTimeoutMs);

   }


   void  w_cassandra_cluster_set_max_schema_wait_time(_In_ w_cass_cluster* pCluster, _In_ unsigned pWaitTimeMs)
   {
       const char* _trace_info = "w_cassandra_cluster_set_max_schema_wait_time";
       if (!pCluster )
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
       }

      cass_cluster_set_max_schema_wait_time(pCluster, pWaitTimeMs);
   }

   void w_cassandra_cluster_set_tracing_max_wait_time(_In_ w_cass_cluster* pCluster, _In_ unsigned pMaxWaitTimeMs)
   {
       const char* _trace_info = "w_cassandra_cluster_set_tracing_max_wait_time";
       if (!pCluster )
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
       }

     cass_cluster_set_tracing_max_wait_time( pCluster, pMaxWaitTimeMs);
   }

   void w_cassandra_cluster_set_tracing_retry_wait_time(_In_ w_cass_cluster* pCluster, _In_ unsigned pRetryWaitTimeMs)
   {
       const char* _trace_info = "w_cassandra_cluster_set_tracing_retry_wait_time";
       if (!pCluster )
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
       }
       cass_cluster_set_tracing_retry_wait_time( pCluster, pRetryWaitTimeMs);
   }

   void w_cassandra_cluster_set_tracing_consistency(_In_ w_cass_cluster* pCluster, _In_  w_cass_consistency pConsistency)
   {
       const char* _trace_info = "w_cassandra_cluster_set_tracing_consistency";
       if (!pCluster)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
       }
       cass_cluster_set_tracing_consistency( pCluster, pConsistency);

   }



   void w_cassandra_cluster_set_credentials(_In_ w_cass_cluster* pCluster, _In_ const char* pUsername, _In_ const char* pPassword)
   {
       const char* _trace_info = "w_cassandra_cluster_set_credentials";
       if (!pCluster || !pUsername  || !pPassword)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
       }

      cass_cluster_set_credentials( pCluster, pUsername, pPassword);
   }

   void w_cassandra_cluster_set_credentials_n(_In_ w_cass_cluster* pCluster, _In_ const char* pUsername, _In_ size_t pUsernameLength, _In_ const char* pPassword, _In_  size_t pPasswordLength)
   {
       const char* _trace_info = "w_cassandra_cluster_set_credentials_n";
       if (!pCluster || !pUsername ||  !pUsernameLength || !pPassword|| !pPasswordLength)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
       }
     cass_cluster_set_credentials_n( pCluster, pUsername, pUsernameLength, pPassword, pPasswordLength);

   }

   void w_cassandra_cluster_set_load_balance_round_robin(_In_ w_cass_cluster* pCluster)
   {
       const char* _trace_info = "w_cassandra_cluster_set_load_balance_round_robin";
       if (!pCluster)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
       }
       cass_cluster_set_load_balance_round_robin( pCluster);
   }

   w_Cass_Error w_cassandra_cluster_set_load_balance_dc_aware(_In_ w_cass_cluster* pCluster, _In_ const char* pLocalDc, _In_  unsigned pUsedHostsPerRemoteDc, _In_ w_bool_t pAllowRemoteDcsForLocalCl)
   {
       const char* _trace_info = "w_cassandra_cluster_set_load_balance_dc_aware";
       if (!pCluster || !pLocalDc || !pUsedHostsPerRemoteDc)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           return -1;
       }
       
         return cass_cluster_set_load_balance_dc_aware(pCluster, pLocalDc, pUsedHostsPerRemoteDc, pAllowRemoteDcsForLocalCl);
   }

  
   w_Cass_Error   w_cassandra_cluster_set_load_balance_dc_aware_n(_In_ w_cass_cluster* pCluster, _In_ const char* pLocalDc, _In_ size_t pLocalDcLength, _In_  unsigned pUsedHostsPerRemoteDc, _In_  w_bool_t pAllowRemoteDcsForLocalCl)
   {
       const char* _trace_info = "w_cassandra_cluster_set_load_balance_dc_aware_n";
       if (!pCluster || !pLocalDc || !pLocalDcLength || !pUsedHostsPerRemoteDc)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           return -1;

       }

            return   cass_cluster_set_load_balance_dc_aware_n( pCluster, pLocalDc, pLocalDcLength, pUsedHostsPerRemoteDc, pAllowRemoteDcsForLocalCl);
   }

   void w_cassandra_cluster_set_token_aware_routing(_In_ w_cass_cluster* pCluster, _In_ w_bool_t pEnabled)
   {
       const char* _trace_info = "w_cassandra_cluster_set_token_aware_routing";
       if (!pCluster)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

       }
           cass_cluster_set_token_aware_routing(pCluster, pEnabled);

   }

   void w_cassandra_cluster_set_token_aware_routing_shuffle_replicas(_In_ w_cass_cluster* pCluster, _In_  w_bool_t pEnabled)
   {
       const char* _trace_info = "w_cassandra_cluster_set_token_aware_routing_shuffle_replicas";
       if (!pCluster)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
       }

           
       cass_cluster_set_token_aware_routing_shuffle_replicas( pCluster, pEnabled);

   }
   void  w_cassandra_cluster_set_latency_aware_routing(_In_ w_cass_cluster* pCluster, _In_  w_bool_t pEnabled)
   {
       const char* _trace_info = "w_cassandra_cluster_set_latency_aware_routing";
     if (!pCluster)
     {
         W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

     }
       cass_cluster_set_latency_aware_routing( pCluster, pEnabled);
   }

    void  w_cassandra_cluster_set_latency_aware_routing_settings(_In_ w_cass_cluster* pCluster, _In_  double pExclusionThreshold, _In_  uint64_t pScaleMs, _In_ uint64_t pRetryPeriodMs, _In_ uint64_t pUpdateRateMs, _In_ uint64_t pMinMeasured)
    {
        const char* _trace_info = "w_cassandra_cluster_set_latency_aware_routing_settings";

        if (!pCluster || !pExclusionThreshold || !pScaleMs || !pRetryPeriodMs|| !pUpdateRateMs || !pMinMeasured)
        {
            W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
        }

          cass_cluster_set_latency_aware_routing_settings( pCluster, pExclusionThreshold, pScaleMs, pRetryPeriodMs, pUpdateRateMs, pMinMeasured);
    }

    void w_cassandra_cluster_set_whitelist_filtering(_In_ w_cass_cluster* pCluster, _In_ const char* pHosts)
    {
        const char* _trace_info = "w_cassandra_cluster_set_whitelist_filtering";
        if (!pCluster || !pHosts)
        {
            W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

        }
           
         cass_cluster_set_whitelist_filtering( pCluster, pHosts);
    }

   void w_cassandra_cluster_set_whitelist_filtering_n(_In_ w_cass_cluster* pCluster, _In_ const char* pHosts, size_t pHostsLength)
   {
       const char* _trace_info = "w_cassandra_cluster_set_whitelist_filtering_n";
       if (!pCluster || !pHosts || !pHostsLength)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
       }
          cass_cluster_set_whitelist_filtering_n( pCluster, pHosts, pHostsLength);
   }

   void w_cassandra_cluster_set_blacklist_filtering(_In_ w_cass_cluster* pCluster, _In_  const char* pHosts)
   {
       const char* _trace_info = "w_cassandra_cluster_set_blacklist_filtering";
       if (!pCluster || !pHosts)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
       }
           
       cass_cluster_set_blacklist_filtering(pCluster, pHosts);
   }

   void w_cassandra_cluster_set_blacklist_filtering_n(_In_ w_cass_cluster* pCluster, _In_  const char* pHosts, _In_ size_t pHostsLength)
   {
       const char* _trace_info = "w_cassandra_cluster_set_blacklist_filtering_n";
       if (!pCluster || !pHosts || !pHostsLength)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
       }
           
     cass_cluster_set_blacklist_filtering_n( pCluster, pHosts, pHostsLength);

   }

   void w_cassandra_cluster_set_whitelist_dc_filtering(_In_ w_cass_cluster* pCluster, _In_ const char* pDcs)
   {
       const char* _trace_info = "w_cassandra_cluster_set_whitelist_dc_filtering";
       if (!pCluster || !pDcs)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
       }
           
       cass_cluster_set_whitelist_dc_filtering( pCluster, pDcs);
   }

   void w_cassandra_cluster_set_whitelist_dc_filtering_n(_In_ w_cass_cluster* pCluster, _In_ const char* pDcs, _In_ size_t pDcsLength)
   {
       const char* _trace_info = "w_cassandra_cluster_set_whitelist_dc_filtering_n";
       if (!pCluster || !pDcs || !pDcsLength)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
       }
           
    cass_cluster_set_whitelist_dc_filtering_n( pCluster, pDcs, pDcsLength);

   }

   void w_cassandra_cluster_set_blacklist_dc_filtering(_In_ w_cass_cluster* pCluster, _In_ const char* pDcs)
   {
       const char* _trace_info = "w_cassandra_cluster_set_blacklist_dc_filtering";
       if (!pCluster || !pDcs)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
       }
          cass_cluster_set_blacklist_dc_filtering( pCluster, pDcs);
   }

   void w_cassandra_cluster_set_blacklist_dc_filtering_n(_In_ w_cass_cluster* pCluster, _In_  const char* pDcs, _In_ size_t pDcsLength)
   {
       const char* _trace_info = "w_cassandra_cluster_set_blacklist_dc_filtering_n";
       if (!pCluster || !pDcs || !pDcsLength)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
       }
           cass_cluster_set_blacklist_dc_filtering_n( pCluster, pDcs, pDcsLength);
   }

   void w_cassandra_cluster_set_tcp_nodelay(_In_ w_cass_cluster* pCluster, _In_  w_bool_t pEnabled)
   {
       const char* _trace_info = "w_cassandra_cluster_set_tcp_nodelay";

       if (!pCluster)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
       }

     cass_cluster_set_tcp_nodelay( pCluster, pEnabled);
   }

   void w_cassandra_cluster_set_tcp_keepalive(_In_ w_cass_cluster* pCluster, _In_ w_bool_t pEnabled, _In_ unsigned pDelaySecs)
   {
       const char* _trace_info = "w_cassandra_cluster_set_tcp_keepalive";

       if (!pCluster || !pDelaySecs)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

       }
       cass_cluster_set_tcp_keepalive( pCluster, pEnabled, pDelaySecs);
   }

   void w_cassandra_cluster_set_timestamp_gen(_In_ w_cass_cluster* pCluster, _In_ w_cass_time_stamp_gen* pTimestampGen)
   {
       const char* _trace_info = "w_cassandra_cluster_set_timestamp_gen";
       if (!pCluster || !pTimestampGen)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
       }
      cass_cluster_set_timestamp_gen( pCluster, pTimestampGen);
   }

    void w_cassandra_cluster_set_connection_heartbeat_interval(_In_ w_cass_cluster* pCluster, _In_ unsigned pIntervalSecs)
    {
        const char* _trace_info = "w_cassandra_cluster_set_connection_heartbeat_interval";
        if (!pCluster  || !pIntervalSecs)
        {
            W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
        }

        cass_cluster_set_connection_heartbeat_interval( pCluster, pIntervalSecs);
    }

   void w_cassandra_cluster_set_connection_idle_timeout(_In_ w_cass_cluster* pCluster, _In_  unsigned pTimeoutSecs)
   {
       const char* _trace_info = "w_cassandra_cluster_set_connection_idle_timeout";
       if (!pCluster || !pTimeoutSecs)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

       }
           cass_cluster_set_connection_idle_timeout( pCluster, pTimeoutSecs);
   }

   void w_cassandra_cluster_set_retry_policy(_In_ w_cass_cluster* pCluster, _In_ w_cass_retry_policy* pRetryPolicy)
   {
       const char* _trace_info = "w_cassandra_cluster_set_retry_policy";
       if (!pCluster || !pRetryPolicy)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
       }
          cass_cluster_set_retry_policy( pCluster, pRetryPolicy);
   }

   void w_cassandra_cluster_set_use_schema(_In_ w_cass_cluster* pCluster, _In_ w_bool_t pEnabled)
   {
       const char* _trace_info = "w_cassandra_cluster_set_use_schema";
       if (!pCluster)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
       }

     cass_cluster_set_use_schema(pCluster, pEnabled);

   }
   w_Cass_Error w_cassandra_cluster_set_use_hostname_resolution(_In_ w_cass_cluster* pCluster, _In_  w_bool_t pEnabled)
   {
       const char* _trace_info = "w_cassandra_cluster_set_use_hostname_resolution";
       if (!pCluster)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

           return -1 ;
       }
           
   return   cass_cluster_set_use_hostname_resolution( pCluster, pEnabled);
   }

   w_Cass_Error w_cassandra_cluster_set_use_randomized_contact_points(_In_ w_cass_cluster* pCluster, _In_  w_bool_t pEnabled)
   {
       const char* _trace_info = "w_cassandra_cluster_set_use_randomized_contact_points";
           
       if (!pCluster)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           return -1;
       }

       return cass_cluster_set_use_randomized_contact_points( pCluster, pEnabled);
       
   
   }


   w_Cass_Error w_cassandra_cluster_set_constant_speculative_execution_policy(_In_ w_cass_cluster* pCluster, _In_ int64_t pConstantDelayMs, _In_ int pMaxSpeculativeExecutions)
   {
       const char* _trace_info = "w_cassandra_cluster_set_constant_speculative_execution_policy";
       if (!pCluster || !pConstantDelayMs || !pMaxSpeculativeExecutions)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

           return -1;
       }
     return cass_cluster_set_constant_speculative_execution_policy(pCluster, pConstantDelayMs, pMaxSpeculativeExecutions);
   }


   w_Cass_Error  w_cassandra_cluster_set_no_speculative_execution_policy(_In_ w_cass_cluster* pCluster)
   {
       const char* _trace_info = "w_cassandra_cluster_set_no_speculative_execution_policy";
       if (!pCluster)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

           return -1;
       }
           return  cass_cluster_set_no_speculative_execution_policy(pCluster);
   }


   w_Cass_Error w_cassandra_cluster_set_max_reusable_write_objects(_In_ w_cass_cluster* pCluster, _In_  unsigned pNumObjects)
   {
       const char* _trace_info = "w_cassandra_cluster_set_max_reusable_write_objects";
       if (!pCluster || !pNumObjects)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           return -1;
       }


        return   cass_cluster_set_max_reusable_write_objects(pCluster, pNumObjects);
   }

   w_Cass_Error  w_cassandra_cluster_set_execution_profile(_In_ w_cass_cluster* pCluster, _In_ const char* pName, _In_  w_cass_exec_profile* pProfile)
   {

       const char* _trace_info = "w_cassandra_cluster_set_execution_profile";
       if (!pCluster || !pName || !pProfile)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           return -1;
       }

           
           return  cass_cluster_set_execution_profile(pCluster, pName, pProfile);
   }

   w_Cass_Error w_cassandra_cluster_set_execution_profile_n(_In_ w_cass_cluster* pCluster, _In_ const char* pName, _In_ size_t pNameLength, _In_ w_cass_exec_profile* pProfile)
   {
       const char* _trace_info = "w_cassandra_cluster_set_execution_profile_n";
       if (!pCluster || !pName || !pNameLength || !pProfile)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

           return -1;
       }
           
      return cass_cluster_set_execution_profile_n(pCluster, pName, pNameLength, pProfile);
   }

   w_Cass_Error w_cassandra_cluster_set_prepare_on_all_hosts(_In_ w_cass_cluster* pCluster, _In_ w_bool_t pEnabled)
   {
       const char* _trace_info = "w_cassandra_cluster_set_prepare_on_all_hosts";
       if (!pCluster)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           return -1;
       }
           
    return cass_cluster_set_prepare_on_all_hosts(pCluster, pEnabled);
   }

   w_Cass_Error  w_cassandra_cluster_set_prepare_on_up_or_add_host(_In_ w_cass_cluster* pCluster, _In_ w_bool_t pEnabled)
   {
       const char* _trace_info = "w_cassandra_cluster_set_prepare_on_up_or_add_host";
       if (!pCluster)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

           return -1;
       }
      return  cass_cluster_set_prepare_on_up_or_add_host(pCluster, pEnabled);
   }

   w_Cass_Error w_cass_cluster_set_host_listener_callback(_In_ w_cass_cluster* pCluster, _In_ w_cass_host_listener_callback pCallback,  _In_  void* pData)
   {
       const char* _trace_info = "w_cass_cluster_set_host_listener_callback";
       if (!pCluster || !pData)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           return -1;
       }

       return   cass_cluster_set_host_listener_callback(pCluster, pCallback, pData);
   }
   w_Cass_Error w_cassandra_cluster_set_no_compact(_In_ w_cass_cluster* pCluster, _In_ w_bool_t pEnabled)
   {
       const char* _trace_info = "w_cassandra_cluster_set_no_compact";
       if (!pCluster)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           return -1;
       }

       return cass_cluster_set_no_compact(pCluster, pEnabled);
   }


   w_Cass_Error w_cassandra_cluster_set_host_listener_callback(_In_ w_cass_cluster* pCluster, _In_ CassHostListenerCallback pCallback, _In_  void* pData)
   {
       const char* _trace_info = "w_cass_cluster_set_host_listener_callback";
       if (!pCluster || !pData)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           return -1;
       }


           
    return   cass_cluster_set_host_listener_callback(pCluster, pCallback, pData);
   }


   w_Cass_Error w_cassandra_cluster_set_cloud_secure_connection_bundle(_In_ w_cass_cluster* pCluster, _In_ const char* pPath)
   {
       const char* _trace_info = "w_cassandra_cluster_set_cloud_secure_connection_bundle";
       if (!pCluster || !pPath)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           return -1;
       }
               
       return cass_cluster_set_cloud_secure_connection_bundle(pCluster, pPath);

   }

   w_Cass_Error w_cassandra_cluster_set_cloud_secure_connection_bundle_n(_In_ w_cass_cluster* pCluster, _In_ const char* pPath, _In_ size_t pPathLength)
   {
       const char* _trace_info = "w_cassandra_cluster_set_cloud_secure_connection_bundle_n";
       if (!pCluster || !pPath || !pPathLength)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           return -1;
       }

     return  cass_cluster_set_cloud_secure_connection_bundle_n(pCluster, pPath, pPathLength);
   }

   w_Cass_Error w_cassandra_cluster_set_cloud_secure_connection_bundle_no_ssl_lib_init(_In_ w_cass_cluster* pCluster, _In_ const char* pPath)
   {
       const char* _trace_info = "w_cassandra_cluster_set_cloud_secure_connection_bundle_no_ssl_lib_init";
       if (!pCluster || !pPath)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

           return -1;
       }
           
      return cass_cluster_set_cloud_secure_connection_bundle_no_ssl_lib_init(pCluster, pPath);
   }

   w_Cass_Error w_cassandra_cluster_set_cloud_secure_connection_bundle_no_ssl_lib_init_n(_In_ w_cass_cluster* pCluster, _In_  const char* pPath, _In_ size_t pPathLength)
   {
       const char* _trace_info = "w_cassandra_cluster_set_cloud_secure_connection_bundle_no_ssl_lib_init_n";
       if (!pCluster || !pPath || !pPathLength)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

           return -1;
       }

     return   cass_cluster_set_cloud_secure_connection_bundle_no_ssl_lib_init_n(pCluster, pPath, pPathLength);
   }

   void  w_cassandra_cluster_set_application_name(_In_ w_cass_cluster* pCluster, _In_ const char* pApplicationName)
   {
       const char* _trace_info = "w_cassandra_cluster_set_application_name";
       if (!pCluster || !pApplicationName)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
       }
      cass_cluster_set_application_name(pCluster, pApplicationName);
   }


   void w_cassandra_cluster_set_application_name_n(_In_ w_cass_cluster* pCluster, _In_ const char* pAplicationName, _In_  size_t pApplicationNameLength)
   {
       const char* _trace_info = "w_cassandra_cluster_set_application_name_n";
       if (!pCluster || !pAplicationName || !pApplicationNameLength)
       {
           W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
       }

    cass_cluster_set_application_name_n(pCluster, pAplicationName, pApplicationNameLength);
   }

    void  w_cassandra_cluster_set_application_version(_In_ w_cass_cluster* pCluster, _In_ const char* pApplicationVersion)
    {
        const char* _trace_info = "w_cassandra_cluster_set_application_version";
        if (!pCluster || !pApplicationVersion)
        {
            W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
        }

           cass_cluster_set_application_version(pCluster, pApplicationVersion);
    }

    void w_cassandra_cluster_set_application_version_n(_In_ w_cass_cluster* pCluster, _In_ const char* pApplicationVersion, _In_ size_t pApplicationVersionLength)
    {
        const char* _trace_info = "w_cassandra_cluster_set_application_version_n";
        if (!pCluster || !pApplicationVersion || !pApplicationVersionLength)
        {
            W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
        }
        
        cass_cluster_set_application_version_n(pCluster, pApplicationVersion, pApplicationVersionLength);
    }


    void w_cassandra_cluster_set_client_id(_In_ w_cass_cluster* pCluster, _In_ w_cass_uuid pClientId)
    {
        const char* _trace_info = "w_cassandra_cluster_set_client_id";

        if (!pCluster )
        {
            W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
        }

          cass_cluster_set_client_id(pCluster, pClientId);
    }

    void w_cassandra_cluster_set_monitor_reporting_interval(_In_ w_cass_cluster* pCluster, _In_   unsigned pIntervalSecs)
    {
        const char* _trace_info = "w_cassandra_cluster_set_monitor_reporting_interval";
        if (!pCluster || !pIntervalSecs)
        {
            W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
        }

       cass_cluster_set_monitor_reporting_interval(pCluster, pIntervalSecs);
    }

    w_cass_session* w_cassandra_session_new()
    { 
        return   cass_session_new();
    }

    void w_cassandra_session_free(_In_ w_cass_session* pSession)
    {
        const char* _trace_info = "w_cassandra_session_free";
        if (!pSession)
        {
            W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
        }
           
        cass_session_free(pSession);
    }


    w_cass_future* w_cassandra_session_connect(_In_ w_cass_session* pSession, _In_ const w_cass_cluster* pCluster)
    {
        const char* _trace_info = "w_cassandra_session_connect";
        if (!pSession || !pCluster)
        {
            W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
            return NULL;
        }


      return  cass_session_connect(pSession, pCluster);
    }

    w_cass_future* w_cassandra_session_connect_keyspace(_In_ w_cass_session* pSession, _In_ const w_cass_cluster* pCluster, _In_ const char* pKeyspace)
    {
        const char* _trace_info = "w_cassandra_session_connect_keyspace";
        if (!pSession || !pCluster || !pKeyspace)
        {
            W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
            return NULL;
        }
           
       return cass_session_connect_keyspace(pSession, pCluster, pKeyspace);
    }

    w_cass_future*  w_cassandra_session_connect_keyspace_n(_In_ w_cass_session* pSession, _In_ const w_cass_cluster* pCluster, _In_  const char* pKeyspace, _In_ size_t pKeyspaceLength)
    {
        const char* _trace_info = "w_cassandra_session_connect_keyspace_n";
        if (!pSession || !pCluster || !pKeyspace  || pKeyspaceLength)
        {
            W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
            return NULL;
        }


           return cass_session_connect_keyspace_n(pSession, pCluster, pKeyspace, pKeyspaceLength);
    }

    w_cass_future*  w_cassandra_session_close(_In_ w_cass_session* pSession)
    {
        const char* _trace_info = "w_cassandra_session_close";
        if (!pSession)
        {
            W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
            return NULL;
        }
           return cass_session_close(pSession);
    }
       
    w_cass_future* w_cassandra_session_prepare(_In_ w_cass_session* pSession, _In_ const char* pQuery)
    {
        const char* _trace_info = "w_cassandra_session_prepare";
        if (!pSession || !pQuery)
        {
            W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
            return NULL;
        }
        
        return cass_session_prepare(pSession, pQuery);
    }

    w_cass_future*  w_cassandra_session_prepare_n(_In_ w_cass_session* pSession, _In_  const char* pQuery, _In_  size_t pQueryLength)
     {
         const char* _trace_info = "w_cassandra_session_prepare_n";
         if (!pSession || !pQuery || !pQueryLength)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }

        return  cass_session_prepare_n(pSession, pQuery, pQueryLength);
     }

    w_cass_future* w_cassandra_session_prepare_from_existing(_In_ w_cass_session* pSession, _In_ w_cass_statement* pStatement)
     {
         const char* _trace_info = "w_cassandra_session_prepare_from_existing";
         if (!pSession || !pStatement)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
           return cass_session_prepare_from_existing(pSession, pStatement);
     }


    w_cass_future*  w_cassandra_session_execute(_In_ w_cass_session* pSession, _In_  const w_cass_statement* pStatement)
     {
         const char* _trace_info = "w_cassandra_session_execute";
         if (!pSession || !pStatement)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }


           return cass_session_execute(pSession, pStatement);
     }

    w_cass_future* w_cassandra_session_execute_batch(_In_ w_cass_session* pSession, _In_ const w_cass_batch* pBatch)
     {
         const char* _trace_info = "w_cassandra_session_execute_batch";
         if (!pSession || !pBatch)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }


        return cass_session_execute_batch(pSession, pBatch);
     }

     const w_cass_schema_meta* w_cassandra_session_get_schema_meta(_In_ const w_cass_session* pSession)
     {
         const char* _trace_info = "w_cassandra_session_get_schema_meta";
         if (!pSession)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

             return NULL;
         }
           return cass_session_get_schema_meta(pSession);

     }


     void w_cassandra_session_get_metrics(_In_ const  w_cass_session* pSession, _Inout_ w_cass_metrics* pOutput)
     {
         const char* _trace_info = "w_cassandra_session_get_metrics";
         if (!pSession || !pOutput)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
         }


        cass_session_get_metrics(pSession, pOutput);
     }

     void  w_cassandra_session_get_speculative_execution_metrics(_In_ const w_cass_session* pSession, _Inout_ w_cass_speculative_execution_metrics* pOutput)
     {
         const char* _trace_info = "w_cassandra_session_get_speculative_execution_metrics";
         if (!pSession || !pOutput)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

         }
            cass_session_get_speculative_execution_metrics(pSession, pOutput);

     }

     w_cass_uuid w_cassandra_session_get_client_id(_In_ w_cass_session* pSession)
     {
         const char* _trace_info = "w_cassandra_session_get_client_id";
         if (!pSession)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

            return ;
         }

           return cass_session_get_client_id(pSession);
     }

     void   w_cassandra_schema_meta_free(_In_ const w_cass_schema_meta* pSchemaMeta)
     {
         const char* _trace_info = "w_cassandra_schema_meta_free";
         if (!pSchemaMeta)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
         }

           cass_schema_meta_free(pSchemaMeta);

     }

     uint32_t w_cassandra_schema_meta_snapshot_version(_In_ const w_cass_schema_meta* pSchemaMeta)
     {
         const char* _trace_info = "w_cassandra_schema_meta_snapshot_version";
         if (!pSchemaMeta)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return 0;
         }

           return cass_schema_meta_snapshot_version(pSchemaMeta);
     }

     w_cass_version w_cassandra_schema_meta_version(_In_ const w_cass_schema_meta* pSchemaMeta)
     {
         const char* _trace_info = "w_cassandra_schema_meta_version";
         if (!pSchemaMeta)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

             return ;
         }
           return cass_schema_meta_version(pSchemaMeta);
     }


     const w_cass_key_space_meta* w_cassandra_schema_meta_keyspace_by_name(_In_ const w_cass_schema_meta* pSchemaMeta, _In_ const char* pKeyspace)
     {
         const char* _trace_info = "w_cassandra_schema_meta_keyspace_by_name";
         if (!pSchemaMeta || !pKeyspace)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }

          return    cass_schema_meta_keyspace_by_name(pSchemaMeta, pKeyspace);
     }

     const w_cass_key_space_meta* w_cassandra_schema_meta_keyspace_by_name_n(_In_ const w_cass_schema_meta* pSchemaMeta, _In_ const char* pKeyspace, _In_ size_t pKeyspaceLength)
     {
         const char* _trace_info = "w_cassandra_schema_meta_keyspace_by_name_n";
         if (!pSchemaMeta || !pKeyspace || !pKeyspaceLength)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
          return cass_schema_meta_keyspace_by_name_n(pSchemaMeta, pKeyspace, pKeyspaceLength);
     }


     void w_cassandra_keyspace_meta_name(_In_ const w_cass_key_space_meta* pSchemaMeta, _In_ const char** pName, _In_ size_t* pNameLength)
     {
         const char* _trace_info = "w_cassandra_keyspace_meta_name";
         if (!pSchemaMeta || !pName || !pNameLength)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

         }

          cass_keyspace_meta_name(pSchemaMeta, pName, pNameLength);
     }

     w_bool_t w_cassandra_keyspace_meta_is_virtual(_In_ const w_cass_key_space_meta* pKeyspaceMeta)
     {
         const char* _trace_info = "w_cassandra_keyspace_meta_is_virtual";
         if (!pKeyspaceMeta)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

             return false ;
         }
          return cass_keyspace_meta_is_virtual(pKeyspaceMeta);
     }

     const w_cass_table_meta*  w_cassandra_keyspace_meta_table_by_name(_In_ const w_cass_key_space_meta* pKeyspaceMeta, _In_ const char* pTable)
     {
         const char* _trace_info = "w_cassandra_keyspace_meta_table_by_name";
         if (!pKeyspaceMeta || !pTable)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }

        return cass_keyspace_meta_table_by_name(pKeyspaceMeta, pTable);
     }

     const w_cass_table_meta*  w_cassandra_keyspace_meta_table_by_name_n(_In_ const w_cass_key_space_meta* pKeyspaceMeta, _In_ const char* pTable, _In_ size_t pTableLength)
     {
         const char* _trace_info = "w_cassandra_keyspace_meta_table_by_name_n";

         if (!pKeyspaceMeta || !pTable || !pTableLength)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

             return NULL;
         }
               return cass_keyspace_meta_table_by_name_n(pKeyspaceMeta, pTable, pTableLength);
     }

     const w_cass_materialized_view_meta* w_cassandra_keyspace_meta_materialized_view_by_name(_In_ const w_cass_key_space_meta* pKeyspaceMeta, _In_ const char* pView)
     {
         const char* _trace_info = "w_cassandra_keyspace_meta_materialized_view_by_name";
         if (!pKeyspaceMeta || !pView)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }


           return cass_keyspace_meta_materialized_view_by_name(pKeyspaceMeta, pView);
     }

     const w_cass_materialized_view_meta*  w_cassandra_keyspace_meta_materialized_view_by_name_n(_In_ const w_cass_key_space_meta* pKeyspaceMeta, _In_ const char* pView, _In_  size_t pViewLength)
     {
         const char* _trace_info = "w_cassandra_keyspace_meta_materialized_view_by_name_n";
         if (!pKeyspaceMeta || !pView || !pViewLength)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
           
             return   cass_keyspace_meta_materialized_view_by_name_n(pKeyspaceMeta, pView, pViewLength);
     }

     const w_cass_data_type*  w_cassandra_keyspace_meta_user_type_by_name(_In_ const w_cass_key_space_meta* pKeyspaceMeta, _In_  const char* pType)
     {
         const char* _trace_info = "w_cassandra_keyspace_meta_user_type_by_name";
         if (!pKeyspaceMeta || !pType)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }

           return cass_keyspace_meta_user_type_by_name(pKeyspaceMeta, pType);
     }

     const w_cass_data_type* w_cassandra_keyspace_meta_user_type_by_name_n(_In_ const w_cass_key_space_meta* pKeyspaceMeta, _In_ const char* pType, _In_ size_t pTypelength)
     {
         const char* _trace_info = "w_cassandra_keyspace_meta_user_type_by_name_n";
         if (!pKeyspaceMeta || !pType || !pTypelength)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }


         return cass_keyspace_meta_user_type_by_name_n(pKeyspaceMeta, pType, pTypelength);
     }

     const w_cass_function_meta*  w_cassandra_keyspace_meta_function_by_name(_In_ const  w_cass_key_space_meta* pKeyspaceMeta, _In_ const char* pName, _In_ const char* pArguments)
     {
         const char* _trace_info = "w_cassandra_keyspace_meta_function_by_name";
         if (!pKeyspaceMeta || !pName || !pArguments)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
             return   cass_keyspace_meta_function_by_name(pKeyspaceMeta, pName, pArguments);
     }

     const w_cass_function_meta*  w_cassandra_keyspace_meta_function_by_name_n(_In_ const  w_cass_key_space_meta* pKeyspaceMeta, _In_ const char* pName, _In_ size_t pNameLength, _In_ const char* pArguments, _In_ size_t pArgumentsLength)
     {
         const char* _trace_info = "w_cassandra_keyspace_meta_function_by_name_n";
         if (!pKeyspaceMeta || !pName || !pNameLength || !pArguments || !pArgumentsLength)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }

          return    cass_keyspace_meta_function_by_name_n(pKeyspaceMeta, pName, pNameLength, pArguments, pArgumentsLength);
     }

     const w_cass_aggregate_meta*  w_cassandra_keyspace_meta_aggregate_by_name(_In_ const w_cass_key_space_meta* pKeyspaceMeta, _In_ const char* pName, _In_ const char* pArguments)
     {
         const char* _trace_info = "w_cassandra_keyspace_meta_aggregate_by_name";
         if (!pKeyspaceMeta || !pName || !pArguments)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }

           return   cass_keyspace_meta_aggregate_by_name(pKeyspaceMeta, pName, pArguments);

     }

     const w_cass_aggregate_meta* w_cassandra_keyspace_meta_aggregate_by_name_n(_In_ const w_cass_key_space_meta* pKeyspaceMeta, _In_ const char* pName, _In_ size_t pNameLength, _In_  const char* pArguments, _In_ size_t pArgumentsLength)
     {
         const char* _trace_info = "w_cassandra_keyspace_meta_aggregate_by_name_n";
         if (!pKeyspaceMeta || !pName || !pNameLength || !pArguments || !pArgumentsLength)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }

           return cass_keyspace_meta_aggregate_by_name_n(pKeyspaceMeta, pName, pNameLength, pArguments, pArgumentsLength);

     }

     const w_cass_value* w_cassandra_keyspace_meta_field_by_name(_In_ const w_cass_key_space_meta* pKeyspaceMeta, _In_  const char* pName)
     {
         const char* _trace_info = "w_cassandra_keyspace_meta_field_by_name";
         if (!pKeyspaceMeta || !pName)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
           return  cass_keyspace_meta_field_by_name(pKeyspaceMeta, pName);

     }

     const w_cass_value*  w_cassandra_keyspace_meta_field_by_name_n(_In_ const w_cass_key_space_meta* pKeyspaceMeta, _In_ const char* pName, _In_ size_t pNameLength)
     {
         const char* _trace_info = "w_cassandra_keyspace_meta_field_by_name_n";
         if (!pKeyspaceMeta || !pName || !pNameLength)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
      return cass_keyspace_meta_field_by_name_n(pKeyspaceMeta, pName, pNameLength);
     }

     void  w_cassandra_table_meta_name(_In_ const w_cass_table_meta* pTableMeta, _Inout_ const char** pName, _Inout_ size_t* pNameLength)
     {
         const char* _trace_info = "w_cassandra_table_meta_name";
         if (!pTableMeta || !pName || !pNameLength)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
         }
           cass_table_meta_name(pTableMeta, pName, pNameLength);

     }

     w_bool_t w_cassandra_table_meta_is_virtual(_In_ const w_cass_table_meta* pTableMeta)
     {
         const char* _trace_info = "w_cassandra_table_meta_is_virtual";
         if (!pTableMeta)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return false;
         }

           return cass_table_meta_is_virtual(pTableMeta);

     }

     const w_cass_column_meta* w_cassandra_table_meta_column_by_name(_In_ const w_cass_table_meta* pTableMeta, _In_ const char* pColumn)
     {
         const char* _trace_info = "w_cassandra_table_meta_column_by_name";
         if (!pTableMeta || !pColumn)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
           return cass_table_meta_column_by_name(pTableMeta, pColumn);

     }

     const w_cass_column_meta* w_cassandra_table_meta_column_by_name_n(_In_ const  w_cass_table_meta* pTableMeta, _In_ const char* pColumn, _In_ size_t pColumnLength)
     {
         const char* _trace_info = "w_cassandra_table_meta_column_by_name_n";
         if (!pTableMeta || !pColumn || !pColumnLength)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
           return  cass_table_meta_column_by_name_n(pTableMeta, pColumn, pColumnLength);

     }


     size_t   w_cassandra_table_meta_column_count(_In_ const w_cass_table_meta* pTableMeta)
     {
         const char* _trace_info = "w_cassandra_table_meta_column_count";
         if (!pTableMeta)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return 0;
         }

           return   cass_table_meta_column_count(pTableMeta);

     }

     const w_cass_column_meta* w_cassandra_table_meta_column(_In_ const w_cass_table_meta* pTableMeta, _In_  size_t pIndex)
     {
         const char* _trace_info = "w_cassandra_table_meta_column";
         if (!pTableMeta || !pIndex)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
           return cass_table_meta_column(pTableMeta, pIndex);

     }

     const w_cass_index_meta* w_cassandra_table_meta_index_by_name(_In_ const w_cass_table_meta* pTableMeta, _In_ const char* pIndex)
     {
         const char* _trace_info = "w_cassandra_table_meta_index_by_name";
         if (!pTableMeta || !pIndex)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
           return cass_table_meta_index_by_name(pTableMeta, pIndex);
     }

     const w_cass_index_meta* w_cassandra_table_meta_index_by_name_n(_In_ const w_cass_table_meta* pTableMeta, _In_ const char* pIndex, _In_  size_t pIndexLength)
     {
         const char* _trace_info = "w_cassandra_table_meta_index_by_name_n";
         if (!pTableMeta || !pIndex || !pIndexLength)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }

           return  cass_table_meta_index_by_name_n(pTableMeta, pIndex, pIndexLength);

     }

     size_t w_cassandra_table_meta_index_count(_In_ const w_cass_table_meta* pTableMeta)
     {
         const char* _trace_info = "w_cassandra_table_meta_index_count";
         if (!pTableMeta)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return 0;
         }

           return  cass_table_meta_index_count(pTableMeta);
     }

     const w_cass_index_meta*  w_cassandra_table_meta_index(_In_ const w_cass_table_meta* pTableMeta, _In_ size_t pIndex)
     {
         const char* _trace_info = "w_cassandra_table_meta_index";
         if (!pTableMeta || !pIndex)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }

           return cass_table_meta_index(pTableMeta, pIndex);

     }

     const w_cass_materialized_view_meta* w_cassandra_table_meta_materialized_view_by_name(_In_ const w_cass_table_meta* pTableMeta, _In_ const char* pView)
     {
         const char* _trace_info = "w_cassandra_table_meta_materialized_view_by_name";
         if (!pTableMeta || !pView)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }

           return cass_table_meta_materialized_view_by_name(pTableMeta, pView);

     }

     const w_cass_materialized_view_meta* w_cassandra_table_meta_materialized_view_by_name_n(_In_ const w_cass_table_meta* pTableMeta, _In_ const char* pView, _In_ size_t pViewLength)
     {
         const char* _trace_info = "w_cassandra_table_meta_materialized_view_by_name_n";
         if (!pTableMeta || !pView || !pViewLength)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }

           return  cass_table_meta_materialized_view_by_name_n(pTableMeta, pView, pViewLength);

     }

      size_t w_cassandra_table_meta_materialized_view_count(_In_ const w_cass_table_meta* pTableMeta)
      {
          const char* _trace_info = "w_cassandra_table_meta_materialized_view_count";
           if (!pTableMeta)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return 0;
           }
          return cass_table_meta_materialized_view_count(pTableMeta);

      }

      const w_cass_materialized_view_meta* w_cassandra_table_meta_materialized_view(_In_ const w_cass_table_meta* pTableMeta, _In_ size_t pIndex)
      {
          const char* _trace_info = "w_cassandra_table_meta_materialized_view";
          if (!pTableMeta)
          {
              W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
              return NULL;
          }
           return cass_table_meta_materialized_view(pTableMeta, pIndex);

      }

      size_t  w_cassandra_table_meta_partition_key_count(_In_ const  w_cass_table_meta* pTableMeta)
      {
          const char* _trace_info = "w_cassandra_table_meta_partition_key_count";
          if (!pTableMeta)
          {
              W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
              return 0;
          }
           return cass_table_meta_partition_key_count(pTableMeta);
      }

      const w_cass_column_meta* w_cassandra_table_meta_partition_key(_In_ const w_cass_table_meta* pTableMeta, _In_ size_t pIndex)
      {
          const char* _trace_info = "w_cassandra_table_meta_partition_key";
          if (!pTableMeta || !pIndex)
          {
              W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);


              return NULL;
          }
           return cass_table_meta_partition_key(pTableMeta, pIndex);

      }

      size_t w_cassandra_table_meta_clustering_key_count(_In_ const w_cass_table_meta* pTableMeta)
      {
          const char* _trace_info = "w_cassandra_table_meta_clustering_key_count";
          if (!pTableMeta)
          {
              W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
              return 0;
          }
           return cass_table_meta_clustering_key_count(pTableMeta);
      }

      const w_cass_column_meta*  w_cassandra_table_meta_clustering_key(_In_ const w_cass_table_meta* pTableMeta, _In_ size_t pIndex)
      {
          const char* _trace_info = "w_cassandra_table_meta_clustering_key";
          if (!pTableMeta || !pIndex)
          {
              W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
              return NULL;
          }
           return  cass_table_meta_clustering_key(pTableMeta, pIndex);

      }



       w_cass_clustering_order w_cassandra_table_meta_clustering_key_order(_In_ const w_cass_table_meta* pTableMeta, _In_ size_t pIndex)
       {
           const char* _trace_info = "w_cassandra_table_meta_clustering_key_order";
           if (!pTableMeta || !pIndex)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return    cass_table_meta_clustering_key_order(pTableMeta, pIndex);

       }

       const w_cass_value* w_cassandra_table_meta_field_by_name(_In_ const w_cass_table_meta* pTableMeta, _In_ const char* pName)
       {
           const char* _trace_info = "w_cassandra_table_meta_field_by_name";
           if (!pTableMeta || !pName)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
          return  cass_table_meta_field_by_name(pTableMeta, pName);
       }

       const w_cass_value* w_cassandra_table_meta_field_by_name_n(_In_ const w_cass_table_meta* pTableMeta, _In_ const char* pName, _In_ size_t pNameLength)
       {
           const char* _trace_info = "w_cassandra_table_meta_field_by_name_n";
           if (!pTableMeta || !pName || !pNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return   cass_table_meta_field_by_name_n(pTableMeta, pName, pNameLength);

       }

       const w_cass_column_meta* w_cassandra_materialized_view_meta_column_by_name(_In_ const w_cass_materialized_view_meta* pViewMeta, _In_ const char* pColumn)
       {
           const char* _trace_info = "w_cassandra_materialized_view_meta_column_by_name";
           if (!pViewMeta || !pColumn)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

               return NULL;
           }
           return    cass_materialized_view_meta_column_by_name(pViewMeta, pColumn);

       }

       const w_cass_column_meta*  w_cassandra_materialized_view_meta_column_by_name_n(_In_ const w_cass_materialized_view_meta* pViewMeta, _In_ const char* pColumn, _In_ size_t pColumnLength)
       {
           const char* _trace_info = "w_cassandra_materialized_view_meta_column_by_name_n";

           if (!pViewMeta || !pColumn || !pColumnLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return       cass_materialized_view_meta_column_by_name_n(pViewMeta, pColumn, pColumnLength);

       }

       void w_cassandra_materialized_view_meta_name(_In_ const w_cass_materialized_view_meta* pViewMeta, _Inout_ const char** pName, _Inout_ size_t* pNameLength)
       {
           const char* _trace_info = "w_cassandra_materialized_view_meta_name";
           if (!pViewMeta || !pName || !pNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           }
           cass_materialized_view_meta_name(pViewMeta, pName, pNameLength);

       }

       const w_cass_table_meta* w_cassandra_materialized_view_meta_base_table(_In_ const w_cass_materialized_view_meta* pViewMeta)
       {
           const char* _trace_info = "w_cassandra_materialized_view_meta_base_table";
           if (!pViewMeta)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return cass_materialized_view_meta_base_table(pViewMeta);

       }

       size_t  w_cassandra_materialized_view_meta_column_count(_In_ const w_cass_materialized_view_meta* pViewMeta)
       {
           const char* _trace_info = "w_cassandra_materialized_view_meta_column_count";
           if (!pViewMeta)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return 0;
           }
           return   cass_materialized_view_meta_column_count(pViewMeta);

       }

       const w_cass_column_meta*  w_cassandra_materialized_view_meta_column(_In_ const w_cass_materialized_view_meta* pViewMeta, _In_ size_t pIndex)
       {
           const char* _trace_info = "w_cassandra_materialized_view_meta_column";
           if (!pViewMeta || !pIndex)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return  cass_materialized_view_meta_column(pViewMeta, pIndex);

       }

       size_t  w_cassandra_materialized_view_meta_partition_key_count(_In_ const w_cass_materialized_view_meta* pViewMeta)
       {
           const char* _trace_info = "w_cassandra_materialized_view_meta_partition_key_count";
           if (!pViewMeta)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return 0;
           }

           return  cass_materialized_view_meta_partition_key_count(pViewMeta);

       }

       const w_cass_column_meta* w_cassandra_materialized_view_meta_partition_key(_In_ const w_cass_materialized_view_meta* pViewMeta, _In_ size_t pIndex)
       {
           const char* _trace_info = "w_cassandra_materialized_view_meta_partition_key";
           if (!pViewMeta || !pIndex)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return   cass_materialized_view_meta_partition_key(pViewMeta, pIndex);

       }

       size_t  w_cassandra_materialized_view_meta_clustering_key_count(_In_ const w_cass_materialized_view_meta* pViewMeta)
       {
           const char* _trace_info = "w_cassandra_materialized_view_meta_clustering_key_count";
           if (!pViewMeta)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return 0;
           }
           return  cass_materialized_view_meta_clustering_key_count(pViewMeta);

       }

       const w_cass_column_meta*  w_cassandra_materialized_view_meta_clustering_key(_In_ const w_cass_materialized_view_meta* pViewMeta, _In_ size_t pIndex)
       {
           const char* _trace_info = "w_cassandra_materialized_view_meta_clustering_key";
           if (!pViewMeta || !pIndex)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
          return  cass_materialized_view_meta_clustering_key(pViewMeta, pIndex);

       }

       w_cass_clustering_order w_cassandra_materialized_view_meta_clustering_key_order(_In_ const w_cass_materialized_view_meta* pViewMeta, _In_ size_t pIndex)
       {
           const char* _trace_info = "w_cassandra_materialized_view_meta_clustering_key_order";
           if (!pViewMeta)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_materialized_view_meta_clustering_key_order(pViewMeta, pIndex);

       }

       const w_cass_value* w_cassandra_materialized_view_meta_field_by_name(_In_ const w_cass_materialized_view_meta* pViewMeta, _In_ const char* pName)
       {
           const char* _trace_info = "w_cassandra_materialized_view_meta_field_by_name";
           if (!pViewMeta || !pName)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }

           
           return  cass_materialized_view_meta_field_by_name (pViewMeta, pName);

       }

       const w_cass_value*  w_cassandra_materialized_view_meta_field_by_name_n(_In_ const w_cass_materialized_view_meta* pViewMeta, _In_ const char* pName, _In_ size_t pNameLength)
       {
           const char* _trace_info = "w_cassandra_materialized_view_meta_field_by_name_n";
           if (!pViewMeta || !pName || !pNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return    cass_materialized_view_meta_field_by_name_n(pViewMeta, pName, pNameLength);

       }

       void w_cassandra_column_meta_name(_In_ const w_cass_column_meta* pColumnMeta, _Inout_ const char** pName, _Inout_ size_t* pNameLength)
       {
           const char* _trace_info = "w_cassandra_column_meta_name";
           if (!pColumnMeta || !pName || !pNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

           }
           cass_column_meta_name(pColumnMeta, pName, pNameLength);

       }

       w_cass_column_type  w_cassandra_column_meta_type(_In_ const w_cass_column_meta* pColumnMeta)
       {
           const char* _trace_info = "w_cassandra_column_meta_type";
           if (!pColumnMeta)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;

           }
           return  cass_column_meta_type(pColumnMeta);

       }

       const w_cass_data_type* w_cassandra_column_meta_data_type(_In_ const w_cass_column_meta* pColumnMeta)
       {
           const char* _trace_info = "w_cassandra_column_meta_data_type";
           if (!pColumnMeta)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;

           }
           return   cass_column_meta_data_type(pColumnMeta);

       }

       const w_cass_value* w_cassandra_column_meta_field_by_name(_In_ const w_cass_column_meta* pColumnMeta, _In_ const char* pName)
       {
           const char* _trace_info = "w_cassandra_column_meta_field_by_name";
           if (!pColumnMeta || !pName)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;

           }
           return   cass_column_meta_field_by_name(pColumnMeta, pName);

       }

       const w_cass_value* w_cassandra_column_meta_field_by_name_n(_In_ const w_cass_column_meta* pColumnMeta, _In_  const char* pName, _In_ size_t pNameLength)
       {
           const char* _trace_info = "w_cassandra_column_meta_field_by_name_n";
           if (!pColumnMeta || !pName || !pNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;

           }
           return     cass_column_meta_field_by_name_n(pColumnMeta, pName, pNameLength);

       }

       void w_cassandra_index_meta_name(_In_ const w_cass_index_meta* pIndexMeta, _Inout_ const char** pName, _Inout_ size_t* pNameLength)
       {
           const char* _trace_info = "w_cassandra_index_meta_name";
           if (!pIndexMeta || !pName || !pNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

           }
           cass_index_meta_name(pIndexMeta, pName, pNameLength);

       }

       w_cass_index_type w_cassandra_index_meta_type(_In_ const w_cass_index_meta* pIndexMeta)
       {
           const char* _trace_info = "w_cassandra_index_meta_type";
           if (!pIndexMeta)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;

           }
           return   cass_index_meta_type(pIndexMeta);

       }

       void  w_cassandra_index_meta_target(_In_ const w_cass_index_meta* pIndexMeta, _Inout_ const char** pTarget, _Inout_ size_t* pTargetLength)
       {
           const char* _trace_info = "w_cassandra_index_meta_target";
           if (!pIndexMeta || !pTarget || !pTargetLength) 
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);


           }
           cass_index_meta_target(pIndexMeta, pTarget, pTargetLength);

       }

       const w_cass_value* w_cassandra_index_meta_options(_In_ const w_cass_index_meta* pIndexMeta)
       {
           const char* _trace_info = "w_cassandra_index_meta_options";
           if (!pIndexMeta)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;

           }
           return cass_index_meta_options(pIndexMeta);

       }

       const w_cass_value* w_cassandra_index_meta_field_by_name(_In_ const w_cass_index_meta* pIndexMeta, _In_ const char* pName)
       {
           const char* _trace_info = "w_cassandra_index_meta_field_by_name";
           if (!pIndexMeta || !pName)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

               return NULL;

           }
           return  cass_index_meta_field_by_name(pIndexMeta, pName);

       }

       const w_cass_value* w_cassandra_index_meta_field_by_name_n(_In_ const  w_cass_index_meta* pIndexMeta, _In_ const char* pName, _In_ size_t pNameLength)
       {
           const char* _trace_info = "w_cassandra_index_meta_field_by_name_n";
           if (!pIndexMeta || !pName || !pNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;

           }
           return   cass_index_meta_field_by_name_n(pIndexMeta, pName, pNameLength);

       }

       void w_cassandra_function_meta_name(_In_ const w_cass_function_meta* pFunctionMeta, _Inout_ const char** pName, _Inout_  size_t* pNameLength)
       {

           const char* _trace_info = "w_cassandra_function_meta_name";
           if (!pFunctionMeta || !pName || !pNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

           }
           cass_function_meta_name(pFunctionMeta, pName, pNameLength);

       }

       void w_cassandra_function_meta_full_name(_In_ const w_cass_function_meta* pFunctionMeta, _Inout_ const char** pFullName, _Inout_ size_t* pFullNameLength)
       {
           const char* _trace_info = "w_cassandra_function_meta_full_name";
           if (!pFunctionMeta || !pFullName || !pFullNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

           }
           cass_function_meta_full_name(pFunctionMeta, pFullName, pFullNameLength);

       }

       void  w_cassandra_function_meta_body(_In_ const w_cass_function_meta* pFunctionMeta, _Inout_ const char** pBody, _Inout_ size_t* pBodyLength)
       {
           const char* _trace_info = "w_cassandra_function_meta_body";
           if (!pFunctionMeta || !pBody || !pBodyLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

           }
           cass_function_meta_body(pFunctionMeta, pBody, pBodyLength);

       }

       void w_cassandra_function_meta_language(_In_ const w_cass_function_meta* pFunctionMeta, _Inout_ const char** pLanguage, _Inout_  size_t* pLanguageLength)
       {
           const char* _trace_info = "w_cassandra_function_meta_language";
           if (!pFunctionMeta || !pLanguage || !pLanguageLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

           }
           cass_function_meta_language(pFunctionMeta, pLanguage, pLanguageLength);

       }

       w_bool_t  w_cassandra_function_meta_called_on_null_input(_In_ const w_cass_function_meta* pFunctionMeta)
       {
           const char* _trace_info = "w_cassandra_function_meta_called_on_null_input";
           if (!pFunctionMeta)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return false;
           }
           return cass_function_meta_called_on_null_input(pFunctionMeta);

       }

       size_t w_cassandra_function_meta_argument_count(_In_ const w_cass_function_meta* pFunctionMeta)
       {
           const char* _trace_info = "w_cassandra_function_meta_argument_count";
           if (!pFunctionMeta)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return 0;
           }
           return   cass_function_meta_argument_count(pFunctionMeta);

       }

       w_Cass_Error  w_cassandra_function_meta_argument(_In_ const w_cass_function_meta* pFunctionMeta, _In_ size_t pIndex, _Inout_ const char** pName, _Inout_ size_t* pNameLength, _Inout_ const w_cass_data_type** pType)
       {
           const char* _trace_info = "w_cassandra_function_meta_argument";
           if (!pFunctionMeta || !pIndex || !pName || !pNameLength || !pType)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_function_meta_argument(pFunctionMeta, pIndex, pName, pNameLength, pType);

       }

       const w_cass_data_type*  w_cassandra_function_meta_argument_type_by_name(_In_ const w_cass_function_meta* pFunctionMeta, _In_ const char* pName)
       {
           const char* _trace_info = "w_cassandra_function_meta_argument_type_by_name";
           if (!pFunctionMeta || !pName)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return   cass_function_meta_argument_type_by_name(pFunctionMeta, pName);

       }

       const w_cass_data_type*   w_cassandra_function_meta_argument_type_by_name_n(_In_ const w_cass_function_meta* pFunctionMeta, _In_ const char* pName, _In_ size_t pNameLength)
       {
           const char* _trace_info = "w_cassandra_function_meta_argument_type_by_name_n";
           if (!pFunctionMeta || !pName || !pNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return   cass_function_meta_argument_type_by_name_n(pFunctionMeta, pName, pNameLength);

       }

       const w_cass_data_type*  w_cassandra_function_meta_return_type(_In_ const w_cass_function_meta* pFunctionMeta)
       {
           const char* _trace_info = "w_cassandra_function_meta_return_type";
           if (!pFunctionMeta)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return cass_function_meta_return_type(pFunctionMeta);

       }

       const w_cass_value* w_cassandra_function_meta_field_by_name(_In_ const w_cass_function_meta* pFunctionMeta, _In_ const char* pName)
       {
           const char* _trace_info = "w_cassandra_function_meta_field_by_name";
           if (!pFunctionMeta || !pName)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return cass_function_meta_field_by_name(pFunctionMeta, pName);

       }

       const w_cass_value*  w_cassandra_function_meta_field_by_name_n(_In_ const w_cass_function_meta* pFunctionMeta, _In_ const char* pName, _In_ size_t pNameLength)
       {
           const char* _trace_info = "w_cassandra_function_meta_field_by_name_n";
           if (!pFunctionMeta || !pName || !pNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return cass_function_meta_field_by_name_n(pFunctionMeta, pName, pNameLength);

       }

       void w_cassandra_aggregate_meta_name(_In_ const w_cass_aggregate_meta* pAggregateMeta, _Inout_ const char** pName, _Inout_ size_t* pNameLength)
       {
           const char* _trace_info = "w_cassandra_aggregate_meta_name";
           if (!pAggregateMeta || !pName || !pNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

           }
           cass_aggregate_meta_name(pAggregateMeta, pName, pNameLength);

       }

       void w_cassandra_aggregate_meta_full_name(_In_ const w_cass_aggregate_meta* pAggregateMeta, _Inout_ const char** pFullName, _Inout_ size_t* pFullNameLength)
       {
           const char* _trace_info = "w_cassandra_aggregate_meta_full_name";

           if (!pAggregateMeta || !pFullName || !pFullNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

           }
           cass_aggregate_meta_full_name(pAggregateMeta, pFullName, pFullNameLength);

       }

       size_t w_cassandra_aggregate_meta_argument_count(_In_ const w_cass_aggregate_meta* pAggregateMeta)
       {
           const char* _trace_info = "w_cassandra_aggregate_meta_argument_count";
           if (!pAggregateMeta)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return 0;
           }
           return cass_aggregate_meta_argument_count(pAggregateMeta);

       }

       const w_cass_data_type* w_cassandra_aggregate_meta_argument_type(_In_ const w_cass_aggregate_meta* pAggregateMeta, _In_ size_t pIndex)
       {
           const char* _trace_info = "w_cassandra_aggregate_meta_argument_type";
           if (!pAggregateMeta || !pIndex)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

               return NULL;
           }
           return cass_aggregate_meta_argument_type(pAggregateMeta, pIndex);

       }
       const w_cass_data_type* w_cassandra_aggregate_meta_return_type(_In_ const w_cass_aggregate_meta* pAggregateMeta)
       {
           const char* _trace_info = "w_cassandra_aggregate_meta_return_type";
           if (!pAggregateMeta)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return  cass_aggregate_meta_return_type(pAggregateMeta);

       }

       const w_cass_data_type* w_cassandra_aggregate_meta_state_type(_In_ const w_cass_aggregate_meta* pAggregateMeta)
       {
           const char* _trace_info = "w_cassandra_aggregate_meta_state_type";
           if (!pAggregateMeta)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
         return   cass_aggregate_meta_state_type(pAggregateMeta);

       }

       const w_cass_function_meta* w_cassandra_aggregate_meta_state_func(_In_ const w_cass_aggregate_meta* pAggregateMeta)
       {
           const char* _trace_info = "w_cassandra_aggregate_meta_state_func";
           if (!pAggregateMeta)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return cass_aggregate_meta_state_func(pAggregateMeta);

       }

       const w_cass_function_meta* w_cassandra_aggregate_meta_final_func(_In_ const w_cass_aggregate_meta* pAggregateMeta)
       {
           const char* _trace_info = "w_cassandra_aggregate_meta_final_func";
           if (!pAggregateMeta)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return cass_aggregate_meta_final_func(pAggregateMeta);

       }

       const w_cass_value* w_cassandra_aggregate_meta_init_cond(_In_ const w_cass_aggregate_meta* pAggregateMeta)
       {
           const char* _trace_info = "w_cassandra_aggregate_meta_init_cond";
           if (!pAggregateMeta)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return  cass_aggregate_meta_init_cond(pAggregateMeta);

       }


       const w_cass_value* w_cassandra_aggregate_meta_field_by_name(_In_ const w_cass_aggregate_meta* pAggregateMeta, _In_ const char* pName)
       {
           const char* _trace_info = "w_cassandra_aggregate_meta_field_by_name";
           if (!pAggregateMeta || !pName)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return  cass_aggregate_meta_field_by_name(pAggregateMeta, pName);

       }

       const w_cass_value*  w_cassandra_aggregate_meta_field_by_name_n(_In_ const w_cass_aggregate_meta* pAggregateMeta, _In_ const char* pName, _In_ size_t pNameLength)
       {
           const char* _trace_info = "w_cassandra_aggregate_meta_field_by_name_n";
           if (!pAggregateMeta || !pName || !pNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return   cass_aggregate_meta_field_by_name_n(pAggregateMeta, pName, pNameLength);

       }

       w_cass_ssl* w_cassandra_ssl_new()
       {
          return  cass_ssl_new();
       }

       w_cass_ssl* w_cassandra_ssl_new_no_lib_init()
       {
           return cass_ssl_new_no_lib_init();
       }

       void w_cassandra_ssl_free(_In_ w_cass_ssl* pSsl)
       {
           const char* _trace_info = "w_cassandra_ssl_free";
           if (!pSsl)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           }
           cass_ssl_free(pSsl);

       }

       w_Cass_Error w_cassandra_ssl_add_trusted_cert(_In_ w_cass_ssl* pSsl, _In_ const char* pCert)
       {
           const char* _trace_info = "w_cassandra_ssl_add_trusted_cert";
           if (!pSsl || !pCert)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

               return -1;
           }
           return  cass_ssl_add_trusted_cert(pSsl, pCert);

       }

       w_Cass_Error  w_cassandra_ssl_add_trusted_cert_n(_In_ w_cass_ssl* pSsl, _In_  const char* pCert, _In_ size_t pCertLength)
       {
           const char* _trace_info = "w_cassandra_ssl_add_trusted_cert_n";
           if (!pSsl || !pCert || !pCertLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_ssl_add_trusted_cert_n(pSsl, pCert, pCertLength);

       }

       void  w_cassandra_ssl_set_verify_flags(_In_ w_cass_ssl* pSsl, _In_ int pFlags)
       {
           const char* _trace_info = "w_cassandra_ssl_set_verify_flags";
           if (!pSsl)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

           }
           cass_ssl_set_verify_flags(pSsl, pFlags);

       }

       w_Cass_Error  w_cassandra_ssl_set_cert(_In_ w_cass_ssl* pSsl, _In_ const char* pCert)
       {
           const char* _trace_info = "w_cassandra_ssl_set_cert";
           if (!pSsl || !pCert)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_ssl_set_cert(pSsl, pCert);

       }

       w_Cass_Error w_cassandra_ssl_set_cert_n(_In_ w_cass_ssl* pSsl, _In_  const char* pCert, _In_ size_t pCertLength)
       {
           const char* _trace_info = "w_cassandra_ssl_set_cert_n";
           if (!pSsl || !pCert || !pCertLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_ssl_set_cert_n(pSsl, pCert, pCertLength);

       }

       w_Cass_Error w_cassandra_ssl_set_private_key(_In_ w_cass_ssl* pSsl, _In_ const char* pKey, _In_ const char* pPassword)
       {
           const char* _trace_info = "w_cassandra_ssl_set_private_key";
           if (!pSsl || !pKey || !pPassword)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_ssl_set_private_key(pSsl, pKey, pPassword);

       }

       w_Cass_Error w_cassandra_ssl_set_private_key_n(_In_ w_cass_ssl* pSsl, _In_ const char* pKey, _In_ size_t pKeyLength, _In_  const char* pPassword, _In_ size_t pPasswordLength)
       {
           const char* _trace_info = "w_cassandra_ssl_set_private_key_n";
           if (!pSsl || !pKey || !pKeyLength || !pPassword || !pPasswordLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           
           return cass_ssl_set_private_key_n(pSsl, pKey, pKeyLength, pPassword, pPasswordLength);
       }

       void w_cassandra_authenticator_address(_In_ const w_cass_authenticator* pAuth, _Inout_ w_cass_inet* pAddress)
       {
           const char* _trace_info = "w_cassandra_authenticator_address";
           if (!pAuth || !pAddress)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           }
           cass_authenticator_address(pAuth, pAddress);

       }


       const char*  w_cassandra_authenticator_hostname(_In_ const w_cass_authenticator* pAuth, _Inout_ size_t* pLength)
       {
           const char* _trace_info = "w_cassandra_authenticator_hostname";
           if (!pAuth || !pLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return  cass_authenticator_hostname(pAuth, pLength);

       }


       const char* w_cassandra_authenticator_class_name(_In_ const w_cass_authenticator* pAuth, _Inout_ size_t* pLength)
       {
           const char* _trace_info = "w_cassandra_authenticator_class_name";
           if (!pAuth || !pLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return cass_authenticator_class_name(pAuth, pLength);

       }


       void* w_cassandra_authenticator_exchange_data(_In_ w_cass_authenticator* pAuth)
       {
           const char* _trace_info = "w_cassandra_authenticator_exchange_data";
           if (!pAuth)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return cass_authenticator_exchange_data(pAuth);

       }

       void w_cassandra_authenticator_set_exchange_data(_In_ w_cass_authenticator* pAuth, _In_ void* pExchangeData)
       {
           const char* _trace_info = "w_cassandra_authenticator_set_exchange_data";
           if (!pAuth || !pExchangeData)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

           }
           cass_authenticator_set_exchange_data(pAuth, pExchangeData);

       }

       char* w_cassandra_authenticator_response(_In_ w_cass_authenticator* pAuth, _In_ size_t pSize)
       {
           const char* _trace_info = "w_cassandra_authenticator_response";
           if (!pAuth || !pSize)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return  cass_authenticator_response(pAuth, pSize);

       }

       void  w_cassandra_authenticator_set_response(_In_ w_cass_authenticator* pAuth, _In_ const char* pResponse, _In_ size_t pResponseSize)
       {
           const char* _trace_info = "w_cassandra_authenticator_set_response";
           if (!pAuth || !pResponse || !pResponseSize)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

           }
           cass_authenticator_set_response(pAuth, pResponse, pResponseSize);

       }


       void w_cassandra_authenticator_set_error(_In_ w_cass_authenticator* pAuth, _In_ const char* pMessage)
       {
           const char* _trace_info  ="w_cassandra_authenticator_set_error";
           if (!pAuth || !pMessage)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

           }
           cass_authenticator_set_error(pAuth, pMessage);

       }

       void w_cassandra_authenticator_set_error_n(_In_ w_cass_authenticator* pAuth, _In_ const char* pMessage, _In_ size_t pMessageLength)
       {
           const char* _trace_info = "w_cassandra_authenticator_set_error_n";
           if (!pAuth || !pMessage || !pMessageLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

           }
           cass_authenticator_set_error_n(pAuth, pMessage, pMessageLength);

       }

       void  w_cassandra_future_free(_In_ w_cass_future* pFuture)
       {
           const char* _trace_info = "w_cassandra_future_free";
           if (!pFuture)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

           }
           cass_future_free(pFuture);

       }

       w_Cass_Error  w_cassandra_future_set_callback(_In_ w_cass_future* pFuture, _In_ w_cass_future_callback pCallback, _In_ void* pData)
       {
           const char* _trace_info = "w_cassandra_future_set_callback";
           if (!pFuture || !pData)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

               return -1;
           }
           return cass_future_set_callback(pFuture, pCallback, pData);

       }

       w_bool_t  w_cassandra_future_ready(_In_ w_cass_future* pFuture)
       {
           const char* _trace_info = "w_cassandra_future_ready";
           if (!pFuture)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return false;
           }
           return cass_future_ready(pFuture);

       }

       void  w_cassandra_future_wait(_In_ w_cass_future* pFuture)
       {
           const char* _trace_info = "w_cassandra_future_wait";
           if (!pFuture)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           }
           cass_future_wait(pFuture);

       }

       w_bool_t w_cassandra_future_wait_timed(_In_ w_cass_future* pFuture, _In_ w_duration_t pTimeoutUs)
       {
           const char* _trace_info = "w_cassandra_future_wait_timed";
           if (!pFuture || !pTimeoutUs)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return false;
           }
           return cass_future_wait_timed(pFuture, pTimeoutUs);

       }

       const w_cass_result* w_cassandra_future_get_result(_In_ w_cass_future* pFuture)
       {
           const char* _trace_info = "w_cassandra_future_get_result";
           if (!pFuture)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return     cass_future_get_result(pFuture);

       }

       const w_cass_error_result* w_cassandra_future_get_error_result(_In_ w_cass_future* pFuture)
       {
           const char* _trace_info = "w_cassandra_future_get_error_result";
           if (!pFuture)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return cass_future_get_error_result(pFuture);

       }

       const w_cass_prepared*  w_cassandra_future_get_prepared(_In_ w_cass_future* pFuture)
       {
           const char* _trace_info = "w_cassandra_future_get_prepared";
           if (!pFuture)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return  cass_future_get_prepared(pFuture);

       }

       w_Cass_Error w_cassandra_future_error_code(_In_ w_cass_future* pFuture)
       {
           const char* _trace_info = "w_cassandra_future_error_code";
           if (!pFuture)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_future_error_code(pFuture);

       }

       void w_cassandra_future_error_message(_In_ w_cass_future* pFuture, _Inout_ const char** pMessage, _Inout_ size_t* pMessageLength)
       {
           const char* _trace_info = "w_cassandra_future_error_message";
           if (!pFuture || !pMessage || !pMessageLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           }
           cass_future_error_message(pFuture, pMessage, pMessageLength);

       }

       w_Cass_Error w_cassandra_future_tracing_id(_In_ w_cass_future* pFuture, _Inout_ w_cass_uuid* pTracingId)
       {
           const char* _trace_info = "w_cassandra_future_tracing_id";
           if (!pFuture)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
          return cass_future_tracing_id(pFuture, pTracingId);

       }

       size_t  w_cassandra_future_custom_payload_item_count(_In_ w_cass_future* pFuture)
       {
           const char* _trace_info = "w_cassandra_future_custom_payload_item_count";
           if (!pFuture)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_future_custom_payload_item_count(pFuture);

       }

       w_Cass_Error  w_cassandra_future_custom_payload_item(_In_ w_cass_future* pFuture, _In_ size_t pIndex, _Inout_ const char** pName, _Inout_ size_t* pNameLength, _Inout_ const w_byte_t** pValue, _Inout_ size_t* pValueSize)
       {
           const char* _trace_info = "w_cassandra_future_custom_payload_item";
           if (!pFuture || !pIndex || !pName || !pNameLength || !pValue || !pValueSize)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return    cass_future_custom_payload_item(pFuture, pIndex, pName, pNameLength, pValue, pValueSize);
       }

       w_cass_statement*  w_cassandra_statement_new(_In_ const char* pQuery, _In_ size_t pParameterCount)
       {
           const char* _trace_info = "w_cassandra_statement_new";
           if (!pQuery )
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return   cass_statement_new(pQuery, pParameterCount);

       }

       w_cass_statement* w_cassandra_statement_new_n(_In_ const char* pQuery, _In_ size_t pQueryLength, _In_ size_t pParameterCount)
       {
           const char* _trace_info = "w_cassandra_statement_new_n";
           if (!pQuery || !pParameterCount || !pQueryLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return  cass_statement_new_n(pQuery, pQueryLength, pParameterCount);

       }

       w_Cass_Error w_cassandra_statement_reset_parameters(_In_ w_cass_statement* pStatement, _In_ size_t pCount)
       {
           const char* _trace_info = "w_cassandra_statement_reset_parameters";
           if (!pStatement || !pCount)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_statement_reset_parameters(pStatement, pCount);

       }


       void w_cassandra_statement_free(_In_ w_cass_statement* pStatement)
       {
           const char* _trace_info = "w_cassandra_statement_free";
           if (!pStatement)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

           }
           cass_statement_free(pStatement);

       }

       w_Cass_Error  w_cassandra_statement_add_key_index(_In_ w_cass_statement* pStatement, _In_ size_t pIndex)

       {
           const char* _trace_info = "w_cassandra_statement_add_key_index";
           if (!pStatement || !pIndex)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;


           }
           return cass_statement_add_key_index(pStatement, pIndex);

       }

       w_Cass_Error w_cassandra_statement_set_keyspace(_In_ w_cass_statement* pStatement, _In_ const char* pKeyspace)
       {
           const char* _trace_info = "w_cassandra_statement_set_keyspace";
           if (!pStatement || !pKeyspace)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return 0;
           }
           return cass_statement_set_keyspace(pStatement, pKeyspace);

       }

       w_Cass_Error   w_cassandra_statement_set_keyspace_n(_In_ w_cass_statement* pStatement, _In_ const char* pKeyspace, _In_ size_t pKeyspaceLength)
       {
           const char* _trace_info = "w_cassandra_statement_set_keyspace_n";
           if (!pStatement || !pKeyspace || !pKeyspaceLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

               return -1;
           }

           return cass_statement_set_keyspace_n(pStatement, pKeyspace, pKeyspaceLength);


       }

       w_Cass_Error  w_cassandra_statement_set_consistency(_In_ w_cass_statement* pStatement, _In_ w_cass_consistency pConsistency)
       {
           const char* _trace_info = "w_cassandra_statement_set_consistency";
           if (!pStatement || !pConsistency)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_statement_set_consistency(pStatement, pConsistency);

       }

       w_Cass_Error w_cassandra_statement_set_serial_consistency(_In_ w_cass_statement* pStatement, _In_ w_cass_consistency pSerialConsistency)
       {
           const char* _trace_info = "w_cassandra_statement_set_serial_consistency";
           if (!pStatement || !pSerialConsistency)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

               return -1;
           }
           return cass_statement_set_serial_consistency(pStatement, pSerialConsistency);

       }

       w_Cass_Error w_cassandra_statement_set_paging_size(_In_ w_cass_statement* pStatement, _In_ int pPageSize)
       {
           const char* _trace_info = "w_cassandra_statement_set_paging_size";
           if (!pStatement || !pPageSize)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
          return  cass_statement_set_paging_size(pStatement, pPageSize);

       }

       w_Cass_Error  w_cassandra_statement_set_paging_state(_In_ w_cass_statement* pStatement, _In_  const w_cass_result* pResult)
       {
           const char* _trace_info = "w_cassandra_statement_set_paging_state";
           if (!pStatement || !pResult)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

               return -1;
           }
           return cass_statement_set_paging_state(pStatement, pResult);

       }

       w_Cass_Error  w_cassandra_statement_set_paging_state_token(_In_ w_cass_statement* pStatement, _In_ const char* pPagingState, _In_ size_t pPagingStateSize)
       {
           const char* _trace_info = "w_cassandra_statement_set_paging_state_token";

           if (!pStatement || !pPagingState || !pPagingStateSize)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_statement_set_paging_state_token(pStatement, pPagingState, pPagingStateSize);

       }

       w_Cass_Error  w_cassandra_statement_set_timestamp(_In_ w_cass_statement* pStatement, _In_ int64_t pTimestamp)
       {
           const char* _trace_info = "w_cassandra_statement_set_timestamp";
           if (!pStatement || !pTimestamp)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_statement_set_timestamp(pStatement, pTimestamp);

       }
       w_Cass_Error w_cassandra_statement_set_request_timeout(_In_ w_cass_statement* pStatement, _In_ uint64_t pTimeoutMs)
       {
           const char* _trace_info = "w_cassandra_statement_set_request_timeout";
           if (!pStatement || !pTimeoutMs)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return   -1;
           }
           return  cass_statement_set_request_timeout(pStatement, pTimeoutMs);

       }

       w_Cass_Error  w_cassandra_statement_set_is_idempotent(_In_ w_cass_statement* pStatement, _In_ w_bool_t pIsIdempotent)
       {
           const char* _trace_info = "w_cassandra_statement_set_is_idempotent";
           if (!pStatement )
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_statement_set_is_idempotent(pStatement, pIsIdempotent);

       }

       w_Cass_Error w_cassandra_statement_set_retry_policy(_In_ w_cass_statement* pStatement, _In_ w_cass_retry_policy* pRetryPolicy)
       {
           const char* _trace_info = "w_cassandra_statement_set_retry_policy";
           if (!pStatement || !pRetryPolicy)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_statement_set_retry_policy(pStatement, pRetryPolicy);

       }

       w_Cass_Error w_cassandra_statement_set_custom_payload(_In_ w_cass_statement* pStatement, _In_ const w_cass_custom_payload* pPayload)
       {
           const char* _trace_info = "w_cassandra_statement_set_custom_payload";
           if (!pStatement || !pPayload)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_statement_set_custom_payload(pStatement, pPayload);

       }

       w_Cass_Error  w_cassandra_statement_set_execution_profile(_In_ w_cass_statement* pStatement, _In_ const char* pName)
       {
           const char* _trace_info = "w_cassandra_statement_set_execution_profile";
           if (!pStatement || !pName)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_statement_set_execution_profile(pStatement, pName);

       }

       w_Cass_Error  w_cassandra_statement_set_execution_profile_n(_In_ w_cass_statement* pStatement,_In_ const char* pName, _In_ size_t pNameLength)
       {
           const char* _trace_info = "w_cassandra_statement_set_execution_profile_n";
           if (!pStatement || !pName || !pNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_statement_set_execution_profile_n(pStatement, pName, pNameLength);

       }

       w_Cass_Error  w_cassandra_statement_set_tracing(_In_ w_cass_statement* pStatement, _In_ w_bool_t pEnabled)
       {
           const char* _trace_info = "w_cassandra_statement_set_tracing";
           if (!pStatement)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_statement_set_tracing(pStatement, pEnabled);

       }

       w_Cass_Error w_cassandra_statement_set_host(_In_ w_cass_statement* pStatement, _In_  const char* pHost, _In_ int pPort)
       {
           const char* _trace_info = "w_cassandra_statement_set_host";

           if (!pStatement ||!pHost || !pPort)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

               return -1;
           }

           return  cass_statement_set_host(pStatement, pHost, pPort);

       }

       w_Cass_Error  w_cassandra_statement_set_host_n(_In_ w_cass_statement* pStatement, _In_ const char* pHost, _In_ size_t pHostLength, _In_ int pPort)
       {
           const char* _trace_info = "w_cassandra_statement_set_host_n";
           if (!pStatement || !pHost || !pHostLength || !pPort)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }

           return   cass_statement_set_host_n(pStatement, pHost, pHostLength, pPort);

       }

       w_Cass_Error  w_cassandra_statement_set_host_inet(_In_ w_cass_statement* pStatement, _In_ const w_cass_inet* pHost, _In_ int pPort)
       {
           const char* _trace_info = "w_cassandra_statement_set_host_inet";
           if (!pStatement || !pHost || !pPort)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_statement_set_host_inet(pStatement, pHost, pPort);

       }

       w_Cass_Error  w_cassandra_statement_bind_null(_In_ w_cass_statement* pStatement, _In_ size_t pIndex)
       {
           const char* _trace_info = "w_cassandra_statement_bind_null";
           if (!pStatement || !pIndex)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

               return -1;
           }
           return   cass_statement_bind_null(pStatement, pIndex);

       }

       w_Cass_Error  w_cassandra_statement_bind_null_by_name(_In_ w_cass_statement* pStatement, _In_ const char* pName)
       {
           const char* _trace_info = "w_cassandra_statement_bind_null_by_name";
           if (!pStatement || !pName)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

               return -1;
           }
           return cass_statement_bind_null_by_name(pStatement, pName);

       }
       w_Cass_Error w_cassandra_statement_bind_null_by_name_n(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_ size_t pNameLength)
       {
           const char* _trace_info = "w_cassandra_statement_bind_null_by_name_n";
           if (!pStatement || !pName || !pNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_statement_bind_null_by_name_n(pStatement, pName, pNameLength);

       }

       w_Cass_Error  w_cassandra_statement_bind_int8(_In_ w_cass_statement* pStatement, _In_  size_t pIndex, _In_ int8_t pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_int8";
           if (!pStatement || !pIndex || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_statement_bind_int8(pStatement, pIndex, pValue);

       }

       w_Cass_Error  w_cassandra_statement_bind_int8_by_name(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_ int8_t pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_int8_by_name";
           if (!pStatement ||!pName || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return    cass_statement_bind_int8_by_name(pStatement, pName, pValue);

       }

       w_Cass_Error w_cassandra_statement_bind_int8_by_name_n(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_ size_t pNameLength, _In_ int8_t pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_int8_by_name_n";
           if (!pStatement || !pName || !pNameLength || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return    cass_statement_bind_int8_by_name_n(pStatement, pName, pNameLength, pValue);

       }

       w_Cass_Error  w_cassandra_statement_bind_int16(_In_ w_cass_statement* pStatement, _In_ size_t pIndex, _In_ int16_t pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_int16";
           if (!pStatement  || !pIndex || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return    cass_statement_bind_int16(pStatement, pIndex, pValue);

       }


       w_Cass_Error  w_cassandra_statement_bind_int16_by_name(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_ int16_t pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_int16_by_name";
           if (!pStatement || !pName || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }

           return  cass_statement_bind_int16_by_name(pStatement, pName, pValue);

       }

       w_Cass_Error  w_cassandra_statement_bind_int16_by_name_n(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_  size_t pNameLength, _In_ int16_t pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_int16_by_name_n";
           if (!pStatement || !pName || !pNameLength || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_statement_bind_int16_by_name_n(pStatement, pName, pNameLength, pValue);

       }

       w_Cass_Error  w_cassandra_statement_bind_int32(_In_ w_cass_statement* pStatement, _In_  size_t pIndex, _In_ int32_t pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_int32";

           if (!pStatement || !pIndex || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_statement_bind_int32(pStatement, pIndex, pValue);

       }

       w_Cass_Error  w_cassandra_statement_bind_int32_by_name(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_  int32_t pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_int32_by_name";
           if (!pStatement || !pValue || !pName)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_statement_bind_int32_by_name(pStatement, pName, pValue);
       }


       w_Cass_Error  w_cassandra_statement_bind_int32_by_name_n(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_ size_t pNameLength, _In_ int32_t pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_int32_by_name_n";
           if (!pStatement || !pName || !pNameLength || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

               return -1;
           }

           return cass_statement_bind_int32_by_name_n(pStatement, pName, pNameLength, pValue);

       }

       w_Cass_Error  w_cassandra_statement_bind_uint32_by_name(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_ uint32_t pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_uint32_by_name";
           if (!pStatement || !pName || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

               return -1;
           }

           return cass_statement_bind_uint32_by_name(pStatement, pName, pValue);
       }

       w_Cass_Error w_cassandra_statement_bind_uint32_by_name_n(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_ size_t pNameLength, _In_ uint32_t pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_uint32_by_name_n";
           if (!pStatement || !pName || !pNameLength ||!pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

               return -1;
           }
           return cass_statement_bind_uint32_by_name_n(pStatement, pName, pNameLength, pValue);
       }


       w_Cass_Error  w_cassandra_statement_bind_int64(_In_ w_cass_statement* pStatement, _In_ size_t pIndex, _In_ int64_t pValue)

       {
           const char* _trace_info = "w_cassandra_statement_bind_int64";
           if (!pStatement || !pIndex || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_statement_bind_int64(pStatement, pIndex, pValue);


       }

       w_Cass_Error   w_cassandra_statement_bind_int64_by_name(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_ uint64_t pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_int64_by_name";
           if (!pStatement || !pName || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_statement_bind_int64_by_name(pStatement, pName, pValue);

       }

       w_Cass_Error w_cassandra_statement_bind_int64_by_name_n(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_ size_t pNameLength, _In_  int64_t pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_int64_by_name_n";
           if (!pStatement || !pName || !pNameLength || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_statement_bind_int64_by_name_n(pStatement, pName, pNameLength, pValue);

       }

       w_Cass_Error w_cassandra_statement_bind_float(_In_ w_cass_statement* pStatement, _In_  size_t pIndex, _In_  float pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_float";
           if (!pStatement || !pIndex || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_statement_bind_float(pStatement, pIndex, pValue);

       }

       w_Cass_Error  w_cassandra_statement_bind_float_by_name(_In_ w_cass_statement* pStatement, _In_  const char* pName, _In_  float pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_float_by_name";
           if (!pStatement || !pName || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_statement_bind_float_by_name(pStatement, pName, pValue);

       }

       w_Cass_Error w_cassandra_statement_bind_float_by_name_n(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_ size_t pNameLength, _In_ float pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_float_by_name_n";
           if (!pStatement || !pName || !pNameLength || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_statement_bind_float_by_name_n(pStatement, pName, pNameLength, pValue);


       }

       w_Cass_Error  w_cassandra_statement_bind_double(_In_ w_cass_statement* pStatement, _In_ size_t pIndex, _In_ double pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_double";
           if (!pStatement || !pIndex || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return    cass_statement_bind_double(pStatement, pIndex, pValue);

       }

       w_Cass_Error  w_cassandra_statement_bind_double_by_name(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_  double pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_double_by_name";
           if (!pStatement || !pName || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return    cass_statement_bind_double_by_name(pStatement, pName, pValue);

       }

       w_Cass_Error  w_cassandra_statement_bind_double_by_name_n(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_ size_t pNameLength, _In_ double pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_double_by_name_n";
           if (!pStatement || !pName || !pNameLength || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_statement_bind_double_by_name_n(pStatement, pName, pNameLength, pValue);

       }

       w_Cass_Error w_cassandra_statement_bind_bool(_In_ w_cass_statement* pStatement, _In_  size_t pIndex, _In_  w_bool_t pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_bool";
           if (!pStatement || !pIndex )
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }

           return  cass_statement_bind_bool(pStatement, pIndex, pValue);

       }

       w_Cass_Error w_cassandra_statement_bind_bool_by_name(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_ w_bool_t pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_bool_by_name";
           if (!pStatement || !pName)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
         return cass_statement_bind_bool_by_name(pStatement, pName, pValue);

       }

       w_Cass_Error  w_cassandra_statement_bind_bool_by_name_n(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_ size_t pNameLength, _In_  w_bool_t pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_bool_by_name_n";
           if (!pStatement || !pName)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_statement_bind_bool_by_name_n(pStatement, pName, pNameLength, pValue);

       }

       w_Cass_Error w_cassandra_statement_bind_string(_In_ w_cass_statement* pStatement, _In_ size_t pIndex, _In_  const char* pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_string";
           if (!pStatement || !pIndex || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_statement_bind_string(pStatement, pIndex, pValue);
       }

       w_Cass_Error  w_cassandra_statement_bind_string_n(_In_ w_cass_statement* pStatement, _In_ size_t pIndex, _In_  const char* pValue, _In_ size_t pValueLength)
       {
           const char* _trace_info = "w_cassandra_statement_bind_string_n";
           if (!pStatement || !pIndex || !pValue || !pValueLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_statement_bind_string_n(pStatement, pIndex, pValue, pValueLength);

       }

       w_Cass_Error  w_cassandra_statement_bind_string_by_name(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_ const char* pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_string_by_name";
           if (!pStatement || !pName || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_statement_bind_string_by_name(pStatement, pName, pValue);

       }

       w_Cass_Error w_cassandra_statement_bind_string_by_name_n(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_  size_t pNameLength, _In_ const char* pValue, _In_  size_t pValueLength)
       {
           const char* _trace_info = "w_cassandra_statement_bind_string_by_name_n";
           if (!pStatement || !pName || !pNameLength || !pValue || !pValueLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
          return  cass_statement_bind_string_by_name_n(pStatement, pName, pNameLength, pValue, pValueLength);

       }

       w_Cass_Error  w_cassandra_statement_bind_bytes(_In_ w_cass_statement* pStatement, _In_ size_t pIndex, _In_  const w_byte_t* pValue, _In_ size_t pValueSize)
       {
           const char* _trace_info = "w_cassandra_statement_bind_bytes";
           if (!pStatement || !pIndex || !pValue || !pValueSize)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }

           return   cass_statement_bind_bytes(pStatement, pIndex, pValue, pValueSize);

       }

       w_Cass_Error  w_cassandra_statement_bind_bytes_by_name(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_ const w_byte_t* pValue, _In_  size_t pValueSize)
       {
           const char* _trace_info = "w_cassandra_statement_bind_bytes_by_name";
           if (!pStatement || !pName || !pValue || !pValueSize)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_statement_bind_bytes_by_name(pStatement, pName, pValue, pValueSize);

       }

       w_Cass_Error w_cassandra_statement_bind_bytes_by_name_n(_In_ w_cass_statement* pStatement, _In_  const char* pName, _In_ size_t pNameLength, _In_ const w_byte_t* pValue, _In_ size_t pValueSize)
       {
           const char* _trace_info = "w_cassandra_statement_bind_bytes_by_name_n";
           if (!pStatement || !pName || !pNameLength || !pValue || !pValueSize)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_statement_bind_bytes_by_name_n(pStatement, pName, pNameLength, pValue, pValueSize);

       }

       w_Cass_Error w_cassandra_statement_bind_custom(_In_ w_cass_statement* pStatement, _In_ size_t pIndex, _In_ const char* pClassName, _In_ const w_byte_t* pValue, _In_ size_t pValueSize)
       {
           const char* _trace_info = "w_cassandra_statement_bind_custom";
           if (!pStatement || !pIndex || !pClassName || !pValue || !pValueSize)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_statement_bind_custom(pStatement, pIndex, pClassName, pValue, pValueSize);

       }

       w_Cass_Error  w_cassandra_statement_bind_custom_n(_In_ w_cass_statement* pStatement, _In_  size_t pIndex, _In_ const char* pClassName, _In_ size_t pClassNameLength, _In_ const w_byte_t* pValue, _In_ size_t pValueSize)
       {
           const char* _trace_info = "w_cassandra_statement_bind_custom_n";
           if (!pStatement || !pIndex || !pClassName || !pClassNameLength || !pValue || !pValueSize)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_statement_bind_custom_n(pStatement, pIndex, pClassName, pClassNameLength, pValue, pValueSize);

       }


       w_Cass_Error w_cassandra_statement_bind_custom_by_name(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_ const char* pClassName, _In_ const w_byte_t* pValue, _In_ size_t pValueSize)
       {
           const char* _trace_info = "w_cassandra_statement_bind_custom_by_name";
           if (!pStatement || !pName || !pClassName || !pValue || !pValueSize)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

               return -1;
           }
           return   cass_statement_bind_custom_by_name(pStatement, pName, pClassName, pValue, pValueSize);

       }

       w_Cass_Error  w_cassandra_statement_bind_custom_by_name_n(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_ size_t pNameLength, _In_ const char* pClassName, _In_ size_t pClassNameLength, _In_ const w_byte_t* pValue, _In_ size_t pValueSize)
       {
           const char* _trace_info = "w_cassandra_statement_bind_custom_by_name_n";
           if (!pStatement || !pName || !pNameLength || !pClassName || !pClassNameLength || !pValue || !pValueSize)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_statement_bind_custom_by_name_n(pStatement, pName, pNameLength, pClassName, pClassNameLength, pValue, pValueSize);

       }

       w_Cass_Error  w_cassandra_statement_bind_uuid(_In_ w_cass_statement* pStatement, _In_ size_t pIndex, _In_ w_cass_uuid pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_uuid";
           if (!pStatement || !pIndex )
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_statement_bind_uuid(pStatement, pIndex, pValue);

       }

       w_Cass_Error w_cassandra_statement_bind_uuid_by_name(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_ w_cass_uuid pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_uuid_by_name";
           if (!pStatement || !pName)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_statement_bind_uuid_by_name(pStatement, pName, pValue);

       }

       w_Cass_Error w_cassandra_statement_bind_uuid_by_name_n(_In_ w_cass_statement* pStatement, _In_  const char* pName, _In_ size_t pNameLength, _In_  w_cass_uuid pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_uuid_by_name_n";
           if (!pStatement || !pName || !pNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_statement_bind_uuid_by_name_n(pStatement, pName, pNameLength, pValue);

       }

       w_Cass_Error  w_cassandra_statement_bind_inet(_In_ w_cass_statement* pStatement, _In_ size_t pIndex, _In_ w_cass_inet pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_inet";
           if (!pStatement)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_statement_bind_inet(pStatement, pIndex, pValue);

       }

       w_Cass_Error w_cassandra_statement_bind_inet_by_name(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_ w_cass_inet pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_inet_by_name";
           if (!pStatement || !pName)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_statement_bind_inet_by_name(pStatement, pName, pValue);

       }

       w_Cass_Error w_cassandra_statement_bind_inet_by_name_n(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_ size_t pNameLength, _In_ w_cass_inet pValue)
       {
           const char* _trace_info = "w_cassandra_statement_bind_inet_by_name_n";
           if (!pStatement || !pName || !pNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_statement_bind_inet_by_name_n(pStatement, pName, pNameLength, pValue);

       }

       w_Cass_Error w_cassandra_statement_bind_decimal(_In_ w_cass_statement* pStatement, _In_ size_t pIndex, _In_ const w_byte_t* pVarint, _In_ size_t pVarintSize, _In_ int32_t pScale)
       {
           const char* _trace_info = "w_cassandra_statement_bind_decimal";
           if (!pStatement || !pIndex || !pScale)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }

           return cass_statement_bind_decimal(pStatement, pIndex, pVarint, pVarintSize, pScale);

       }

       w_Cass_Error  w_cassandra_statement_bind_decimal_by_name(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_ const w_byte_t* pVarint, _In_ size_t pVarintSize, _In_ int32_t pScale)
       {
           const char* _trace_info = "w_cassandra_statement_bind_decimal_by_name";
           if (!pStatement || !pName || !pVarintSize || !pScale)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_statement_bind_decimal_by_name(pStatement, pName, pVarint, pVarintSize, pScale);

       }

       w_Cass_Error  w_cassandra_statement_bind_decimal_by_name_n(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_ size_t pNameLength, _In_  const w_byte_t* pVarint, _In_ size_t pVarintSize, _In_  int32_t pScale)
       {
           const char* _trace_info = "w_cassandra_statement_bind_decimal_by_name_n";
           if (!pStatement || !pName || !pNameLength ||!pVarintSize || !pScale)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

               return -1;
           }
           return  cass_statement_bind_decimal_by_name_n(pStatement, pName, pNameLength, pVarint, pVarintSize, pScale);

       }

       w_Cass_Error w_cassandra_statement_bind_duration(_In_ w_cass_statement* pStatement, _In_ size_t pIndex, _In_ int32_t pMonths, _In_ int32_t pDays, _In_ int64_t pNanos)
       {
           const char* _trace_info = "w_cassandra_statement_bind_duration";
           if (!pStatement || !pIndex || !pMonths || !pDays || !pNanos)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }

           return   cass_statement_bind_duration(pStatement, pIndex, pMonths, pDays, pNanos);

       }

       w_Cass_Error  w_cassandra_statement_bind_duration_by_name(_In_ w_cass_statement* pStatement, _In_  const char* pName, _In_ int32_t pMonths, _In_  int32_t pDays, _In_ int64_t pNanos)
       {
           const char* _trace_info = "w_cassandra_statement_bind_duration_by_name";
           if (!pStatement || !pName || !pMonths ||!pDays || !pNanos)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_statement_bind_duration_by_name(pStatement, pName, pMonths, pDays, pNanos);

       }

       w_Cass_Error  w_cassandra_statement_bind_duration_by_name_n(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_ size_t pNameLength, _In_ int32_t pMonths, _In_ int32_t pDays, _In_ int64_t pNanos)
       {
           const char* _trace_info = "w_cassandra_statement_bind_duration_by_name_n";
           if (!pStatement || !pName || !pNameLength || !pMonths || !pDays || !pNanos)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_statement_bind_duration_by_name_n(pStatement, pName, pNameLength, pMonths, pDays, pNanos);

       }

       w_Cass_Error  w_cassandra_statement_bind_collection(_In_ w_cass_statement* pStatement, _In_ size_t pIndex, _In_ const w_cass_collection* pCollection)
       {
           const char* _trace_info = "w_cassandra_statement_bind_collection";
           if (!pStatement || !pCollection || !pIndex)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_statement_bind_collection(pStatement, pIndex, pCollection);

       }

       w_Cass_Error  w_cassandra_statement_bind_collection_by_name(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_ const w_cass_collection* pCollection)
       {
           const char* _trace_info = "w_cassandra_statement_bind_collection_by_name";
           if (!pStatement || !pName || !pCollection)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }

           return cass_statement_bind_collection_by_name(pStatement, pName, pCollection);

       }
       w_Cass_Error w_cassandra_statement_bind_collection_by_name_n(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_ size_t pNameLength, _In_ const w_cass_collection* pCollection)
       {
           const char* _trace_info = "w_cassandra_statement_bind_collection_by_name_n";
           if (!pStatement || !pName || !pNameLength || !pCollection)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_statement_bind_collection_by_name_n(pStatement, pName, pNameLength, pCollection);

       }

       w_Cass_Error  w_cassandra_statement_bind_tuple(_In_ w_cass_statement* pStatement, _In_ size_t pIndex, _In_ const w_cass_tuple* pTuple)
       {
           const char* _trace_info = "w_cassandra_statement_bind_tuple";
           if (!pStatement || !pIndex || !pTuple)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_statement_bind_tuple(pStatement, pIndex, pTuple);

       }

       w_Cass_Error  w_cassandra_statement_bind_tuple_by_name(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_ const w_cass_tuple* pTuple)
       {
           const char* _trace_info = "w_cassandra_statement_bind_tuple_by_name";
           if (!pStatement || !pName || !pTuple )
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_statement_bind_tuple_by_name(pStatement, pName, pTuple);

       }

       w_Cass_Error w_cassandra_statement_bind_tuple_by_name_n(_In_ w_cass_statement* pStatement, _In_  const char* pName, _In_ size_t pNameLength, _In_ const w_cass_tuple* pTuple)
       {
           const char* _trace_info = "w_cassandra_statement_bind_tuple_by_name_n";
           if (!pStatement || !pName || !pNameLength || !pTuple)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_statement_bind_tuple_by_name_n(pStatement, pName, pNameLength, pTuple);
       }

       w_Cass_Error w_cassandra_statement_bind_user_type(_In_ w_cass_statement* pStatement, _In_ size_t pIndex, _In_ const w_cass_user_type* pUserType)
       {
           const char* _trace_info = "w_cassandra_statement_bind_user_type";
           if (!pStatement || !pIndex || !pUserType)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_statement_bind_user_type(pStatement, pIndex, pUserType);
       }

       w_Cass_Error w_cassandra_statement_bind_user_type_by_name(_In_ w_cass_statement* pStatement, _In_  const char* pName, _In_ const w_cass_user_type* pUserType)
       {
           const char* _trace_info = "w_cassandra_statement_bind_user_type_by_name";
           if (!pStatement || !pName || !pUserType)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_statement_bind_user_type_by_name(pStatement, pName, pUserType);
       }

       w_Cass_Error  w_cassandra_statement_bind_user_type_by_name_n(_In_ w_cass_statement* pStatement, _In_ const char* pName, _In_ size_t pNameLength, _In_ const w_cass_user_type* pUserType)
       {
           const char* _trace_info = "w_cassandra_statement_bind_user_type_by_name_n";
           if (!pStatement || !pName || !pUserType  || !pUserType)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_statement_bind_user_type_by_name_n(pStatement, pName, pNameLength, pUserType);
       }

       void w_cassandra_prepared_free(_In_ const w_cass_prepared* pPrepared)
       {
           const char* _trace_info = "w_cassandra_prepared_free";
           if (!pPrepared)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           }
           cass_prepared_free(pPrepared);

       }

       w_cass_statement* w_cassandra_prepared_bind(_In_ const w_cass_prepared* pPrepared)
       {
           const char* _trace_info = "w_cassandra_prepared_bind";
           if (!pPrepared)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return   cass_prepared_bind(pPrepared);

       }

       w_Cass_Error w_cassandra_prepared_parameter_name(_In_ const w_cass_prepared* pPrepared, _In_  size_t pIndex, _Inout_ const char** pName, _Inout_ size_t* pNameLength)
       {
           const char* _trace_info = "w_cassandra_prepared_parameter_name";
           if (!pPrepared || !pNameLength || !pIndex || !pName)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_prepared_parameter_name(pPrepared, pIndex, pName, pNameLength);
       }

       const w_cass_data_type* w_cassandra_prepared_parameter_data_type(_In_ const  w_cass_prepared* pPrepared, _In_  size_t pIndex)
       {
           const char* _trace_info = "w_cassandra_prepared_parameter_data_type";
           if (!pPrepared || !pIndex)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return cass_prepared_parameter_data_type(pPrepared, pIndex);
       }

       const w_cass_data_type* w_cassandra_prepared_parameter_data_type_by_name(_In_ const w_cass_prepared* pPrepared, _In_ const char* pName)
       {
           const char* _trace_info = "w_cassandra_prepared_parameter_data_type_by_name";
           if (!pPrepared || !pName)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
               return cass_prepared_parameter_data_type_by_name(pPrepared, pName);
       }

       const w_cass_data_type* w_cassandra_prepared_parameter_data_type_by_name_n(_In_ const w_cass_prepared* pPrepared, _In_  const char* pName, _In_ size_t pNameLength)
       {
           const char* _trace_info = "w_cassandra_prepared_parameter_data_type_by_name_n";
           if (!pPrepared || !pName || !pNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
               return cass_prepared_parameter_data_type_by_name_n(pPrepared, pName, pNameLength);

       }

       w_cass_batch* w_cassandra_batch_new(_In_ w_cass_batch_type pType)
       {
           
               return cass_batch_new(pType);
       }

       void w_cassandra_batch_free(_In_ w_cass_batch* pBatch)
       {
           const char* _trace_info = "w_cassandra_batch_free";
           if (!pBatch)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

           }
           cass_batch_free(pBatch);

       }

       w_Cass_Error  w_cass_batch_set_keyspace(_In_ w_cass_batch* pBatch, _In_ const char* pKeyspace)
       {
           const char* _trace_info = "w_cass_batch_set_keyspace";
           if (!pBatch || !pKeyspace)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_batch_set_keyspace(pBatch, pKeyspace);
       }

       w_Cass_Error w_cassandra_batch_set_keyspace_n(_In_ w_cass_batch* pBatch, _In_  const char* pKeyspace, _In_ size_t pKeyspaceLength)
       {
           const char* _trace_info = "w_cassandra_batch_set_keyspace_n";
           if (!pBatch || !pKeyspace || !pKeyspaceLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_batch_set_keyspace_n(pBatch, pKeyspace, pKeyspaceLength);

       }

       w_Cass_Error w_cassandra_batch_set_consistency(_In_ w_cass_batch* pBatch, _In_ w_cass_consistency pConsistency)
       {
           const char* _trace_info = "w_cassandra_batch_set_consistency";
           if (!pBatch || !pConsistency)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }

           return cass_batch_set_consistency(pBatch, pConsistency);

       }

       w_Cass_Error w_cassandra_batch_set_serial_consistency(_In_ w_cass_batch* pBatch, _In_  w_cass_consistency pSerialConsistency)
       {
           const char* _trace_info = "w_cassandra_batch_set_serial_consistency";
           if (!pBatch || pSerialConsistency)
           {        
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

               return -1;
           }

           return 
               cass_batch_set_serial_consistency(pBatch, pSerialConsistency);
       }

       w_Cass_Error w_cassandra_batch_set_timestamp(_In_ w_cass_batch* pBatch, _In_ int64_t pTimeStamp)
       {
           const char* _trace_info = "w_cassandra_batch_set_timestamp";
           if (!pBatch || !pTimeStamp)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_batch_set_timestamp(pBatch, pTimeStamp);

       }

       w_Cass_Error w_cassandra_batch_set_request_timeout(_In_ w_cass_batch* pBatch, _In_ uint64_t pTimeOutms)
       {
           const char* _trace_info = "w_cassandra_batch_set_request_timeout";
           if (!pBatch)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_batch_set_request_timeout(pBatch, pTimeOutms);


       }

       w_Cass_Error  w_cassandra_batch_set_is_idempotent(_In_ w_cass_batch* pBatch, _In_  w_bool_t pIsIdempotent)
       {
           const char* _trace_info = "w_cassandra_batch_set_is_idempotent";
           if (!pBatch)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_batch_set_is_idempotent(pBatch, pIsIdempotent);

       }

       w_Cass_Error w_cassandra_batch_set_retry_policy(_In_ w_cass_batch* pBatch, _In_ w_cass_retry_policy* pRetryPolicy)
       {
           const char* _trace_info = "w_cassandra_batch_set_retry_policy";
           if (!pBatch || !pRetryPolicy)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_batch_set_retry_policy(pBatch, pRetryPolicy);

       }

       w_Cass_Error  w_cassandra_batch_set_custom_payload(_In_ w_cass_batch* pBatch, _In_ const w_cass_custom_payload* pPayload)
       {
           const char* _trace_info = "w_cassandra_batch_set_custom_payload";
           if (!pBatch || !pPayload)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_batch_set_custom_payload(pBatch, pPayload);

       }

       w_Cass_Error w_cassandra_batch_set_tracing(_In_ w_cass_batch* pBatch, _In_ w_bool_t pEnabled)
       {
           const char* _trace_info = "w_cassandra_batch_set_tracing";
           if (!pBatch)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_batch_set_tracing(pBatch, pEnabled);

       }

       w_Cass_Error  w_cassandra_batch_add_statement(_In_ w_cass_batch* pBatch, _In_ w_cass_statement* pStatement)
       {
           const char* _trace_info = "w_cassandra_batch_add_statement";
           if (!pBatch || !pStatement)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           } 
           return cass_batch_add_statement(pBatch, pStatement);

       }

       w_Cass_Error  w_cassandra_batch_set_execution_profile(_In_ w_cass_batch* pBatch, _In_ const char* pName)
       {
           const char* _trace_info = "w_cassandra_batch_set_execution_profile";
           if (!pBatch || !pName)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_batch_set_execution_profile(pBatch, pName);

       }

       w_Cass_Error w_cassandra_batch_set_execution_profile_n(_In_ w_cass_batch* pBatch, _In_ const char* pName, _In_ size_t pNameLength)
       {
           const char* _trace_info = "w_cassandra_batch_set_execution_profile_n";
           if (!pBatch || !pName || !pNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_batch_set_execution_profile_n(pBatch, pName, pNameLength);

       }

       w_cass_data_type * w_cassandra_data_type_new(_In_ w_cass_value_type pType)

       {
           return cass_data_type_new(pType);

       }

       w_cass_data_type* w_cassandra_data_type_new_from_existing(_In_ const w_cass_data_type* pDataType)
       {
           const char* _trace_info = "w_cassandra_data_type_new_from_existing";
           if (!pDataType)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return cass_data_type_new_from_existing(pDataType);

       }

       w_cass_data_type* w_cassandra_data_type_new_tuple(_In_ size_t ptemCount)
       {
           const char* _trace_info = "w_cassandra_data_type_new_tuple";
           if (!ptemCount)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return cass_data_type_new_tuple(ptemCount);

       }

       w_cass_data_type* w_cassandra_data_type_new_udt(_In_ size_t pFieldCount)
       {
           const char* _trace_info = "w_cassandra_data_type_new_udt";
           if (!pFieldCount)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return cass_data_type_new_udt(pFieldCount);

       }

       void w_cassandra_data_type_free(_In_ w_cass_data_type* pDataType)
       {
           const char* _trace_info = "w_cassandra_data_type_free";
           if (!pDataType)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

           }
            cass_data_type_free(pDataType);
       }

       w_cass_value_type w_cassandra_data_type_type(_In_ const w_cass_data_type* pDataType)
       {
           const char* _trace_info = "w_cassandra_data_type_type";
           if (!pDataType)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_data_type_type(pDataType);

       }

       w_bool_t w_cassandra_data_type_is_frozen(_In_ const w_cass_data_type* pDataType)
       {
           const char* _trace_info = "w_cassandra_data_type_is_frozen";
           if (!pDataType)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return false;
           }
           return cass_data_type_is_frozen(pDataType);

       }
       w_Cass_Error w_cassandra_data_type_type_name(_In_ const w_cass_data_type* pDataType, _Inout_  const char** pTypeName, _Inout_ size_t* pTypeNameLength)
       {
           const char* _trace_info = "w_cassandra_data_type_type_name";
           if (!pDataType || !pTypeName || !pTypeNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_data_type_type_name(pDataType, pTypeName, pTypeNameLength);
       }
      
       w_Cass_Error w_cassandra_data_type_set_type_name(_In_ w_cass_data_type* pDataType, _In_ const char* pTypeName)
       {
           const char* _trace_info = "w_cassandra_data_type_set_type_name";
           if (!pDataType || !pTypeName)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_data_type_set_type_name(pDataType, pTypeName);

       }

       w_Cass_Error w_cassandra_data_type_set_type_name_n(_In_ w_cass_data_type* pDataType, _In_ const char* pTypeName, _In_ size_t pTypeNameLength)
       {
           const char* _trace_info = "w_cassandra_data_type_set_type_name_n";
           if (!pDataType || !pTypeName || !pTypeNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_data_type_set_type_name_n(pDataType, pTypeName, pTypeNameLength);

       }

       w_Cass_Error w_cassandra_data_type_keyspace(_In_ const w_cass_data_type* pDataType, _Inout_ const char** pKeyspace, _Inout_ size_t* pKeypaceLength)
       {
           const char* _trace_info = "w_cassandra_data_type_keyspace";
           if (!pDataType || !pKeyspace || !pKeypaceLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_data_type_keyspace(pDataType, pKeyspace, pKeypaceLength);

       }

       w_Cass_Error w_cassandra_data_type_set_keyspace(_In_ w_cass_data_type* pDataType, _In_ const char* pKeyspace)
       {
           const char* _trace_info = "w_cassandra_data_type_set_keyspace";
           if (!pDataType || !pKeyspace)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_data_type_set_keyspace(pDataType, pKeyspace);

       }

       w_Cass_Error  w_cassandra_data_type_set_keyspace_n(_In_ w_cass_data_type* pDataType, _In_ const char* pKeypace, _In_ size_t pKeypaceLength)
       {
           const char* _trace_info = "w_cassandra_data_type_set_keyspace_n";
           if (!pDataType || !pKeypace || !pKeypaceLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_data_type_set_keyspace_n(pDataType, pKeypace, pKeypaceLength);

       }

       w_Cass_Error w_cassandra_data_type_class_name(_In_ const w_cass_data_type* pDataType, _Inout_ const char** pClassName, _Inout_ size_t* pClassNameLength)
       {
           const char* _trace_info = "w_cassandra_data_type_class_name";
           if (!pDataType || !pClassName || !pClassNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_data_type_class_name(pDataType, pClassName, pClassNameLength);

       }

       w_Cass_Error w_cassandra_data_type_set_class_name(_In_ w_cass_data_type* pDataType, _In_ const char* pClassName)
       {
           const char* _trace_info = "w_cassandra_data_type_set_class_name";
           if (!pDataType || !pClassName)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_data_type_set_class_name(pDataType, pClassName);

       }

       w_Cass_Error  w_cassandra_data_type_set_class_name_n(_In_ w_cass_data_type* pDataType, _In_ const char* pClassName, _In_ size_t pClassNameLength)
       {
           const char* _trace_info = "w_cassandra_data_type_set_class_name_n";
           if (!pDataType || !pClassName)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_data_type_set_class_name_n(pDataType, pClassName, pClassNameLength);

       }

       size_t w_cassandra_data_type_sub_type_count(_In_ const w_cass_data_type* pDataType)
       {
           const char* _trace_info = "w_cassandra_data_type_sub_type_count";
           if (!pDataType)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return 0;
           }
           return  cass_data_type_sub_type_count(pDataType);

       }
       /*deprecated*/
       size_t  w_cassandra_data_sub_type_count(const w_cass_data_type* pDataType)
       {
           const char* _trace_info = "w_cassandra_data_sub_type_count";
           if (!pDataType)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return 0;
           }
           return  cass_data_sub_type_count(pDataType);

       }

       const w_cass_data_type*  w_cassandra_data_type_sub_data_type(_In_ const w_cass_data_type* pDataType, _In_ size_t pIndex)
       {
           const char* _trace_info = "w_cassandra_data_type_sub_data_type";
           if (!pDataType || !pIndex)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return  cass_data_type_sub_data_type(pDataType, pIndex);

       }

       const w_cass_data_type* w_cassandra_data_type_sub_data_type_by_name(_In_ const w_cass_data_type* pDataType, _In_ const char* pName)
       {
           const char* _trace_info = "w_cassandra_data_type_sub_data_type_by_name";
           if (!pDataType || !pName)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return  cass_data_type_sub_data_type_by_name(pDataType, pName);

       }

       const w_cass_data_type* w_cassandra_data_type_sub_data_type_by_name_n(_In_ const w_cass_data_type* pDataType, _In_ const char* pName, _In_ size_t pNameLength)
       {
           const char* _trace_info = "w_cassandra_data_type_sub_data_type_by_name_n";
           if (!pDataType || !pName || !pNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return cass_data_type_sub_data_type_by_name_n(pDataType, pName, pNameLength);

       }

       w_Cass_Error w_cassandra_data_type_sub_type_name(_In_ const w_cass_data_type* pDataType, _In_ size_t pIndex, _In_  const char** pName, _In_  size_t* pNameLength)
       {
           const char* _trace_info = "w_cassandra_data_type_sub_type_name";
           if (!pDataType || !pName || !pIndex || !pNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_data_type_sub_type_name(pDataType, pIndex, pName, pNameLength);

       }

       w_Cass_Error  w_cassandra_data_type_add_sub_type(_In_ w_cass_data_type* pDataType, _In_ const w_cass_data_type* pSubDataType)
       {
           const char* _trace_info = "w_cassandra_data_type_add_sub_type";

           if (!pDataType || !pSubDataType)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_data_type_add_sub_type(pDataType, pSubDataType);

       }

       w_Cass_Error w_cassandra_data_type_add_sub_type_by_name(_In_ w_cass_data_type* pDataType, _In_ const char* pName, _In_ const w_cass_data_type* pSubDataType)
       {
           const char* _trace_info = "w_cassandra_data_type_add_sub_type_by_name";
           if (!pDataType || !pName || !pSubDataType)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }

           return cass_data_type_add_sub_type_by_name(pDataType, pName, pSubDataType);

       }

       w_Cass_Error  w_cassandra_data_type_add_sub_type_by_name_n(_In_ w_cass_data_type* pDataType, _In_  const char* pName, _In_  size_t pNameLength, _In_  const w_cass_data_type* pSubDataType)
       {
           const char* _trace_info = "w_cassandra_data_type_add_sub_type_by_name_n";
           if (!pDataType || !pName)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_data_type_add_sub_type_by_name_n(pDataType, pName, pNameLength, pSubDataType);
       }

       w_Cass_Error w_cassandra_data_type_add_sub_value_type(_In_ w_cass_data_type* pDataType, _In_ w_cass_value_type pSubValueType)
       {
           const char* _trace_info = "w_cassandra_data_type_add_sub_value_type";
           if (!pDataType)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_data_type_add_sub_value_type(pDataType, pSubValueType);

       }

       w_Cass_Error   w_cassandra_data_type_add_sub_value_type_by_name(_In_ w_cass_data_type* pDataType, _In_  const char* pName, _In_  w_cass_value_type pSubValueType)
       {
           const char* _trace_info = "w_cassandra_data_type_add_sub_value_type_by_name";
           if (!pDataType || !pName )
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_data_type_add_sub_value_type_by_name(pDataType, pName, pSubValueType);

       }
       
       w_Cass_Error  w_cassandra_data_type_add_sub_value_type_by_name_n(_In_ w_cass_data_type* pDataType, _In_  const char* pName, _In_  size_t pNameLength, _In_  w_cass_value_type sub_value_type)
       {
           const char* _trace_info = "w_cassandra_data_type_add_sub_value_type_by_name_n";
           if (!pDataType || !pName || !pNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_data_type_add_sub_value_type_by_name_n(pDataType, pName, pNameLength, sub_value_type);

       }

       w_cass_collection*  w_cassandra_collection_new(_In_ w_cass_collection_type pType, _In_ size_t pItemCount)
       {
           const char* _trace_info = "w_cassandra_collection_new";
           if (!pItemCount)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return   cass_collection_new(pType, pItemCount);

       }

       w_cass_collection*   w_cassandra_collection_new_from_data_type(_In_ const w_cass_data_type* pDataType, _In_ size_t pItemCount)
       {
           const char* _trace_info = "w_cassandra_collection_new_from_data_type";
           if (!pDataType || !pItemCount)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
               
           }
           
           return  cass_collection_new_from_data_type(pDataType, pItemCount);

       }

       void  w_cassandra_collection_free(_In_ w_cass_collection* pCollection)
       {
           const char* _trace_info = "w_cassandra_collection_free";
           if (!pCollection)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           }
           cass_collection_free(pCollection);

       }

       const w_cass_data_type* w_cassandra_collection_data_type(_In_ const w_cass_collection* pCollection)
       {
           const char* _trace_info = "w_cassandra_collection_data_type";
           if (!pCollection)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return cass_collection_data_type(pCollection);

       }

       w_Cass_Error w_cassandra_collection_append_int8(_In_ w_cass_collection* pCollection, _In_ int8_t pValue)
       {
           const char* _trace_info = "w_cassandra_collection_append_int8";
           if (!pCollection || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_collection_append_int8(pCollection, pValue);

       }

       w_Cass_Error  w_cassandra_collection_append_int16(_In_ w_cass_collection* pCollection, _In_ int16_t pValue)
       {
           const char* _trace_info = "w_cassandra_collection_append_int16";
           if (!pCollection || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_collection_append_int16(pCollection, pValue);

       }

       w_Cass_Error  w_cassandra_collection_append_int32(_In_ w_cass_collection* pCollection, _In_ int32_t pValue)
       {
           const char* _trace_info = "w_cassandra_collection_append_int32";
           if (!pCollection || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_collection_append_int32(pCollection, pValue);

       }

       w_Cass_Error w_cassandra_collection_append_uint32(_In_ w_cass_collection* pCollection, _In_ uint32_t pValue)
       {
           const char* _trace_info = "w_cassandra_collection_append_uint32";
           if (!pCollection || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_collection_append_uint32(pCollection, pValue);

       }

       w_Cass_Error  w_cassandra_collection_append_int64(_In_ w_cass_collection* pCollection, _In_ int64_t pValue)
       {
           const char* _trace_info = "w_cassandra_collection_append_int64";
           if (!pCollection || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_collection_append_int64(pCollection, pValue);

       }

       w_Cass_Error  w_cassandra_collection_append_float(_In_ w_cass_collection* pCollection, _In_ float pValue)
       {
           const char* _trace_info = "w_cassandra_collection_append_float";
           if (!pCollection || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_collection_append_float(pCollection, pValue);

       }

       w_Cass_Error   w_cassandra_collection_append_double(_In_ w_cass_collection* pCollection, _In_ double pValue)
       {
           const char* _trace_info = "w_cassandra_collection_append_double";
           if (!pCollection || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_collection_append_double(pCollection, pValue);

       }

       w_Cass_Error  w_cassandra_collection_append_bool(_In_ w_cass_collection* pCollection, _In_ w_bool_t pValue)
       {
           const char* _trace_info = "w_cassandra_collection_append_bool";
           if (!pCollection )
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_collection_append_bool(pCollection, pValue);

       }

       w_Cass_Error  w_cassandra_collection_append_string(_In_ w_cass_collection* pCollection, _In_  const char* pValue)
       {
           const char* _trace_info = "w_cassandra_collection_append_string";
           if (!pCollection || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_collection_append_string(pCollection, pValue);

       }

       w_Cass_Error w_cassandra_collection_append_string_n(_In_ w_cass_collection* pCollection, _In_  const char* pValue, _In_ size_t pValueLength)
       {
           const char* _trace_info = "w_cassandra_collection_append_string_n";
           if (!pCollection || !pValue || !pValueLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_collection_append_string_n(pCollection, pValue, pValueLength);

       }

       w_Cass_Error w_cassandra_collection_append_bytes(_In_ w_cass_collection* pCollection, _In_  const w_byte_t* pValue, _In_ size_t pValueSize)
       {
           const char* _trace_info = "w_cassandra_collection_append_bytes";
           if (!pCollection || !pValue || !pValueSize)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_collection_append_bytes(pCollection, pValue, pValueSize);

       }

       w_Cass_Error  w_cassandra_collection_append_custom(_In_ w_cass_collection* pCollection, _In_ const char* pClassName, _In_  const w_byte_t* pValue, _In_ size_t pValueSize)
       {
           const char* _trace_info = "w_cassandra_collection_append_custom";
           if (!pCollection || !pClassName || !pValue || !pValueSize)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_collection_append_custom(pCollection, pClassName, pValue, pValueSize);

       }

       w_Cass_Error  w_cassandra_collection_append_custom_n(_In_ w_cass_collection* pCollection, _In_  const char* pClassName, _In_ size_t pClassNameLength, _In_ const w_byte_t* pValue, _In_ size_t pValueSize)
       {
           const char* _trace_info = "w_cassandra_collection_append_custom_n";
           if (!pCollection || !pClassName || !pClassNameLength || !pValue || !pValueSize)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_collection_append_custom_n(pCollection, pClassName, pClassNameLength, pValue, pValueSize);
       }

       w_Cass_Error w_cassandra_collection_append_uuid(_In_ w_cass_collection* pCollection, _In_ w_cass_uuid pValue)
       {
           const char* _trace_info = "w_cassandra_collection_append_uuid";
           if (!pCollection )
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_collection_append_uuid(pCollection, pValue);

       }

       w_Cass_Error  w_cassandra_collection_append_inet(_In_ w_cass_collection* pCollection, _In_ w_cass_inet pValue)
       {
           const char* _trace_info = "w_cassandra_collection_append_inet";
           if (!pCollection)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_collection_append_inet(pCollection, pValue);

       }

       w_Cass_Error  w_cassandra_collection_append_decimal(_In_ w_cass_collection* pCollection, _In_ const w_byte_t* pVarint, _In_ size_t pVarintSize, _In_ int32_t pScale)
       {
           const char* _trace_info = "w_cassandra_collection_append_decimal";
           if (!pCollection || !pVarint || !pVarintSize || !pScale)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_collection_append_decimal(pCollection, pVarint, pVarintSize, pScale);

       }

       w_Cass_Error  w_cassandra_collection_append_duration(_In_ w_cass_collection* pCollection, _In_ int32_t pMonths, _In_ int32_t pDays, _In_ int64_t pNanos)
       {
           const char* _trace_info = "w_cassandra_collection_append_duration";
           if (!pCollection || !pMonths || !pDays || !pNanos)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_collection_append_duration(pCollection, pMonths, pDays, pNanos);

       }

       w_Cass_Error  w_cassandra_collection_append_collection(_In_ w_cass_collection* pCollection, _In_ const w_cass_collection* pValue)
       {
           const char* _trace_info = "w_cassandra_collection_append_collection";
           if (!pCollection || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_collection_append_collection(pCollection, pValue);

       }

       w_Cass_Error  w_cassandra_collection_append_tuple(_In_ w_cass_collection* pCollection, _In_ const w_cass_tuple* pValue)
       {
           const char* _trace_info = "w_cassandra_collection_append_tuple";
           if (!pCollection || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_collection_append_tuple(pCollection, pValue);

       }

       w_Cass_Error  w_cassandra_collection_append_user_type(_In_ w_cass_collection* pCollection, _In_ const w_cass_user_type* pValue)
       {
           const char* _trace_info = "w_cassandra_collection_append_user_type";
           if (!pCollection || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_collection_append_user_type(pCollection, pValue);

       }

       w_cass_tuple* w_cassandra_tuple_new(_In_ size_t pItemCount)
       {
           const char* _trace_info = "w_cassandra_tuple_new";
           if (!pItemCount)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
          
           return   cass_tuple_new(pItemCount);

       }

       w_cass_tuple* w_cassandra_tuple_new_from_data_type(_In_ const w_cass_data_type* pDataType)
       {
           const char* _trace_info = "w_cassandra_tuple_new_from_data_type";
           if (!pDataType)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return  cass_tuple_new_from_data_type(pDataType);

       }

       void  w_cassandra_tuple_free(_In_ w_cass_tuple* pTuple)
       {
           const char* _trace_info = "w_cassandra_tuple_free";
           if (!pTuple)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           }
       
           cass_tuple_free(pTuple);

       }

       const w_cass_data_type* w_cassandra_tuple_data_type(_In_ const w_cass_tuple* pTuple)
       {
           const char* _trace_info = "w_cassandra_tuple_data_type";
           if (!pTuple)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return  cass_tuple_data_type(pTuple);

       }

       w_Cass_Error w_cassandra_tuple_set_null(_In_ w_cass_tuple* pTuple, _In_  size_t pIndex)
       {
           const char* _trace_info = "w_cassandra_tuple_set_null";
           if (!pTuple || !pIndex)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_tuple_set_null(pTuple, pIndex);

       }

       w_Cass_Error w_cassandra_tuple_set_int8(_In_ w_cass_tuple* pTuple, _In_ size_t pIndex,int8_t pValue)
       {
           const char* _trace_info = "w_cassandra_tuple_set_int8";
           if (!pTuple || !pIndex || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_tuple_set_int8(pTuple, pIndex, pValue);

       }

       w_Cass_Error w_cassandra_tuple_set_int16(_In_ w_cass_tuple* pTuple, _In_ size_t pIndex, _In_ int16_t pValue)
       {
           const char* _trace_info = "w_cassandra_tuple_set_int16";
           if(!pTuple || !pValue || !pIndex)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_tuple_set_int16(pTuple, pIndex, pValue);

       }

       w_Cass_Error w_cassandra_tuple_set_int32(_In_ w_cass_tuple* pTuple, _In_ size_t pIndex, _In_ int32_t pValue)
       {
           const char* _trace_info = "w_cassandra_tuple_set_int32";
           if (!pTuple || !pIndex || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_tuple_set_int32(pTuple, pIndex, pValue);

       }

       w_Cass_Error   w_cassandra_tuple_set_uint32(_In_ w_cass_tuple* pTuple, _In_ size_t pIndex, _In_  uint32_t pValue)
       {
           const char* _trace_info = "w_cassandra_tuple_set_uint32";
           if (!pTuple || !pIndex || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_tuple_set_uint32(pTuple, pIndex, pValue);

       }

       w_Cass_Error w_cassandra_tuple_set_int64(_In_ w_cass_tuple* pTuple, _In_ size_t pIndex, _In_ int64_t pValue)
       {
           const char* _trace_info = "w_cassandra_tuple_set_int64";
           if (!pTuple || !pIndex || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_tuple_set_int64(pTuple, pIndex, pValue);

       }

       w_Cass_Error  w_cassandra_tuple_set_float(_In_ w_cass_tuple* pTuple, _In_ size_t pIndex, _In_ float pValue)
       {
           const char* _trace_info = "w_cassandra_tuple_set_float";
           if (!pTuple || !pIndex || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_tuple_set_float(pTuple, pIndex, pValue);

       }

       w_Cass_Error w_cassandra_tuple_set_double(_In_ w_cass_tuple* pTuple, _In_ size_t pIndex, _In_ double pValue)
       {
           const char* _trace_info = "w_cassandra_tuple_set_double";
           if (!pTuple || !pIndex || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_tuple_set_double(pTuple, pIndex, pValue);

       }

       w_Cass_Error  w_cassandra_tuple_set_bool(_In_ w_cass_tuple* pTuple, _In_ size_t pIndex, _In_ w_bool_t pValue)
       {
           const char* _trace_info = "w_cassandra_tuple_set_bool";
           if (!pTuple || !pIndex )
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_tuple_set_bool(pTuple, pIndex, pValue);

       }

       w_Cass_Error  w_cassandra_tuple_set_string(_In_ w_cass_tuple* pTuple, _In_ size_t pIndex, _In_ const char* pValue)
       {
           const char* _trace_info = "w_cassandra_tuple_set_string";
           if (!pTuple || !pIndex || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_tuple_set_string(pTuple, pIndex, pValue);

       }

       w_Cass_Error  w_cassandra_tuple_set_string_n(_In_ w_cass_tuple* pTuple, _In_  size_t pIndex, _In_ const char* pValue, _In_ size_t pValueLength)
       {
           const char* _trace_info = "w_cassandra_tuple_set_string_n";
           if (!pTuple || !pIndex || !pValue || !pValueLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_tuple_set_string_n(pTuple, pIndex, pValue, pValueLength);

       }

       w_Cass_Error  w_cassandra_tuple_set_bytes(_In_ w_cass_tuple* pTuple, _In_ size_t pIndex, _In_ const w_byte_t* pValue, _In_ size_t pValueSize)
       {
           const char* _trace_info = "w_cassandra_tuple_set_bytes";
           if (!pTuple || !pValue || !pIndex || !pValueSize)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_tuple_set_bytes(pTuple, pIndex, pValue, pValueSize);

       }

       w_Cass_Error  w_cassandra_tuple_set_custom(_In_ w_cass_tuple* pTuple, _In_ size_t pIndex, _In_ const char* pClassName, _In_ const w_byte_t* pValue, _In_ size_t pValueSize)
       {
           const char* _trace_info = "w_cassandra_tuple_set_custom";
           if (!pTuple || !pIndex || !pClassName|| !pValue || !pValueSize)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_tuple_set_custom(pTuple, pIndex, pClassName, pValue, pValueSize);

       }

       w_Cass_Error   w_cassandra_tuple_set_custom_n(_In_ w_cass_tuple* pTuple, _In_ size_t pIndex, _In_ const char* pClassName, _In_  size_t pClassNameLength, _In_ const w_byte_t* pValue, _In_ size_t pValueSize)
       {
           const char* _trace_info = "w_cassandra_tuple_set_custom_n";
           if (!pTuple || !pIndex || !pClassName || !pClassNameLength || !pValue || !pValueSize)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_tuple_set_custom_n(pTuple, pIndex, pClassName,pClassNameLength, pValue, pValueSize);

       }

       w_Cass_Error w_cassandra_tuple_set_uuid(_In_ w_cass_tuple* pTuple, _In_ size_t pIndex, _In_ w_cass_uuid pValue)
       {
           const char* _trace_info = "w_cassandra_tuple_set_uuid";
           if (!pTuple || !pIndex )
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_tuple_set_uuid(pTuple, pIndex, pValue);

       }

       w_Cass_Error  w_cassandra_tuple_set_inet(_In_ w_cass_tuple* pTuple, _In_ size_t pIndex, _In_ w_cass_inet pValue)
       {
           const char* _trace_info = "w_cassandra_tuple_set_inet";
           if (!pTuple || !pIndex)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }

           return  cass_tuple_set_inet(pTuple, pIndex, pValue);

       }

       w_Cass_Error w_cassandra_tuple_set_decimal(_In_ w_cass_tuple* pTuple, _In_ size_t pIndex, _In_ const w_byte_t* pVarint, _In_ size_t pVarintSize, _In_ int32_t pScale)
       {
           const char* _trace_info = "w_cassandra_tuple_set_decimal";
           if (!pTuple || !pIndex || !pVarint || !pVarintSize || !pScale)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return    cass_tuple_set_decimal(pTuple, pIndex, pVarint, pVarintSize, pScale);

       }

       w_Cass_Error   w_cassandra_tuple_set_duration(_In_ w_cass_tuple* pTuple, _In_  size_t pIndex, _In_ int32_t pMonths, _In_ int32_t pDays, _In_ int64_t pNanos)
       {
           const char* _trace_info = "w_cassandra_tuple_set_duration";
           if (!pTuple || !pIndex || !pMonths || !pDays || !pNanos)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_tuple_set_duration(pTuple, pIndex, pMonths, pDays, pNanos);

       }

       w_Cass_Error  w_cassandra_tuple_set_collection(_In_ w_cass_tuple* pTuple, _In_ size_t pIndex, _In_ const w_cass_collection* pValue)
       {
           const char* _trace_info = "w_cassandra_tuple_set_collection";
           if (!pTuple || !pIndex || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_tuple_set_collection(pTuple, pIndex, pValue);

       }

       w_Cass_Error w_cassandra_tuple_set_tuple(_In_ w_cass_tuple* pTuple, _In_ size_t pIndex, _In_ const w_cass_tuple* pValue)
       {
           const char* _trace_info = "w_cassandra_tuple_set_tuple";
           if (!pTuple || !pIndex  || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_tuple_set_tuple(pTuple, pIndex, pValue);

       }

       w_Cass_Error  w_cassandra_tuple_set_user_type(_In_ w_cass_tuple* pTuple, _In_ size_t pIndex, _In_ const w_cass_user_type* pValue)
       {
           const char* _trace_info = "w_cassandra_tuple_set_user_type";
           if (!pTuple || !pIndex || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_tuple_set_user_type(pTuple, pIndex, pValue);

       }

       w_cass_user_type*  w_cassandra_user_type_new_from_data_type(_In_ const w_cass_data_type* pDataType)
       {
           const char* _trace_info = "w_cassandra_user_type_new_from_data_type";
           if (!pDataType)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return cass_user_type_new_from_data_type(pDataType);

       }

       void  w_cassandra_user_type_free(_In_ w_cass_user_type* pUserType)
       {
           const char* _trace_info = "w_cassandra_user_type_free";
           if (!pUserType)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
           }
             cass_user_type_free(pUserType);

       }

       const w_cass_data_type* w_cassandra_user_type_data_type(_In_ const w_cass_user_type* pUserType)
       {
           const char* _trace_info = "w_cassandra_user_type_data_type";
           if (!pUserType)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }

           return cass_user_type_data_type(pUserType);

       }

       w_Cass_Error  w_cassandra_user_type_set_null(_In_ w_cass_user_type* pUserType, _In_ size_t pIndex)
       {
           const char* _trace_info = "w_cassandra_user_type_set_null";
           if (!pUserType || !pIndex)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_user_type_set_null(pUserType, pIndex);

       }

       w_Cass_Error  w_cassandra_user_type_set_null_by_name(_In_ w_cass_user_type* pUserType, _In_ const char* pName)
       {
           const char* _trace_info = "w_cassandra_user_type_set_null_by_name";
           if (!pUserType || !pName)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_user_type_set_null_by_name(pUserType, pName);

       }

       w_Cass_Error  w_cassandra_user_type_set_null_by_name_n(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ size_t pNameLength)
       {
           const char* _trace_info = "w_cassandra_user_type_set_null_by_name_n";
           if (!pUserType || !pName || !pNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_user_type_set_null_by_name_n(pUserType, pName, pNameLength);

       }

       w_Cass_Error  w_cassandra_user_type_set_int8(_In_ w_cass_user_type* pUserType, _In_ size_t pIndex, _In_ int8_t pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_int8";
           if (!pUserType || !pIndex || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_user_type_set_int8(pUserType, pIndex, pValue);

       }

       w_Cass_Error  w_cassandra_user_type_set_int8_by_name(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ int8_t pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_int8_by_name";
           if (!pUserType || !pName || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_user_type_set_int8_by_name(pUserType, pName, pValue);

       }

       w_Cass_Error w_cassandra_user_type_set_int8_by_name_n(_In_ w_cass_user_type* pUserType, _In_  const char* pName, _In_  size_t pNameLength, _In_ int8_t pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_int8_by_name_n";
           if (!pUserType || !pName || !pNameLength || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

               return -1;
           }
           return   cass_user_type_set_int8_by_name_n(pUserType, pName, pNameLength, pValue);

       }

       w_Cass_Error  w_cassandra_user_type_set_int16(_In_ w_cass_user_type* pUserType, _In_ size_t pIndex, _In_ int16_t pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_int16";
           if (!pUserType ||  !pIndex  ||!pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_user_type_set_int16(pUserType, pIndex, pValue);

       }

       w_Cass_Error  w_cassandra_user_type_set_int16_by_name(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ int16_t pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_int16_by_name";
           if (!pUserType || !pName || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_user_type_set_int16_by_name(pUserType, pName, pValue);

       }
       w_Cass_Error w_cassandra_user_type_set_int16_by_name_n(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ size_t pNameLength, _In_ int16_t pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_int16_by_name_n";
           if (!pUserType || !pName || !pNameLength || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_user_type_set_int16_by_name_n(pUserType, pName, pNameLength, pValue);

       }

       w_Cass_Error  w_cassandra_user_type_set_int32(_In_ w_cass_user_type* pUserType, _In_ size_t pIndex, _In_ int32_t pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_int32";
           if (!pUserType || !pIndex || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }

           return  cass_user_type_set_int32(pUserType, pIndex, pValue);

       }

       w_Cass_Error  w_cassandra_user_type_set_int32_by_name(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ int32_t pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_int32_by_name";
           if (!pUserType || !pName || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_user_type_set_int32_by_name(pUserType, pName, pValue);

       }

       w_Cass_Error   w_cassandra_user_type_set_int32_by_name_n(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ size_t pNameLength,int32_t pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_int32_by_name_n";
           if (!pUserType || !pName|| !pNameLength|| !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_user_type_set_int32_by_name_n(pUserType,  pName,  pNameLength,  pValue);

       }

       w_Cass_Error  w_cassandra_user_type_set_uint32(_In_ w_cass_user_type* pUserType, _In_ size_t pIndex, _In_ uint32_t pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_uint32";
           if (!pUserType || !pIndex|| !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }

           return cass_user_type_set_uint32(pUserType, pIndex, pValue);

       }

       w_Cass_Error  w_cassandra_user_type_set_uint32_by_name(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ uint32_t pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_uint32_by_name";
           if (!pUserType || !pName || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_user_type_set_uint32_by_name(pUserType, pName, pValue);

       }

       w_Cass_Error w_cassandra_user_type_set_uint32_by_name_n(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ size_t pNameLength, _In_ uint32_t pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_uint32_by_name_n";
           if (!pUserType || !pName || !pNameLength || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }

           return   cass_user_type_set_uint32_by_name_n(pUserType, pName, pNameLength, pValue);

       }

       w_Cass_Error w_cassandra_user_type_set_int64(_In_ w_cass_user_type* pUserType, _In_ size_t pIndex, _In_ int64_t pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_int64";
           if(!pUserType || !pIndex || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }

           return   cass_user_type_set_int64(pUserType, pIndex, pValue);

       }

       w_Cass_Error w_cassandra_user_type_set_int64_by_name(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ int64_t pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_int64_by_name";
           if(!pUserType || !pName ||  !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }

           return    cass_user_type_set_int64_by_name(pUserType, pName, pValue);

       }

       w_Cass_Error  w_cassandra_user_type_set_int64_by_name_n(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ size_t pNameLength, _In_ int64_t pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_int64_by_name_n";
           if(!pUserType || !pName || !pNameLength || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }

           return    cass_user_type_set_int64_by_name_n(pUserType, pName, pNameLength, pValue);

       }

       w_Cass_Error w_cassandra_user_type_set_float(_In_ w_cass_user_type* pUserType, _In_ size_t pIndex, _In_ float pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_float";
           if (!pUserType || !pIndex|| !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

               return -1;
           }
           return    cass_user_type_set_float(pUserType, pIndex, pValue);

       }
       w_Cass_Error  w_cassandra_user_type_set_float_by_name(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ float pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_float_by_name";
           if (!pUserType || !pName || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_user_type_set_float_by_name(pUserType, pName, pValue);

       }

       w_Cass_Error w_cassandra_user_type_set_float_by_name_n(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ size_t pNameLength, _In_ float pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_float_by_name_n";
           if (!pUserType || !pName || !pNameLength || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return    cass_user_type_set_float_by_name_n(pUserType, pName, pNameLength, pValue);

       }

       w_Cass_Error  w_cassandra_user_type_set_double(_In_ w_cass_user_type* pUserType, _In_ size_t pIndex, _In_ double pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_double";
           if (!pUserType || !pIndex || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_user_type_set_double(pUserType, pIndex, pValue);

       }

       w_Cass_Error  w_cassandra_user_type_set_double_by_name(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ double pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_double_by_name";
           if (!pUserType || !pName || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_user_type_set_double_by_name(pUserType, pName, pValue);

       }

       w_Cass_Error w_cass_user_type_set_double_by_name_n(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ size_t pNameLength, _In_ double pValue)
       {
           const char* _trace_info = "w_cass_user_type_set_double_by_name_n";
           if (!pUserType || !pName || !pNameLength ||  !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_user_type_set_double_by_name_n(pUserType, pName, pNameLength, pValue);

       }
       w_Cass_Error  w_cassandra_user_type_set_bool(_In_ w_cass_user_type* pUserType, _In_ size_t pIndex, _In_ w_bool_t pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_bool";
           if (!pUserType || !pIndex)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_user_type_set_bool(pUserType, pIndex, pValue);

       }

       w_Cass_Error   w_cassandra_user_type_set_bool_by_name(_In_ w_cass_user_type* pUserType, _In_  const char* pName, _In_ w_bool_t pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_bool_by_name";
           if (!pUserType || !pName)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return    cass_user_type_set_bool_by_name(pUserType, pName, pValue);

       }

       w_Cass_Error  w_cassandra_user_type_set_bool_by_name_n(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ size_t pNameLength, _In_ w_bool_t pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_bool_by_name_n";
           if (!pUserType || !pName ||  !pNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return    cass_user_type_set_bool_by_name_n(pUserType, pName, pNameLength, pValue);

       }

       w_Cass_Error  w_cassandra_user_type_set_string(_In_ w_cass_user_type* pUserType, _In_ size_t pIndex, _In_ const char* pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_string";
           if (!pUserType || !pIndex || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_user_type_set_string(pUserType, pIndex, pValue);

       }

       w_Cass_Error  w_cassandra_user_type_set_string_n(_In_ w_cass_user_type* pUserType, _In_ size_t pIndex, _In_ const char* pValue, _In_ size_t pValueLength)
       {

           const char* _trace_info = "w_cassandra_user_type_set_string_n";
           if (!pUserType || !pIndex|| !pValue|| !pValueLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }

           return    cass_user_type_set_string_n(pUserType, pIndex, pValue, pValueLength);

       }

       w_Cass_Error  w_cassandra_user_type_set_string_by_name(_In_ w_cass_user_type* pUserType, _In_  const char* pName, _In_ const char* pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_string_by_name";
           if (!pUserType || !pName || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_user_type_set_string_by_name(pUserType, pName, pValue);

       }

       w_Cass_Error w_cassandra_user_type_set_string_by_name_n(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ size_t pNameLength, _In_ const char* pValue, _In_  size_t pValueLength)
       {
           const char* _trace_info = "w_cassandra_user_type_set_string_by_name_n";
           if (!pUserType || !pName || !pNameLength || !pValue || !pValueLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_user_type_set_string_by_name_n(pUserType, pName, pNameLength, pValue, pValueLength);

       }

       w_Cass_Error  w_cassandra_user_type_set_bytes(_In_ w_cass_user_type* pUserType, _In_ size_t pIndex, _In_ const w_byte_t* pValue, _In_ size_t pValueSize)
       {
           const char* _trace_info = "w_cassandra_user_type_set_bytes";
           if (!pUserType ||  !pIndex || !pValue || !pValueSize)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_user_type_set_bytes(pUserType, pIndex, pValue, pValueSize);

       }

       w_Cass_Error  w_cassandra_user_type_set_bytes_by_name(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ const w_byte_t* pValue, _In_ size_t pValueSize)
       {
           const char* _trace_info = "w_cassandra_user_type_set_bytes_by_name";
           if (!pUserType || !pName || !pValue || !pValueSize)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return    cass_user_type_set_bytes_by_name(pUserType, pName, pValue, pValueSize);

       }

       w_Cass_Error   w_cassandra_user_type_set_bytes_by_name_n(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ size_t pNameLength, _In_ const w_byte_t* pValue, _In_ size_t pValueSize)
       {
           const char* _trace_info = "w_cassandra_user_type_set_bytes_by_name_n";
           if (!pUserType || !pName || !pNameLength || !pValue || !pValueSize)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return    cass_user_type_set_bytes_by_name_n(pUserType, pName, pNameLength, pValue, pValueSize);

       }

       w_Cass_Error  w_cassandra_user_type_set_custom(_In_ w_cass_user_type* pUserType, _In_ size_t pIndex, _In_  const char* pClassName, _In_ const w_byte_t* pValue, _In_ size_t pValueSize)
       {
           const char* _trace_info = "w_cassandra_user_type_set_custom";
           if (!pUserType || !pIndex|| !pClassName || !pValue|| !pValueSize)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }

           return  cass_user_type_set_custom(pUserType, pIndex, pClassName, pValue, pValueSize);

       }

       w_Cass_Error   w_cassandra_user_type_set_custom_n(_In_ w_cass_user_type* pUserType, _In_ size_t pIndex, _In_ const char* pClassName, _In_ size_t pClassNameLength, _In_  const w_byte_t* pValue, _In_ size_t pValueSize)
       {
           const char* _trace_info = "w_cassandra_user_type_set_custom_n";

           if (!pUserType || !pIndex || !pClassName || !pClassNameLength || !pValue || !pValueSize)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return    cass_user_type_set_custom_n(pUserType, pIndex, pClassName, pClassNameLength, pValue, pValueSize);

       }

       w_Cass_Error   w_cassandra_user_type_set_custom_by_name(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ const char* pClassName, _In_ const w_byte_t* pValue, _In_ size_t pValueSize)
       {
           const char* _trace_info = "w_cassandra_user_type_set_custom_by_name";
           if (!pUserType || !pName || !pClassName || !pValue ||!pValueSize)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_user_type_set_custom_by_name(pUserType, pName, pClassName, pValue, pValueSize);

       }

       w_Cass_Error  w_cassandra_user_type_set_custom_by_name_n(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ size_t pNameLength, _In_ const char* pClassName, _In_ size_t pClassNameLength, _In_ const w_byte_t* pValue, _In_ size_t pValueSize)
       {
           const char* _trace_info = "w_cassandra_user_type_set_custom_by_name_n";
           if (!pUserType || !pName || !pNameLength || !pClassName || !pClassNameLength || !pValue || !pValueSize)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_user_type_set_custom_by_name_n(pUserType, pName, pNameLength, pClassName, pClassNameLength, pValue, pValueSize);

       }

       w_Cass_Error  w_cassandra_user_type_set_uuid(_In_ w_cass_user_type* pUserType, _In_ size_t pIndex, _In_ w_cass_uuid pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_uuid";
           if (!pUserType ||  !pIndex )
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return    cass_user_type_set_uuid(pUserType, pIndex, pValue);

       }

       w_Cass_Error w_cassandra_user_type_set_uuid_by_name(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ w_cass_uuid pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_uuid_by_name";
           if (!pUserType || !pName )
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_user_type_set_uuid_by_name(pUserType, pName, pValue);

       }

       w_Cass_Error  w_cassandra_user_type_set_uuid_by_name_n(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ size_t pNameLength, _In_ w_cass_uuid pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_uuid_by_name_n";
           if (!pUserType || !pName || !pNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

               return -1;
           }
           return    cass_user_type_set_uuid_by_name_n(pUserType, pName, pNameLength, pValue);

       }

       w_Cass_Error  w_cass_user_type_set_inet(_In_ w_cass_user_type* pUserType, _In_  size_t pIndex, _In_ w_cass_inet pValue)
       {
           const char* _trace_info = "w_cass_user_type_set_inet";
           if (!pUserType || !pIndex)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }

           return    cass_user_type_set_inet(pUserType, pIndex, pValue);

       }

       w_Cass_Error w_cass_user_type_set_inet_by_name(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ w_cass_inet pValue)
       {
           const char* _trace_info = "w_cass_user_type_set_inet_by_name";
           if (!pUserType || !pName)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_user_type_set_inet_by_name(pUserType, pName, pValue);

       }

       w_Cass_Error  w_cassandra_user_type_set_inet_by_name_n(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ size_t pNameLength, _In_ w_cass_inet pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_inet_by_name_n";
           if (!pUserType || !pName || !pNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_user_type_set_inet_by_name_n(pUserType, pName, pNameLength, pValue);

       }

       w_Cass_Error   w_cassandra_user_type_set_decimal(_In_ w_cass_user_type* pUserType, _In_ size_t pIndex, _In_ const w_byte_t* pVarint, _In_ size_t pVarintSize, _In_  int pScale)
       {
           const char* _trace_info = "w_cassandra_user_type_set_decimal";

           if (!pUserType  || !pIndex ||  !pVarint || !pVarintSize || !pScale)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }

           return   cass_user_type_set_decimal(pUserType, pIndex, pVarint, pVarintSize, pScale);

       }

       w_Cass_Error  w_cassandra_user_type_set_decimal_by_name(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ const w_byte_t* pVarint, _In_ size_t pVarintSize, _In_ int pScale)
       {
           const char* _trace_info = "w_cassandra_user_type_set_decimal_by_name";
           if (!pUserType || !pName || !pVarint || !pVarintSize || !pScale)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_user_type_set_decimal_by_name(pUserType, pName, pVarint, pVarintSize, pScale);

       }

       w_Cass_Error  w_cassandra_user_type_set_decimal_by_name_n(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ size_t pNameLength, _In_ const w_byte_t* pVarint, _In_ size_t pVarintSize, _In_ int pScale)
       {
           const char* _trace_info = "w_cassandra_user_type_set_decimal_by_name_n";
           if (!pUserType || !pName || !pNameLength ||! pVarint || !pVarintSize || !pScale)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return    cass_user_type_set_decimal_by_name_n(pUserType, pName, pNameLength, pVarint, pVarintSize, pScale);

       }

       w_Cass_Error  w_cassandra_user_type_set_duration(_In_ w_cass_user_type* pUserType, _In_ size_t pIndex, _In_  int32_t pMonths, _In_ int32_t pDays, _In_ int64_t pNanos)
       {
           const char* _trace_info = "w_cassandra_user_type_set_duration";
           if (!pUserType || !pIndex || !pMonths || !pDays || !pNanos)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_user_type_set_duration(pUserType, pIndex, pMonths, pDays, pNanos);

       }

       w_Cass_Error  w_cassandra_user_type_set_duration_by_name(_In_ w_cass_user_type* pUserType, _In_  const char* pName, _In_ int32_t pMonths, _In_  int32_t pDays, _In_  int64_t pNanos)
       {
           const char* _trace_info = "w_cassandra_user_type_set_duration_by_name";
           if (!pUserType || !pName || !pMonths || !pDays || !pNanos)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return    cass_user_type_set_duration_by_name(pUserType, pName, pMonths, pDays, pNanos);

       }

       w_Cass_Error  w_cassandra_user_type_set_duration_by_name_n(_In_ w_cass_user_type* pUserType, _In_  const char* pName, _In_   size_t pNameLength, _In_ int32_t pMonths, _In_ int32_t pDays, _In_ int64_t pNanos)
       {
           const char* _trace_info = "w_cassandra_user_type_set_duration_by_name_n";
           if (!pUserType || !pName || !pNameLength || !pMonths || !pDays || !pNanos)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
          return     cass_user_type_set_duration_by_name_n(pUserType, pName, pNameLength, pMonths, pDays, pNanos);

       }

       w_Cass_Error  w_cassandra_user_type_set_collection(_In_ w_cass_user_type* pUserType, _In_ size_t pIndex, _In_ const w_cass_collection* pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_collection";
           if (!pUserType  || !pIndex || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return    cass_user_type_set_collection(pUserType, pIndex, pValue);

       }

       w_Cass_Error w_cassandra_user_type_set_collection_by_name(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ const w_cass_collection* pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_collection_by_name";
           if (!pUserType || !pName || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
               return  cass_user_type_set_collection_by_name(pUserType, pName,   pValue);

       }
       w_Cass_Error w_cassandra_user_type_set_collection_by_name_n(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ size_t pNameLength, _In_ const w_cass_collection* pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_collection_by_name_n";
           if (!pUserType || !pName || !pNameLength || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return  cass_user_type_set_collection_by_name_n(pUserType, pName, pNameLength, pValue);

       }

       w_Cass_Error w_cassandra_user_type_set_tuple(_In_ w_cass_user_type* pUserType, _In_ size_t pIndex, _In_ const w_cass_tuple* pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_tuple";

           if (!pUserType || !pIndex || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }

           return     cass_user_type_set_tuple(pUserType, pIndex, pValue);

       }

       w_Cass_Error  w_cassandra_user_type_set_tuple_by_name(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ const w_cass_tuple* pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_tuple_by_name";

           if (!pUserType || !pName || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return    cass_user_type_set_tuple_by_name(pUserType, pName, pValue);

       }

       w_Cass_Error   w_cassandra_user_type_set_tuple_by_name_n(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ size_t pNameLength, _In_ const w_cass_tuple* pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_tuple_by_name_n";

           if (!pUserType || !pName || !pNameLength || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_user_type_set_tuple_by_name_n(pUserType, pName, pNameLength, pValue);

       }

       w_Cass_Error  w_cassandra_user_type_set_user_type(_In_ w_cass_user_type* pUserType, _In_ size_t pIndex, _In_ const w_cass_user_type* pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_user_type";

           if (!pUserType || !pIndex || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_user_type_set_user_type(pUserType, pIndex, pValue);

       }

       w_Cass_Error w_cass_user_type_set_user_type_by_name(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ const w_cass_user_type* pValue)
       {
           const char* _trace_info = "w_cass_user_type_set_user_type_by_name";

           if (!pUserType || !pName || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return    cass_user_type_set_user_type_by_name(pUserType, pName, pValue);

       }

       w_Cass_Error w_cassandra_user_type_set_user_type_by_name_n(_In_ w_cass_user_type* pUserType, _In_ const char* pName, _In_ size_t pNameLength, _In_ const w_cass_user_type* pValue)
       {
           const char* _trace_info = "w_cassandra_user_type_set_user_type_by_name_n";

           if (!pUserType || !pName || !pNameLength || !pValue)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return cass_user_type_set_user_type_by_name_n(pUserType, pName, pNameLength, pValue);

       }

       void w_cassandra_result_free(_In_ const w_cass_result* pResult)
       {
           const char* _trace_info = "w_cassandra_result_free";
           if (!pResult)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

           }
           cass_result_free(pResult);

       }

       size_t  w_cassandra_result_row_count(_In_ const w_cass_result* pResult)
       {
           const char* _trace_info = "w_cassandra_result_row_count";

           if (!pResult)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return 0;
           }
           return   cass_result_row_count(pResult);

       }

       size_t w_cassandra_result_column_count(_In_ const w_cass_result* pResult)
       {
           const char* _trace_info = "w_cassandra_result_column_count";

           if (!pResult)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return 0;
           }
           return  cass_result_column_count(pResult);

       }

       w_Cass_Error w_cassandra_result_column_name(_In_ const w_cass_result* pResult, _In_  size_t pIndex, _Inout_ const char** pName, _Inout_  size_t* pNameLength)
       {
           const char* _trace_info = "w_cassandra_result_column_name";
           if (!pResult || !pIndex || !pName || !pNameLength)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_result_column_name(pResult, pIndex, pName, pNameLength);

       }
       w_cass_value_type  w_cassandra_result_column_type(_In_ const w_cass_result* pResult, _In_ size_t pIndex)
       {
           const char* _trace_info = "w_cassandra_result_column_type";

           if (!pResult || !pIndex)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_result_column_type(pResult, pIndex);

       }

       const w_cass_data_type* w_cassandra_result_column_data_type(_In_ const w_cass_result* pResult, _In_ size_t pIndex)
       {
           const char* _trace_info = "w_cassandra_result_column_data_type";

           if (!pResult || !pIndex)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return  cass_result_column_data_type(pResult, pIndex);

       }
       const w_cass_row* w_cassandra_result_first_row(_In_ const w_cass_result* pResult)
       {
           const char* _trace_info = "w_cassandra_result_first_row";

           if (!pResult)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return NULL;
           }
           return   cass_result_first_row(pResult);

       }

       w_bool_t w_cassandra_result_has_more_pages(_In_ const w_cass_result* pResult)
       {
           const char* _trace_info = "w_cassandra_result_has_more_pages";

           if (!pResult)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return false;
           }
           return  cass_result_has_more_pages(pResult);

       }

       w_Cass_Error   w_cassandra_result_paging_state_token(_In_ const w_cass_result* pResult, _Inout_ const char** pPagingState, _Inout_ size_t* pPagingStateSize)
       {
           const char* _trace_info = "w_cassandra_result_paging_state_token";

           if (!pResult || !pPagingState || !pPagingStateSize)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;
           }
           return   cass_result_paging_state_token(pResult, pPagingState, pPagingStateSize);

       }

       void w_cassandra_error_result_free(_In_ const w_cass_error_result* pErrorResult)
       {
           const char* _trace_info = "w_cassandra_error_result_free";

           if (!pErrorResult)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

           }
           cass_error_result_free(pErrorResult);

       }


       w_Cass_Error w_cassandra_error_result_code(_In_ const w_cass_error_result* pErrorResult)
       {
           const char* _trace_info = "w_cassandra_error_result_code";

           if (!pErrorResult)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;

           }
           return  cass_error_result_code(pErrorResult);

       }

       w_cass_consistency w_cassandra_error_result_consistency(_In_ const w_cass_error_result* pErrorResult)
       {
           const char* _trace_info = "w_cassandra_error_result_consistency";

           if (!pErrorResult)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;

           }
        return cass_error_result_consistency(pErrorResult);

       }

       int32_t  w_cass_error_result_responses_received(_In_ const w_cass_error_result* pErrorResult)
       {
           const char* _trace_info = "w_cass_error_result_responses_received";

           if (!pErrorResult)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;

           }
           return  cass_error_result_responses_received(pErrorResult);

       }


       int32_t w_cassandra_error_result_responses_required(_In_ const w_cass_error_result* pErrorResult)
       {
           const char* _trace_info = "w_cassandra_error_result_responses_required";

           if (!pErrorResult)
           {
               W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
               return -1;

           }
           return  cass_error_result_responses_required(pErrorResult);

       }

      int32_t  w_cassandra_error_result_num_failures(_In_ const w_cass_error_result* pErrorResult)
      {
          const char* _trace_info = "w_cassandra_error_result_num_failures";

          if (!pErrorResult)
          {
              W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
              return -1;

          }
         return  cass_error_result_num_failures(pErrorResult);

      }

      w_bool_t  w_cassandra_error_result_data_present(_In_ const w_cass_error_result* pErrorResult)
      {
          const char* _trace_info = "w_cassandra_error_result_data_present";

          if (!pErrorResult)
          {
              W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
              return false;

          }
         return  cass_error_result_data_present(pErrorResult);

      }

      w_cass_writetype w_cassandra_error_result_write_type(_In_ const w_cass_error_result* pErrorResult)
      {
          const char* _trace_info = "w_cassandra_error_result_write_type";

          if (!pErrorResult)
          {
              W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
              return -1;

          }

         return cass_error_result_write_type(pErrorResult);

      }

      w_Cass_Error w_cassandra_error_result_keyspace(_In_ const w_cass_error_result* pErrorResult, _Inout_ const char** pKeyspace, _Inout_ size_t* pKeyspaceLength)
      {
          const char* _trace_info = "w_cassandra_error_result_keyspace";

          if (!pErrorResult || !pKeyspace || !pKeyspaceLength)
          {
              W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
              return -1;

          }
           return cass_error_result_keyspace(pErrorResult, pKeyspace, pKeyspaceLength);

      }

      w_Cass_Error  w_cassandra_error_result_table(_In_ const w_cass_error_result* pErrorResult, _Inout_ const char** pTable, _Inout_ size_t* pTableLength)
      {
          const char* _trace_info = "w_cassandra_error_result_table";

          if (!pErrorResult || !pTable || !pTableLength)
          {
              W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
              return -1;

          }
         return  cass_error_result_table(pErrorResult, pTable, pTableLength);

      }

     w_Cass_Error w_cassandra_error_result_function(_In_ const w_cass_error_result* pErrorResult, _Inout_  const char** pFunction, _Inout_ size_t* pFunctionLength)
     {
         const char* _trace_info = "w_cassandra_error_result_function";

         if (!pErrorResult || !pFunction || !pFunctionLength)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return -1;

         }
         return cass_error_result_function(pErrorResult, pFunction, pFunctionLength);

     }

     size_t w_cassandra_error_num_arg_types(_In_ const w_cass_error_result* pErrorResult)
     {
         const char* _trace_info = "w_cassandra_error_num_arg_types";

         if (!pErrorResult)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return 0;

         }

         return   cass_error_num_arg_types(pErrorResult);

     }

     w_Cass_Error  w_cassandra_error_result_arg_type(_In_ const w_cass_error_result* pErrorResult, _In_  size_t pIndex, _Inout_ const char** pArgType, _Inout_ size_t* pArgTypeLength)
     {
         const char* _trace_info = "w_cassandra_error_result_arg_type";

         if (!pErrorResult || !pIndex || !pArgType || !pArgTypeLength)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return -1;

         }
         return  cass_error_result_arg_type(pErrorResult, pIndex, pArgType, pArgTypeLength);

     }

     void w_cassandra_iterator_free(_In_ w_cass_iterator* pIterator)
     {
         const char* _trace_info = "w_cassandra_iterator_free";

         if (!pIterator)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

         }
         cass_iterator_free(pIterator);
     }

     w_cass_iterator_type  w_cassandra_iterator_type(_In_ w_cass_iterator* pIterator)
     {
         const char* _trace_info = "w_cassandra_iterator_type";

         if (!pIterator)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return -1;
         }
       return cass_iterator_type(pIterator);

     }

     w_cass_iterator* w_cassandra_iterator_from_result(_In_ const w_cass_result* pResult)
     {
         const char* _trace_info = "w_cassandra_iterator_from_result";

         if (!pResult)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return  cass_iterator_from_result(pResult);

     }

     w_cass_iterator* w_cassandra_iterator_from_row(_In_ const w_cass_row* pRow)
     {
         const char* _trace_info = "w_cassandra_iterator_from_row";

         if (!pRow)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return   cass_iterator_from_row(pRow);

     }

     w_cass_iterator*   w_cassandra_iterator_from_collection(_In_ const w_cass_value* pValue)
     {
         const char* _trace_info = "w_cassandra_iterator_from_collection";

         if (!pValue)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return cass_iterator_from_collection(pValue);

     }

     w_cass_iterator* w_cassandra_iterator_from_map(_In_ const w_cass_value* pValue)
     {
         const char* _trace_info = "w_cassandra_iterator_from_map";

         if (!pValue)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return cass_iterator_from_map(pValue);
     }

     w_cass_iterator* w_cassandra_iterator_from_tuple(_In_ const w_cass_value* pValue)
     {
         const char* _trace_info = "w_cassandra_iterator_from_tuple";

         if (!pValue)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }

         return  cass_iterator_from_tuple(pValue);
     }

     w_cass_iterator* w_cassandra_iterator_fields_from_user_type(_In_ const w_cass_value* pValue)
     {
         const char* _trace_info = "w_cassandra_iterator_fields_from_user_type";

         if (!pValue)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return  cass_iterator_fields_from_user_type(pValue);
     }

     w_cass_iterator* w_cassandra_iterator_keyspaces_from_schema_meta(_In_ const w_cass_schema_meta* pSchemaMeta)
     {
         const char* _trace_info = "w_cassandra_iterator_keyspaces_from_schema_meta";

         if (!pSchemaMeta)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return    cass_iterator_keyspaces_from_schema_meta(pSchemaMeta);


     }

     w_cass_iterator* w_cassandra_iterator_tables_from_keyspace_meta(_In_ const w_cass_key_space_meta* pKeyspaceMeta)
     {
         const char* _trace_info = "w_cassandra_iterator_tables_from_keyspace_meta";

         if (!pKeyspaceMeta)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return  cass_iterator_tables_from_keyspace_meta(pKeyspaceMeta);

     }


     w_cass_iterator* w_cassandra_iterator_materialized_views_from_keyspace_meta(_In_ const w_cass_key_space_meta* pKeyspaceMeta)
     {
         const char* _trace_info = "w_cassandra_iterator_materialized_views_from_keyspace_meta";

         if (!pKeyspaceMeta)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return cass_iterator_materialized_views_from_keyspace_meta(pKeyspaceMeta);

     }

     w_cass_iterator*  w_cassandra_iterator_user_types_from_keyspace_meta(_In_ const w_cass_key_space_meta* pKeyspaceMeta)
     {
         const char* _trace_info = "w_cassandra_iterator_user_types_from_keyspace_meta";
         if (!pKeyspaceMeta)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return  cass_iterator_user_types_from_keyspace_meta(pKeyspaceMeta);

     }

     w_cass_iterator* w_cassandra_iterator_functions_from_keyspace_meta(_In_ const w_cass_key_space_meta* pKeyspaceMeta)
     {
         const char* _trace_info = "w_cassandra_iterator_functions_from_keyspace_meta";
         if (!pKeyspaceMeta)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return   cass_iterator_functions_from_keyspace_meta(pKeyspaceMeta);


     }
     w_cass_iterator*  w_cassandra_iterator_aggregates_from_keyspace_meta(_In_ const w_cass_key_space_meta* pKeyspaceMeta)
     {
         const char* _trace_info = "w_cassandra_iterator_aggregates_from_keyspace_meta";

         if (!pKeyspaceMeta)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return  cass_iterator_aggregates_from_keyspace_meta(pKeyspaceMeta);

     }
     w_cass_iterator*  w_cassandra_iterator_fields_from_keyspace_meta(_In_ const w_cass_key_space_meta* pKeyspaceMeta)
     {
         const char* _trace_info = "w_cassandra_iterator_fields_from_keyspace_meta";

         if (!pKeyspaceMeta)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return  cass_iterator_fields_from_keyspace_meta(pKeyspaceMeta);

     }

     w_cass_iterator*  w_cassandra_iterator_columns_from_table_meta(_In_ const w_cass_table_meta* pTableMeta)
     {
         const char* _trace_info = "w_cassandra_iterator_columns_from_table_meta";

         if (!pTableMeta)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
        return cass_iterator_columns_from_table_meta(pTableMeta);
     }

     w_cass_iterator*  w_cassandra_iterator_indexes_from_table_meta(_In_ const w_cass_table_meta* pTableMeta)
     {
         const char* _trace_info = "w_cassandra_iterator_indexes_from_table_meta";

         if (!pTableMeta)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return   cass_iterator_indexes_from_table_meta(pTableMeta);

     }

     w_cass_iterator*  w_cassandra_iterator_materialized_views_from_table_meta(_In_ const w_cass_table_meta* pTableMeta)
     {
         const char* _trace_info = "w_cassandra_iterator_materialized_views_from_table_meta";

         if (!pTableMeta)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return cass_iterator_materialized_views_from_table_meta(pTableMeta);


     }

     w_cass_iterator*  w_cassandra_iterator_fields_from_table_meta(_In_ const w_cass_table_meta* pTableMeta)
     {
         const char* _trace_info = "w_cassandra_iterator_fields_from_table_meta";

         if (!pTableMeta)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return cass_iterator_fields_from_table_meta(pTableMeta);
     }

     w_cass_iterator*  w_cassandra_iterator_columns_from_materialized_view_meta(_In_ const w_cass_materialized_view_meta* pViewMe)
     {
         const char* _trace_info = "w_cassandra_iterator_columns_from_materialized_view_meta";

         if (!pViewMe)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return  cass_iterator_columns_from_materialized_view_meta(pViewMe);

     }

     w_cass_iterator*  w_cassandra_iterator_fields_from_materialized_view_meta(_In_ const w_cass_materialized_view_meta* pViewMe)
     {
         const char* _trace_info = "w_cassandra_iterator_fields_from_materialized_view_meta";

         if (!pViewMe)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return cass_iterator_fields_from_materialized_view_meta(pViewMe);
     }

     w_cass_iterator*  w_cassandra_iterator_fields_from_column_meta(_In_ const w_cass_column_meta* pColumnMeta)
     {
         const char* _trace_info = "w_cassandra_iterator_fields_from_column_meta";

         if (!pColumnMeta)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return    cass_iterator_fields_from_column_meta(pColumnMeta);

     }

     w_cass_iterator*  w_cassandra_iterator_fields_from_index_meta(_In_ const w_cass_index_meta* pIndexMeta)
     {
         const char* _trace_info = "w_cassandra_iterator_fields_from_index_meta";

         if (!pIndexMeta)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return  cass_iterator_fields_from_index_meta(pIndexMeta);

     }

     w_cass_iterator*  w_cassandra_iterator_fields_from_function_meta(_In_ const w_cass_function_meta* pFunctionMeta)
     {
         const char* _trace_info = "w_cassandra_iterator_fields_from_function_meta";

         if (!pFunctionMeta)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return      cass_iterator_fields_from_function_meta(pFunctionMeta);

     }

     w_cass_iterator*  w_cassandra_iterator_fields_from_aggregate_meta(_In_ const w_cass_aggregate_meta* pAggregateMeta)
     {
         const char* _trace_info = "w_cassandra_iterator_fields_from_aggregate_meta";

         if (!pAggregateMeta)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return   cass_iterator_fields_from_aggregate_meta(pAggregateMeta);

     }

     w_bool_t w_cassandra_iterator_next(_In_ w_cass_iterator* pIterator)
     {
         const char* _trace_info = "w_cassandra_iterator_next";

         if (!pIterator)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return false;
         }
         return  cass_iterator_next(pIterator);

     }

     const w_cass_row*  w_cassandra_iterator_get_row(_In_ const w_cass_iterator* pIterator)
     {
         const char* _trace_info = "w_cassandra_iterator_get_row";
         if (!pIterator)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return   cass_iterator_get_row(pIterator);

     }

     const w_cass_value*  w_cassandra_iterator_get_column(_In_ const w_cass_iterator* pIterator)
     {
         const char* _trace_info = "w_cassandra_iterator_get_column";
         if (!pIterator)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return  cass_iterator_get_column(pIterator);

     }

     const w_cass_value*  w_cassandra_iterator_get_value(_In_ const w_cass_iterator* pIterator)
     {
         const char* _trace_info = "w_cassandra_iterator_get_value";
         if (!pIterator)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return cass_iterator_get_value(pIterator);
     }

     const w_cass_value*  w_cassandra_iterator_get_map_key(_In_ const w_cass_iterator* pIterator)
     {
         const char* _trace_info = "w_cassandra_iterator_get_map_key";
         if (!pIterator)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return cass_iterator_get_map_key(pIterator);

     }

     const w_cass_value* w_cassandra_iterator_get_map_value(_In_ const w_cass_iterator* pIterator)
     {
         const char* _trace_info = "w_cassandra_iterator_get_map_value";
         if (!pIterator)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return   cass_iterator_get_map_value(pIterator);

     }

     w_Cass_Error w_cassandra_iterator_get_user_type_field_name(_In_ const w_cass_iterator* pIterator, _Inout_ const char** pName, _Inout_ size_t* pNameLength)
     {
         const char* _trace_info = "w_cassandra_iterator_get_user_type_field_name";
         if (!pIterator || !pName || !pNameLength)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return -1;
         }
         return  cass_iterator_get_user_type_field_name(pIterator, pName, pNameLength);

     }

     const w_cass_value* w_cassandra_iterator_get_user_type_field_value(_In_ const w_cass_iterator* pIterator)
     {
         const char* _trace_info = "w_cassandra_iterator_get_user_type_field_value";
         if (!pIterator)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return  cass_iterator_get_user_type_field_value(pIterator);

     }

     const w_cass_key_space_meta* w_cassandra_iterator_get_keyspace_meta(_In_ const w_cass_iterator* pIterator)
     {
         const char* _trace_info = "w_cassandra_iterator_get_keyspace_meta";
         if (!pIterator)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }

         return     cass_iterator_get_keyspace_meta(pIterator);

     }

     const w_cass_table_meta* w_cassandra_iterator_get_table_meta(_In_ const w_cass_iterator* pIterator)
     {
         const char* _trace_info = "w_cassandra_iterator_get_table_meta";
         if (!pIterator)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return    cass_iterator_get_table_meta(pIterator);

     }

     const w_cass_materialized_view_meta*  w_cassandra_iterator_get_materialized_view_meta(_In_ const w_cass_iterator* pIterator)
     {
         const char* _trace_info = "w_cassandra_iterator_get_materialized_view_meta";
         if (!pIterator)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return cass_iterator_get_materialized_view_meta(pIterator);

     }


     const w_cass_data_type* w_cassandra_iterator_get_user_type(_In_ const w_cass_iterator* pIterator)
     {
         const char* _trace_info = "w_cassandra_iterator_get_user_type";
         if (!pIterator)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return cass_iterator_get_user_type(pIterator);
     }

     const w_cass_function_meta* w_cassandra_iterator_get_function_meta(_In_ const w_cass_iterator* pIterator)
     {
         const char* _trace_info = "w_cassandra_iterator_get_function_meta";
         if (!pIterator)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return   cass_iterator_get_function_meta(pIterator);

     }

     const w_cass_aggregate_meta* w_cassandra_iterator_get_aggregate_meta(_In_ const w_cass_iterator* pIterator)
     {
         const char* _trace_info = "w_cassandra_iterator_get_aggregate_meta";
         if (!pIterator)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return   cass_iterator_get_aggregate_meta(pIterator);

     }

     const w_cass_column_meta* w_cassandra_iterator_get_column_meta(_In_ const w_cass_iterator* pIterator)
     {
         const char* _trace_info = "w_cassandra_iterator_get_column_meta";
         if (!pIterator)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return   cass_iterator_get_column_meta(pIterator);

     }

     const w_cass_index_meta* w_cassandra_iterator_get_index_meta(_In_ const w_cass_iterator* pIterator)
     {
         const char* _trace_info = "w_cassandra_iterator_get_index_meta";
         if (!pIterator)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return  cass_iterator_get_index_meta(pIterator);

     }

     w_Cass_Error w_cassandra_iterator_get_meta_field_name(_In_ const w_cass_iterator* pIterator, _Inout_ const char** pName, _Inout_ size_t* pNameLength)
     {
         const char* _trace_info = "w_cassandra_iterator_get_meta_field_name";
         if (!pIterator || !pName || !pNameLength)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return -1;
         }

         return  cass_iterator_get_meta_field_name(pIterator, pName, pNameLength);

     }

     const w_cass_value*   w_cassandra_iterator_get_meta_field_value(_In_ const w_cass_iterator* pIterator)
     {
         const char* _trace_info = "w_cassandra_iterator_get_meta_field_value";
         if (!pIterator)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return  cass_iterator_get_meta_field_value(pIterator);

     }

     const w_cass_value*  w_cassandra_row_get_column(_In_ const w_cass_row* pRow, _In_ size_t pIndex)
     {
         const char* _trace_info = "w_cassandra_row_get_column";
         if (!pRow || !pIndex)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return  cass_row_get_column(pRow, pIndex);

     }

     const w_cass_value* w_cassandra_row_get_column_by_name(_In_ const w_cass_row* pRow, _In_ const char* pName)
     {
         const char* _trace_info = "w_cassandra_row_get_column_by_name";
         if (!pRow || !pName)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return   cass_row_get_column_by_name(pRow, pName);

     }

     const w_cass_value* w_cassandra_row_get_column_by_name_n(_In_ const w_cass_row* pRow, _In_  const char* pName, _In_ size_t pNameLength)
     {
         const char* _trace_info = "w_cassandra_row_get_column_by_name_n";
         if (!pRow || !pName || !pNameLength)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return  cass_row_get_column_by_name_n(pRow, pName, pNameLength);

     }

     const w_cass_data_type* w_cassandra_value_data_type(_In_ const w_cass_value* pValue)
     {
         const char* _trace_info = "w_cassandra_value_data_type";
         if (!pValue)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return   cass_value_data_type(pValue);

     }

     w_Cass_Error  w_cassandra_value_get_int8(_In_ const w_cass_value* pValue, _Inout_ int8_t* pOutput)
     {
         const char* _trace_info = "w_cassandra_value_get_int8";
         if (!pValue || !pOutput)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return -1;
         }
         return  cass_value_get_int8(pValue, pOutput);

     }

     w_Cass_Error  w_cassandra_value_get_int16(_In_ const w_cass_value* pValue, _Inout_ int16_t* pOutput)
     {
         const char* _trace_info = "w_cassandra_value_get_int16";
         if (!pValue || !pOutput)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return -1;
         }
         return  cass_value_get_int16(pValue, pOutput);

     }

     w_Cass_Error  w_cassandra_value_get_int32(_In_ const w_cass_value* pValue, _Inout_ int32_t* pOutput)
     {
         const char* _trace_info = "w_cassandra_value_get_int32";
         if (!pValue || !pOutput)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return -1;
         }
         return  cass_value_get_int32(pValue, pOutput);

     }

     w_Cass_Error w_cassandra_value_get_uint32(_In_ const w_cass_value* pValue, _Inout_  uint32_t* pOutput)
     {
         const char* _trace_info = "w_cassandra_value_get_uint32";
         if (!pValue || !pOutput)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return -1;
         }
         return  cass_value_get_uint32(pValue, pOutput);

     }


     w_Cass_Error  w_cassandra_value_get_int64(_In_ const w_cass_value* pValue, _Inout_ int64_t* pOutput)
     {
         const char* _trace_info = "w_cassandra_value_get_int64";
         if (!pValue || !pOutput)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return -1;
         }
         return   cass_value_get_int64(pValue, pOutput);

     }


     w_Cass_Error w_cassandra_value_get_float(_In_ const w_cass_value* pValue, _Inout_  float* pOutput)
     {
         const char* _trace_info = "w_cassandra_value_get_float";
         if (!pValue || !pOutput)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return -1;
         }
         return   cass_value_get_float(pValue, pOutput);

     }

     w_Cass_Error w_cassandra_value_get_double(_In_ const w_cass_value* pValue, _Inout_ double* pOutput)
     {
         const char* _trace_info = "w_cassandra_value_get_double";
         if (!pValue || !pOutput)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return -1;
         }
         return  cass_value_get_double(pValue, pOutput);

     }

     w_Cass_Error  w_cassandra_value_get_bool(_In_ const w_cass_value* pValue, _Inout_ w_bool_t* pOutput)
     {
         const char* _trace_info = "w_cassandra_value_get_bool";
         if (!pValue )
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return -1;
         }
         return  cass_value_get_bool(pValue, pOutput);

     }

     w_Cass_Error   w_cassandra_value_get_uuid(_In_ const w_cass_value* pValue, _Inout_  w_cass_uuid* pOutput)
     {
         const char* _trace_info = "w_cassandra_value_get_uuid";
         if (!pValue || !pOutput)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return -1;
         }
         return cass_value_get_uuid(pValue, pOutput);

     }

     w_Cass_Error w_cassandra_value_get_inet(_In_ const w_cass_value* pValue, _Inout_ w_cass_inet* pOutput)
     {
         const char* _trace_info = "w_cassandra_value_get_inet";
         if (!pValue || !pOutput)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return -1;
         }
         return  cass_value_get_inet(pValue, pOutput);

     }

     w_Cass_Error  w_cassandra_value_get_string(_In_ const w_cass_value* pValue, _Inout_ const char** pOutput, _Inout_ size_t* pOutputSize)
     {
         const char* _trace_info = "w_cassandra_value_get_string";
         if (!pValue || !pOutput || !pOutputSize)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return -1;
         }
         return   cass_value_get_string(pValue, pOutput, pOutputSize);

     }

     w_Cass_Error w_cassandra_value_get_bytes(_In_ const w_cass_value* pValue, _Inout_ const w_byte_t** pOutput, _Inout_ size_t* pOutputSize)
     {
         const char* _trace_info = "w_cassandra_value_get_bytes";
         if (!pValue || !pOutput || !pOutputSize)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return -1;
         }
         return   cass_value_get_bytes(pValue, pOutput, pOutputSize);

     }

     w_Cass_Error  w_cassandra_value_get_decimal(_In_ const w_cass_value* pValue, _Inout_ const w_byte_t** pVarint, _Inout_ size_t* pVarintSize, _Inout_ int32_t* pScale)
     {
         const char* _trace_info = "w_cassandra_value_get_decimal";
         if (!pValue || !pVarint || !pVarintSize || !pScale)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return -1;
         }
         return  cass_value_get_decimal(pValue, pVarint, pVarintSize, pScale);

     }

     w_Cass_Error  w_cassandra_value_get_duration(_In_ const w_cass_value* pValue, _Inout_ int32_t* pMonths, _Inout_ int32_t* pDays, _Inout_ int64_t* pNanos)
     {
         const char* _trace_info = "w_cassandra_value_get_duration";
         if (!pValue || !pMonths || !pDays || !pNanos)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return -1;
         }
         return  cass_value_get_duration(pValue, pMonths, pDays, pNanos);

     }

     w_cass_value_type w_cassandra_value_type(_In_ const w_cass_value* pValue)
     {
         const char* _trace_info = "w_cassandra_value_type";
         if (!pValue)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return -1;
         }
         return     cass_value_type(pValue);

     }

     w_bool_t w_cassandra_value_is_null(_In_ const w_cass_value* pValue)
     {
         const char* _trace_info = "w_cassandra_value_is_null";
         if (!pValue)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return false;
         }
         return  cass_value_is_null(pValue);

     }
     w_bool_t w_cassandra_value_is_collection(_In_ const w_cass_value* pValue)
     {
         const char* _trace_info = "w_cassandra_value_is_collection";
         if (!pValue)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return false;
         }
         return    cass_value_is_collection(pValue);

     }
     w_bool_t w_cassandra_value_is_duration(_In_ const w_cass_value* pValue)
     {
         const char* _trace_info = "w_cassandra_value_is_duration";
         if (!pValue)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return false;
         }
         return  cass_value_is_duration(pValue);

     }

     size_t  w_cassandra_value_item_count(_In_ const w_cass_value* pCollection)
     {
         const char* _trace_info = "w_cassandra_value_item_count";
         if (!pCollection)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return -1;
         }
         return   cass_value_item_count(pCollection);

     }

     w_cass_value_type  w_cassandra_value_primary_sub_type(_In_ const w_cass_value* pCollection)
     {
         const char* _trace_info = "w_cassandra_value_primary_sub_type";
         if (!pCollection)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return -1;
         }
         return   cass_value_primary_sub_type(pCollection);

     }

     w_cass_value_type w_cassandra_value_secondary_sub_type(_In_ const  w_cass_value* pCollection)
     {

         const char* _trace_info = "w_cassandra_value_secondary_sub_type";
         if (!pCollection)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return -1;
         }
            return  cass_value_secondary_sub_type(pCollection);
     }

     w_cass_uuid_gen*  w_cassandra_uuid_gen_new()
     {
         return   cass_uuid_gen_new();

     }

     w_cass_uuid_gen* w_cassandra_uuid_gen_new_with_node(_In_ uint64_t pNode)
     {
         const char* _trace_info = "w_cassandra_uuid_gen_new_with_node";
         if (!pNode)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
        
         return  cass_uuid_gen_new_with_node(pNode);

     }

     void  w_cassandra_uuid_gen_free(_In_ w_cass_uuid_gen* pUuiGen)
      {
         const char* _trace_info = "w_cassandra_uuid_gen_free";
         if (!pUuiGen)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
         }
         cass_uuid_gen_free(pUuiGen);

      }

     void w_cassandra_uuid_gen_time(_In_ w_cass_uuid_gen* pUuiGen, _Inout_ w_cass_uuid * pOutput)
     {
         const char* _trace_info = "w_cassandra_uuid_gen_time";
         if (!pUuiGen || !pOutput)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
         }
           cass_uuid_gen_time(pUuiGen, pOutput);

     }

     void  w_cassandra_uuid_gen_random(_In_ w_cass_uuid_gen* pUuiGen, _Inout_  w_cass_uuid* pOutput)
     {
         const char* _trace_info = "w_cassandra_uuid_gen_random";
         if (!pUuiGen || !pOutput)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
         }
         cass_uuid_gen_random(pUuiGen, pOutput);

     }

     void  w_cassandra_uuid_gen_from_time(_In_ w_cass_uuid_gen* pUuidGen, _In_ uint64_t pTimeStamp, _Inout_ w_cass_uuid* pOutput)
     {
         const char* _trace_info = "w_cassandra_uuid_gen_from_time";
         if (!pUuidGen || !pTimeStamp || !pOutput)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
         }
         cass_uuid_gen_from_time(pUuidGen, pTimeStamp, pOutput);
     }

     void  w_cassandra_uuid_min_from_time(_In_ uint64_t pTime, _Inout_ w_cass_uuid* pOutput)
     {
         const char* _trace_info = "w_cassandra_uuid_min_from_time";
         if (!pTime || !pOutput)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
         }
         cass_uuid_min_from_time(pTime, pOutput);

     }

     void w_cassandra_uuid_max_from_time(_In_ uint64_t pTime, _Inout_ w_cass_uuid* pOutput)
     {
         const char* _trace_info = "w_cassandra_uuid_max_from_time";
         if (!pTime || !pOutput)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
         }
         cass_uuid_max_from_time(pTime, pOutput);

     }


     uint64_t  w_cassandra_uuid_timestamp(_In_ w_cass_uuid pUuid)
     {
         
         return   cass_uuid_timestamp(pUuid);

     }

     uint8_t w_cassandra_uuid_version(_In_ w_cass_uuid pUuid)
     {

       return  cass_uuid_version(pUuid);

     }

     void w_cassandra_uuid_string(_In_ w_cass_uuid pUuid, _Inout_ char* pOutput)
     {
         const char* _trace_info = "w_cassandra_uuid_string";
         if (!pOutput)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
            
         }
         
         cass_uuid_string(pUuid, pOutput);

     }

     w_Cass_Error w_cassandra_uuid_from_string(_In_ const char* pStr, _Inout_ w_cass_uuid* pOutput)
     {
         const char* _trace_info = "w_cassandra_uuid_from_string";
         if (!pStr || !pOutput)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return -1;
         }
         
         return cass_uuid_from_string(pStr, pOutput);

     }

     w_Cass_Error w_cassandra_uuid_from_string_n(_In_ const char* pStr, _In_ size_t pStrLength, _Inout_ w_cass_uuid* pOutput)
     {
         const char* _trace_info = "w_cassandra_uuid_from_string_n";
         if (!pStr || !pOutput)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return -1;
         }
         return   cass_uuid_from_string_n(pStr, pStrLength, pOutput);

     }

     w_cass_time_stamp_gen*  w_cassandra_timestamp_gen_server_side_new()
     {
         
         return cass_timestamp_gen_server_side_new  ();
     }

     w_cass_time_stamp_gen* w_cassandra_timestamp_gen_monotonic_new()
     {
         return cass_timestamp_gen_monotonic_new();
     }

     w_cass_time_stamp_gen* w_cassandra_timestamp_gen_monotonic_new_with_settings(_In_ int64_t pWarningThresholdUs, _In_ int64_t pWarningIntervalMs)
     {
         const char* _trace_info = "w_cassandra_timestamp_gen_monotonic_new_with_settings";
         if (!pWarningThresholdUs || !pWarningIntervalMs)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return   cass_timestamp_gen_monotonic_new_with_settings(pWarningThresholdUs, pWarningIntervalMs);
     }

     void w_cassandra_timestamp_gen_free(_In_ w_cass_time_stamp_gen* pTimestampGen)
     {
         const char* _trace_info = "w_cassandra_timestamp_gen_free";
         if (!pTimestampGen)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

         }
         cass_timestamp_gen_free(pTimestampGen);

     }

     w_cass_retry_policy * w_cassandra_retry_policy_default_new()
     {

       return   cass_retry_policy_default_new();
     }

    /*deprecated*/
    w_cass_retry_policy*  w_cassandra_retry_policy_downgrading_consistency_new()
     {
         return cass_retry_policy_downgrading_consistency_new();
         
     }

     w_cass_retry_policy *  w_cassandra_retry_policy_fallthrough_new()
     {
         return  cass_retry_policy_fallthrough_new();

     }

     w_cass_retry_policy * w_cassandra_retry_policy_logging_new(_In_ w_cass_retry_policy* pChildRetryPolicy)
     {
         const char* _trace_info = "w_cassandra_retry_policy_logging_new";
         if (!pChildRetryPolicy)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return NULL;
         }
         return    cass_retry_policy_logging_new(pChildRetryPolicy);
     }

     void  w_cassandra_retry_policy_free(_In_ w_cass_retry_policy* pPolicy)
     {
         const char* _trace_info = "w_cassandra_retry_policy_free";
         if (!pPolicy)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

         }
        cass_retry_policy_free(pPolicy);
     }

     w_cass_custom_payload*  w_cassandra_custom_payload_new()
     {

         return  cass_custom_payload_new();

     }

     void w_cassandra_custom_payload_free(_In_ w_cass_custom_payload* pPayload)
     {
         const char* _trace_info = "w_cassandra_custom_payload_free";
         if (!pPayload)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

         }
         cass_custom_payload_free(pPayload);

     }

     void  w_cassandra_custom_payload_set(_In_ w_cass_custom_payload* pPayload, _In_  const char* pName, _In_ const w_byte_t* pValue, _In_ size_t pValueSize)
     {
         const char* _trace_info = "w_cassandra_custom_payload_set";
         if (!pPayload || !pName || !pValue || !pValueSize)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

         }
         cass_custom_payload_set(pPayload, pName, pValue, pValueSize);

     }

     void  w_cassandra_custom_payload_set_n(_In_ w_cass_custom_payload* pPayload, _In_ const char* pName, _In_  size_t pNameLength, _In_ const w_byte_t* pValue, _In_ size_t pValueSize)
     {
         const char* _trace_info = "w_cassandra_custom_payload_set_n";
         if (!pPayload || !pName ||  !pNameLength || !pValue || !pValueSize)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

         }
         cass_custom_payload_set_n(pPayload, pName, pNameLength, pValue, pValueSize);

     }

     void w_cassandra_custom_payload_remove(_In_ w_cass_custom_payload* pPayload, _In_ const char* pName)
     {
         const char* _trace_info = "w_cassandra_custom_payload_remove";
         if (!pPayload || !pName)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

         }
             cass_custom_payload_remove(pPayload, pName);

     }

     void  w_cassandra_custom_payload_remove_n(_In_ w_cass_custom_payload* pPayload, _In_ const char* pName, _In_ size_t pNameLength)
     {
         const char* _trace_info = "w_cassandra_custom_payload_remove_n";
         if (!pPayload || !pName || !pNameLength)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);

         }
         cass_custom_payload_remove_n(pPayload, pName, pNameLength);

     }

     const char*  w_cassandra_consistency_string(_In_ w_cass_consistency pConsistency)
     {
         
         return cass_consistency_string(pConsistency);

     }

     const char*  w_cassandra_write_type_string(_In_ w_cass_writetype pWriteType)
     {
         return  cass_write_type_string(pWriteType);

     }

     const char* w_cassandra_error_desc(_In_ w_Cass_Error error)
     {
         
             return cass_error_desc( error);
     }

     /*deprecated*/
     void  w_cassandra_log_cleanup()
     {
         cass_log_cleanup();
     }

     void w_cassandra_log_set_level(_In_ w_cass_log_level pLogLevel)
     {
         cass_log_set_level(pLogLevel);
     }


     void w_cassandra_log_set_callback(_In_ w_cass_log_callback callback, _In_  void* data)
     {
            cass_log_set_callback((CassLogCallback)callback, data);
     }

     /*deprecated*/
     void   w_cassandra_log_set_queue_size(size_t queue_size)
     {
         cass_log_set_queue_size( queue_size);
     }

     const char* w_cassandra_log_level_string(_In_ w_cass_log_level pLogLevel)
     {

      return cass_log_level_string(pLogLevel);

     }

     w_cass_inet  w_cassandra_inet_init_v4(_In_ const uint8_t* pAddress)
     {
         const char* _trace_info = "w_cassandra_inet_init_v4";
         if (!pAddress)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return;
         }
         return  cass_inet_init_v4(pAddress);
     }

     w_cass_inet  w_cassandra_inet_init_v6(_In_ const uint8_t* pAddress)
     {
         const char* _trace_info = "w_cassandra_inet_init_v6";
         if (!pAddress)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return;
         }
         return    cass_inet_init_v6(pAddress);

     }

     void w_cassandra_inet_string(_In_ w_cass_inet pInet, _Inout_ char* pOutput)
     {
         const char* _trace_info = "w_cassandra_inet_string";
         if (!pOutput )
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
         }
             cass_inet_string(pInet, pOutput);
     }


     w_Cass_Error  w_cassandra_inet_from_string(_In_ const char* pStr, _Inout_ w_cass_inet* pOutput)
     {
         const char* _trace_info = "w_cassandra_inet_from_string";
         if (!pStr || !pOutput)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return  -1;
         }
         return  cass_inet_from_string (pStr, pOutput);

     }

     w_Cass_Error  w_cassandra_inet_from_string_n(_In_ const char* pStr, _In_  size_t pStrLength, _Inout_  w_cass_inet* pOutput)
     {
         const char* _trace_info = "w_cassandra_inet_from_string_n";
         if (!pStr || !pStrLength || !pOutput)
         {
             W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
             return  -1;
         }
         return   cass_inet_from_string_n(pStr, pStrLength, pOutput);

     }

     uint32_t  w_cassandra_date_from_epoch(_In_ int64_t pEpochSecs)
     {
         
         return    cass_date_from_epoch(pEpochSecs);

     }

     int64_t  w_cassandra_time_from_epoch(_In_ int64_t pEpochSecs)
     {
         
         return    cass_time_from_epoch(pEpochSecs);

     }


     int64_t  w_cassandra_date_time_to_epoch(_In_ uint32_t pDate, _In_ int64_t pTime)
     {
         
         return cass_date_time_to_epoch(pDate, pTime);
     }

     void  w_cassandra_alloc_set_functions(_In_ w_cass_malloc_function pMallocFunc, _In_  w_cass_realloc_function RpeallocFunc, _In_ w_cass_free_function pFreeFunc)
     {
         cass_alloc_set_functions(pMallocFunc, RpeallocFunc, pFreeFunc);

     }





























