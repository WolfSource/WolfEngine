
#include"db/w_cassandra.h"
#include"cassandra.h"
#include <log/w_log.h>



/**
     * Describes the version of the connected Cassandra cluster.
     *
     * @struct CassVersion
     */

    w_CassExecProfile* w_cassandra_execution_profile_new() 
    {
    return cass_execution_profile_new();
    }

   void w_cassandra_execution_profile_free(w_CassExecProfile* profile)
   {
       if (!profile )
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_execution_profile_free");
           
       }
    
    cass_execution_profile_free(profile);


   }

   w_Cass_Error w_cassandra_execution_profile_set_request_timeout(w_CassExecProfile* profile, uint64_t timeout_ms)
   {
    if (!profile || !timeout_ms )
    {
        W_ASSERT(false, "missing parameters!. trace info: w_cassandra_execution_profile_set_request_timeout");
        return -1;
    }
   return cass_execution_profile_set_request_timeout(profile, timeout_ms);

   }

   w_Cass_Error w_cassandra_execution_profile_set_consistency(w_CassExecProfile* profile,  W_Cass_Consistency consistency) 
   {
    if (!profile || !consistency)
    {
        W_ASSERT(false, "missing parameters!. trace info: w_cassandra_execution_profile_set_consistency");
        return -1;
    }
    
       return cass_execution_profile_set_consistency( profile,consistency);
   }

   w_Cass_Error w_cassandra_execution_profile_set_serial_consistency(w_CassExecProfile* profile, W_Cass_Consistency serial_consistency)
   {
    if (!profile || !serial_consistency)
    {
        W_ASSERT(false, "missing parameters!. trace info: w_cassandra_execution_profile_set_serial_consistency");
        return -1;
    }
   return cass_execution_profile_set_serial_consistency(profile,serial_consistency);
   }

   w_Cass_Error  w_cassandra_execution_profile_set_load_balance_round_robin(w_CassExecProfile* profile)
   {
    if (!profile )
    {
        W_ASSERT(false, "missing parameters!. trace info: w_cassandra_execution_profile_set_load_balance_round_robin");
        return -1;
    }
   return cass_execution_profile_set_load_balance_round_robin(profile);
   }

   w_Cass_Error w_cassandra_execution_profile_set_load_balance_dc_aware(w_CassExecProfile* profile, const char* local_dc, unsigned used_hosts_per_remote_dc, w_bool_t allow_remote_dcs_for_local_cl) 
   {
    if (!profile)
    {
        W_ASSERT(false, "missing parameters!. trace info: w_cassandra_execution_profile_set_load_balance_dc_aware");
        return -1;
    }

     return cass_execution_profile_set_load_balance_dc_aware(profile, local_dc, used_hosts_per_remote_dc,     allow_remote_dcs_for_local_cl);
   }

   w_Cass_Error w_cassandra_execution_profile_set_load_balance_dc_aware_n(w_CassExecProfile* profile, const char* local_dc, size_t local_dc_length, unsigned used_hosts_per_remote_dc,  w_bool_t allow_remote_dcs_for_local_cl)
   {

    if (!profile || !local_dc_length || !used_hosts_per_remote_dc)
    {
        W_ASSERT(false, "missing parameters!. trace info: w_cassandra_execution_profile_set_load_balance_dc_aware_n");
        return -1;
    }
    return cass_execution_profile_set_load_balance_dc_aware_n(profile,  local_dc,   local_dc_length,   used_hosts_per_remote_dc,  allow_remote_dcs_for_local_cl);
   }

   w_Cass_Error w_cassandra_execution_profile_set_token_aware_routing(w_CassExecProfile* profile,w_bool_t enabled)
   {
    if (!profile )
    {
        W_ASSERT(false, "missing parameters!. trace info: w_cassandra_execution_profile_set_token_aware_routing");
        return -1;
    }
   
       return cass_execution_profile_set_token_aware_routing( profile, enabled);
   }

   w_Cass_Error w_cassandra_execution_profile_set_token_aware_routing_shuffle_replicas(w_CassExecProfile* profile, w_bool_t enabled) 

   {
    if (!profile)
    {
        W_ASSERT(false, "missing parameters!. trace info: w_cassandra_execution_profile_set_token_aware_routing_shuffle_replicas");
        return -1;
    }
    
    return cass_execution_profile_set_token_aware_routing_shuffle_replicas(profile, enabled);
   }

   w_Cass_Error w_cassandra_execution_profile_set_latency_aware_routing(w_CassExecProfile* profile, w_bool_t enabled)
   {
    if (!profile)
    {
        W_ASSERT(false, "missing parameters!. trace info: w_cassandra_execution_profile_set_latency_aware_routing");
        return -1;
    }
      return   cass_execution_profile_set_latency_aware_routing( profile,
             enabled);
    }

    w_Cass_Error w_cassandra_execution_profile_set_latency_aware_routing_settings(w_CassExecProfile* profile, double exclusion_threshold, uint64_t scale_ms, uint64_t retry_period_ms,uint64_t update_rate_ms, uint64_t min_measured)
    {
     if (!profile )
     {
        W_ASSERT(false, "missing parameters!. trace info: w_cassandra_execution_profile_set_latency_aware_routing_settings");
        return -1;
      }
      return    cass_execution_profile_set_latency_aware_routing_settings(  profile,   exclusion_threshold,    scale_ms,  retry_period_ms,update_rate_ms, min_measured);
    }

   w_Cass_Error w_cassandra_execution_profile_set_whitelist_filtering(w_CassExecProfile* profile, const char* hosts)
   {
    if (!profile || !hosts)
    {
        W_ASSERT(false, "missing parameters!. trace info: w_cassandra_execution_profile_set_whitelist_filtering");
        return -1;
    }
    
      return  cass_execution_profile_set_whitelist_filtering( profile, hosts);
    }

    w_Cass_Error w_cassandra_execution_profile_set_whitelist_filtering_n(w_CassExecProfile* profile, const char* hosts, size_t hosts_length)
    {
      if (!profile || !hosts )
      {
        W_ASSERT(false, "missing parameters!. trace info: w_cassandra_execution_profile_set_whitelist_filtering_n");
        return -1;
      }
    return cass_execution_profile_set_whitelist_filtering_n(profile, hosts, hosts_length);
    }

    w_Cass_Error w_cassandra_execution_profile_set_blacklist_filtering(w_CassExecProfile* profile,
    const char* hosts)
    {

     if (!profile || !hosts)
     {
        W_ASSERT(false, "missing parameters!. trace info: w_cassandra_execution_profile_set_blacklist_filtering");
        return -1;
     }
    
     return   cass_execution_profile_set_blacklist_filtering(profile, hosts);

    }

    w_Cass_Error w_cassandra_execution_profile_set_blacklist_filtering_n(w_CassExecProfile* profile, const char* hosts,size_t hosts_length)
    {
      if (!profile || !hosts)
      {
        W_ASSERT(false, "missing parameters!. trace info: w_cassandra_execution_profile_set_blacklist_filtering_n");
        return -1;
      }
     return   cass_execution_profile_set_blacklist_filtering_n(profile, hosts,
        hosts_length);
    }

    w_Cass_Error w_cassandra_execution_profile_set_whitelist_dc_filtering(w_CassExecProfile* profile,  const char* dcs)
    {
      if (!profile )
      {
        W_ASSERT(false, "missing parameters!. trace info: w_cassandra_execution_profile_set_whitelist_dc_filtering");
        return -1;
      }
    
      return  cass_execution_profile_set_whitelist_dc_filtering( profile, dcs);
    }

    w_Cass_Error w_cassandra_execution_profile_set_whitelist_dc_filtering_n(w_CassExecProfile* profile, const char* dcs, size_t dcs_length)
    {
     if (!profile)
     {
        W_ASSERT(false, "missing parameters!. trace info: w_cassandra_execution_profile_set_whitelist_dc_filtering_n");
        return -1;
     }
   
     return   cass_execution_profile_set_whitelist_dc_filtering_n( profile,
         dcs, dcs_length);
    }

    w_Cass_Error w_cassandra_execution_profile_set_blacklist_dc_filtering(w_CassExecProfile* profile, const char* dcs)
    {
       if (!profile)
       {
        W_ASSERT(false, "missing parameters!. trace info: w_cassandra_execution_profile_set_blacklist_dc_filtering");
        return -1;
       }
        return cass_execution_profile_set_blacklist_dc_filtering( profile, dcs);
    }

    w_Cass_Error  w_cassandra_execution_profile_set_blacklist_dc_filtering_n(w_CassExecProfile* profile,   const char* dcs,   size_t dcs_length)
    {
     if (!profile)
     {
         W_ASSERT(false, "missing parameters!. trace info: w_cassandra_execution_profile_set_blacklist_dc_filtering_n");
         return -1;
     }
     
        return cass_execution_profile_set_blacklist_dc_filtering_n(profile,dcs,dcs_length);
    }

    w_Cass_Error w_cassandra_execution_profile_set_retry_policy(w_CassExecProfile* profile,  w_Cass_Retry_Policy* retry_policy) 
    {
     if (!profile)
     {
         W_ASSERT(false, "missing parameters!. trace info: w_cassandra_execution_profile_set_blacklist_dc_filtering_n");
         return -1;
     }
        return cass_execution_profile_set_retry_policy( profile, retry_policy);

    }

    w_Cass_Error w_cassandra_execution_profile_set_constant_speculative_execution_policy(w_CassExecProfile* profile,  int64_t constant_delay_ms,   int max_speculative_executions)
    {
      if (!profile)
      {
         W_ASSERT(false, "missing parameters!. trace info: w_cassandra_execution_profile_set_constant_speculative_execution_policy");
         return -1;
      }
     
        return cass_execution_profile_set_constant_speculative_execution_policy( profile,
              constant_delay_ms,
              max_speculative_executions);
    }

    w_Cass_Error w_cassandra_execution_profile_set_no_speculative_execution_policy(w_CassExecProfile* profile)
    {
     if (!profile)
     {
         W_ASSERT(false, "missing parameters!. trace info: w_cassandra_execution_profile_set_no_speculative_execution_policy");
         return -1;
     }
     
         return cass_execution_profile_set_no_speculative_execution_policy(profile);
    }

   w_Cass_Cluster*w_cassandra_cluster_new()
   {
    return  cass_cluster_new();

   }

   void w_cassandra_cluster_free(w_Cass_Cluster* cluster)
   {
     if (!cluster)
     {
         W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_free");
         
     }
    cass_cluster_free(cluster);

   }

   w_Cass_Error  w_cassandra_cluster_set_contact_points(w_Cass_Cluster* cluster,  const char* contact_points)
   {
     if (!cluster)
     {
         W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_contact_points");
         return -1;
     }
     return cass_cluster_set_contact_points( cluster, contact_points);
   }

   w_Cass_Error  w_cassandra_cluster_set_contact_points_n(w_Cass_Cluster* cluster,  const char* contact_points,    size_t contact_points_length) 
   {
     if (!cluster || !contact_points)
     {
         W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_contact_points_n");
         return -1;
     }
     
     return cass_cluster_set_contact_points_n(cluster, contact_points, contact_points_length);
     
   }

   w_Cass_Error w_cassandra_cluster_set_port(w_Cass_Cluster* cluster, int port) 
   {

     if (!cluster || !port)
     {
         W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_port");
         return -1;
     }

     return 
         cass_cluster_set_port(cluster, port);
   }

   w_Cass_Error  w_cassandra_cluster_set_local_address(w_Cass_Cluster* cluster,  const char* name)
   {
      if (!cluster || !name)
      {
          W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_local_address");
          return -1;
      }
       
         return cass_cluster_set_local_address( cluster, name);
   }

   w_Cass_Error w_cassandra_cluster_set_local_address_n(w_Cass_Cluster* cluster,const char* name, size_t name_length) 
   {
      if (!cluster || !name)
      {
          W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_local_address_n");
          return -1;
      }
      
        return  cass_cluster_set_local_address_n( cluster,name,name_length);
   }


   void w_cassandra_cluster_set_ssl(w_Cass_Cluster* cluster, w_Cass_Ssl* ssl)
   {
      if (!cluster || !ssl)
      {
          W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_ssl");
      }
       cass_cluster_set_ssl( cluster,ssl);
   }

   w_Cass_Error w_cassandra_cluster_set_authenticator_callbacks(w_Cass_Cluster* cluster,const w_Cass_Authenticator_Callbacks exchange_callbacks,CassAuthenticatorDataCleanupCallback cleanup_callback,  void* data)
   {
      if (!cluster )
      {
          W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_authenticator_callbacks");
          return -1;

      }
      return  cass_cluster_set_authenticator_callbacks(cluster,exchange_callbacks, cleanup_callback,data);
   }

   w_Cass_Error w_cassandra_cluster_set_protocol_version(w_Cass_Cluster* cluster, int protocol_version)
   {

      if (!cluster ||  !protocol_version)
      {
          W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_protocol_version");
          return -1;

      }
      return cass_cluster_set_protocol_version( cluster, protocol_version);
   }


   w_Cass_Error  w_cassandra_cluster_set_use_beta_protocol_version(w_Cass_Cluster* cluster,  w_bool_t enable)
   {

      if (!cluster || !enable)
      {
          W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_use_beta_protocol_version");
          return -1;

      }
      return     cass_cluster_set_use_beta_protocol_version( cluster,enable);

   }


   w_Cass_Error  w_cassandra_cluster_set_consistency(w_Cass_Cluster* cluster,  W_Cass_Consistency consistency)
   {
      if (!cluster || !consistency)
      {
          W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_consistency");
          return -1;

      }
      
         return  cass_cluster_set_consistency(cluster, consistency);
   }

   w_Cass_Error w_cassandra_cluster_set_serial_consistency(w_Cass_Cluster* cluster, W_Cass_Consistency consistency)
   {

      if (!cluster || !consistency)
      {
          W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_serial_consistency");
          return -1;

      }

       return cass_cluster_set_serial_consistency(cluster, consistency);

   }

   w_Cass_Error w_cassandra_cluster_set_num_threads_io(w_Cass_Cluster* cluster, unsigned num_threads)
   {

       if (!cluster || !num_threads)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_num_threads_io");
           return -1;

       }
       return cass_cluster_set_num_threads_io(cluster, num_threads);
   }


   w_Cass_Error w_cassandra_cluster_set_queue_size_io(w_Cass_Cluster* cluster, unsigned queue_size) 
   {

       if (!cluster || !queue_size)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_queue_size_io");
           return 0;

       }

     return  cass_cluster_set_queue_size_io(cluster, queue_size);
   }

   w_Cass_Error w_cassandra_cluster_set_queue_size_event(w_Cass_Cluster* cluster, unsigned queue_size)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_queue_size_event");
           return 0;

       }
       return cass_cluster_set_queue_size_event(cluster, queue_size);

   }
   w_Cass_Error w_cassandra_cluster_set_core_connections_per_host(w_Cass_Cluster* cluster,unsigned num_connections)
   {

       if (!cluster )
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_core_connections_per_host");
           return 0;

       }
           return cass_cluster_set_core_connections_per_host( cluster, num_connections);
   }

   w_Cass_Error w_cassandra_cluster_set_max_connections_per_host(w_Cass_Cluster* cluster, unsigned num_connections)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_max_connections_per_host");

       }
       return cass_cluster_set_max_connections_per_host(cluster, num_connections);

   }

   void w_cassandra_cluster_set_constant_reconnect(w_Cass_Cluster* cluster, uint64_t delay_ms)
   {
       if (!cluster )
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_constant_reconnect");
           
       }

       
           cass_cluster_set_constant_reconnect( cluster,delay_ms);
   }

   void  w_cassandra_cluster_set_reconnect_wait_time(w_Cass_Cluster* cluster, unsigned wait_time)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_reconnect_wait_time");

       }
       cass_cluster_set_reconnect_wait_time(cluster, wait_time);
   }
   w_Cass_Error  w_cassandra_cluster_set_exponential_reconnect(w_Cass_Cluster* cluster,uint64_t base_delay_ms, uint64_t max_delay_ms)
   {

       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_exponential_reconnect");
           return -1;
       }
      return  cass_cluster_set_exponential_reconnect(cluster, base_delay_ms, max_delay_ms);
   }


   w_Cass_Error  w_cassandra_cluster_set_coalesce_delay(w_Cass_Cluster* cluster, int64_t delay_us)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_coalesce_delay");
           return -1;
       }
         return   cass_cluster_set_coalesce_delay( cluster, delay_us);
   }

   w_Cass_Error w_cassandra_cluster_set_new_request_ratio(w_Cass_Cluster* cluster, int32_t ratio)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_new_request_ratio");
           return -1;
       }
       return cass_cluster_set_new_request_ratio( cluster, ratio);

   }

   w_Cass_Error  w_cassandra_cluster_set_max_concurrent_creation(w_Cass_Cluster* cluster, unsigned num_connections)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_max_concurrent_creation");
           return -1;
       }
       return cass_cluster_set_max_concurrent_creation(cluster, num_connections);

   }

   w_Cass_Error  w_cassandra_cluster_set_max_concurrent_requests_threshold(w_Cass_Cluster* cluster, unsigned num_requests)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_max_concurrent_requests_threshold");
           return -1;
       }
       return cass_cluster_set_max_concurrent_requests_threshold(cluster, num_requests);
   }

   w_Cass_Error  w_cassandra_cluster_set_max_requests_per_flush(w_Cass_Cluster* cluster,  unsigned num_requests)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_max_requests_per_flush");
           return -1;
       }
       return cass_cluster_set_max_requests_per_flush(cluster, num_requests);
   }

   w_Cass_Error  w_cassandra_cluster_set_write_bytes_high_water_mark(w_Cass_Cluster* cluster, unsigned num_bytes)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_write_bytes_high_water_mark");
           return -1;
       }
       return cass_cluster_set_write_bytes_high_water_mark(cluster, num_bytes);
   }

   w_Cass_Error w_cassandra_cluster_set_write_bytes_low_water_mark(w_Cass_Cluster* cluster, unsigned num_bytes)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_write_bytes_low_water_mark");
           return -1;
       }
       return cass_cluster_set_write_bytes_low_water_mark(cluster, num_bytes);

   }
   w_Cass_Error   w_cassandra_cluster_set_pending_requests_high_water_mark(w_Cass_Cluster* cluster, unsigned num_requests)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_pending_requests_high_water_mark");
           return -1;
       }
       return cass_cluster_set_pending_requests_high_water_mark(cluster, num_requests);

   }
   w_Cass_Error  w_cassandra_cluster_set_pending_requests_low_water_mark(w_Cass_Cluster* cluster, unsigned num_requests)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_pending_requests_low_water_mark");
           return -1;
       }
       return cass_cluster_set_pending_requests_low_water_mark(cluster, num_requests);

   }
   w_Cass_Retry_Policy*  w_cassandra_retry_policy_downgrading_consistency_new()
   {
       return cass_retry_policy_downgrading_consistency_new();
   }

   void w_cassandra_log_cleanup()
   {
       cass_log_cleanup();
   }

   void w_cassandra_cluster_set_connect_timeout(w_Cass_Cluster* cluster,  unsigned timeout_ms)
   {

       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_connect_timeout");
       }
      cass_cluster_set_connect_timeout(cluster, timeout_ms);
   }

   const char* w_cassandra_consistency_string(W_Cass_Consistency consistency)
   {
       if (!consistency)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_consistency_string");
           return NULL;
       }
       cass_consistency_string(consistency);
   }


   void  w_cassandra_log_set_queue_size(size_t queue_size)
   {
       cass_log_set_queue_size(queue_size);
   }
   void w_cassandra_cluster_set_request_timeout(w_Cass_Cluster* cluster, unsigned timeout_ms)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_request_timeout");
       }
           cass_cluster_set_request_timeout(cluster, timeout_ms);
   }

   void w_cassandra_cluster_set_resolve_timeout(w_Cass_Cluster* cluster, unsigned timeout_ms)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_resolve_timeout");
       }
       cass_cluster_set_resolve_timeout( cluster, timeout_ms);

   }


   void  w_cassandra_cluster_set_max_schema_wait_time(w_Cass_Cluster* cluster, unsigned wait_time_ms)
   {

       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_max_schema_wait_time");
       }

      cass_cluster_set_max_schema_wait_time(cluster, wait_time_ms);
   }

   void w_cassandra_cluster_set_tracing_max_wait_time(w_Cass_Cluster* cluster, unsigned max_wait_time_ms)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_tracing_max_wait_time");
       }

     cass_cluster_set_tracing_max_wait_time( cluster, max_wait_time_ms);
   }

   void w_cassandra_cluster_set_tracing_retry_wait_time(w_Cass_Cluster* cluster,unsigned retry_wait_time_ms)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_tracing_retry_wait_time");
       }
       cass_cluster_set_tracing_retry_wait_time( cluster, retry_wait_time_ms);
   }

   void w_cassandra_cluster_set_tracing_consistency(w_Cass_Cluster* cluster, W_Cass_Consistency consistency)
   {

       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_tracing_consistency");
       }
       cass_cluster_set_tracing_consistency( cluster, consistency);

   }



   void w_cassandra_cluster_set_credentials(w_Cass_Cluster* cluster, const char* username,const char* password)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_credentials");
       }

      cass_cluster_set_credentials( cluster, username, password);
   }

   void w_cassandra_cluster_set_credentials_n(w_Cass_Cluster* cluster, const char* username,size_t username_length, const char* password, size_t password_length)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_credentials_n");
       }
     cass_cluster_set_credentials_n( cluster, username,  username_length,password, password_length);

   }
   void w_cassandra_cluster_set_load_balance_round_robin(w_Cass_Cluster* cluster)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_load_balance_round_robin");
       }
       cass_cluster_set_load_balance_round_robin( cluster);
   }

   w_Cass_Error w_cassandra_cluster_set_load_balance_dc_aware(w_Cass_Cluster* cluster, const char* local_dc, unsigned used_hosts_per_remote_dc, w_bool_t allow_remote_dcs_for_local_cl)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_load_balance_dc_aware");
           return -1;
       }
       
         return cass_cluster_set_load_balance_dc_aware(cluster, local_dc,  used_hosts_per_remote_dc,  allow_remote_dcs_for_local_cl);
   }

  
   w_Cass_Error   w_cassandra_cluster_set_load_balance_dc_aware_n(w_Cass_Cluster* cluster, const char* local_dc, size_t local_dc_length,  unsigned used_hosts_per_remote_dc,  w_bool_t allow_remote_dcs_for_local_cl)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_load_balance_dc_aware_n");
           return -1;

       }

           
            return   cass_cluster_set_load_balance_dc_aware_n( cluster, local_dc,  local_dc_length, used_hosts_per_remote_dc, allow_remote_dcs_for_local_cl);
   }

   void w_cassandra_cluster_set_token_aware_routing(w_Cass_Cluster* cluster, w_bool_t enabled)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_token_aware_routing");
       }
           cass_cluster_set_token_aware_routing(cluster, enabled);

   }

   void w_cassandra_cluster_set_token_aware_routing_shuffle_replicas(w_Cass_Cluster* cluster,  w_bool_t enabled)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_token_aware_routing_shuffle_replicas");
       }

           
       cass_cluster_set_token_aware_routing_shuffle_replicas( cluster, enabled);

   }
   void  w_cassandra_cluster_set_latency_aware_routing(w_Cass_Cluster* cluster,   w_bool_t enabled)
   {
     if (!cluster)
     {
         W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_latency_aware_routing");
     }
       cass_cluster_set_latency_aware_routing( cluster, enabled);
   }

    void  w_cassandra_cluster_set_latency_aware_routing_settings(w_Cass_Cluster* cluster,  double exclusion_threshold,  uint64_t scale_ms, uint64_t retry_period_ms, uint64_t update_rate_ms, uint64_t min_measured) 
    {
        if (!cluster)
        {
            W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_latency_aware_routing_settings");
        }

          cass_cluster_set_latency_aware_routing_settings( cluster,  exclusion_threshold, scale_ms, retry_period_ms, update_rate_ms, min_measured);
    }

    void w_cassandra_cluster_set_whitelist_filtering(w_Cass_Cluster* cluster, const char* hosts)
    {
        if (!cluster)
        {
            W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_whitelist_filtering");
        }
           
         cass_cluster_set_whitelist_filtering( cluster, hosts);
    }

   void w_cassandra_cluster_set_whitelist_filtering_n(w_Cass_Cluster* cluster, const char* hosts, size_t hosts_length)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_whitelist_filtering_n");
       }
          cass_cluster_set_whitelist_filtering_n( cluster, hosts, hosts_length);
   }

   void w_cassandra_cluster_set_blacklist_filtering(w_Cass_Cluster* cluster,  const char* hosts)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_blacklist_filtering");
       }
           
       cass_cluster_set_blacklist_filtering(cluster, hosts);
   }

   void w_cassandra_cluster_set_blacklist_filtering_n(w_Cass_Cluster* cluster,  const char* hosts, size_t hosts_length)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_blacklist_filtering_n");
       }
           
     cass_cluster_set_blacklist_filtering_n( cluster, hosts,hosts_length);

   }

   void w_cassandra_cluster_set_whitelist_dc_filtering(w_Cass_Cluster* cluster, const char* dcs)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_whitelist_dc_filtering");
       }
           
       cass_cluster_set_whitelist_dc_filtering( cluster, dcs);
   }

   void w_cassandra_cluster_set_whitelist_dc_filtering_n(w_Cass_Cluster* cluster, const char* dcs, size_t dcs_length)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_whitelist_dc_filtering_n");
       }
           
    cass_cluster_set_whitelist_dc_filtering_n( cluster, dcs,dcs_length);

   }

   void w_cassandra_cluster_set_blacklist_dc_filtering(w_Cass_Cluster* cluster, const char* dcs)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_blacklist_dc_filtering");
       }
          cass_cluster_set_blacklist_dc_filtering( cluster, dcs);
   }

   void w_cassandra_cluster_set_blacklist_dc_filtering_n(w_Cass_Cluster* cluster,  const char* dcs,size_t dcs_length)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_blacklist_dc_filtering_n");
       }
           cass_cluster_set_blacklist_dc_filtering_n( cluster, dcs, dcs_length);
   }

   void w_cassandra_cluster_set_tcp_nodelay(w_Cass_Cluster* cluster,  w_bool_t enabled) 
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_tcp_nodelay");
       }

     cass_cluster_set_tcp_nodelay( cluster, enabled);
   }

   void w_cassandra_cluster_set_tcp_keepalive(w_Cass_Cluster* cluster, w_bool_t enabled, unsigned delay_secs)
   {

       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_tcp_keepalive");
       }
       cass_cluster_set_tcp_keepalive( cluster,  enabled,  delay_secs);
   }

   void w_cassandra_cluster_set_timestamp_gen(w_Cass_Cluster* cluster,  w_Cass_Time_stamp_Gen* timestamp_gen)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_timestamp_gen");
       }
      cass_cluster_set_timestamp_gen( cluster, timestamp_gen);
   }

    void w_cassandra_cluster_set_connection_heartbeat_interval(w_Cass_Cluster* cluster, unsigned interval_secs)
    {
        if (!cluster)
        {
            W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_connection_heartbeat_interval");
        }

        cass_cluster_set_connection_heartbeat_interval( cluster,interval_secs);
    }

   void w_cassandra_cluster_set_connection_idle_timeout(w_Cass_Cluster* cluster,  unsigned timeout_secs)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_connection_idle_timeout");
       }
           cass_cluster_set_connection_idle_timeout( cluster,timeout_secs);
   }

   void w_cassandra_cluster_set_retry_policy(w_Cass_Cluster* cluster, w_Cass_Retry_Policy* retry_policy)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_retry_policy");
       }
          cass_cluster_set_retry_policy( cluster,  retry_policy);
   }

   void w_cassandra_cluster_set_use_schema(w_Cass_Cluster* cluster, w_bool_t enabled)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_use_schema");
       }

     cass_cluster_set_use_schema(cluster, enabled);

   }
   w_Cass_Error w_cassandra_cluster_set_use_hostname_resolution(w_Cass_Cluster* cluster,  w_bool_t enabled)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_use_hostname_resolution");
           return -1 ;
       }
           
   return   cass_cluster_set_use_hostname_resolution( cluster,  enabled);
   }

   w_Cass_Error w_cassandra_cluster_set_use_randomized_contact_points(w_Cass_Cluster* cluster,  w_bool_t enabled)
   {
           
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_use_randomized_contact_points");
           return -1;
       }

       return cass_cluster_set_use_randomized_contact_points( cluster, enabled);
       
   
   }


   w_Cass_Error w_cassandra_cluster_set_constant_speculative_execution_policy(w_Cass_Cluster* cluster, int64_t constant_delay_ms, int max_speculative_executions)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_constant_speculative_execution_policy");
           return -1;
       }
     return cass_cluster_set_constant_speculative_execution_policy(  cluster,  constant_delay_ms, max_speculative_executions);
   }


   w_Cass_Error  w_cassandra_cluster_set_no_speculative_execution_policy(w_Cass_Cluster* cluster)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_no_speculative_execution_policy");
           return -1;
       }
           return  cass_cluster_set_no_speculative_execution_policy( cluster);
   }


   w_Cass_Error w_cassandra_cluster_set_max_reusable_write_objects(w_Cass_Cluster* cluster,  unsigned num_objects)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_max_reusable_write_objects");
           return -1;
       }


        return   cass_cluster_set_max_reusable_write_objects( cluster, num_objects);
   }

   w_Cass_Error  w_cassandra_cluster_set_execution_profile(w_Cass_Cluster* cluster,  const char* name,  w_CassExecProfile* profile) 
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_execution_profile");
           return -1;
       }

           
           return  cass_cluster_set_execution_profile( cluster,name, profile);
   }

   w_Cass_Error w_cassandra_cluster_set_execution_profile_n(w_Cass_Cluster* cluster, const char* name, size_t name_length, w_CassExecProfile* profile)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_execution_profile_n");
           return -1;
       }
           
      return cass_cluster_set_execution_profile_n( cluster, name, name_length, profile);
   }

   w_Cass_Error w_cassandra_cluster_set_prepare_on_all_hosts(w_Cass_Cluster* cluster, w_bool_t enabled)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_prepare_on_all_hosts");
           return -1;
       }
           
    return cass_cluster_set_prepare_on_all_hosts( cluster,enabled);
   }

   w_Cass_Error  w_cassandra_cluster_set_prepare_on_up_or_add_host(w_Cass_Cluster* cluster, w_bool_t enabled)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_prepare_on_up_or_add_host");
           return -1;
       }
      return  cass_cluster_set_prepare_on_up_or_add_host(cluster, enabled);
   }


   w_Cass_Error w_cassandra_cluster_set_no_compact(w_Cass_Cluster* cluster,w_bool_t enabled)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_no_compact");
           return -1;
       }

       return cass_cluster_set_no_compact(cluster, enabled);
   }


   w_Cass_Error w_cass_cluster_set_host_listener_callback(w_Cass_Cluster* cluster, CassHostListenerCallback callback,  void* data)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cass_cluster_set_host_listener_callback");
           return -1;
       }


           
    return   cass_cluster_set_host_listener_callback( cluster,callback,data);
   }


   w_Cass_Error w_cassandra_cluster_set_cloud_secure_connection_bundle(w_Cass_Cluster* cluster, const char* path)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_cloud_secure_connection_bundle");
           return -1;
       }
               
       return cass_cluster_set_cloud_secure_connection_bundle( cluster, path);

   }

   w_Cass_Error w_cassandra_cluster_set_cloud_secure_connection_bundle_n(w_Cass_Cluster* cluster, const char* path, size_t path_length)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_cloud_secure_connection_bundle");
           return -1;
       }

     return  cass_cluster_set_cloud_secure_connection_bundle_n( cluster, path, path_length);
   }

   w_Cass_Error w_cassandra_cluster_set_cloud_secure_connection_bundle_no_ssl_lib_init(w_Cass_Cluster* cluster,  const char* path)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_cloud_secure_connection_bundle_no_ssl_lib_init");
           return -1;
       }
           
      return cass_cluster_set_cloud_secure_connection_bundle_no_ssl_lib_init( cluster, path);
   }

   w_Cass_Error w_cassandra_cluster_set_cloud_secure_connection_bundle_no_ssl_lib_init_n(w_Cass_Cluster* cluster,  const char* path, size_t path_length)
   {

       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_cloud_secure_connection_bundle_no_ssl_lib_init_n");
           return -1;
       }

     return   cass_cluster_set_cloud_secure_connection_bundle_no_ssl_lib_init_n( cluster,path,path_length);
   }

   void  w_cassandra_cluster_set_application_name(w_Cass_Cluster* cluster, const char* application_name)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_application_name");
       }
      cass_cluster_set_application_name( cluster,application_name);
   }


   void w_cassandra_cluster_set_application_name_n(w_Cass_Cluster* cluster, const char* application_name,  size_t application_name_length)
   {
       if (!cluster)
       {
           W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_application_name_n");
       }

    cass_cluster_set_application_name_n( cluster,application_name,application_name_length);
   }

    void  w_cassandra_cluster_set_application_version(w_Cass_Cluster* cluster, const char* application_version)
    {

        if (!cluster)
        {
            W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_application_version");
        }

           cass_cluster_set_application_version(cluster,application_version);
    }

    void w_cassandra_cluster_set_application_version_n(w_Cass_Cluster* cluster, const char* application_version,size_t application_version_length)
    {
         
        if (!cluster)
        {
            W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_application_version_n");
        }
        
        cass_cluster_set_application_version_n( cluster, application_version, application_version_length);
    }


    void w_cassandra_cluster_set_client_id(w_Cass_Cluster* cluster, w_CassUuid client_id)
    {
        if (!cluster)
        {
            W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_client_id");
        }

          cass_cluster_set_client_id( cluster,  client_id);
    }

    void w_cassandra_cluster_set_monitor_reporting_interval(w_Cass_Cluster* cluster,   unsigned interval_secs)
    {
        if (!cluster)
        {
            W_ASSERT(false, "missing parameters!. trace info: w_cassandra_cluster_set_monitor_reporting_interval");
        }

       cass_cluster_set_monitor_reporting_interval(cluster,interval_secs);
    }

    w_Cass_Session* w_cassandra_session_new()
    { 
        return   cass_session_new();
    }

    void w_cassandra_session_free(w_Cass_Session* session)
    {
        if (!session)
        {
            W_ASSERT(false, "missing parameters!. trace info: w_cassandra_session_free");
        }
           
        cass_session_free( session);
    }


    w_Cass_Future* w_cassandra_session_connect(w_Cass_Session* session, const w_Cass_Cluster* cluster)
    {
        if (!session || !cluster)
        {
            W_ASSERT(false, "missing parameters!. trace info: w_cassandra_session_connect");
            return NULL;
        }


      return  cass_session_connect( session, cluster);
    }

    w_Cass_Future* w_cassandra_session_connect_keyspace(w_Cass_Session* session, const w_Cass_Cluster* cluster, const char* keyspace)
    {
        if (!session || !cluster)
        {
            W_ASSERT(false, "missing parameters!. trace info: w_cassandra_session_connect_keyspace");
            return NULL;
        }
           
       return cass_session_connect_keyspace( session, cluster, keyspace);
    }

    w_Cass_Future*  w_cassandra_session_connect_keyspace_n(w_Cass_Session* session,const w_Cass_Cluster* cluster,  const char* keyspace, size_t keyspace_length)
    {
        if (!session || !cluster)
        {
            W_ASSERT(false, "missing parameters!. trace info: w_cassandra_session_connect_keyspace_n");
            return NULL;
        }


           return cass_session_connect_keyspace_n(session, cluster,keyspace, keyspace_length);
    }

    w_Cass_Future*  w_cassandra_session_close(w_Cass_Session* session)
    {
        if (!session )
        {
            W_ASSERT(false, "missing parameters!. trace info: w_cassandra_session_close");
            return NULL;
        }
           return cass_session_close(session);
    }
       
    w_Cass_Future* w_cassandra_session_prepare(w_Cass_Session* session, const char* query)
    {
        if (!session)
        {
            W_ASSERT(false, "missing parameters!. trace info: w_cassandra_session_prepare");
            return NULL;
        }
        
        return cass_session_prepare( session, query);
    }

     w_Cass_Future*  w_cassandra_session_prepare_n(w_Cass_Session* session,  const char* query,  size_t query_length)
     {
         if (!session)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_session_prepare_n");
             return NULL;
         }

        return  cass_session_prepare_n(session, query,query_length);
     }

     w_Cass_Future* w_cassandra_session_prepare_from_existing(w_Cass_Session* session,  w_Cass_Statement* statement)
     {
         if (!session || !statement)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_session_prepare_from_existing");
             return NULL;
         }
           return cass_session_prepare_from_existing( session, statement);
     }


     w_Cass_Future*  w_cassandra_session_execute(w_Cass_Session* session,  const w_Cass_Statement* statement)
     {
         if (!session || !statement)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_session_execute");
             return NULL;
         }


           return cass_session_execute( session, statement);
     }

     w_Cass_Future* w_cassandra_session_execute_batch(w_Cass_Session* session,const w_Cass_Batch* batch)
     {
         if (!session || !batch)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_session_execute_batch");
             return NULL;
         }


        return cass_session_execute_batch( session,batch);
     }

     const w_Cass_Schema_Meta* w_cassandra_session_get_schema_meta(const w_Cass_Session* session)
     {
         if (!session )
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_session_get_schema_meta");
             return NULL;
         }
           return cass_session_get_schema_meta( session);

     }


     void w_cassandra_session_get_metrics(const w_Cass_Session* session, w_CassMetrics* output)
     {
         if (!session || !output)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_session_get_metrics");
         }


        cass_session_get_metrics(session, output);
     }

     void  w_cassandra_session_get_speculative_execution_metrics(const w_Cass_Session* session,  w_Cass_Speculative_Execution_Metrics* output)
     {
         if (!session || !output)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_session_get_speculative_execution_metrics");
         }
            cass_session_get_speculative_execution_metrics( session, output);

     }

   /*  w_CassUuid w_cassandra_session_get_client_id(w_Cass_Session* session)
     {
         if (!session )
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_session_get_client_id");
            return ;
         }

           return cass_session_get_client_id( session);
     }*/

     void   w_cassandra_schema_meta_free(const w_Cass_Schema_Meta* schema_meta)
     {
         if (!schema_meta)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_schema_meta_free");
         }

           cass_schema_meta_free(schema_meta);

     }

     uint32_t w_cassandra_schema_meta_snapshot_version(const w_Cass_Schema_Meta* schema_meta)
     {
         if (!schema_meta)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_schema_meta_snapshot_version");
             return 0;
         }

           return cass_schema_meta_snapshot_version( schema_meta);
     }

   /*  w_CassVersion w_cassandra_schema_meta_version(const w_Cass_Schema_Meta* schema_meta)
     {
         if (!schema_meta)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_schema_meta_version");
             return ;
         }
           return cass_schema_meta_version(schema_meta);
     }*/


     const w_Cass_Key_space_Meta* w_cassandra_schema_meta_keyspace_by_name(const w_Cass_Schema_Meta* schema_meta,  const char* keyspace)
     {
         if (!schema_meta || !keyspace)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_schema_meta_keyspace_by_name");
             return NULL;
         }

          return    cass_schema_meta_keyspace_by_name( schema_meta, keyspace);
     }

     const w_Cass_Key_space_Meta* w_cassandra_schema_meta_keyspace_by_name_n(const w_Cass_Schema_Meta* schema_meta,const char* keyspace, size_t keyspace_length)
     {
         if (!schema_meta || !keyspace)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_schema_meta_keyspace_by_name_n");
             return NULL;
         }
          return cass_schema_meta_keyspace_by_name_n( schema_meta, keyspace, keyspace_length);
     }


     void w_cassandra_keyspace_meta_name(const w_Cass_Key_space_Meta* keyspace_meta, const char** name, size_t* name_length)
     {
         if (!keyspace_meta || !name)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_keyspace_meta_name");
         }

          cass_keyspace_meta_name( keyspace_meta, name, name_length);
     }

     w_bool_t w_cassandra_keyspace_meta_is_virtual(const w_Cass_Key_space_Meta* keyspace_meta)
     {
         if (!keyspace_meta )
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_keyspace_meta_is_virtual");
             return false ;
         }
          return cass_keyspace_meta_is_virtual( keyspace_meta);
     }

     const w_Cass_Table_Meta*  w_cassandra_keyspace_meta_table_by_name(const w_Cass_Key_space_Meta* keyspace_meta, const char* table)
     {

         if (!keyspace_meta || !table)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_keyspace_meta_table_by_name");
             return NULL;
         }

        return cass_keyspace_meta_table_by_name( keyspace_meta,table);
     }

     const w_Cass_Table_Meta*  w_cassandra_keyspace_meta_table_by_name_n(const w_Cass_Key_space_Meta* keyspace_meta, const char* table, size_t table_length)
     {
         if (!keyspace_meta || !table)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_keyspace_meta_table_by_name_n");
             return NULL;
         }
               return cass_keyspace_meta_table_by_name_n( keyspace_meta, table, table_length);
     }

     const w_Cass_Materialized_View_Meta* w_cassandra_keyspace_meta_materialized_view_by_name(const w_Cass_Key_space_Meta* keyspace_meta,const char* view)
     {
         if (!keyspace_meta || !view)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_keyspace_meta_materialized_view_by_name");
             return NULL;
         }


           return cass_keyspace_meta_materialized_view_by_name( keyspace_meta,view);
     }

     const w_Cass_Materialized_View_Meta*  w_cassandra_keyspace_meta_materialized_view_by_name_n(const w_Cass_Key_space_Meta* keyspace_meta,const char* view,  size_t view_length)
     {
         if (!keyspace_meta || !view)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_keyspace_meta_materialized_view_by_name_n");
             return NULL;
         }
           
             return   cass_keyspace_meta_materialized_view_by_name_n( keyspace_meta, view, view_length);
     }

     const w_Cass_Data_Type*  w_cassandra_keyspace_meta_user_type_by_name(const w_Cass_Key_space_Meta* keyspace_meta,   const char* type)
     {
         if (!keyspace_meta )
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_keyspace_meta_user_type_by_name");
             return NULL;
         }

           return cass_keyspace_meta_user_type_by_name(keyspace_meta, type);
     }

     const w_Cass_Data_Type* w_cassandra_keyspace_meta_user_type_by_name_n(const w_Cass_Key_space_Meta* keyspace_meta,const char* type,size_t type_length)
     {
         if (!keyspace_meta || !type)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_keyspace_meta_user_type_by_name_n");
             return NULL;
         }


         return cass_keyspace_meta_user_type_by_name_n( keyspace_meta, type, type_length);
     }

     const w_Cass_Function_Meta*  w_cassandra_keyspace_meta_function_by_name(const w_Cass_Key_space_Meta* keyspace_meta, const char* name, const char* arguments)
     {
         if (!keyspace_meta || !name)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_keyspace_meta_function_by_name");
             return NULL;
         }
             return   cass_keyspace_meta_function_by_name( keyspace_meta, name, arguments);
     }

     const w_Cass_Function_Meta*  w_cassandra_keyspace_meta_function_by_name_n(const w_Cass_Key_space_Meta* keyspace_meta, const char* name, size_t name_length, const char* arguments, size_t arguments_length)
     {
         if (!keyspace_meta || !name)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_keyspace_meta_function_by_name_n");
             return NULL;
         }

          return    cass_keyspace_meta_function_by_name_n( keyspace_meta, name, name_length,arguments,arguments_length);
     }

     const w_Cass_Aggregate_Meta*  w_cassandra_keyspace_meta_aggregate_by_name(const w_Cass_Key_space_Meta* keyspace_meta,const char* name,const char* arguments)
     {
         if (!keyspace_meta || !name)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_keyspace_meta_aggregate_by_name");
             return NULL;
         }

           return   cass_keyspace_meta_aggregate_by_name(keyspace_meta, name, arguments);

     }

     const w_Cass_Aggregate_Meta* w_cassandra_keyspace_meta_aggregate_by_name_n(const w_Cass_Key_space_Meta* keyspace_meta,const char* name, size_t name_length, const char* arguments,size_t arguments_length)
     {

         if (!keyspace_meta || !name)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_keyspace_meta_aggregate_by_name_n");
             return NULL;
         }

           return cass_keyspace_meta_aggregate_by_name_n(keyspace_meta, name, name_length, arguments, arguments_length);

     }

     const w_Cass_Value* w_cassandra_keyspace_meta_field_by_name(const w_Cass_Key_space_Meta* keyspace_meta,  const char* name)
     {

         if (!keyspace_meta || !name)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_keyspace_meta_field_by_name");
             return NULL;
         }
           return  cass_keyspace_meta_field_by_name(keyspace_meta, name);

     }

     const w_Cass_Value*  w_cassandra_keyspace_meta_field_by_name_n(const w_Cass_Key_space_Meta* keyspace_meta, const char* name, size_t name_length)
     {

         if (!keyspace_meta || !name)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_keyspace_meta_field_by_name_n");
             return NULL;
         }
      return cass_keyspace_meta_field_by_name_n( keyspace_meta, name,name_length);
     }

     void  w_cassandra_table_meta_name(const w_Cass_Table_Meta* table_meta, const char** name, size_t* name_length)
     {
         if (!table_meta || !name)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_table_meta_name");
         }
           cass_table_meta_name(table_meta, name, name_length);

     }

     w_bool_t w_cassandra_table_meta_is_virtual(const w_Cass_Table_Meta* table_meta)
     {
         if (!table_meta )
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_table_meta_is_virtual");
             return false;
         }

           return cass_table_meta_is_virtual(table_meta);

     }

     const w_Cass_Column_Meta* w_cassandra_table_meta_column_by_name(const w_Cass_Table_Meta* table_meta, const char* column)
     {
         if (!table_meta)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_table_meta_column_by_name");
             return NULL;
         }
           return cass_table_meta_column_by_name(table_meta, column);

     }

     const w_Cass_Column_Meta* w_cassandra_table_meta_column_by_name_n(const w_Cass_Table_Meta* table_meta,const char* column,size_t column_length)
     {
         if (!table_meta)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_table_meta_column_by_name_n");
             return NULL;
         }
           return  cass_table_meta_column_by_name_n(table_meta, column, column_length);

     }


     size_t   w_cassandra_table_meta_column_count(const w_Cass_Table_Meta* table_meta)
     {
         if (!table_meta)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_table_meta_column_count");
             return 0;
         }

           return   cass_table_meta_column_count(table_meta);

     }

     const w_Cass_Column_Meta* w_cassandra_table_meta_column(const w_Cass_Table_Meta* table_meta, size_t index)
     {

         if (!table_meta)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_table_meta_column");
             return NULL;
         }
           return cass_table_meta_column( table_meta, index);

     }

     const w_Cass_Index_Meta* w_cassandra_table_meta_index_by_name(const w_Cass_Table_Meta* table_meta, const char* index)
     {
         if (!table_meta)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_table_meta_index_by_name");
             return NULL;
         }
           return cass_table_meta_index_by_name(table_meta,index);
     }

     const w_Cass_Index_Meta* w_cassandra_table_meta_index_by_name_n(const w_Cass_Table_Meta* table_meta,const char* index, size_t index_length)
     {
         if (!table_meta)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_table_meta_index_by_name_n");
             return NULL;
         }

           return  cass_table_meta_index_by_name_n(table_meta, index, index_length);

     }

     size_t w_cassandra_table_meta_index_count(const w_Cass_Table_Meta* table_meta)
     {
         if (!table_meta)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_table_meta_index_count");
             return 0;
         }

           return  cass_table_meta_index_count(table_meta);
     }

     const w_Cass_Index_Meta*  w_cassandra_table_meta_index(const w_Cass_Table_Meta* table_meta, size_t index)
     {
         if (!table_meta)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_table_meta_index");
             return NULL;
         }

           return cass_table_meta_index(table_meta, index);

     }

     const w_Cass_Materialized_View_Meta* w_cassandra_table_meta_materialized_view_by_name(const w_Cass_Table_Meta* table_meta,const char* view)
     {
         if (!table_meta)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_table_meta_materialized_view_by_name");
             return NULL;
         }

           return cass_table_meta_materialized_view_by_name(table_meta, view);

     }

     const w_Cass_Materialized_View_Meta* w_cassandra_table_meta_materialized_view_by_name_n(const w_Cass_Table_Meta* table_meta, const char* view,size_t view_length)
     {
         if (!table_meta)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_table_meta_materialized_view_by_name_n");
             return NULL;
         }

           return  cass_table_meta_materialized_view_by_name_n(table_meta, view, view_length);

     }

      size_t w_cassandra_table_meta_materialized_view_count(const w_Cass_Table_Meta* table_meta)
      {

           if (!table_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_table_meta_materialized_view_count");
               return 0;
           }
          return cass_table_meta_materialized_view_count(table_meta);

      }

      const w_Cass_Materialized_View_Meta* w_cassandra_table_meta_materialized_view(const w_Cass_Table_Meta* table_meta, size_t index)
      {
          if (!table_meta)
          {
              W_ASSERT(false, "missing parameters!. trace info: w_cassandra_table_meta_materialized_view");
              return 0;
          }
           return cass_table_meta_materialized_view(table_meta, index);

      }

      size_t  w_cassandra_table_meta_partition_key_count(const w_Cass_Table_Meta* table_meta)
      {
          if (!table_meta)
          {
              W_ASSERT(false, "missing parameters!. trace info: w_cassandra_table_meta_partition_key_count");
              return 0;
          }
           return cass_table_meta_partition_key_count(table_meta);
      }

      const w_Cass_Column_Meta* w_cassandra_table_meta_partition_key(const w_Cass_Table_Meta* table_meta, size_t index)
      {
          if (!table_meta)
          {
              W_ASSERT(false, "missing parameters!. trace info: w_cassandra_table_meta_partition_key");
              return NULL;
          }
           return cass_table_meta_partition_key(table_meta, index);

      }

      size_t w_cassandra_table_meta_clustering_key_count(const w_Cass_Table_Meta* table_meta)
      {
          if (!table_meta)
          {
              W_ASSERT(false, "missing parameters!. trace info: w_cassandra_table_meta_clustering_key_count");
              return 0;
          }
           return cass_table_meta_clustering_key_count(table_meta);
      }

      const w_Cass_Column_Meta*  w_cassandra_table_meta_clustering_key(const w_Cass_Table_Meta* table_meta, size_t index)
      {
          if (!table_meta)
          {
              W_ASSERT(false, "missing parameters!. trace info: w_cassandra_table_meta_clustering_key");
              return NULL;
          }
           return  cass_table_meta_clustering_key(table_meta, index);

      }

       w_Cass_Clustering_Order w_cassandra_table_meta_clustering_key_order(const w_Cass_Table_Meta* table_meta, size_t index)
       {
           if (!table_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_table_meta_clustering_key_order");
               return -1;
           }
           return    cass_table_meta_clustering_key_order(table_meta, index);

       }

       const w_Cass_Value* w_cassandra_table_meta_field_by_name(const w_Cass_Table_Meta* table_meta, const char* name)
       {
           if (!table_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_table_meta_field_by_name");
               return NULL;
           }
          return  cass_table_meta_field_by_name( table_meta,name);
       }

       const w_Cass_Value* w_cassandra_table_meta_field_by_name_n(const w_Cass_Table_Meta* table_meta,const char* name, size_t name_length)
       {
           if (!table_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_table_meta_field_by_name_n");
               return NULL;
           }
           return   cass_table_meta_field_by_name_n(table_meta, name, name_length);

       }

       const w_Cass_Column_Meta* w_cassandra_materialized_view_meta_column_by_name(const w_Cass_Materialized_View_Meta* view_meta, const char* column)
       {

           if (!view_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_materialized_view_meta_column_by_name");
               return NULL;
           }
           return    cass_materialized_view_meta_column_by_name(view_meta, column);

       }

       const w_Cass_Column_Meta*  w_cassandra_materialized_view_meta_column_by_name_n(const w_Cass_Materialized_View_Meta* view_meta,const char* column,size_t column_length)
       {

           if (!view_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_materialized_view_meta_column_by_name_n");
               return NULL;
           }
           return       cass_materialized_view_meta_column_by_name_n(view_meta, column, column_length);

       }

       void w_cassandra_materialized_view_meta_name(const w_Cass_Materialized_View_Meta* view_meta,const char** name,size_t* name_length)
       {

           if (!view_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_materialized_view_meta_name");
           }
           cass_materialized_view_meta_name(view_meta, name, name_length);

       }

       const w_Cass_Table_Meta* w_cassandra_materialized_view_meta_base_table(const w_Cass_Materialized_View_Meta* view_meta)
       {

           if (!view_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_materialized_view_meta_base_table");
               return NULL;
           }
           return cass_materialized_view_meta_base_table(view_meta);

       }

       size_t  w_cassandra_materialized_view_meta_column_count(const w_Cass_Materialized_View_Meta* view_meta)
       {
           if (!view_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_materialized_view_meta_column_count");
               return 0;
           }
           return   cass_materialized_view_meta_column_count(view_meta);

       }

       const w_Cass_Column_Meta*  w_cassandra_materialized_view_meta_column(const w_Cass_Materialized_View_Meta* view_meta,size_t index)
       {
           if (!view_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_materialized_view_meta_column");
               return NULL;
           }
           return  cass_materialized_view_meta_column(view_meta, index);

       }

       size_t  w_cassandra_materialized_view_meta_partition_key_count(const w_Cass_Materialized_View_Meta*  view_meta)
       {
           if (!view_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_materialized_view_meta_partition_key_count");
               return 0;
           }

           return  cass_materialized_view_meta_partition_key_count(view_meta);

       }

       const w_Cass_Column_Meta* w_cassandra_materialized_view_meta_partition_key(const w_Cass_Materialized_View_Meta* view_meta, size_t index)
       {
           if (!view_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_materialized_view_meta_partition_key");
               return NULL;
           }
           return   cass_materialized_view_meta_partition_key(view_meta, index);

       }

       size_t  w_cassandra_materialized_view_meta_clustering_key_count(const w_Cass_Materialized_View_Meta* view_meta)
       {
           if (!view_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_materialized_view_meta_clustering_key_count");
               return 0;
           }
           return  cass_materialized_view_meta_clustering_key_count(view_meta);

       }

       const w_Cass_Column_Meta*  w_cassandra_materialized_view_meta_clustering_key(const w_Cass_Materialized_View_Meta* view_meta,size_t index)
       {
           if (!view_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_materialized_view_meta_clustering_key");
               return NULL;
           }
          return  cass_materialized_view_meta_clustering_key(view_meta, index);

       }

       w_Cass_Clustering_Order w_cassandra_materialized_view_meta_clustering_key_order(const w_Cass_Materialized_View_Meta* view_meta, size_t index)
       {

           if (!view_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_materialized_view_meta_clustering_key_order");
               return -1;
           }
           return  cass_materialized_view_meta_clustering_key_order(view_meta, index);

       }

       const w_Cass_Value* w_cassandra_materialized_view_meta_field_by_name(const w_Cass_Materialized_View_Meta* view_meta, const char* name)
       {
           if (!view_meta || !name )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_materialized_view_meta_field_by_name");
               return NULL;
           }

           
           return  cass_materialized_view_meta_field_by_name (view_meta, name);

       }

       const w_Cass_Value*  w_cassandra_materialized_view_meta_field_by_name_n(const w_Cass_Materialized_View_Meta* view_meta,const char* name,size_t name_length)
       {
           if (!view_meta || !name )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_materialized_view_meta_field_by_name_n");
               return NULL;
           }
           return    cass_materialized_view_meta_field_by_name_n(view_meta, name, name_length);

       }

       void w_cassandra_column_meta_name(const w_Cass_Column_Meta* column_meta, const char** name, size_t* name_length)
       {
           if (!column_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_column_meta_name");
               
           }
           cass_column_meta_name(column_meta, name, name_length);

       }

       w_Cass_Column_Type  w_cassandra_column_meta_type(const w_Cass_Column_Meta* column_meta)
       {
           if (!column_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_column_meta_type");
               return -1;

           }
           return  cass_column_meta_type(column_meta);

       }

       const w_Cass_Data_Type* w_cassandra_column_meta_data_type(const w_Cass_Column_Meta* column_meta)
       {
           if (!column_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_column_meta_data_type");
               return NULL;

           }
           return   cass_column_meta_data_type(column_meta);

       }

       const w_Cass_Value* w_cassandra_column_meta_field_by_name(const w_Cass_Column_Meta* column_meta, const char* name)
       {
           if (!column_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_column_meta_field_by_name");
               return NULL;

           }
           return   cass_column_meta_field_by_name(column_meta, name);

       }

       const w_Cass_Value* w_cassandra_column_meta_field_by_name_n(const w_Cass_Column_Meta* column_meta, const char* name, size_t name_length)
       {
           if (!column_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_column_meta_field_by_name_n");
               return NULL;

           }
           return     cass_column_meta_field_by_name_n(column_meta, name, name_length);

       }

       void w_cassandra_index_meta_name(const w_Cass_Index_Meta* index_meta, const char** name, size_t* name_length)
       {
           if (!index_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_index_meta_name");

           }
           cass_index_meta_name(index_meta, name, name_length);

       }

       w_Cass_Index_Type w_cassandra_index_meta_type(const w_Cass_Index_Meta* index_meta)
       {
           if (!index_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_index_meta_type");
               return -1;

           }
           return   cass_index_meta_type(index_meta);

       }

       void  w_cassandra_index_meta_target(const w_Cass_Index_Meta* index_meta,const char** target, size_t* target_length)
       {
           if (!index_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_index_meta_target");
              

           }
           cass_index_meta_target(index_meta, target, target_length);

       }

       const w_Cass_Value* w_cassandra_index_meta_options(const w_Cass_Index_Meta* index_meta)
       {
           if (!index_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_index_meta_options");
               return NULL;

           }
           return cass_index_meta_options(index_meta);

       }

       const w_Cass_Value* w_cassandra_index_meta_field_by_name(const w_Cass_Index_Meta* index_meta,const char* name)
       {

           if (!index_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_index_meta_field_by_name");
               return NULL;

           }
           return  cass_index_meta_field_by_name(index_meta, name);

       }

       const w_Cass_Value* w_cassandra_index_meta_field_by_name_n(const w_Cass_Index_Meta* index_meta, const char* name, size_t name_length)
       {

           if (!index_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_index_meta_field_by_name_n");
               return NULL;

           }
           return   cass_index_meta_field_by_name_n(index_meta, name, name_length);

       }

       void w_cassandra_function_meta_name(const w_Cass_Function_Meta* function_meta,const char** name, size_t* name_length)
       {


           if (!function_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_function_meta_name");

           }
           cass_function_meta_name(function_meta, name, name_length);

       }

       void w_cassandra_function_meta_full_name(const w_Cass_Function_Meta* function_meta, const char** full_name,size_t* full_name_length)
       {
           if (!function_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_function_meta_full_name");

           }
           cass_function_meta_full_name(function_meta, full_name, full_name_length);

       }

       void  w_cassandra_function_meta_body(const w_Cass_Function_Meta* function_meta,const char** body,size_t* body_length)
       {
           if (!function_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_function_meta_body");

           }
           cass_function_meta_body(function_meta, body, body_length);

       }

       void w_cassandra_function_meta_language(const w_Cass_Function_Meta* function_meta, const char** language,  size_t* language_length)
       {
           if (!function_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_function_meta_language");

           }
           cass_function_meta_language(function_meta, language, language_length);

       }

       w_bool_t  w_cassandra_function_meta_called_on_null_input(const w_Cass_Function_Meta* function_meta)
       {

           if (!function_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_function_meta_called_on_null_input");
               return false;
           }
           return cass_function_meta_called_on_null_input(function_meta);

       }

       size_t w_cassandra_function_meta_argument_count(const w_Cass_Function_Meta* function_meta)
       {
           if (!function_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_function_meta_argument_count");
               return 0;
           }
           return   cass_function_meta_argument_count(function_meta);

       }

       w_Cass_Error  w_cassandra_function_meta_argument(const w_Cass_Function_Meta* function_meta, size_t index,const char** name, size_t* name_length, const w_Cass_Data_Type** type)
       {
           if (!function_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_function_meta_argument");
               return -1;
           }
           return cass_function_meta_argument(function_meta, index, name, name_length, type);

       }

       const w_Cass_Data_Type*  w_cassandra_function_meta_argument_type_by_name(const w_Cass_Function_Meta* function_meta, const char* name)
       {
           if (!function_meta || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_function_meta_argument_type_by_name");
               return NULL;
           }
           return   cass_function_meta_argument_type_by_name(function_meta, name);

       }

       const w_Cass_Data_Type*   w_cassandra_function_meta_argument_type_by_name_n(const w_Cass_Function_Meta* function_meta, const char* name, size_t name_length)
       {
           if (!function_meta || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_function_meta_argument_type_by_name_n");
               return NULL;
           }
           return   cass_function_meta_argument_type_by_name_n(function_meta, name, name_length);

       }

       const w_Cass_Data_Type*  w_cassandra_function_meta_return_type(const w_Cass_Function_Meta* function_meta)
       {
           if (!function_meta )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_function_meta_return_type");
               return NULL;
           }
           return cass_function_meta_return_type( function_meta);

       }

       const w_Cass_Value* w_cassandra_function_meta_field_by_name(const w_Cass_Function_Meta* function_meta, const char* name) 
       {
           if (!function_meta || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_function_meta_field_by_name");
               return NULL;
           }
           return cass_function_meta_field_by_name(function_meta, name);

       }

       const w_Cass_Value*  w_cassandra_function_meta_field_by_name_n(const w_Cass_Function_Meta* function_meta, const char* name, size_t name_length)
       {
           if (!function_meta || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_function_meta_field_by_name_n");
               return NULL;
           }
           return cass_function_meta_field_by_name_n(function_meta, name, name_length);

       }

       void w_cassandra_aggregate_meta_name(const w_Cass_Aggregate_Meta* aggregate_meta,const char** name,size_t* name_length)
       {
           if (!aggregate_meta || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_aggregate_meta_name");
              
           }
           cass_aggregate_meta_name(aggregate_meta, name, name_length);

       }

       void w_cassandra_aggregate_meta_full_name(const w_Cass_Aggregate_Meta* aggregate_meta, const char** full_name, size_t* full_name_length)
       {

           if (!aggregate_meta )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_aggregate_meta_full_name");

           }
           cass_aggregate_meta_full_name(aggregate_meta, full_name, full_name_length);

       }

       size_t w_cassandra_aggregate_meta_argument_count(const w_Cass_Aggregate_Meta* aggregate_meta)
       {
           if (!aggregate_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_aggregate_meta_argument_count");
               return 0;
           }
           return cass_aggregate_meta_argument_count(aggregate_meta);

       }

       const w_Cass_Data_Type* w_cassandra_aggregate_meta_argument_type(const w_Cass_Aggregate_Meta* aggregate_meta, size_t index)
       {
           if (!aggregate_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_aggregate_meta_argument_type");
               return NULL;
           }
           return cass_aggregate_meta_argument_type(aggregate_meta, index);

       }
       const w_Cass_Data_Type* w_cassandra_aggregate_meta_return_type(const w_Cass_Aggregate_Meta* aggregate_meta)
       {
           if (!aggregate_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_aggregate_meta_return_type");
               return NULL;
           }
           return  cass_aggregate_meta_return_type(aggregate_meta);

       }

       const w_Cass_Data_Type* w_cassandra_aggregate_meta_state_type(const w_Cass_Aggregate_Meta* aggregate_meta)
       {
           if (!aggregate_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_aggregate_meta_state_type");
               return NULL;
           }
         return   cass_aggregate_meta_state_type(aggregate_meta);

       }

       const w_Cass_Function_Meta* w_cassandra_aggregate_meta_state_func(const w_Cass_Aggregate_Meta* aggregate_meta)
       {

           if (!aggregate_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_aggregate_meta_state_func");
               return NULL;
           }
           return cass_aggregate_meta_state_func(aggregate_meta);

       }

       const w_Cass_Function_Meta* w_cassandra_aggregate_meta_final_func(const w_Cass_Aggregate_Meta* aggregate_meta)
       {
           if (!aggregate_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_aggregate_meta_final_func");
               return NULL;
           }
           return cass_aggregate_meta_final_func(aggregate_meta);

       }

       const w_Cass_Value* w_cassandra_aggregate_meta_init_cond(const w_Cass_Aggregate_Meta* aggregate_meta)
       {
           if (!aggregate_meta)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_aggregate_meta_init_cond");
               return NULL;
           }
           return  cass_aggregate_meta_init_cond(aggregate_meta);

       }


       const w_Cass_Value* w_cassandra_aggregate_meta_field_by_name(const w_Cass_Aggregate_Meta* aggregate_meta,const char* name)
       {
           if (!aggregate_meta || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_aggregate_meta_field_by_name");
               return NULL;
           }
           return  cass_aggregate_meta_field_by_name(aggregate_meta, name);

       }

       const w_Cass_Value*  w_cassandra_aggregate_meta_field_by_name_n(const w_Cass_Aggregate_Meta* aggregate_meta,const char* name,size_t name_length)
       {
           if (!aggregate_meta || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_aggregate_meta_field_by_name_n");
               return NULL;
           }
           return   cass_aggregate_meta_field_by_name_n(aggregate_meta, name, name_length);

       }

       w_Cass_Ssl* w_cassandra_ssl_new()
       {
          return  cass_ssl_new();
       }

       w_Cass_Ssl* w_cassandra_ssl_new_no_lib_init()
       {
           return cass_ssl_new_no_lib_init();
       }

       void w_cassandra_ssl_free(w_Cass_Ssl* ssl)
       {
           if (!ssl)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_ssl_free");
           }
           cass_ssl_free(ssl);

       }

       w_Cass_Error w_cassandra_ssl_add_trusted_cert(w_Cass_Ssl* ssl,const char* cert)
       {
           if (!ssl || !cert)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_ssl_add_trusted_cert");
               return -1;
           }
           return  cass_ssl_add_trusted_cert(ssl, cert);

       }

       w_Cass_Error  w_cassandra_ssl_add_trusted_cert_n(w_Cass_Ssl* ssl, const char* cert, size_t cert_length)
       {
           if (!ssl || !cert)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_ssl_add_trusted_cert_n");
               return -1;
           }
           return cass_ssl_add_trusted_cert_n(ssl, cert, cert_length);

       }

       void  w_cassandra_ssl_set_verify_flags(w_Cass_Ssl* ssl,int flags)
       {
           if (!ssl )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_ssl_set_verify_flags");
               
           }
           cass_ssl_set_verify_flags(ssl, flags);

       }

       w_Cass_Error  w_cassandra_ssl_set_cert(w_Cass_Ssl* ssl, const char* cert)
       {
           if (!ssl || !cert)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_ssl_set_cert");
               return -1;
           }
           return cass_ssl_set_cert(ssl, cert);

       }

       w_Cass_Error w_cassandra_ssl_set_cert_n(w_Cass_Ssl* ssl, const char* cert, size_t cert_length)
       {
           if (!ssl || !cert)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_ssl_set_cert_n");
               return -1;
           }
           return cass_ssl_set_cert_n(ssl, cert, cert_length);

       }

       w_Cass_Error w_cassandra_ssl_set_private_key(w_Cass_Ssl* ssl, const char* key, const char* password)
       {

           if (!ssl || !key)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_ssl_set_private_key");
               return -1;
           }
           return  cass_ssl_set_private_key(ssl, key, password);

       }

       w_Cass_Error w_cassandra_ssl_set_private_key_n(w_Cass_Ssl* ssl, const char* key, size_t key_length,  const char* password, size_t password_length)
       {
           if (!ssl || !key)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_ssl_set_private_key_n");
               return -1;
           }
           
           return cass_ssl_set_private_key_n(ssl, key, key_length, password, password_length);
       }

       void w_cassandra_authenticator_address(const W_Cass_Authenticator* auth, w_CassInet* address)
       {
           if (!auth)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_authenticator_address");
           }
           cass_authenticator_address(auth, address);

       }


       const char*  w_cassandra_authenticator_hostname(const W_Cass_Authenticator* auth,size_t* length)
       {

           if (!auth)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_authenticator_hostname");
               return NULL;
           }
           return  cass_authenticator_hostname(auth, length);

       }


       const char* w_cassandra_authenticator_class_name(const W_Cass_Authenticator* auth, size_t* length)
       {

           if (!auth)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_authenticator_class_name");
               return NULL;
           }
           return cass_authenticator_class_name(auth, length);

       }


       void* w_cassandra_authenticator_exchange_data(W_Cass_Authenticator* auth)
       {
           if (!auth)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_authenticator_exchange_data");
               return NULL;
           }
           return cass_authenticator_exchange_data(auth);

       }

       void w_cassandra_authenticator_set_exchange_data(W_Cass_Authenticator* auth, void* exchange_data)
       {
           if (!auth)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_authenticator_set_exchange_data");
               
           }
           cass_authenticator_set_exchange_data(auth, exchange_data);

       }

       char* w_cassandra_authenticator_response(W_Cass_Authenticator* auth, size_t size)
       {
           if (!auth)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_authenticator_response");
               return NULL;
           }
           return  cass_authenticator_response(auth, size);

       }

       void  w_cassandra_authenticator_set_response(W_Cass_Authenticator* auth, const char* response, size_t response_size)
       {
           if (!auth)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_authenticator_set_response");

           }
           cass_authenticator_set_response(auth, response, response_size);

       }


       void w_cassandra_authenticator_set_error(W_Cass_Authenticator* auth, const char* message)
       {

           if (!auth)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_authenticator_set_error");

           }
           cass_authenticator_set_error(auth, message);

       }

       void w_cassandra_authenticator_set_error_n(W_Cass_Authenticator* auth,const char* message, size_t message_length)
       {
           if (!auth)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_authenticator_set_error_n");

           }
           cass_authenticator_set_error_n(auth, message, message_length);

       }

       void  w_cassandra_future_free(w_Cass_Future* future)
       {
           if (!future)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_future_free");

           }
           cass_future_free(future);

       }

       w_Cass_Error  w_cassandra_future_set_callback(w_Cass_Future* future, CassFutureCallback callback,void* data)
       {
           if (!future)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_future_set_callback");
               return -1;
           }
           return cass_future_set_callback(future, callback, data);

       }

       w_bool_t  w_cassandra_future_ready(w_Cass_Future* future)
       {
           if (!future)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_future_ready");
               return false;
           }
           return cass_future_ready(future);

       }

       void  w_cassandra_future_wait(w_Cass_Future* future)
       {
           if (!future)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_future_wait");
           }
           cass_future_wait(future);

       }

       w_bool_t w_cassandra_future_wait_timed(w_Cass_Future* future, w_duration_t timeout_us)
       {
           if (!future)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_future_wait_timed");
               return false;
           }
           return cass_future_wait_timed(future, timeout_us);

       }

       const w_Cass_Result* w_cassandra_future_get_result(w_Cass_Future* future)
       {
           if (!future)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_future_get_result");
               return NULL;
           }
           return     cass_future_get_result(future);

       }

       const w_Cass_Error_Result* w_cassandra_future_get_error_result(w_Cass_Future* future)
       {
           if (!future)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_future_get_error_result");
               return NULL;
           }
           return cass_future_get_error_result(future);

       }

       const w_Cass_Prepared*  w_cassandra_future_get_prepared(w_Cass_Future* future)
       {
           if (!future)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_future_get_prepared");
               return NULL;
           }
           return  cass_future_get_prepared(future);

       }

       w_Cass_Error w_cassandra_future_error_code(w_Cass_Future* future)
       {
           if (!future)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_future_error_code");
               return -1;
           }
           return cass_future_error_code(future);

       }

       void w_cassandra_future_error_message(w_Cass_Future* future,const char** message, size_t* message_length)
       {
           if (!future)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_future_error_message");
           }
           cass_future_error_message(future, message, message_length);

       }

       w_Cass_Error w_cassandra_future_tracing_id(w_Cass_Future* future, w_CassUuid* tracing_id)
       {
           if (!future)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_future_tracing_id");
               return -1;
           }
          return cass_future_tracing_id(future, tracing_id);

       }

       size_t  w_cassandra_future_custom_payload_item_count(w_Cass_Future* future)
       {
           if (!future)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_future_custom_payload_item_count");
               return -1;
           }
           return  cass_future_custom_payload_item_count(future);

       }

       w_Cass_Error  w_cassandra_future_custom_payload_item(w_Cass_Future* future,size_t index, const char** name, size_t* name_length, const w_byte_t** value, size_t* value_size)
       {

           if (!future)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_future_custom_payload_item");
               return -1;
           }
           return    cass_future_custom_payload_item(future, index, name, name_length, value, value_size);
       }

       w_Cass_Statement*  w_cassandra_statement_new(const char* query, size_t parameter_count)
       {
           if (!query)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_new");
               return NULL;
           }
           return   cass_statement_new(query, parameter_count);

       }

       w_Cass_Statement* w_cassandra_statement_new_n(const char* query, size_t query_length, size_t parameter_count)
       {
           if (!query)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_new_n");
               return NULL;
           }
           return  cass_statement_new_n(query, query_length, parameter_count);

       }

       w_Cass_Error w_cassandra_statement_reset_parameters(w_Cass_Statement* statement, size_t count)
       {
           if (!statement)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_reset_parameters");
               return -1;
           }
           return  cass_statement_reset_parameters(statement, count);

       }


       void w_cassandra_statement_free(w_Cass_Statement* statement)
       {
           if (!statement)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_free");
              
           }
           cass_statement_free(statement);

       }

       w_Cass_Error  w_cassandra_statement_add_key_index(w_Cass_Statement* statement,  size_t index)

       {
           if (!statement)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_add_key_index");
               return -1;


           }
           return cass_statement_add_key_index(statement, index);

       }

       w_Cass_Error w_cassandra_statement_set_keyspace(w_Cass_Statement* statement,const char* keyspace)
       {
           if (!statement || !keyspace)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_set_keyspace");
               return 0;
           }
           return cass_statement_set_keyspace(statement, keyspace);

       }

       w_Cass_Error   w_cassandra_statement_set_keyspace_n(w_Cass_Statement* statement, const char* keyspace,size_t keyspace_length)
       {
           if (!statement || !keyspace)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_set_keyspace_n");
               return -1;
           }

           return cass_statement_set_keyspace_n(statement, keyspace, keyspace_length);


       }

       w_Cass_Error  w_cassandra_statement_set_consistency(w_Cass_Statement* statement, W_Cass_Consistency consistency)
       {
           if (!statement || !consistency)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_set_consistency");
               return -1;
           }
           return cass_statement_set_consistency(statement, consistency);

       }

       w_Cass_Error w_cassandra_statement_set_serial_consistency(w_Cass_Statement* statement, W_Cass_Consistency serial_consistency)
       {
           if (!statement || !serial_consistency)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_set_serial_consistency");
               return -1;
           }
           return cass_statement_set_serial_consistency(statement, serial_consistency);

       }

       w_Cass_Error w_cassandra_statement_set_paging_size(w_Cass_Statement* statement, int page_size)
       {
           if (!statement )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_set_paging_size");
               return -1;
           }
          return  cass_statement_set_paging_size(statement, page_size);

       }

       w_Cass_Error  w_cassandra_statement_set_paging_state(w_Cass_Statement* statement, const w_Cass_Result* result)
       {
           if (!statement || !result)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_set_paging_state");
               return -1;
           }
           return cass_statement_set_paging_state(statement, result);

       }

       w_Cass_Error  w_cassandra_statement_set_paging_state_token(w_Cass_Statement* statement, const char* paging_state, size_t paging_state_size)
       {

           if (!statement )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_set_paging_state_token");
               return -1;
           }
           return   cass_statement_set_paging_state_token(statement, paging_state, paging_state_size);

       }

       w_Cass_Error  w_cassandra_statement_set_timestamp(w_Cass_Statement* statement, int64_t timestamp)
       {
           if (!statement)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_set_timestamp");
               return -1;
           }
           return  cass_statement_set_timestamp(statement, timestamp);

       }
       w_Cass_Error w_cassandra_statement_set_request_timeout(w_Cass_Statement* statement, uint64_t timeout_ms)
       {
           if (!statement)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_set_request_timeout");
               return   -1;
           }
           return  cass_statement_set_request_timeout(statement, timeout_ms);

       }

       w_Cass_Error  w_cassandra_statement_set_is_idempotent(w_Cass_Statement* statement, w_bool_t is_idempotent)
       {
           if (!statement)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_set_is_idempotent");
               return -1;
           }
           return  cass_statement_set_is_idempotent(statement, is_idempotent);

       }

       w_Cass_Error w_cassandra_statement_set_retry_policy(w_Cass_Statement* statement,w_Cass_Retry_Policy* retry_policy)
       {
           if (!statement || !retry_policy)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_set_retry_policy");
               return -1;
           }
           return   cass_statement_set_retry_policy(statement, retry_policy);

       }

       w_Cass_Error w_cassandra_statement_set_custom_payload(w_Cass_Statement* statement, const w_Cass_Custom_Payload* payload)
       {
           if (!statement || !payload)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_set_custom_payload");
               return -1;
           }
           return cass_statement_set_custom_payload(statement, payload);

       }

       w_Cass_Error  w_cassandra_statement_set_execution_profile(w_Cass_Statement* statement, const char* name)
       {
           if (!statement )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_set_execution_profile");
               return -1;
           }
           return  cass_statement_set_execution_profile(statement, name);

       }

       w_Cass_Error  w_cassandra_statement_set_execution_profile_n(w_Cass_Statement* statement, const char* name, size_t name_length)
       {
           if (!statement)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_set_execution_profile_n");
               return -1;
           }
           return   cass_statement_set_execution_profile_n(statement, name, name_length);

       }

       w_Cass_Error  w_cassandra_statement_set_tracing(w_Cass_Statement* statement, w_bool_t enabled)
       {
           if (!statement)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_set_tracing");
               return -1;
           }
           return  cass_statement_set_tracing(statement, enabled);

       }

       w_Cass_Error w_cassandra_statement_set_host(w_Cass_Statement* statement,  const char* host, int port)
       {
           if (!statement ||!host)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_set_host");
               return -1;
           }

           return  cass_statement_set_host(statement, host, port);

       }

       w_Cass_Error  w_cassandra_statement_set_host_n(w_Cass_Statement* statement, const char* host, size_t host_length, int port)
       {
           if (!statement || !host)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_set_host_n");
               return -1;
           }

           return   cass_statement_set_host_n(statement, host, host_length, port);

       }

       w_Cass_Error  w_cassandra_statement_set_host_inet(w_Cass_Statement* statement, const w_CassInet* host,  int port)
       {
           if (!statement || !host)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_set_host_inet");
               return -1;
           }
           return cass_statement_set_host_inet( statement, host,  port);

       }

       w_Cass_Error  w_cassandra_statement_bind_null(w_Cass_Statement* statement, size_t index)
       {
           if (!statement )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_null");
               return -1;
           }
           return   cass_statement_bind_null(statement, index);

       }

       w_Cass_Error  w_cassandra_statement_bind_null_by_name(w_Cass_Statement* statement,const char* name)
       {
           if (!statement || !name )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_null_by_name");
               return -1;
           }
           return cass_statement_bind_null_by_name(statement, name);

       }
       w_Cass_Error w_cassandra_statement_bind_null_by_name_n(w_Cass_Statement* statement, const char* name, size_t name_length)
       {
           if (!statement || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_null_by_name_n");
               return -1;
           }
           return  cass_statement_bind_null_by_name_n(statement, name, name_length);

       }

       w_Cass_Error  w_cassandra_statement_bind_int8(w_Cass_Statement* statement, size_t index, int8_t value)
       {
           if (!statement )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_int8");
               return -1;
           }
           return  cass_statement_bind_int8(statement, index, value);

       }

       w_Cass_Error  w_cassandra_statement_bind_int8_by_name(w_Cass_Statement* statement, const char* name, int8_t value)
       {
           if (!statement ||!name )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_int8_by_name");
               return -1;
           }
           return    cass_statement_bind_int8_by_name(statement, name, value);

       }

       w_Cass_Error w_cassandra_statement_bind_int8_by_name_n(w_Cass_Statement* statement, const char* name, size_t name_length, int8_t value)
       {

           if (!statement || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_int8_by_name_n");
               return -1;
           }
           return    cass_statement_bind_int8_by_name_n(statement, name, name_length, value);

       }

       w_Cass_Error  w_cassandra_statement_bind_int16(w_Cass_Statement* statement, size_t index, int16_t value)
       {
           if (!statement )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_int16");
               return -1;
           }
           return    cass_statement_bind_int16(statement, index, value);

       }


       w_Cass_Error  w_cassandra_statement_bind_int16_by_name(w_Cass_Statement* statement, const char* name, int16_t value)
       {
           if (!statement)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_int16_by_name");
               return -1;
           }

           return  cass_statement_bind_int16_by_name(statement, name, value);

       }

       w_Cass_Error  w_cassandra_statement_bind_int16_by_name_n(w_Cass_Statement* statement, const char* name,  size_t name_length, int16_t value)
       {
           if (!statement)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_int16_by_name_n");
               return -1;
           }
           return   cass_statement_bind_int16_by_name_n(statement, name, name_length, value);

       }

       w_Cass_Error  w_cassandra_statement_bind_int32(w_Cass_Statement* statement, size_t index, int32_t value)
       {
           if (!statement)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_int32");
               return -1;
           }
           return  cass_statement_bind_int32(statement, index, value);

       }

       w_Cass_Error  w_cassandra_statement_bind_int32_by_name(w_Cass_Statement* statement, const char* name,  int32_t value)
       {
           if (!statement || !name )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_int32_by_name");
               return -1;
           }
           return cass_statement_bind_int32_by_name( statement, name, value);
       }


       w_Cass_Error  w_cassandra_statement_bind_int32_by_name_n(w_Cass_Statement* statement, const char* name, size_t name_length, int32_t value)
       {
           if (!statement || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_int32_by_name_n");
               return -1;
           }

           return cass_statement_bind_int32_by_name_n(statement, name, name_length, value);

       }

       w_Cass_Error w_cassandra_statement_bind_uint32(w_Cass_Statement* statement, size_t index, uint32_t value)
       {
           if (!statement )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_uint32");
               return -1;
           }
           return   cass_statement_bind_uint32(statement, index,  value);

       }

       w_Cass_Error w_cassandra_statement_bind_uint32_by_name(w_Cass_Statement* statement, const char* name, uint32_t value)
       {
           if (!statement || !name )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_uint32_by_name");
               return -1;
           }
           return  cass_statement_bind_uint32_by_name(statement, name, value);

       }

       w_Cass_Error  w_cassandra_statement_bind_uint32_by_name_n(w_Cass_Statement* statement,  const char* name, size_t name_length,  uint32_t value)
       {
           if (!statement || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_uint32_by_name_n");
               return -1;
           }
          return  cass_statement_bind_uint32_by_name_n( statement, name, name_length, value);
       }

       w_Cass_Error  w_cassandra_statement_bind_int64(w_Cass_Statement* statement,size_t index,int64_t value)
       {

           if (!statement )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_int64");
               return -1;
           }
           return cass_statement_bind_int64(statement, index, value);


       }

       w_Cass_Error   w_cassandra_statement_bind_int64_by_name(w_Cass_Statement* statement,const char* name,uint64_t value)
       {
           if (!statement || !name )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_int64_by_name");
               return -1;
           }
           return  cass_statement_bind_int64_by_name(statement, name, value);

       }

       w_Cass_Error w_cassandra_statement_bind_int64_by_name_n(w_Cass_Statement* statement, const char* name,size_t name_length, int64_t value)
       {
           if (!statement || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_int64_by_name_n");
               return -1;
           }
           return  cass_statement_bind_int64_by_name_n(statement, name, name_length, value);

       }

       w_Cass_Error w_cassandra_statement_bind_float(w_Cass_Statement* statement, size_t index, float value)
       {
           if (!statement )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_float");
               return -1;
           }
           return cass_statement_bind_float(statement, index, value);

       }

       w_Cass_Error  w_cassandra_statement_bind_float_by_name(w_Cass_Statement* statement, const char* name, float value)
       {
           if (!statement || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_float_by_name");
               return -1;
           }
           return cass_statement_bind_float_by_name(statement, name, value);

       }

       w_Cass_Error w_cassandra_statement_bind_float_by_name_n(w_Cass_Statement* statement,const char* name,size_t name_length, float value)
       {
           if (!statement || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_float_by_name_n");
               return -1;
           }
           return cass_statement_bind_float_by_name_n(statement, name, name_length, value);


       }

       w_Cass_Error  w_cassandra_statement_bind_double(w_Cass_Statement* statement, size_t index, double value)
       {
           if (!statement )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_double");
               return -1;
           }
           return    cass_statement_bind_double(statement, index, value);

       }

       w_Cass_Error  w_cassandra_statement_bind_double_by_name(w_Cass_Statement* statement,const char* name, double value)
       {
           if (!statement || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_double_by_name");
               return -1;
           }
           return    cass_statement_bind_double_by_name(statement, name, value);

       }

       w_Cass_Error  w_cassandra_statement_bind_double_by_name_n(w_Cass_Statement* statement,const char* name,size_t name_length,double value)
       {
           if (!statement || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_double_by_name_n");
               return -1;
           }
           return  cass_statement_bind_double_by_name_n(statement, name, name_length, value);

       }

       w_Cass_Error w_cassandra_statement_bind_bool(w_Cass_Statement* statement, size_t index, w_bool_t value)
       {
           if (!statement )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_bool");
               return -1;
           }

           return  cass_statement_bind_bool(statement, index, value);

       }

       w_Cass_Error w_cassandra_statement_bind_bool_by_name(w_Cass_Statement* statement, const char* name,w_bool_t value)
       {
           if (!statement || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_bool_by_name");
               return -1;
           }
         return cass_statement_bind_bool_by_name( statement,  name,  value);

       }

       w_Cass_Error  w_cassandra_statement_bind_bool_by_name_n(w_Cass_Statement* statement,const char* name,size_t name_length, w_bool_t value)
       {
           if (!statement || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_bool_by_name_n");
               return -1;
           }
           return cass_statement_bind_bool_by_name_n( statement,  name,  name_length,  value);

       }

       w_Cass_Error w_cassandra_statement_bind_string(w_Cass_Statement* statement, size_t index,  const char* value)
       {
           if (!statement )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_string");
               return -1;
           }
           return cass_statement_bind_string(statement, index, value);
       }

       w_Cass_Error  w_cassandra_statement_bind_string_n(w_Cass_Statement* statement,size_t index,  const char* value, size_t value_length)
       {
           if (!statement)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_string_n");
               return -1;
           }
           return cass_statement_bind_string_n(statement, index, value, value_length);

       }

       w_Cass_Error  w_cassandra_statement_bind_string_by_name(w_Cass_Statement* statement,const char* name, const char* value)
       {
           if (!statement || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_string_by_name");
               return -1;
           }
           return cass_statement_bind_string_by_name(statement, name, value);

       }

       w_Cass_Error w_cassandra_statement_bind_string_by_name_n(w_Cass_Statement* statement,const char* name, size_t name_length,const char* value, size_t value_length)
       {
           if (!statement || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_string_by_name_n");
               return -1;
           }
          return  cass_statement_bind_string_by_name_n(statement, name, name_length, value, value_length);

       }

       w_Cass_Error  w_cassandra_statement_bind_bytes(w_Cass_Statement* statement,size_t index, const w_byte_t* value,size_t value_size)
       {
           if (!statement)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_bytes");
               return -1;
           }

           return   cass_statement_bind_bytes(statement, index, value, value_size);

       }

       w_Cass_Error  w_cassandra_statement_bind_bytes_by_name(w_Cass_Statement* statement,const char* name,const w_byte_t* value, size_t value_size)
       {
           if (!statement || !name )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_bytes_by_name");
               return -1;
           }
           return  cass_statement_bind_bytes_by_name(statement, name, value, value_size);

       }

       w_Cass_Error w_cassandra_statement_bind_bytes_by_name_n(w_Cass_Statement* statement, const char* name,size_t name_length, const w_byte_t* value, size_t value_size)
       {
           if (!statement || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_bytes_by_name_n");
               return -1;
           }
           return cass_statement_bind_bytes_by_name_n(statement, name, name_length, value, value_size);

       }

       w_Cass_Error w_cassandra_statement_bind_custom(w_Cass_Statement* statement,size_t index,const char* class_name, const w_byte_t* value,size_t value_size)
       {
           if (!statement )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_custom");
               return -1;
           }
           return   cass_statement_bind_custom(statement, index, class_name, value, value_size);

       }

       w_Cass_Error  w_cassandra_statement_bind_custom_n(w_Cass_Statement* statement, size_t index, const char* class_name, size_t class_name_length,const w_byte_t* value,size_t value_size)
       {
           if (!statement)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_custom_n");
               return -1;
           }
           return   cass_statement_bind_custom_n( statement,  index,  class_name,  class_name_length,  value,  value_size);

       }


       w_Cass_Error w_cassandra_statement_bind_custom_by_name(w_Cass_Statement* statement, const char* name, const char* class_name, const w_byte_t* value,size_t value_size)
       {
           if (!statement || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_custom_by_name");
               return -1;
           }
           return   cass_statement_bind_custom_by_name(statement, name, class_name, value, value_size);

       }

       w_Cass_Error  w_cassandra_statement_bind_custom_by_name_n(w_Cass_Statement* statement, const char* name,size_t name_length, const char* class_name,size_t class_name_length, const w_byte_t* value, size_t value_size)
       {
           if (!statement || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_custom_by_name_n");
               return -1;
           }
           return cass_statement_bind_custom_by_name_n(statement, name, name_length, class_name, class_name_length, value, value_size);

       }

       w_Cass_Error  w_cassandra_statement_bind_uuid(w_Cass_Statement* statement, size_t index, w_CassUuid value)
       {
           if (!statement )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_uuid");
               return -1;
           }
           return cass_statement_bind_uuid(statement, index, value);

       }

       w_Cass_Error w_cassandra_statement_bind_uuid_by_name(w_Cass_Statement* statement,const char* name,w_CassUuid value)
       {
           if (!statement || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_uuid_by_name");
               return -1;
           }
           return  cass_statement_bind_uuid_by_name(statement, name, value);

       }

       w_Cass_Error w_cassandra_statement_bind_uuid_by_name_n(w_Cass_Statement* statement, const char* name,size_t name_length, w_CassUuid value)
       {
           if (!statement || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_uuid_by_name_n");
               return -1;
           }
           return   cass_statement_bind_uuid_by_name_n(statement, name, name_length, value);

       }

       w_Cass_Error  w_cassandra_statement_bind_inet(w_Cass_Statement* statement, size_t index, w_CassInet value)
       {

           if (!statement )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_inet");
               return -1;
           }
           return cass_statement_bind_inet( statement,  index,  value);

       }

       w_Cass_Error w_cassandra_statement_bind_inet_by_name(w_Cass_Statement* statement, const char* name, w_CassInet value)
       {
           if (!statement || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_inet_by_name");
               return -1;
           }
           return cass_statement_bind_inet_by_name(statement, name, value);

       }

       w_Cass_Error w_cassandra_statement_bind_inet_by_name_n(w_Cass_Statement* statement,const char* name,size_t name_length, w_CassInet value)
       {
           if (!statement || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_inet_by_name_n");
               return -1;
           }
           return cass_statement_bind_inet_by_name_n(statement, name, name_length, value);

       }

       w_Cass_Error w_cassandra_statement_bind_decimal(w_Cass_Statement* statement, size_t index, const w_byte_t* varint, size_t varint_size, int32_t scale)
       {
           if (!statement )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_decimal");
               return -1;
           }

           return cass_statement_bind_decimal(statement, index, varint, varint_size, scale);

       }

       w_Cass_Error  w_cassandra_statement_bind_decimal_by_name(w_Cass_Statement* statement, const char* name, const w_byte_t* varint, size_t varint_size, int32_t scale)
       {
           if (!statement || !name )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_decimal_by_name");
               return -1;
           }
           return cass_statement_bind_decimal_by_name(statement, name, varint, varint_size, scale);

       }

       w_Cass_Error  w_cassandra_statement_bind_decimal_by_name_n(w_Cass_Statement* statement, const char* name, size_t name_length,  const w_byte_t* varint, size_t varint_size,  int32_t scale)
       {
           if (!statement || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_decimal_by_name_n");
               return -1;
           }
           return  cass_statement_bind_decimal_by_name_n(statement, name, name_length, varint, varint_size, scale);

       }

       w_Cass_Error w_cassandra_statement_bind_duration(w_Cass_Statement* statement,size_t index,int32_t months,int32_t days,int64_t nanos)
       {
           if (!statement )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_duration");
               return -1;
           }

           return   cass_statement_bind_duration(statement, index, months, days, nanos);

       }

       w_Cass_Error  w_cassandra_statement_bind_duration_by_name(w_Cass_Statement* statement, const char* name,int32_t months, int32_t days,int64_t nanos)
       {
           if (!statement || !name )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_duration_by_name");
               return -1;
           }
           return cass_statement_bind_duration_by_name(statement, name, months, days, nanos);

       }

       w_Cass_Error  w_cassandra_statement_bind_duration_by_name_n(w_Cass_Statement* statement, const char* name,size_t name_length,int32_t months,int32_t days,int64_t nanos)
       {
           if (!statement || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_duration_by_name_n");
               return -1;
           }
           return cass_statement_bind_duration_by_name_n(  statement,  name,  name_length,  months,  days,  nanos);

       }

       w_Cass_Error  w_cassandra_statement_bind_collection(w_Cass_Statement* statement,size_t index, const w_Cass_Collection* collection)
       {
           if (!statement )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_collection");
               return -1;
           }
           return  cass_statement_bind_collection(statement, index, collection);

       }

       w_Cass_Error  w_cassandra_statement_bind_collection_by_name(w_Cass_Statement* statement,const char* name, const w_Cass_Collection* collection)
       {
           if (!statement || !name )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_collection_by_name");
               return -1;
           }

           return cass_statement_bind_collection_by_name(statement, name, collection);

       }
       w_Cass_Error w_cassandra_statement_bind_collection_by_name_n(w_Cass_Statement* statement,const char* name, size_t name_length, const w_Cass_Collection* collection)
       {
           if (!statement || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_collection_by_name_n");
               return -1;
           }
           return  cass_statement_bind_collection_by_name_n(statement, name, name_length, collection);

       }

       w_Cass_Error  w_cassandra_statement_bind_tuple(w_Cass_Statement* statement,size_t index, const w_Cass_Tuple* tuple)
       {
           if (!statement )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_tuple");
               return -1;
           }
           return cass_statement_bind_tuple(statement, index, tuple);

       }

       w_Cass_Error  w_cassandra_statement_bind_tuple_by_name(w_Cass_Statement* statement,const char* name,const w_Cass_Tuple* tuple)
       {
           if (!statement || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_tuple_by_name");
               return -1;
           }
           return  cass_statement_bind_tuple_by_name(statement, name, tuple);

       }

       w_Cass_Error w_cassandra_statement_bind_tuple_by_name_n(w_Cass_Statement* statement,  const char* name,size_t name_length, const w_Cass_Tuple* tuple)
       {
           if (!statement || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_tuple_by_name_n");
               return -1;
           }
           return cass_statement_bind_tuple_by_name_n(  statement, name,  name_length,  tuple);
       }

       w_Cass_Error w_cassandra_statement_bind_user_type(w_Cass_Statement* statement,size_t index, const w_Cass_User_Type* user_type)
       {
           if (!statement )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_user_type");
               return -1;
           }
           return cass_statement_bind_user_type(statement, index, user_type);
       }

       w_Cass_Error w_cassandra_statement_bind_user_type_by_name(w_Cass_Statement* statement,  const char* name,const w_Cass_User_Type* user_type)
       {
           if (!statement || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_statement_bind_user_type_by_name");
               return -1;
           }
           return cass_statement_bind_user_type_by_name(statement, name, user_type);
       }

       void w_cassandra_prepared_free(const w_Cass_Prepared* prepared)
       {
           if (!prepared)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_prepared_free");
           }
           cass_prepared_free(prepared);

       }

       w_Cass_Statement* w_cassandra_prepared_bind(const w_Cass_Prepared* prepared)
       {
           if (!prepared)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandraandra_prepared_bind");
               return NULL;
           }
           return   cass_prepared_bind(prepared);

       }

       w_Cass_Error w_cassandra_prepared_parameter_name(const w_Cass_Prepared* prepared, size_t index,const char** name,size_t* name_length)
       {
           if (!prepared || !name )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandraandra_prepared_parameter_name");
               return -1;
           }
           return cass_prepared_parameter_name(prepared, index, name, name_length);
       }

       const w_Cass_Data_Type* w_cassandra_prepared_parameter_data_type(const w_Cass_Prepared* prepared, size_t index)
       {
           if (!prepared )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_prepared_parameter_data_type");
               return NULL;
           }
           return cass_prepared_parameter_data_type(   prepared, index);
       }

       const w_Cass_Data_Type* w_cassandra_prepared_parameter_data_type_by_name(const w_Cass_Prepared* prepared, const char* name)
       {
           if (!prepared || !name )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_prepared_parameter_data_type_by_name");
               return NULL;
           }
               return cass_prepared_parameter_data_type_by_name( prepared, name);
       }

       const w_Cass_Data_Type* w_cassandra_prepared_parameter_data_type_by_name_n(const w_Cass_Prepared* prepared, const char* name,size_t name_length)
       {
           if (!prepared || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_prepared_parameter_data_type_by_name_n");
               return NULL;
           }
               return cass_prepared_parameter_data_type_by_name_n( prepared, name, name_length);

       }

       w_Cass_Batch* w_cassandra_batch_new(w_Cass_Batch_Type type)
       {
           
               return cass_batch_new( type);
       }

       void w_cassandra_batch_free(w_Cass_Batch* batch)
       {
           if (!batch)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_batch_free");
               
           }
           cass_batch_free(  batch);

       }

       w_Cass_Error  w_cassandra_batch_set_keyspace(w_Cass_Batch* batch, const char* keyspace)
       {
           if (!batch || !keyspace)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_batch_set_keyspace");
               return -1;
           }
           return  cass_batch_set_keyspace( batch, keyspace);
       }

       w_Cass_Error w_cassandra_batch_set_keyspace_n(w_Cass_Batch* batch,const char* keyspace,size_t keyspace_length)
       {
           if (!batch || !keyspace)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_batch_set_keyspace_n");
               return -1;
           }
           return  cass_batch_set_keyspace_n(batch, keyspace, keyspace_length);

       }

       w_Cass_Error w_cassandra_batch_set_consistency(w_Cass_Batch* batch, W_Cass_Consistency consistency)
       {
           if (!batch )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_batch_set_consistency");
               return -1;
           }

           return cass_batch_set_consistency(batch, consistency);

       }

       w_Cass_Error w_cassandra_batch_set_serial_consistency(w_Cass_Batch* batch, W_Cass_Consistency serial_consistency)
       {
           if (!batch)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_batch_set_serial_consistency");
               return -1;
           }

           return 
               cass_batch_set_serial_consistency(  batch,serial_consistency);
       }

       w_Cass_Error w_cassandra_batch_set_timestamp(w_Cass_Batch* batch,int64_t timestamp)
       {
           if (!batch)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_batch_set_timestamp");
               return -1;
           }
           return  cass_batch_set_timestamp(  batch,  timestamp);

       }

       w_Cass_Error w_cassandra_batch_set_request_timeout(w_Cass_Batch* batch,uint64_t timeout_ms)
       {
           if (!batch)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_batch_set_request_timeout");
               return -1;
           }
           return  cass_batch_set_request_timeout(batch, timeout_ms);


       }

       w_Cass_Error  w_cassandra_batch_set_is_idempotent(w_Cass_Batch* batch, w_bool_t is_idempotent)
       {
           if (!batch)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_batch_set_is_idempotent");
               return -1;
           }
           return   cass_batch_set_is_idempotent(batch, is_idempotent);

       }

       w_Cass_Error w_cassandra_batch_set_retry_policy(w_Cass_Batch* batch,w_Cass_Retry_Policy* retry_policy)
       {
           if (!batch || !retry_policy)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_batch_set_retry_policy");
               return -1;
           }
           return cass_batch_set_retry_policy(batch, retry_policy);

       }

       w_Cass_Error  w_cassandra_batch_set_custom_payload(w_Cass_Batch* batch,const w_Cass_Custom_Payload* payload)
       {
           if (!batch || !payload)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_batch_set_custom_payload");
               return -1;
           }
           return  cass_batch_set_custom_payload(batch, payload);

       }

       w_Cass_Error w_cassandra_batch_set_tracing(w_Cass_Batch* batch, w_bool_t enabled)
       {
           if (!batch )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_batch_set_tracing");
               return -1;
           }
           return cass_batch_set_tracing(batch, enabled);

       }

       w_Cass_Error  w_cassandra_batch_add_statement(w_Cass_Batch* batch,w_Cass_Statement* statement)
       {
           if (!batch || !statement)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_batch_add_statement");
               return -1;
           } 
           return cass_batch_add_statement(batch, statement);

       }

       w_Cass_Error  w_cassandra_batch_set_execution_profile(w_Cass_Batch* batch,const char* name)
       {
           if (!batch || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_batch_set_execution_profile");
               return -1;
           }
           return  cass_batch_set_execution_profile(batch, name);

       }

       w_Cass_Error w_cassandra_batch_set_execution_profile_n(w_Cass_Batch* batch, const char* name,size_t name_length)
       {
           if (!batch || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_batch_set_execution_profile_n");
               return -1;
           }
           return cass_batch_set_execution_profile_n(batch, name, name_length);

       }

       w_Cass_Data_Type * w_cassandra_data_type_new(w_Cass_Value_Type type)

       {
           return cass_data_type_new( type);

       }

       w_Cass_Data_Type* w_cassandra_data_type_new_from_existing(const w_Cass_Data_Type* data_type)
       {
           if (!data_type)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_data_type_new_from_existing");
               return NULL;
           }
           return cass_data_type_new_from_existing(data_type);

       }

       w_Cass_Data_Type* w_cassandra_data_type_new_tuple(size_t item_count)
       {
           return cass_data_type_new_tuple(item_count);

       }

       w_Cass_Data_Type* w_cassandra_data_type_new_udt(size_t field_count)
       {
           return cass_data_type_new_udt(field_count);

       }

       void w_cassandra_data_type_free(w_Cass_Data_Type* data_type)
       {
           if (!data_type)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_data_type_free");
               
           }
            cass_data_type_free(  data_type);
       }

       w_Cass_Value_Type w_cassandra_data_type_type(const w_Cass_Data_Type* data_type)
       {
           if (!data_type)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_data_type_type");
               return -1;
           }
           return  cass_data_type_type( data_type);

       }

       w_bool_t w_cassandra_data_type_is_frozen(const w_Cass_Data_Type* data_type)
       {
           if (!data_type)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_data_type_is_frozen");
               return false;
           }
           return cass_data_type_is_frozen(data_type);

       }
       w_Cass_Error w_cassandra_data_type_type_name(const w_Cass_Data_Type* data_type,  const char** type_name, size_t* type_name_length)
       {

           if (!data_type || !type_name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_data_type_type_name");
               return -1;
           }
           return cass_data_type_type_name( data_type, type_name, type_name_length);
       }
      
       w_Cass_Error w_cassandra_data_type_set_type_name(w_Cass_Data_Type* data_type,const char* type_name)
       {
           if (!data_type || !type_name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_data_type_set_type_name");
               return -1;
           }
           return cass_data_type_set_type_name(data_type, type_name);

       }

       w_Cass_Error w_cassandra_data_type_set_type_name_n(w_Cass_Data_Type* data_type,const char* type_name,size_t type_name_length)
       {
           if (!data_type || !type_name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_data_type_set_type_name_n");
               return -1;
           }
           return  cass_data_type_set_type_name_n(data_type, type_name, type_name_length);

       }

       w_Cass_Error w_cassandra_data_type_keyspace(const w_Cass_Data_Type* data_type,const char** keyspace,size_t* keyspace_length)
       {
           if (!data_type || !keyspace)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_data_type_keyspace");
               return -1;
           }
           return cass_data_type_keyspace(data_type, keyspace, keyspace_length);

       }

       w_Cass_Error w_cassandra_data_type_set_keyspace(w_Cass_Data_Type* data_type,const char* keyspace)
       {
           if (!data_type || !keyspace)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_data_type_set_keyspace");
               return -1;
           }
           return  cass_data_type_set_keyspace(data_type, keyspace);

       }

       w_Cass_Error  w_cassandra_data_type_set_keyspace_n(w_Cass_Data_Type* data_type,const char* keyspace, size_t keyspace_length)
       {
           if (!data_type || !keyspace)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_data_type_set_keyspace_n");
               return -1;
           }
           return cass_data_type_set_keyspace_n(data_type, keyspace, keyspace_length);

       }

       w_Cass_Error w_cassandra_data_type_class_name(const w_Cass_Data_Type* data_type, const char** class_name,size_t* class_name_length)
       {
           if (!data_type || !class_name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_data_type_class_name");
               return -1;
           }
           return  cass_data_type_class_name(data_type, class_name, class_name_length);

       }

       w_Cass_Error w_cassandra_data_type_set_class_name(w_Cass_Data_Type* data_type,const char* class_name)
       {
           if (!data_type || !class_name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_data_type_set_class_name");
               return -1;
           }
           return cass_data_type_set_class_name(data_type, class_name);

       }

       w_Cass_Error  w_cassandra_data_type_set_class_name_n(w_Cass_Data_Type* data_type, const char* class_name,size_t class_name_length)
       {
           if (!data_type || !class_name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_data_type_set_class_name_n");
               return -1;
           }
           return  cass_data_type_set_class_name_n(data_type, class_name, class_name_length);

       }

       size_t w_cassandra_data_type_sub_type_count(const w_Cass_Data_Type* data_type)
       {
           if (!data_type )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_data_type_sub_type_count");
               return 0;
           }
           return  cass_data_type_sub_type_count(data_type);

       }

      /* size_t
           w_cassandra_data_sub_type_count(const w_Cass_Data_Type* data_type)
       {
           return  cass_data_sub_type_count( data_type);

       }*/

       const w_Cass_Data_Type*  w_cassandra_data_type_sub_data_type(const w_Cass_Data_Type* data_type,size_t index)
       {
           if (!data_type)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_data_type_sub_data_type");
               return NULL;
           }
           return  cass_data_type_sub_data_type(data_type, index);

       }

       const w_Cass_Data_Type* w_cassandra_data_type_sub_data_type_by_name(const w_Cass_Data_Type* data_type,const char* name)
       {
           if (!data_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_data_type_sub_data_type_by_name");
               return NULL;
           }
           return  cass_data_type_sub_data_type_by_name(data_type, name);

       }

       const w_Cass_Data_Type* w_cassandra_data_type_sub_data_type_by_name_n(const w_Cass_Data_Type* data_type,const char* name,size_t name_length)
       {
           if (!data_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_data_type_sub_data_type_by_name_n");
               return NULL;
           }
           return cass_data_type_sub_data_type_by_name_n(data_type, name, name_length);

       }

       w_Cass_Error w_cassandra_data_type_sub_type_name(const w_Cass_Data_Type* data_type,size_t index, const char** name, size_t* name_length)
       {
           if (!data_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_data_type_sub_type_name");
               return -1;
           }
           return cass_data_type_sub_type_name(data_type, index, name, name_length);

       }

       w_Cass_Error  w_cassandra_data_type_add_sub_type(w_Cass_Data_Type* data_type,const w_Cass_Data_Type* sub_data_type)
       {
           if (!data_type || !sub_data_type)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_data_type_add_sub_type");
               return -1;
           }
           return cass_data_type_add_sub_type(data_type, sub_data_type);

       }

       w_Cass_Error w_cassandra_data_type_add_sub_type_by_name(w_Cass_Data_Type* data_type,const char* name,const w_Cass_Data_Type* sub_data_type)
       {
           if (!data_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_data_type_add_sub_type_by_name");
               return -1;
           }

           return cass_data_type_add_sub_type_by_name(data_type, name, sub_data_type);

       }

       w_Cass_Error  w_cassandra_data_type_add_sub_type_by_name_n(w_Cass_Data_Type* data_type, const char* name, size_t name_length, const w_Cass_Data_Type* sub_data_type)
       {
           if (!data_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_data_type_add_sub_type_by_name_n");
               return -1;
           }
           return  cass_data_type_add_sub_type_by_name_n( data_type, name,  name_length, sub_data_type);
       }

       w_Cass_Error w_cassandra_data_type_add_sub_value_type(w_Cass_Data_Type* data_type,w_Cass_Value_Type sub_value_type)
       {
           if (!data_type )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_data_type_add_sub_value_type");
               return -1;
           }
           return cass_data_type_add_sub_value_type(data_type, sub_value_type);

       }

       w_Cass_Error   w_cassandra_data_type_add_sub_value_type_by_name(w_Cass_Data_Type* data_type, const char* name, w_Cass_Value_Type sub_value_type)
       {
           if (!data_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_data_type_add_sub_value_type_by_name");
               return -1;
           }
           return  cass_data_type_add_sub_value_type_by_name(data_type, name, sub_value_type);

       }
       
       w_Cass_Error  w_cassandra_data_type_add_sub_value_type_by_name_n(w_Cass_Data_Type* data_type, const char* name, size_t name_length, w_Cass_Value_Type sub_value_type)
       {
           if (!data_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_data_type_add_sub_value_type_by_name_n");
               return -1;
           }
           return  cass_data_type_add_sub_value_type_by_name_n(data_type, name, name_length, sub_value_type);

       }

       w_Cass_Collection*  w_cassandra_collection_new(w_Cass_Collection_Type type,size_t item_count)
       {
           
           return   cass_collection_new(type, item_count);

       }

       w_Cass_Collection*   w_cassandra_collection_new_from_data_type(const w_Cass_Data_Type* data_type, size_t item_count)
       {
           if (!data_type )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_collection_new_from_data_type");
               return NULL;
               
           }
           
           return  cass_collection_new_from_data_type(data_type, item_count);

       }

       void  w_cassandra_collection_free(w_Cass_Collection* collection)
       {
           if (!collection)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_collection_free");
           }
           cass_collection_free(collection);

       }

       const w_Cass_Data_Type* w_cassandra_collection_data_type(const w_Cass_Collection* collection)
       {
           if (!collection)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_collection_data_type");
               return NULL;
           }
           return cass_collection_data_type(collection);

       }

       w_Cass_Error w_cassandra_collection_append_int8(w_Cass_Collection* collection,int8_t value)
       {
           if (!collection)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_collection_append_int8");
               return -1;
           }
           return  cass_collection_append_int8(collection, value);

       }

       w_Cass_Error  w_cassandra_collection_append_int16(w_Cass_Collection* collection, int16_t value)
       {
           if (!collection)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_collection_append_int16");
               return -1;
           }
           return  cass_collection_append_int16(collection, value);

       }

       w_Cass_Error  w_cassandra_collection_append_int32(w_Cass_Collection* collection,int32_t value)
       {
           if (!collection)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_collection_append_int32");
               return -1;
           }
           return cass_collection_append_int32(collection, value);

       }

       w_Cass_Error w_cassandra_collection_append_uint32(w_Cass_Collection* collection,uint32_t value)
       {
           if (!collection)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_collection_append_uint32");
               return -1;
           }
           return cass_collection_append_uint32(collection, value);

       }

       w_Cass_Error  w_cassandra_collection_append_int64(w_Cass_Collection* collection,int64_t value)
       {
           if (!collection)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_collection_append_int64");
               return -1;
           }
           return  cass_collection_append_int64(collection, value);

       }

       w_Cass_Error  w_cassandra_collection_append_float(w_Cass_Collection* collection,float value)
       {
           if (!collection)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_collection_append_float");
               return -1;
           }
           return  cass_collection_append_float(collection, value);

       }

       w_Cass_Error   w_cassandra_collection_append_double(w_Cass_Collection* collection, double value)
       {
           if (!collection)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_collection_append_double");
               return -1;
           }
           return cass_collection_append_double(collection, value);

       }

       w_Cass_Error  w_cassandra_collection_append_bool(w_Cass_Collection* collection,w_bool_t value)
       {
           if (!collection)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_collection_append_bool");
               return -1;
           }
           return cass_collection_append_bool(collection, value);

       }

       w_Cass_Error  w_cassandra_collection_append_string(w_Cass_Collection* collection, const char* value)
       {
           if (!collection || !value)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_collection_append_string");
               return -1;
           }
           return cass_collection_append_string(collection, value);

       }

       w_Cass_Error w_cassandra_collection_append_string_n(w_Cass_Collection* collection, const char* value, size_t value_length)
       {
           if (!collection || !value)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_collection_append_string_n");
               return -1;
           }
           return  cass_collection_append_string_n(collection, value, value_length);

       }

       w_Cass_Error w_cassandra_collection_append_bytes(w_Cass_Collection* collection, const w_byte_t* value,size_t value_size)
       {
           if (!collection )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_collection_append_bytes");
               return -1;
           }
           return cass_collection_append_bytes(collection, value, value_size);

       }

       w_Cass_Error  w_cassandra_collection_append_custom(w_Cass_Collection* collection, const char* class_name, const w_byte_t* value, size_t value_size)
       {
           if (!collection || !class_name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_collection_append_custom");
               return -1;
           }
           return cass_collection_append_custom(collection, class_name, value, value_size);

       }

       w_Cass_Error  w_cassandra_collection_append_custom_n(w_Cass_Collection* collection,  const char* class_name, size_t class_name_length, const w_byte_t* value, size_t value_size)
       {
           if (!collection || !class_name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_collection_append_custom_n");
               return -1;
           }
           return cass_collection_append_custom_n(collection, class_name,  class_name_length,  value,  value_size);
       }

       w_Cass_Error w_cassandra_collection_append_uuid(w_Cass_Collection* collection, w_CassUuid value)
       {
           if (!collection )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_collection_append_uuid");
               return -1;
           }
           return  cass_collection_append_uuid(collection, value);

       }

       w_Cass_Error  w_cassandra_collection_append_inet(w_Cass_Collection* collection, w_CassInet value)
       {
           if (!collection)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_collection_append_inet");
               return -1;
           }
           return  cass_collection_append_inet(collection, value);

       }

       w_Cass_Error  w_cassandra_collection_append_decimal(w_Cass_Collection* collection,const w_byte_t* varint, size_t varint_size,int32_t scale)
       {
           if (!collection)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_collection_append_decimal");
               return -1;
           }
           return  cass_collection_append_decimal(collection, varint, varint_size, scale);

       }

       w_Cass_Error  w_cassandra_collection_append_duration(w_Cass_Collection* collection,int32_t months, int32_t days,int64_t nanos)
       {
           if (!collection)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_collection_append_duration");
               return -1;
           }
           return  cass_collection_append_duration(collection, months, days, nanos);

       }

       w_Cass_Error  w_cassandra_collection_append_collection(w_Cass_Collection* collection,const w_Cass_Collection* value)
       {
           if (!collection || !value)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_collection_append_collection");
               return -1;
           }
           return cass_collection_append_collection(collection, value);

       }

       w_Cass_Error  w_cassandra_collection_append_tuple(w_Cass_Collection* collection, const w_Cass_Tuple* value)
       {
           if (!collection || !value)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_collection_append_tuple");
               return -1;
           }
           return   cass_collection_append_tuple(collection, value);

       }

       w_Cass_Error  w_cassandra_collection_append_user_type(w_Cass_Collection* collection, const w_Cass_User_Type* value)
       {
           if (!collection || !value)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_collection_append_user_type");
               return -1;
           }
           return   cass_collection_append_user_type(collection, value);

       }

       w_Cass_Tuple* w_cassandra_tuple_new(size_t item_count)
       {
          
           return   cass_tuple_new( item_count);

       }

       w_Cass_Tuple* w_cassandra_tuple_new_from_data_type(const w_Cass_Data_Type* data_type)
       {
           if (!data_type)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_tuple_new_from_data_type");
               return NULL;
           }
           return  cass_tuple_new_from_data_type(data_type);

       }

       void  w_cassandra_tuple_free(w_Cass_Tuple* tuple)
       {
           if (!tuple)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_tuple_free");
           }
       
           cass_tuple_free(tuple);

       }

       const w_Cass_Data_Type* w_cassandra_tuple_data_type(const w_Cass_Tuple* tuple)
       {
           if (!tuple)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_tuple_data_type");
               return NULL;
           }
           return  cass_tuple_data_type(tuple);

       }

       w_Cass_Error w_cassandra_tuple_set_null(w_Cass_Tuple* tuple, size_t index)
       {
           if (!tuple)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_tuple_set_null");
               return -1;
           }
           return cass_tuple_set_null(tuple, index);

       }

       w_Cass_Error w_cassandra_tuple_set_int8(w_Cass_Tuple* tuple,size_t index,int8_t value)
       {
           if (!tuple)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_tuple_set_int8");
               return -1;
           }
           return  cass_tuple_set_int8(tuple, index, value);

       }

       w_Cass_Error w_cassandra_tuple_set_int16(w_Cass_Tuple* tuple, size_t index, int16_t value)
       {
           if(!tuple)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_tuple_set_int16");
               return -1;
           }
           return cass_tuple_set_int16(tuple, index, value);

       }

       w_Cass_Error w_cassandra_tuple_set_int32(w_Cass_Tuple* tuple,size_t index,int32_t value)
       {
           if (!tuple)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_tuple_set_int32");
               return -1;
           }
           return  cass_tuple_set_int32(tuple, index, value);

       }

       w_Cass_Error   w_cassandra_tuple_set_uint32(w_Cass_Tuple* tuple,size_t index, uint32_t value)
       {
           if (!tuple)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_tuple_set_uint32");
               return -1;
           }
           return cass_tuple_set_uint32(tuple, index, value);

       }

       w_Cass_Error w_cassandra_tuple_set_int64(w_Cass_Tuple* tuple,size_t index,int64_t value)
       {
           if (!tuple)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_tuple_set_int64");
               return -1;
           }
           return cass_tuple_set_int64(tuple, index, value);

       }

       w_Cass_Error  w_cassandra_tuple_set_float(w_Cass_Tuple* tuple,size_t index, float value)
       {
           if (!tuple)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_tuple_set_float");
               return -1;
           }
           return cass_tuple_set_float(tuple, index, value);

       }

       w_Cass_Error w_cassandra_tuple_set_double(w_Cass_Tuple* tuple, size_t index,double value)
       {
           if (!tuple)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_tuple_set_double");
               return -1;
           }
           return cass_tuple_set_double(tuple, index, value);

       }

       w_Cass_Error  w_cassandra_tuple_set_bool(w_Cass_Tuple* tuple, size_t index, w_bool_t value)
       {
           if (!tuple)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_tuple_set_bool");
               return -1;
           }
           return   cass_tuple_set_bool(tuple, index, value);

       }

       w_Cass_Error  w_cassandra_tuple_set_string(w_Cass_Tuple* tuple, size_t index, const char* value)
       {
           if (!tuple)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_tuple_set_string");
               return -1;
           }
           return   cass_tuple_set_string(tuple,  index,  value);

       }

       w_Cass_Error  w_cassandra_tuple_set_string_n(w_Cass_Tuple* tuple,  size_t index, const char* value, size_t value_length)
       {
           if (!tuple)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_tuple_set_string_n");
               return -1;
           }
           return  cass_tuple_set_string_n(tuple, index, value, value_length);

       }

       w_Cass_Error  w_cassandra_tuple_set_bytes(w_Cass_Tuple* tuple,size_t index, const w_byte_t* value,size_t value_size)
       {
           if (!tuple)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_tuple_set_bytes");
               return -1;
           }
           return  cass_tuple_set_bytes(tuple, index, value, value_size);

       }

       w_Cass_Error  w_cassandra_tuple_set_custom(w_Cass_Tuple* tuple, size_t index, const char* class_name, const w_byte_t* value, size_t value_size)
       {
           if (!tuple)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_tuple_set_custom");
               return -1;
           }
           return cass_tuple_set_custom(tuple, index, class_name, value, value_size);

       }

       w_Cass_Error   w_cassandra_tuple_set_custom_n(w_Cass_Tuple* tuple, size_t index, const char* class_name,  size_t class_name_length, const w_byte_t* value,size_t value_size)
       {
           if (!tuple)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_tuple_set_custom_n");
               return -1;
           }
           return cass_tuple_set_custom_n(tuple, index, class_name,class_name_length, value, value_size);

       }

       w_Cass_Error w_cassandra_tuple_set_uuid(w_Cass_Tuple* tuple,size_t index, w_CassUuid value)
       {
           if (!tuple)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_tuple_set_uuid");
               return -1;
           }
           return   cass_tuple_set_uuid(tuple, index, value);

       }

       w_Cass_Error  w_cassandra_tuple_set_inet(w_Cass_Tuple* tuple,size_t index, w_CassInet value)
       {
           if (!tuple)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_tuple_set_inet");
               return -1;
           }

           return  cass_tuple_set_inet(tuple, index, value);

       }

       w_Cass_Error w_cassandra_tuple_set_decimal(w_Cass_Tuple* tuple, size_t index, const w_byte_t* varint, size_t varint_size, int32_t scale)
       {
           if (!tuple)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_tuple_set_decimal");
               return -1;
           }
           return    cass_tuple_set_decimal(tuple, index, varint, varint_size, scale);

       }

       w_Cass_Error   w_cassandra_tuple_set_duration(w_Cass_Tuple* tuple, size_t index, int32_t months, int32_t days, int64_t nanos)
       {
           if (!tuple)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_tuple_set_duration");
               return -1;
           }
           return cass_tuple_set_duration(tuple, index, months, days, nanos);

       }

       w_Cass_Error  w_cassandra_tuple_set_collection(w_Cass_Tuple* tuple, size_t index, const w_Cass_Collection* value)
       {
           if (!tuple)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_tuple_set_collection");
               return -1;
           }
           return  cass_tuple_set_collection(tuple, index, value);

       }

       w_Cass_Error w_cassandra_tuple_set_tuple(w_Cass_Tuple* tuple,size_t index, const w_Cass_Tuple* value)
       {
           if (!tuple)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_tuple_set_tuple");
               return -1;
           }
           return  cass_tuple_set_tuple(tuple, index, value);

       }

       w_Cass_Error  w_cassandra_tuple_set_user_type(w_Cass_Tuple* tuple, size_t index, const w_Cass_User_Type* value)
       {
           if (!tuple)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_tuple_set_user_type");
               return -1;
           }
           return  cass_tuple_set_user_type(tuple, index, value);

       }

       w_Cass_User_Type*  w_cassandra_user_type_new_from_data_type(const w_Cass_Data_Type* data_type)
       {
           if (!data_type)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_new_from_data_type");
               return NULL;
           }
           return cass_user_type_new_from_data_type(data_type);

       }

       void  w_cassandra_user_type_free(w_Cass_User_Type* user_type)
       {
           if (!user_type)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_free");
           }
             cass_user_type_free(user_type);

       }

       const w_Cass_Data_Type* w_cassandra_user_type_data_type(const w_Cass_User_Type* user_type)
       {
           if (!user_type)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_data_type");
               return NULL;
           }

           return cass_user_type_data_type( user_type);

       }

       w_Cass_Error  w_cassandra_user_type_set_null(w_Cass_User_Type* user_type,size_t index)
       {
           if (!user_type)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_null");
               return -1;
           }
           return cass_user_type_set_null(user_type, index);

       }

       w_Cass_Error  w_cassandra_user_type_set_null_by_name(w_Cass_User_Type* user_type,const char* name)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_null_by_name");
               return -1;
           }
           return  cass_user_type_set_null_by_name(user_type, name);

       }

       w_Cass_Error  w_cassandra_user_type_set_null_by_name_n(w_Cass_User_Type* user_type,const char* name, size_t name_length)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_null_by_name_n");
               return -1;
           }
           return   cass_user_type_set_null_by_name_n(user_type, name, name_length);

       }

       w_Cass_Error  w_cassandra_user_type_set_int8(w_Cass_User_Type* user_type, size_t index, int8_t value)
       {
           if (!user_type )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_int8");
               return -1;
           }
           return   cass_user_type_set_int8(user_type, index, value);

       }

       w_Cass_Error  w_cassandra_user_type_set_int8_by_name(w_Cass_User_Type* user_type, const char* name, int8_t value)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_int8_by_name");
               return -1;
           }
           return cass_user_type_set_int8_by_name(user_type, name, value);

       }

       w_Cass_Error w_cassandra_user_type_set_int8_by_name_n(w_Cass_User_Type* user_type, const char* name, size_t name_length, int8_t value)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_int8_by_name_n");
               return -1;
           }
           return   cass_user_type_set_int8_by_name_n(user_type, name, name_length, value);

       }

       w_Cass_Error  w_cassandra_user_type_set_int16(w_Cass_User_Type* user_type, size_t index, int16_t value)
       {
           if (!user_type )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_int16");
               return -1;
           }
           return cass_user_type_set_int16(user_type, index, value);

       }

       w_Cass_Error  w_cassandra_user_type_set_int16_by_name(w_Cass_User_Type* user_type, const char* name,int16_t value)
       {
           if (!user_type || !name )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_int16_by_name");
               return -1;
           }
           return   cass_user_type_set_int16_by_name(user_type, name, value);

       }
       w_Cass_Error w_cassandra_user_type_set_int16_by_name_n(w_Cass_User_Type* user_type,const char* name,size_t name_length,int16_t value)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_int16_by_name_n");
               return -1;
           }
           return cass_user_type_set_int16_by_name_n(user_type, name, name_length, value);

       }

       w_Cass_Error  w_cassandra_user_type_set_int32(w_Cass_User_Type* user_type, size_t index, int32_t value)
       {
           if (!user_type )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_int32");
               return -1;
           }

           return  cass_user_type_set_int32(user_type, index, value);

       }

       w_Cass_Error  w_cassandra_user_type_set_int32_by_name(w_Cass_User_Type* user_type,const char* name,int32_t value)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_int32_by_name");
               return -1;
           }
           return cass_user_type_set_int32_by_name(user_type, name, value);

       }

       w_Cass_Error   w_cassandra_user_type_set_int32_by_name_n(w_Cass_User_Type* user_type, const char* name,size_t name_length,int32_t value)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_int32_by_name_n");
               return -1;
           }
           return  cass_user_type_set_int32_by_name_n(  user_type,  name,  name_length,  value);

       }

       w_Cass_Error  w_cassandra_user_type_set_uint32(w_Cass_User_Type* user_type,size_t index, uint32_t value)
       {
           if (!user_type )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_uint32");
               return -1;
           }

           return cass_user_type_set_uint32(user_type, index, value);

       }

       w_Cass_Error  w_cassandra_user_type_set_uint32_by_name(w_Cass_User_Type* user_type, const char* name, uint32_t value)
       {
           if (!user_type || !name )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_uint32_by_name");
               return -1;
           }
           return   cass_user_type_set_uint32_by_name(user_type, name, value);

       }

       w_Cass_Error w_cassandra_user_type_set_uint32_by_name_n(w_Cass_User_Type* user_type, const char* name,size_t name_length,uint32_t value)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_uint32_by_name_n");
               return -1;
           }

           return   cass_user_type_set_uint32_by_name_n(user_type, name, name_length, value);

       }

       w_Cass_Error w_cassandra_user_type_set_int64(w_Cass_User_Type* user_type, size_t index, int64_t value)
       {
           if(!user_type )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_int64");
               return -1;
           }

           return   cass_user_type_set_int64(user_type, index, value);

       }

       w_Cass_Error w_cassandra_user_type_set_int64_by_name(w_Cass_User_Type* user_type,const char* name, int64_t value)
       {
           if(!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_int64_by_name");
               return -1;
           }

           return    cass_user_type_set_int64_by_name(user_type, name, value);

       }

       w_Cass_Error  w_cassandra_user_type_set_int64_by_name_n(w_Cass_User_Type* user_type, const char* name, size_t name_length, int64_t value)
       {
           if(!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_int64_by_name_n");
               return -1;
           }

           return    cass_user_type_set_int64_by_name_n(user_type, name, name_length, value);

       }

       w_Cass_Error w_cassandra_user_type_set_float(w_Cass_User_Type* user_type,size_t index,float value)
       {
           if (!user_type )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_float");
               return -1;
           }
           return    cass_user_type_set_float(user_type, index, value);

       }
       w_Cass_Error  w_cassandra_user_type_set_float_by_name(w_Cass_User_Type* user_type, const char* name, float value)
       {
           if (!user_type || !name )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_float_by_name");
               return -1;
           }
           return cass_user_type_set_float_by_name(user_type, name, value);

       }

       w_Cass_Error w_cassandra_user_type_set_float_by_name_n(w_Cass_User_Type* user_type, const char* name, size_t name_length, float value)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_float_by_name_n");
               return -1;
           }
           return    cass_user_type_set_float_by_name_n(user_type, name, name_length, value);

       }

       w_Cass_Error  w_cassandra_user_type_set_double(w_Cass_User_Type* user_type, size_t index,double value)
       {
           if (!user_type )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_double");
               return -1;
           }
           return  cass_user_type_set_double(user_type, index, value);

       }

       w_Cass_Error  w_cassandra_user_type_set_double_by_name(w_Cass_User_Type* user_type,const char* name, double value)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_double_by_name");
               return -1;
           }
           return   cass_user_type_set_double_by_name(user_type, name, value);

       }

       w_Cass_Error w_cass_user_type_set_double_by_name_n(w_Cass_User_Type* user_type, const char* name,size_t name_length, double value)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_double_by_name_n");
               return -1;
           }
           return cass_user_type_set_double_by_name_n(user_type, name, name_length, value);

       }
       w_Cass_Error  w_cassandra_user_type_set_bool(w_Cass_User_Type* user_type, size_t index, w_bool_t value)
       {
           if (!user_type )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_bool");
               return -1;
           }
           return  cass_user_type_set_bool(user_type, index, value);

       }

       w_Cass_Error   w_cassandra_user_type_set_bool_by_name(w_Cass_User_Type* user_type, const char* name, w_bool_t value)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_bool_by_name");
               return -1;
           }
           return    cass_user_type_set_bool_by_name(user_type, name, value);

       }

       w_Cass_Error  w_cassandra_user_type_set_bool_by_name_n(w_Cass_User_Type* user_type, const char* name, size_t name_length, w_bool_t value)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_bool_by_name_n");
               return -1;
           }
           return    cass_user_type_set_bool_by_name_n(user_type, name, name_length, value);

       }

       w_Cass_Error  w_cassandra_user_type_set_string(w_Cass_User_Type* user_type,size_t index,const char* value)
       {
           if (!user_type )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_string");
               return -1;
           }
           return   cass_user_type_set_string(user_type, index, value);

       }

       w_Cass_Error  w_cassandra_user_type_set_string_n(w_Cass_User_Type* user_type,size_t index,const char* value, size_t value_length)
       {
           if (!user_type)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_string_n");
               return -1;
           }

           return    cass_user_type_set_string_n(user_type, index, value, value_length);

       }

       w_Cass_Error  w_cassandra_user_type_set_string_by_name(w_Cass_User_Type* user_type, const char* name,const char* value)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_string_by_name");
               return -1;
           }
           return cass_user_type_set_string_by_name(user_type, name, value);

       }

       w_Cass_Error w_cassandra_user_type_set_string_by_name_n(w_Cass_User_Type* user_type, const char* name, size_t name_length,const char* value,  size_t value_length)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_string_by_name_n");
               return -1;
           }
           return   cass_user_type_set_string_by_name_n(user_type, name, name_length, value, value_length);

       }

       w_Cass_Error  w_cassandra_user_type_set_bytes(w_Cass_User_Type* user_type, size_t index, const w_byte_t* value,size_t value_size)
       {
           if (!user_type)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_bytes");
               return -1;
           }
           return   cass_user_type_set_bytes(user_type, index, value, value_size);

       }

       w_Cass_Error  w_cassandra_user_type_set_bytes_by_name(w_Cass_User_Type* user_type,const char* name,const w_byte_t* value, size_t value_size)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_bytes_by_name");
               return -1;
           }
           return    cass_user_type_set_bytes_by_name(user_type, name, value, value_size);

       }

       w_Cass_Error   w_cassandra_user_type_set_bytes_by_name_n(w_Cass_User_Type* user_type, const char* name, size_t name_length,const w_byte_t* value, size_t value_size) 
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_bytes_by_name_n");
               return -1;
           }
           return    cass_user_type_set_bytes_by_name_n(user_type, name, name_length, value, value_size);

       }

       w_Cass_Error  w_cassandra_user_type_set_custom(w_Cass_User_Type* user_type, size_t index, const char* class_name, const w_byte_t* value, size_t value_size)
       {
           if (!user_type )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_custom");
               return -1;
           }

           return  cass_user_type_set_custom(user_type, index, class_name, value, value_size);

       }

       w_Cass_Error   w_cassandra_user_type_set_custom_n(w_Cass_User_Type* user_type,size_t index, const char* class_name, size_t class_name_length,  const w_byte_t* value, size_t value_size)
       {
           if (!user_type)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_custom_n");
               return -1;
           }
           return    cass_user_type_set_custom_n(user_type, index, class_name, class_name_length, value, value_size);

       }

       w_Cass_Error   w_cassandra_user_type_set_custom_by_name(w_Cass_User_Type* user_type, const char* name, const char* class_name, const w_byte_t* value, size_t value_size)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_custom_by_name");
               return -1;
           }
           return  cass_user_type_set_custom_by_name(user_type, name, class_name, value, value_size);

       }

       w_Cass_Error  w_cassandra_user_type_set_custom_by_name_n(w_Cass_User_Type* user_type, const char* name,size_t name_length, const char* class_name,size_t class_name_length, const w_byte_t* value, size_t value_size)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_custom_by_name_n");
               return -1;
           }
           return   cass_user_type_set_custom_by_name_n(user_type, name, name_length, class_name, class_name_length, value, value_size);

       }

       w_Cass_Error  w_cassandra_user_type_set_uuid(w_Cass_User_Type* user_type, size_t index, w_CassUuid value)
       {
           if (!user_type )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_uuid");
               return -1;
           }
           return    cass_user_type_set_uuid(user_type, index, value);

       }

       w_Cass_Error w_cassandra_user_type_set_uuid_by_name(w_Cass_User_Type* user_type,const char* name, w_CassUuid value)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_uuid_by_name");
               return -1;
           }
           return   cass_user_type_set_uuid_by_name(user_type, name, value);

       }

       w_Cass_Error  w_cassandra_user_type_set_uuid_by_name_n(w_Cass_User_Type* user_type, const char* name, size_t name_length,w_CassUuid value)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cass_user_type_set_uuid_by_name_n");
               return -1;
           }
           return    cass_user_type_set_uuid_by_name_n(user_type, name, name_length, value);

       }

       w_Cass_Error  w_cass_user_type_set_inet(w_Cass_User_Type* user_type,  size_t index, w_CassInet value)
       {
           if (!user_type )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cass_user_type_set_inet");
               return -1;
           }

           return    cass_user_type_set_inet(user_type, index, value);

       }

       w_Cass_Error w_cass_user_type_set_inet_by_name(w_Cass_User_Type* user_type, const char* name, w_CassInet value)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cass_user_type_set_inet_by_name");
               return -1;
           }
           return   cass_user_type_set_inet_by_name(user_type, name, value);

       }

       w_Cass_Error  w_cassandra_user_type_set_inet_by_name_n(w_Cass_User_Type* user_type, const char* name,size_t name_length,w_CassInet value)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_inet_by_name_n");
               return -1;
           }
           return   cass_user_type_set_inet_by_name_n(user_type, name, name_length, value);

       }

       w_Cass_Error   w_cassandra_user_type_set_decimal(w_Cass_User_Type* user_type,size_t index, const w_byte_t* varint, size_t varint_size,  int scale)
       {
           if (!user_type )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_decimal");
               return -1;
           }

           return   cass_user_type_set_decimal(user_type, index, varint, varint_size, scale);

       }

       w_Cass_Error  w_cassandra_user_type_set_decimal_by_name(w_Cass_User_Type* user_type, const char* name, const w_byte_t* varint, size_t varint_size, int scale)
       {
           if (!user_type || !name )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_decimal_by_name");
               return -1;
           }
           return cass_user_type_set_decimal_by_name(user_type, name, varint, varint_size, scale);

       }

       w_Cass_Error  w_cassandra_user_type_set_decimal_by_name_n(w_Cass_User_Type* user_type, const char* name, size_t name_length, const w_byte_t* varint, size_t varint_size, int scale)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_decimal_by_name_n");
               return -1;
           }
           return    cass_user_type_set_decimal_by_name_n(user_type, name, name_length, varint, varint_size, scale);

       }

       w_Cass_Error  w_cassandra_user_type_set_duration(w_Cass_User_Type* user_type,size_t index, int32_t months, int32_t days, int64_t nanos)
       {
           if (!user_type )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_duration");
               return -1;
           }
           return   cass_user_type_set_duration(user_type, index, months, days, nanos);

       }

       w_Cass_Error  w_cassandra_user_type_set_duration_by_name(w_Cass_User_Type* user_type,  const char* name, int32_t months,  int32_t days,  int64_t nanos)
       {
           if (!user_type || !name )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_duration_by_name");
               return -1;
           }
           return    cass_user_type_set_duration_by_name(user_type, name, months, days, nanos);

       }

       w_Cass_Error  w_cassandra_user_type_set_duration_by_name_n(w_Cass_User_Type* user_type, const char* name,   size_t name_length,int32_t months,int32_t days, int64_t nanos)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_duration_by_name_n");
               return -1;
           }
          return     cass_user_type_set_duration_by_name_n(user_type, name, name_length, months, days, nanos);

       }

       w_Cass_Error  w_cassandra_user_type_set_collection(w_Cass_User_Type* user_type, size_t index,const w_Cass_Collection* value)
       {
           if (!user_type )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_collection");
               return -1;
           }
           return    cass_user_type_set_collection(user_type, index, value);

       }

       w_Cass_Error w_cassandra_user_type_set_collection_by_name(w_Cass_User_Type* user_type, const char* name, const w_Cass_Collection* value)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_collection_by_name");
               return -1;
           }
               return  cass_user_type_set_collection_by_name(user_type, name,   value);

       }
       w_Cass_Error w_cassandra_user_type_set_collection_by_name_n(w_Cass_User_Type* user_type, const char* name, size_t name_length, const w_Cass_Collection* value)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_collection_by_name_n");
               return -1;
           }
           return  cass_user_type_set_collection_by_name_n(user_type, name, name_length, value);

       }

       w_Cass_Error w_cassandra_user_type_set_tuple(w_Cass_User_Type* user_type,size_t index, const w_Cass_Tuple* value)
       {
           if (!user_type )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_tuple");
               return -1;
           }

           return     cass_user_type_set_tuple(user_type, index, value);

       }

       w_Cass_Error  w_cassandra_user_type_set_tuple_by_name(w_Cass_User_Type* user_type, const char* name, const w_Cass_Tuple* value)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_tuple_by_name");
               return -1;
           }
           return    cass_user_type_set_tuple_by_name(user_type, name, value);

       }

       w_Cass_Error   w_cassandra_user_type_set_tuple_by_name_n(w_Cass_User_Type* user_type,const char* name, size_t name_length, const w_Cass_Tuple* value)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_tuple_by_name_n");
               return -1;
           }
           return   cass_user_type_set_tuple_by_name_n(user_type, name, name_length, value);

       }

       w_Cass_Error  w_cassandra_user_type_set_user_type(w_Cass_User_Type* user_type,  size_t index,const w_Cass_User_Type* value)
       {
           if (!user_type )
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cass_user_type_set_user_type");
               return -1;
           }
           return   cass_user_type_set_user_type(user_type, index, value);

       }

       w_Cass_Error w_cass_user_type_set_user_type_by_name(w_Cass_User_Type* user_type, const char* name, const w_Cass_User_Type* value)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_user_type_by_name");
               return -1;
           }
           return    cass_user_type_set_user_type_by_name(user_type, name, value);

       }

       w_Cass_Error w_cassandra_user_type_set_user_type_by_name_n(w_Cass_User_Type* user_type,const char* name, size_t name_length,const w_Cass_User_Type* value)
       {
           if (!user_type || !name)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_user_type_set_user_type_by_name_n");
               return -1;
           }
           return cass_user_type_set_user_type_by_name_n(user_type, name, name_length, value);

       }

       void w_cassandra_result_free(const w_Cass_Result* result)
       {
           if (!result)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_result_free");
              
           }
           cass_result_free(result);

       }

       size_t  w_cassandra_result_row_count(const w_Cass_Result* result)
       {
           if (!result)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_result_row_count");
               return 0;
           }
           return   cass_result_row_count(result);

       }

       size_t w_cassandra_result_column_count(const w_Cass_Result* result)
       {
           if (!result)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_result_column_count");
               return 0;
           }
           return  cass_result_column_count(result);

       }

       w_Cass_Error w_cassandra_result_column_name(const w_Cass_Result* result, size_t index,const char** name, size_t* name_length)
       {
           if (!result)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_result_column_name");
               return -1;
           }
           return   cass_result_column_name(result, index, name, name_length);

       }
       w_Cass_Value_Type  w_cassandra_result_column_type(const w_Cass_Result* result, size_t index)
       {
           if (!result)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_result_column_type");
               return -1;
           }
           return   cass_result_column_type(result, index);

       }

       const w_Cass_Data_Type* w_cassandra_result_column_data_type(const w_Cass_Result* result, size_t index)
       {
           if (!result)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_result_column_data_type");
               return NULL;
           }
           return  cass_result_column_data_type(result, index);

       }
       const w_Cass_Row* w_cassandra_result_first_row(const w_Cass_Result* result)
       {
           if (!result)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_result_first_row");
               return NULL;
           }
           return   cass_result_first_row(result);

       }

       w_bool_t w_cassandra_result_has_more_pages(const w_Cass_Result* result)
       {
           if (!result)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_result_has_more_pages");
               return false;
           }
           return  cass_result_has_more_pages(result);

       }

       w_Cass_Error   w_cassandra_result_paging_state_token(const w_Cass_Result* result,const char** paging_state, size_t* paging_state_size)
       {
           if (!result)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_result_paging_state_token");
               return -1;
           }
           return   cass_result_paging_state_token(result, paging_state, paging_state_size);

       }

       void w_cassandra_error_result_free(const w_Cass_Error_Result* error_result)
       {
           if (!error_result)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_error_result_free");
              
           }
           cass_error_result_free(error_result);

       }


       w_Cass_Error w_cassandra_error_result_code(const w_Cass_Error_Result* error_result)
       {

           if (!error_result)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_error_result_code");
               return -1;

           }
           return  cass_error_result_code(error_result);

       }

       W_Cass_Consistency w_cassandra_error_result_consistency(const w_Cass_Error_Result* error_result)
       {
           if (!error_result)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cass_error_result_consistency");
               return -1;

           }
        return cass_error_result_consistency(error_result);

       }

       int32_t  w_cass_error_result_responses_received(const w_Cass_Error_Result* error_result)
       {
           if (!error_result)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_error_result_responses_received");
               return -1;

           }
           return  cass_error_result_responses_received(error_result);

       }


       int32_t w_cassandra_error_result_responses_required(const w_Cass_Error_Result* error_result)
       {
           if (!error_result)
           {
               W_ASSERT(false, "missing parameters!. trace info: w_cassandra_error_result_responses_required");
               return -1;

           }
           return  cass_error_result_responses_required(error_result);

       }

      int32_t  w_cassandra_error_result_num_failures(const w_Cass_Error_Result* error_result)
      {
          if (!error_result)
          {
              W_ASSERT(false, "missing parameters!. trace info: w_cassandra_error_result_num_failures");
              return -1;

          }
         return  cass_error_result_num_failures( error_result);

      }

      w_bool_t  w_cassandra_error_result_data_present(const w_Cass_Error_Result* error_result)
      {
          if (!error_result)
          {
              W_ASSERT(false, "missing parameters!. trace info: w_cassandra_error_result_data_present");
              return false;

          }
         return  cass_error_result_data_present(error_result);

      }

      w_Cass_WriteType w_cassandra_error_result_write_type(const w_Cass_Error_Result* error_result)
      {
          if (!error_result)
          {
              W_ASSERT(false, "missing parameters!. trace info: w_cassandra_error_result_write_type");
              return -1;

          }

         return cass_error_result_write_type(error_result);

      }

      w_Cass_Error w_cassandra_error_result_keyspace(const w_Cass_Error_Result* error_result, const char** keyspace, size_t* keyspace_length)
      {
          if (!error_result || !keyspace)
          {
              W_ASSERT(false, "missing parameters!. trace info: w_cassandra_error_result_keyspace");
              return -1;

          }
           return cass_error_result_keyspace(error_result,  keyspace, keyspace_length);

      }

      w_Cass_Error  w_cassandra_error_result_table(const w_Cass_Error_Result* error_result, const char** table, size_t* table_length)
      {
          if (!error_result || !table)
          {
              W_ASSERT(false, "missing parameters!. trace info: w_cassandra_error_result_table");
              return -1;

          }
         return  cass_error_result_table(error_result, table, table_length);

      }

     w_Cass_Error w_cassandra_error_result_function(const w_Cass_Error_Result* error_result, const char** function,size_t* function_length)
     {
         if (!error_result || !function)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_error_result_function");
             return -1;

         }
         return cass_error_result_function(error_result, function, function_length);

     }

     size_t w_cassandra_error_num_arg_types(const w_Cass_Error_Result* error_result)
     {
         if (!error_result )
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_error_num_arg_types");
             return 0;

         }

         return   cass_error_num_arg_types(error_result);

     }

     w_Cass_Error  w_cassandra_error_result_arg_type(const w_Cass_Error_Result* error_result, size_t index, const char** arg_type, size_t* arg_type_length)
     {
         if (!error_result)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_error_result_arg_type");
             return -1;

         }
         return  cass_error_result_arg_type(error_result, index, arg_type, arg_type_length);

     }

     void w_cassandra_iterator_free(w_Cass_Iterator* iterator)
     {
         if (!iterator)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_free");
           
         }
         cass_iterator_free(iterator);
     }

     w_Cass_Iterator_Type  w_cassandra_iterator_type(w_Cass_Iterator* iterator)
     {
         if (!iterator)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_type");
             return -1;
         }
       return cass_iterator_type(iterator);

     }

     w_Cass_Iterator* w_cassandra_iterator_from_result(const w_Cass_Result* result)
     {
         if (!result)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_from_result");
             return NULL;
         }
         return  cass_iterator_from_result(result);

     }

     w_Cass_Iterator* w_cassandra_iterator_from_row(const w_Cass_Row* row)
     {
         if (!row)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_from_row");
             return NULL;
         }
         return   cass_iterator_from_row(row);

     }

     w_Cass_Iterator*   w_cassandra_iterator_from_collection(const w_Cass_Value* value)
     {
         if (!value)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_from_collection");
             return NULL;
         }
         return cass_iterator_from_collection(value);

     }

     w_Cass_Iterator* w_cassandra_iterator_from_map(const w_Cass_Value* value)
     {
         if (!value)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_from_map");
             return NULL;
         }
         return cass_iterator_from_map( value);
     }

     w_Cass_Iterator* w_cassandra_iterator_from_tuple(const w_Cass_Value* value)
     {
         if (!value)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_from_tuple");
             return NULL;
         }

         return  cass_iterator_from_tuple(value);
     }

     w_Cass_Iterator* w_cassandra_iterator_fields_from_user_type(const w_Cass_Value* value)
     {
         if (!value)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_fields_from_user_type");
             return NULL;
         }
         return  cass_iterator_fields_from_user_type(value);
     }

     w_Cass_Iterator* w_cassandra_iterator_keyspaces_from_schema_meta(const w_Cass_Schema_Meta* schema_meta)
     {
         if (!schema_meta)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_keyspaces_from_schema_meta");
             return NULL;
         }
         return    cass_iterator_keyspaces_from_schema_meta(schema_meta);


     }

     w_Cass_Iterator* w_cassandra_iterator_tables_from_keyspace_meta(const w_Cass_Key_space_Meta* keyspace_meta)
     {
         if (!keyspace_meta)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_tables_from_keyspace_meta");
             return NULL;
         }
         return  cass_iterator_tables_from_keyspace_meta(keyspace_meta);

     }


     w_Cass_Iterator* w_cassandra_iterator_materialized_views_from_keyspace_meta(const w_Cass_Key_space_Meta* keyspace_meta) 
     {
         if (!keyspace_meta)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_materialized_views_from_keyspace_meta");
             return NULL;
         }
         return cass_iterator_materialized_views_from_keyspace_meta( keyspace_meta);

     }

     w_Cass_Iterator*  w_cassandra_iterator_user_types_from_keyspace_meta(const w_Cass_Key_space_Meta* keyspace_meta)
     {
         if (!keyspace_meta)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_user_types_from_keyspace_meta");
             return NULL;
         }
         return  cass_iterator_user_types_from_keyspace_meta(keyspace_meta);

     }

     w_Cass_Iterator* w_cassandra_iterator_functions_from_keyspace_meta(const w_Cass_Key_space_Meta* keyspace_meta)
     {
         if (!keyspace_meta)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_functions_from_keyspace_meta");
             return NULL;
         }
         return   cass_iterator_functions_from_keyspace_meta(keyspace_meta);


     }
     w_Cass_Iterator*  w_cassandra_iterator_aggregates_from_keyspace_meta(const w_Cass_Key_space_Meta* keyspace_meta)
     {
         if (!keyspace_meta)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_aggregates_from_keyspace_meta");
             return NULL;
         }
         return  cass_iterator_aggregates_from_keyspace_meta(keyspace_meta);

     }
     w_Cass_Iterator*  w_cassandra_iterator_fields_from_keyspace_meta(const w_Cass_Key_space_Meta* keyspace_meta)
     {
         if (!keyspace_meta)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_fields_from_keyspace_meta");
             return NULL;
         }
         return  cass_iterator_fields_from_keyspace_meta(keyspace_meta);

     }

     w_Cass_Iterator*  w_cassandra_iterator_columns_from_table_meta(const w_Cass_Table_Meta* table_meta)
     {
         if (!table_meta)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_columns_from_table_meta");
             return NULL;
         }
        return cass_iterator_columns_from_table_meta( table_meta);
     }

     w_Cass_Iterator*  w_cassandra_iterator_indexes_from_table_meta(const w_Cass_Table_Meta* table_meta)
     {
         if (!table_meta)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_indexes_from_table_meta");
             return NULL;
         }
         return   cass_iterator_indexes_from_table_meta(table_meta);

     }

     w_Cass_Iterator*  w_cassandra_iterator_materialized_views_from_table_meta(const w_Cass_Table_Meta* table_meta)
     {
         if (!table_meta)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_materialized_views_from_table_meta");
             return NULL;
         }
         return cass_iterator_materialized_views_from_table_meta( table_meta);


     }

     w_Cass_Iterator*  w_cassandra_iterator_fields_from_table_meta(const w_Cass_Table_Meta* table_meta)
     {
         if (!table_meta)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_fields_from_table_meta");
             return NULL;
         }
         return cass_iterator_fields_from_table_meta(table_meta);
     }

     w_Cass_Iterator*  w_cassandra_iterator_columns_from_materialized_view_meta(const w_Cass_Materialized_View_Meta* view_me)
     {
         if (!view_me)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_columns_from_materialized_view_meta");
             return NULL;
         }
         return  cass_iterator_columns_from_materialized_view_meta(view_me);

     }

     w_Cass_Iterator*  w_cassandra_iterator_fields_from_materialized_view_meta(const w_Cass_Materialized_View_Meta* view_meta)
     {
         if (!view_meta)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_fields_from_materialized_view_meta");
             return NULL;
         }
         return cass_iterator_fields_from_materialized_view_meta( view_meta);
     }

     w_Cass_Iterator*  w_cassandra_iterator_fields_from_column_meta(const w_Cass_Column_Meta* column_meta)
     {
         if (!column_meta)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_fields_from_column_meta");
             return NULL;
         }
         return    cass_iterator_fields_from_column_meta(column_meta);

     }

     w_Cass_Iterator*  w_cassandra_iterator_fields_from_index_meta(const w_Cass_Index_Meta* index_meta)
     {
         if (!index_meta)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_fields_from_index_meta");
             return NULL;
         }
         return  cass_iterator_fields_from_index_meta(index_meta);

     }

     w_Cass_Iterator*  w_cassandra_iterator_fields_from_function_meta(const w_Cass_Function_Meta* function_meta)
     {
         if (!function_meta)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_fields_from_function_meta");
             return NULL;
         }
         return      cass_iterator_fields_from_function_meta(function_meta);

     }
     w_Cass_Iterator*  w_cassandra_iterator_fields_from_aggregate_meta(const w_Cass_Aggregate_Meta* aggregate_meta)
     {
         if (!aggregate_meta)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_fields_from_aggregate_meta");
             return NULL;
         }
         return   cass_iterator_fields_from_aggregate_meta(aggregate_meta);

     }

     w_bool_t w_cassandra_iterator_next(w_Cass_Iterator* iterator)
     {
         if (!iterator)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_next");
             return false;
         }
         return  cass_iterator_next(iterator);

     }

     const w_Cass_Row*  w_cassandra_iterator_get_row(const w_Cass_Iterator* iterator)
     {
         if (!iterator)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_get_row");
             return NULL;
         }
         return   cass_iterator_get_row(iterator);

     }

     const w_Cass_Value*  w_cassandra_iterator_get_column(const w_Cass_Iterator* iterator)
     {
         if (!iterator)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_get_column");
             return NULL;
         }
         return  cass_iterator_get_column(iterator);

     }

     const w_Cass_Value*  w_cassandra_iterator_get_value(const w_Cass_Iterator* iterator)
     {
         if (!iterator)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_get_value");
             return NULL;
         }
         return cass_iterator_get_value(iterator);
     }

     const w_Cass_Value*  w_cassandra_iterator_get_map_key(const w_Cass_Iterator* iterator)
     {
         if (!iterator)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_get_map_key");
             return NULL;
         }
         return cass_iterator_get_map_key(iterator);

     }

     const w_Cass_Value* w_cassandra_iterator_get_map_value(const w_Cass_Iterator* iterator) 
     {
         if (!iterator)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_get_map_value");
             return NULL;
         }
         return   cass_iterator_get_map_value(iterator);

     }

     w_Cass_Error w_cassandra_iterator_get_user_type_field_name(const w_Cass_Iterator* iterator, const char** name, size_t* name_length)
     {
         if (!iterator || !name)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_get_user_type_field_name");
             return -1;
         }
         return  cass_iterator_get_user_type_field_name(iterator, name, name_length);

     }

     const w_Cass_Value* w_cassandra_iterator_get_user_type_field_value(const w_Cass_Iterator* iterator)
     {
         if (!iterator )
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_get_user_type_field_value");
             return NULL;
         }
         return  cass_iterator_get_user_type_field_value(iterator);

     }

     const w_Cass_Key_space_Meta* w_cassandra_iterator_get_keyspace_meta(const w_Cass_Iterator* iterator)
     {
         if (!iterator)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_get_keyspace_meta");
             return NULL;
         }

         return     cass_iterator_get_keyspace_meta(iterator);

     }

     const w_Cass_Table_Meta* w_cassandra_iterator_get_table_meta(const w_Cass_Iterator* iterator)
     {
         if (!iterator)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_get_table_meta");
             return NULL;
         }
         return    cass_iterator_get_table_meta(iterator);

     }

     const w_Cass_Materialized_View_Meta*  w_cassandra_iterator_get_materialized_view_meta(const w_Cass_Iterator* iterator)
     {
         if (!iterator)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_get_materialized_view_meta");
             return NULL;
         }
         return cass_iterator_get_materialized_view_meta(iterator);

     }


     const w_Cass_Data_Type* w_cassandra_iterator_get_user_type(const w_Cass_Iterator* iterator)
     {
         if (!iterator)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_get_user_type");
             return NULL;
         }
         return cass_iterator_get_user_type(iterator);
     }

     const w_Cass_Function_Meta* w_cassandra_iterator_get_function_meta(const w_Cass_Iterator* iterator)
     {
         if (!iterator)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_get_function_meta");
             return NULL;
         }
         return   cass_iterator_get_function_meta(iterator);

     }

     const w_Cass_Aggregate_Meta* w_cassandra_iterator_get_aggregate_meta(const w_Cass_Iterator* iterator)
     {
         if (!iterator)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_get_aggregate_meta");
             return NULL;
         }
         return   cass_iterator_get_aggregate_meta(iterator);

     }

     const w_Cass_Column_Meta* w_cassandra_iterator_get_column_meta(const w_Cass_Iterator* iterator)
     {
         if (!iterator)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_get_column_meta");
             return NULL;
         }
         return   cass_iterator_get_column_meta(iterator);

     }

     const w_Cass_Index_Meta* w_cassandra_iterator_get_index_meta(const w_Cass_Iterator* iterator)
     {
         if (!iterator)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_get_index_meta");
             return NULL;
         }
         return  cass_iterator_get_index_meta(iterator);

     }

     w_Cass_Error w_cassandra_iterator_get_meta_field_name(const w_Cass_Iterator* iterator,const char** name, size_t* name_length)
     {
         if (!iterator)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_get_meta_field_name");
             return -1;
         }

         return  cass_iterator_get_meta_field_name(iterator, name, name_length);

     }

     const w_Cass_Value*   w_cassandra_iterator_get_meta_field_value(const w_Cass_Iterator* iterator)
     {
         if (!iterator)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_iterator_get_meta_field_value");
             return NULL;
         }
         return  cass_iterator_get_meta_field_value(iterator);

     }

     const w_Cass_Value*  w_cassandra_row_get_column(const w_Cass_Row* row,size_t index)
     {
         if (!row)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_row_get_column");
             return NULL;
         }
         return  cass_row_get_column(row, index);

     }

     const w_Cass_Value* w_cassandra_row_get_column_by_name(const w_Cass_Row* row, const char* name)
     {
         if (!row || !name )
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandraandra_row_get_column_by_name");
             return NULL;
         }
         return   cass_row_get_column_by_name(row, name);

     }

     const w_Cass_Value* w_cassandra_row_get_column_by_name_n(const w_Cass_Row* row, const char* name, size_t name_length)
     {
         if (!row || !name)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_row_get_column_by_name_n");
             return NULL;
         }
         return  cass_row_get_column_by_name_n(row, name, name_length);

     }

     const w_Cass_Data_Type* w_cassandra_value_data_type(const w_Cass_Value* value)
     {
         if (!value)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_value_data_type");
             return NULL;
         }
         return   cass_value_data_type(value);

     }

     w_Cass_Error  w_cassandra_value_get_int8(const w_Cass_Value* value, int8_t* output)
     {
         if (!value)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_value_get_int8");
             return -1;
         }
         return  cass_value_get_int8(value, output);

     }

     w_Cass_Error  w_cassandra_value_get_int16(const w_Cass_Value* value, int16_t* output)
     {
         if (!value)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_value_get_int16");
             return -1;
         }
         return  cass_value_get_int16(value, output);

     }

     w_Cass_Error  w_cassandra_value_get_int32(const w_Cass_Value* value, int32_t* output)
     {
         if (!value)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_value_get_int32");
             return -1;
         }
         return  cass_value_get_int32(value, output);

     }

     w_Cass_Error w_cassandra_value_get_uint32(const w_Cass_Value* value,  uint32_t* output)
     {
         if (!value)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_value_get_uint32");
             return -1;
         }
         return  cass_value_get_uint32(value, output);

     }


     w_Cass_Error  w_cassandra_value_get_int64(const w_Cass_Value* value, int64_t* output)
     {
         if (!value)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_value_get_int64");
             return -1;
         }
         return   cass_value_get_int64(value, output);

     }


     w_Cass_Error w_cassandra_value_get_float(const w_Cass_Value* value,  float* output)
     {
         if (!value)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_value_get_float");
             return -1;
         }
         return   cass_value_get_float(value, output);

     }

     w_Cass_Error w_cassandra_value_get_double(const w_Cass_Value* value,double* output)
     {
         if (!value)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_value_get_double");
             return -1;
         }
         return  cass_value_get_double(value, output);

     }

     w_Cass_Error  w_cassandra_value_get_bool(const w_Cass_Value* value, w_bool_t* output)
     {
         if (!value)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_value_get_bool");
             return -1;
         }
         return  cass_value_get_bool(value, output);

     }

     w_Cass_Error   w_cassandra_value_get_uuid(const w_Cass_Value* value,  w_CassUuid* output)
     {
         if (!value || !output)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_value_get_uuid");
             return -1;
         }
         return cass_value_get_uuid(value, output);

     }

     w_Cass_Error w_cassandra_value_get_inet(const w_Cass_Value* value, w_CassInet* output)
     {
         if (!value || !output)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_value_get_inet");
             return -1;
         }
         return  cass_value_get_inet(value, output);

     }

     w_Cass_Error  w_cassandra_value_get_string(const w_Cass_Value* value, const char** output, size_t* output_size)
     {
         if (!value || !output)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_value_get_string");
             return -1;
         }
         return   cass_value_get_string(value, output, output_size);

     }

     w_Cass_Error w_cassandra_value_get_bytes(const w_Cass_Value* value,const w_byte_t** output, size_t* output_size)
     {
         if (!value )
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_value_get_bytes");
             return -1;
         }
         return   cass_value_get_bytes(value, output, output_size);

     }

     w_Cass_Error  w_cassandra_value_get_decimal(const w_Cass_Value* value, const w_byte_t** varint,size_t* varint_size,int32_t* scale)
     {
         if (!value)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_value_get_decimal");
             return -1;
         }
         return  cass_value_get_decimal(value, varint, varint_size, scale);

     }

     w_Cass_Error  w_cassandra_value_get_duration(const w_Cass_Value* value, int32_t* months, int32_t* days, int64_t* nanos)
     {
         if (!value)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_value_get_duration");
             return -1;
         }
         return  cass_value_get_duration(value, months, days, nanos);

     }

     w_Cass_Value_Type w_cassandra_value_type(const w_Cass_Value* value)
     {
         if (!value)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_value_type");
             return -1;
         }
         return     cass_value_type(value);

     }

     w_bool_t w_cassandra_value_is_null(const w_Cass_Value* value)
     {
         if (!value)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_value_is_null");
             return false;
         }
         return  cass_value_is_null(value);

     }
     w_bool_t w_cassandra_value_is_collection(const w_Cass_Value* value)
     {
         if (!value)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_value_is_collection");
             return false;
         }
         return    cass_value_is_collection(value);

     }
     w_bool_t w_cassandra_value_is_duration(const w_Cass_Value* value)
     {
         if (!value)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_value_is_duration");
             return false;
         }
         return  cass_value_is_duration(value);

     }

     size_t  w_cassandra_value_item_count(const w_Cass_Value* collection)
     {
         if (!collection)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_value_item_count");
             return -1;
         }
         return   cass_value_item_count(collection);

     }

     w_Cass_Value_Type  w_cassandra_value_primary_sub_type(const w_Cass_Value* collection)
     {
         if (!collection)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_value_primary_sub_type");
             return -1;
         }
         return   cass_value_primary_sub_type(collection);

     }

     w_Cass_Value_Type w_cassandra_value_secondary_sub_type(const w_Cass_Value* collection)
     {
         if (!collection)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_value_secondary_sub_type");
             return -1;
         }
            return  cass_value_secondary_sub_type( collection);
     }

     w_Cass_Uuid_Gen*  w_cassandra_uuid_gen_new()
     {
         return   cass_uuid_gen_new();

     }

     w_Cass_Uuid_Gen* w_cassandra_uuid_gen_new_with_node(uint64_t node)
     {
        
         return  cass_uuid_gen_new_with_node(node);

     }

     void  w_cassandra_uuid_gen_free(w_Cass_Uuid_Gen* uuid_gen)
      {
         if (!uuid_gen)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_uuid_gen_free");
         }
         cass_uuid_gen_free(uuid_gen);

      }

     void w_cassandra_uuid_gen_time(w_Cass_Uuid_Gen* uuid_gen, w_CassUuid* output)
     {
         if (!uuid_gen || !output)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_uuid_gen_time");
         }
           cass_uuid_gen_time(uuid_gen, output);

     }

     void  w_cassandra_uuid_gen_random(w_Cass_Uuid_Gen* uuid_gen,  w_CassUuid* output)
     {
         if (!uuid_gen || !output)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_uuid_gen_random");
         }
         cass_uuid_gen_random(uuid_gen, output);

     }

     void  w_cassandra_uuid_min_from_time(uint64_t time, w_CassUuid* output)
     {
         if (!time || !output)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_uuid_min_from_time");
         }
         cass_uuid_min_from_time(time, output);

     }

     void w_cassandra_uuid_max_from_time(uint64_t time, w_CassUuid* output)
     {
         if (!time || !output)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_uuid_max_from_time");
         }
         cass_uuid_max_from_time(time, output);

     }


     uint64_t  w_cassandra_uuid_timestamp(w_CassUuid uuid)
     {
         
         return   cass_uuid_timestamp(uuid);

     }

     uint8_t w_cassandra_uuid_version(w_CassUuid uuid)
     {
       return  cass_uuid_version( uuid);
     }

     void w_cassandra_uuid_string(w_CassUuid uuid,char* output)
     {
         

         cass_uuid_string(uuid, output);

     }

     w_Cass_Error w_cassandra_uuid_from_string(const char* str, w_CassUuid* output)
     {
         if (!str || !output)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_uuid_from_string");
             return -1;
         }
         
         return cass_uuid_from_string(str, output);

     }

     w_Cass_Error w_cassandra_uuid_from_string_n(const char* str, size_t str_length, w_CassUuid* output)
     {
         if (!str || !output)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_uuid_from_string_n");
             return -1;
         }
         return   cass_uuid_from_string_n(str, str_length, output);

     }

     w_Cass_Time_stamp_Gen*  w_cassandra_timestamp_gen_server_side_new()
     {
         
         return cass_timestamp_gen_server_side_new  ();
     }

     w_Cass_Time_stamp_Gen* w_cassandra_timestamp_gen_monotonic_new()
     {
         return cass_timestamp_gen_monotonic_new();
     }

     w_Cass_Time_stamp_Gen* w_cassandra_timestamp_gen_monotonic_new_with_settings(int64_t warning_threshold_us, int64_t warning_interval_ms)
     {
        

         return   cass_timestamp_gen_monotonic_new_with_settings( warning_threshold_us, warning_interval_ms);
     }

     void w_cassandra_timestamp_gen_free(w_Cass_Time_stamp_Gen* timestamp_gen)
     {
         if (!timestamp_gen)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_timestamp_gen_free");
             
         }
         cass_timestamp_gen_free(timestamp_gen);

     }

     w_Cass_Retry_Policy* w_cassandra_retry_policy_default_new()
     {

       return   cass_retry_policy_default_new();
     }

   /* w_Cass_Retry_Policy*
         w_cassandra_retry_policy_downgrading_consistency_new()
     {
         return cass_retry_policy_downgrading_consistency_new();
         
     }*/

     w_Cass_Retry_Policy*  w_cassandra_retry_policy_fallthrough_new()
     {
         return  cass_retry_policy_fallthrough_new();

     }

     w_Cass_Retry_Policy* w_cassandra_retry_policy_logging_new(w_Cass_Retry_Policy* child_retry_policy)
     {
         if (!child_retry_policy)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_retry_policy_logging_new");
             return NULL;
         }
         return    cass_retry_policy_logging_new( child_retry_policy);
     }

     void  w_cassandra_retry_policy_free(w_Cass_Retry_Policy* policy)
     {
         if (!policy)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_retry_policy_free");
           
         }
        cass_retry_policy_free(  policy);
     }

     w_Cass_Custom_Payload*  w_cassandra_custom_payload_new()
     {

         return  cass_custom_payload_new();

     }

     void w_cassandra_custom_payload_free(w_Cass_Custom_Payload* payload)
     {
         if (!payload)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_custom_payload_free");

         }
         cass_custom_payload_free(payload);

     }

     void  w_cassandra_custom_payload_set(w_Cass_Custom_Payload* payload,  const char* name, const w_byte_t* value, size_t value_size)
     {
         if (!payload || !name)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_custom_payload_set");

         }
         cass_custom_payload_set(payload, name, value, value_size);

     }

     void  w_cassandra_custom_payload_set_n(w_Cass_Custom_Payload* payload, const char* name,  size_t name_length, const w_byte_t* value, size_t value_size)
     {
         if (!payload || !name)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_custom_payload_set_n");

         }
         cass_custom_payload_set_n(payload, name, name_length, value, value_size);

     }

     void w_cassandra_custom_payload_remove(w_Cass_Custom_Payload* payload,const char* name)
     {
         if (!payload || !name)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_custom_payload_remove");

         }
             cass_custom_payload_remove( payload,  name);

     }

     void  w_cassandra_custom_payload_remove_n(w_Cass_Custom_Payload* payload, const char* name,size_t name_length)
     {
         if (!payload || !name)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_custom_payload_remove_n");

         }
         cass_custom_payload_remove_n(payload, name, name_length);

     }

     const char*  w_cassandraandra_consistency_string(W_Cass_Consistency consistency)
     {
         
         return cass_consistency_string(consistency);

     }

     const char*  w_cassandra_write_type_string(w_Cass_WriteType write_type)
     {
         return  cass_write_type_string(write_type);

     }

     const char* w_cassandra_error_desc(w_Cass_Error error)
     {
         
             return cass_error_desc( error);
     }
     /*void
         w_cassandra_log_cleanup()
     {
         cass_log_cleanup();
     }*/

     void w_cassandra_log_set_level(w_Cass_Log_Level log_level)
     {
         cass_log_set_level( log_level);
     }


     void w_cassandra_log_set_callback(w_Cass_Log_Callback callback, void* data)
     {
            cass_log_set_callback((CassLogCallback)callback, data);
     }

    /* void
         w_cassandra_log_set_queue_size(size_t queue_size)
     {
         cass_log_set_queue_size( queue_size);
     }*/

     const char* w_cassandra_log_level_string(w_Cass_Log_Level log_level)
     {

      return cass_log_level_string( log_level);

     }

     w_CassInet  w_cassandra_inet_init_v4(const uint8_t* address)
     {
         if (!address)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_inet_init_v4");
             return;
         }
         return  cass_inet_init_v4(  address);
     }
    
     w_CassInet  w_cassandra_inet_init_v6(const uint8_t* address)
     {
         if (!address)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_inet_init_v6");
             return;
         }
         return    cass_inet_init_v6(address);

     }

     void w_cassandra_inet_string(w_CassInet inet, char* output)
     {
         if (!output)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_inet_string");
         }
             cass_inet_string( inet,output);
     }


     w_Cass_Error  w_cassandra_inet_from_string(const char* str,w_CassInet* output)
     {
         if (!str || !output)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_inet_from_string");
             return  -1;
         }
         return  cass_inet_from_string (str, output);

     }

     w_Cass_Error  w_cassandra_inet_from_string_n(const char* str, size_t str_length, w_CassInet* output)
     {
         if (!str || !output)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_inet_from_string_n");
             return  -1;
         }
         return   cass_inet_from_string_n(str, str_length, output);

     }

     uint32_t  w_cassandra_date_from_epoch(int64_t epoch_secs)
     {
         if (!epoch_secs)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_date_from_epoch");
             return  0;
         }
         return    cass_date_from_epoch(epoch_secs);

     }

     int64_t  w_cassandra_time_from_epoch(int64_t epoch_secs)
     {
         if (!epoch_secs)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_time_from_epoch");
             return  -1;
         }
         return    cass_time_from_epoch(epoch_secs);

     }


     int64_t  w_cassandra_date_time_to_epoch(uint32_t date, int64_t time)
     {
         if (!date || !time)
         {
             W_ASSERT(false, "missing parameters!. trace info: w_cassandra_date_time_to_epoch");
             return  -1;
         }
         return cass_date_time_to_epoch( date, time);
     }

     void  w_cassandra_alloc_set_functions(CassMallocFunction malloc_func, CassReallocFunction realloc_func,CassFreeFunction free_func)
     {
         cass_alloc_set_functions(malloc_func, realloc_func, free_func);

     }





























