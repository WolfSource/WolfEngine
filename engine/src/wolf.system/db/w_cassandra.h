
/*
  Copyright (c) DataStax, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif



#include"wolf.h"


    /**
     * @file include/cassandra.h
     *
     * C/C++ driver for Apache Cassandra. Uses the Cassandra Query Language versions 3
     * over the Cassandra Binary Protocol (versions 1, 2, or 3).
     */



#ifdef __cplusplus
    extern "C" {
#endif

        typedef struct CassSpeculativeExecutionMetrics_ w_cass_speculative_execution_metrics;
        typedef struct CassUuid_ w_cass_uuid;
        typedef struct CassInet_ w_cass_inet;
        typedef struct CassVersion_ w_cass_version;
        typedef struct CassMetrics_  w_cass_metrics;
        typedef struct  CassLogMessage_* w_cass_log_message;
        typedef struct CassAuthenticatorCallbacks_* w_cass_authenticator_callbacks;
        typedef enum { w_false = 0, w_true = 1 } w_bool_t;






#define CASS_UINT64_MAX 18446744073709551615ULL

        typedef uint8_t w_byte_t;
        typedef uint64_t w_duration_t;

        /**
         * The size of an IPv4 address
         */
#define CASS_INET_V4_LENGTH 4

         /**
          * The size of an IPv6 address
          */
#define CASS_INET_V6_LENGTH 16

          /**
           * The size of an inet string including a null terminator.
           */
#define CASS_INET_STRING_LENGTH 46



           /**
            * The size of a hexadecimal UUID string including a null terminator.
            */
#define CASS_UUID_STRING_LENGTH 37

            /**
             * Version 1 (time-based) or version 4 (random) UUID.
             *
             * @struct w_cass_uuid
             */


             /**
              * A cluster object describes the configuration of the Cassandra cluster and is used
              * to construct a session instance. Unlike other DataStax drivers the cluster object
              * does not maintain the control connection.
              *
              * @struct CassCluster
              */
        typedef struct CassCluster_ w_cass_cluster;

        /**
         * A session object is used to execute queries and maintains cluster state through
         * the control connection. The control connection is used to auto-discover nodes and
         * monitor cluster changes (topology and schema). Each session also maintains multiple
         * pools of connections to cluster nodes which are used to query the cluster.
         *
         * Instances of the session object are thread-safe to execute queries.
         *
         * @struct w_Cass_Session
         */
        typedef struct CassSession_  w_cass_session;

        /**
         * A statement object is an executable query. It represents either a regular
         * (adhoc) statement or a prepared statement. It maintains the queries' parameter
         * values along with query options (consistency level, paging state, etc.)
         *
         * <b>Note:</b> Parameters for regular queries are not supported by the binary protocol
         * version 1.
         *
         * @struct w_Cass_Statement
         */
        typedef struct CassStatement_  w_cass_statement;

        /**
         * A group of statements that are executed as a single batch.
         *
         * <b>Note:</b> Batches are not supported by the binary protocol version 1.
         *
         * @cassandra{2.0+}
         *
         * @struct w_Cass_Batch
         */
        typedef struct CassBatch_  w_cass_batch;

        /**
         * The future result of an operation.
         *
         * It can represent a result if the operation completed successfully or an
         * error if the operation failed. It can be waited on, polled or a callback
         * can be attached.
         *
         * @struct CassFuture
         */
        typedef struct CassFuture_ w_cass_future;

        /**
         * A statement that has been prepared cluster-side (It has been pre-parsed
         * and cached).
         *
         * A prepared statement is read-only and it is thread-safe to concurrently
         * bind new statements.
         *
         * @struct w_Cass_Prepared
         */
        typedef struct CassPrepared_ w_cass_prepared;

        /**
         * The result of a query.
         *
         * A result object is read-only and is thread-safe to read or iterate over
         * concurrently.
         *
         * @struct w_Cass_Result
         */
        typedef struct CassResult_ w_cass_result;

        /**
         * A error result of a request
         *
         * @struct w_Cass_Error_Result
         */
        typedef struct CassErrorResult_ w_cass_error_result;

        /**
         * An object used to iterate over a group of rows, columns or collection values.
         *
         * @struct w_Cass_Iterator
         */
        typedef struct CassIterator_ w_cass_iterator;

        /**
         * A collection of column values.
         *
         * @struct w_Cass_Row
         */
        typedef struct CassRow_  w_cass_row;

        /**
         * A single primitive value or a collection of values.
         *
         * @struct w_Cass_Value
         */
        typedef struct CassValue_  w_cass_value;

        /**
         * A data type used to describe a value, collection or
         * user defined type.
         *
         * @struct w_Cass_Data_Type
         */
        typedef struct CassDataType_ w_cass_data_type;

        /**
         * @struct w_Cass_Function_Meta
         *
         * @cassandra{2.2+}
         */
        typedef struct CassFunctionMeta_ w_cass_function_meta;

        /**
         * @struct w_Cass_Aggregate_Meta
         *
         * @cassandra{2.2+}
         */
        typedef struct CassAggregateMeta_ w_cass_aggregate_meta;

        /**
         *  A collection of values.
         *
         * @struct w_Cass_Collection
         */
        typedef struct CassCollection_ w_cass_collection;

        /**
         * A tuple of values.
         *
         * @struct w_Cass_Tuple
         *
         * @cassandra{2.1+}
         */
        typedef struct CassTuple_ w_cass_tuple;

        /**
         * A user defined type.
         *
         * @struct w_Cass_User_Type
         *
         * @cassandra{2.1+}
         */
        typedef struct CassUserType_ w_cass_user_type;

        /**
         * Describes the SSL configuration of a cluster.
         *
         * @struct w_Cass_Ssl
         */
        typedef struct CassSsl_ w_cass_ssl;



        /**
         * A snapshot of the schema's metadata.
         *
         * @struct w_Cass_Schema_Meta
         */
        typedef struct CassSchemaMeta_  w_cass_schema_meta;

        /**
         * Keyspace metadata
         *
         * @struct w_Cass_Key_space_Meta
         */
        typedef struct CassKeyspaceMeta_ w_cass_key_space_meta;

        /**
         * Table metadata
         *
         * @struct w_Cass_Table_Meta
         */
        typedef struct CassTableMeta_ w_cass_table_meta;

        /**
         * MaterializedView metadata
         *
         * @struct w_Cass_Materialized_View_Meta
         *
         * @cassandra{3.0+}
         */
        typedef struct CassMaterializedViewMeta_  w_cass_materialized_view_meta;

        /**
         * Column metadata
         *
         * @struct w_Cass_Column_Meta
         */
        typedef struct CassColumnMeta_  w_cass_column_meta;

        /**
         * Index metadata
         *
         * @struct w_Cass_Index_Meta
         */
        typedef struct CassIndexMeta_ w_cass_index_meta;

        /**
         * A UUID generator object.
         *
         * Instances of the UUID generator object are thread-safe to generate UUIDs.
         *
         * @struct w_Cass_Uuid_Gen
         */
        typedef struct CassUuidGen_ w_cass_uuid_gen;

        /**
         * Policies that defined the behavior of a request when a server-side
         * read/write timeout or unavailable error occurs.
         *
         * Generators of client-side, microsecond-precision timestamps.
         *
         * @struct w_Cass_Time_stamp_Gen
         *
         * @cassandra{2.1+}
         */
        typedef struct CassTimestampGen_ w_cass_time_stamp_gen;

        /**
         * @struct w_Cass_Retry_Policy
         */
        typedef struct CassRetryPolicy_ w_cass_retry_policy;

        /**
         * @struct w_Cass_Custom_Payload
         *
         * @cassandra{2.2+}
         */
        typedef struct CassCustomPayload_ w_cass_custom_payload;




        typedef enum {
            W_CASS_CONSISTENCY_UNKNOWN = 0xFFFF,
            W_CASS_CONSISTENCY_ANY = 0x0000,
            W_CASS_CONSISTENCY_ONE = 0x0001,
            W_CASS_CONSISTENCY_TWO = 0x0002,
            W_CASS_CONSISTENCY_THREE = 0x0003,
            W_CASS_CONSISTENCY_QUORUM = 0x0004,
            W_CASS_CONSISTENCY_ALL = 0x0005,
            W_CASS_CONSISTENCY_LOCAL_QUORUM = 0x0006,
            W_CASS_CONSISTENCY_EACH_QUORUM = 0x0007,
            W_CASS_CONSISTENCY_SERIAL = 0x0008,
            W_CASS_CONSISTENCY_LOCAL_SERIAL = 0x0009,
            W_CASS_CONSISTENCY_LOCAL_ONE = 0x000A
        } w_cass_consistency;

#define CASS_CONSISTENCY_MAPPING(XX) \
  XX(CASS_CONSISTENCY_UNKNOWN, "UNKNOWN") \
  XX(CASS_CONSISTENCY_ANY, "ANY") \
  XX(CASS_CONSISTENCY_ONE, "ONE") \
  XX(CASS_CONSISTENCY_TWO, "TWO") \
  XX(CASS_CONSISTENCY_THREE, "THREE") \
  XX(CASS_CONSISTENCY_QUORUM, "QUORUM") \
  XX(CASS_CONSISTENCY_ALL, "ALL") \
  XX(CASS_CONSISTENCY_LOCAL_QUORUM, "LOCAL_QUORUM") \
  XX(CASS_CONSISTENCY_EACH_QUORUM, "EACH_QUORUM") \
  XX(CASS_CONSISTENCY_SERIAL, "SERIAL") \
  XX(CASS_CONSISTENCY_LOCAL_SERIAL, "LOCAL_SERIAL") \
  XX(CASS_CONSISTENCY_LOCAL_ONE, "LOCAL_ONE")

        /* @cond IGNORE */
#define CASS_CONSISTENCY_MAP CASS_CONSISTENCY_MAPPING /* Deprecated */
/* @endcond */

        typedef enum {
            W_CASS_WRITE_TYPE_UNKNOWN,
            W_CASS_WRITE_TYPE_SIMPLE,
            W_CASS_WRITE_TYPE_BATCH,
            W_CASS_WRITE_TYPE_UNLOGGED_BATCH,
            W_CASS_WRITE_TYPE_COUNTER,
            W_CASS_WRITE_TYPE_BATCH_LOG,
            W_CASS_WRITE_TYPE_CAS,
            W_CASS_WRITE_TYPE_VIEW,
            W_CASS_WRITE_TYPE_CDC
        } w_cass_writetype;

#define CASS_WRITE_TYPE_MAPPING(XX) \
  XX(CASS_WRITE_TYPE_SIMPLE, "SIMPLE") \
  XX(CASS_WRITE_TYPE_BATCH, "BATCH") \
  XX(CASS_WRITE_TYPE_UNLOGGED_BATCH, "UNLOGGED_BATCH") \
  XX(CASS_WRITE_TYPE_COUNTER, "COUNTER") \
  XX(CASS_WRITE_TYPE_BATCH_LOG, "BATCH_LOG") \
  XX(CASS_WRITE_TYPE_CAS, "CAS") \
  XX(CASS_WRITE_TYPE_VIEW, "VIEW") \
  XX(CASS_WRITE_TYPE_CDC, "CDC")

        /* @cond IGNORE */
#define CASS_WRITE_TYPE_MAP CASS_WRITE_TYPE_MAPPING /* Deprecated */
/* @endcond */

        typedef enum {
            W_CASS_COLUMN_TYPE_REGULAR,
            W_CASS_COLUMN_TYPE_PARTITION_KEY,
            W_CASS_COLUMN_TYPE_CLUSTERING_KEY,
            W_CASS_COLUMN_TYPE_STATIC,
            W_CASS_COLUMN_TYPE_COMPACT_VALUE
        } w_cass_column_type;

        typedef enum {
            W_CASS_INDEX_TYPE_UNKNOWN,
            W_CASS_INDEX_TYPE_KEYS,
            W_CASS_INDEX_TYPE_CUSTOM,
            W_CASS_INDEX_TYPE_COMPOSITES
        } w_cass_index_type;

#define W_CASS_VALUE_TYPE_MAPPING(XX) \
  XX(W_CASS_VALUE_TYPE_CUSTOM,  0x0000, "", "") \
  XX(W_CASS_VALUE_TYPE_ASCII,  0x0001, "ascii", "org.apache.cassandra.db.marshal.AsciiType") \
  XX(W_CASS_VALUE_TYPE_BIGINT,  0x0002, "bigint", "org.apache.cassandra.db.marshal.LongType") \
  XX(W_CASS_VALUE_TYPE_BLOB,  0x0003, "blob", "org.apache.cassandra.db.marshal.BytesType") \
  XX(W_CASS_VALUE_TYPE_BOOLEAN,  0x0004, "boolean", "org.apache.cassandra.db.marshal.BooleanType") \
  XX(W_CASS_VALUE_TYPE_COUNTER,  0x0005, "counter", "org.apache.cassandra.db.marshal.CounterColumnType") \
  XX(W_CASS_VALUE_TYPE_DECIMAL,  0x0006, "decimal", "org.apache.cassandra.db.marshal.DecimalType") \
  XX(W_CASS_VALUE_TYPE_DOUBLE,  0x0007, "double", "org.apache.cassandra.db.marshal.DoubleType") \
  XX(W_CASS_VALUE_TYPE_FLOAT,  0x0008, "float", "org.apache.cassandra.db.marshal.FloatType") \
  XX(W_CASS_VALUE_TYPE_INT,  0x0009, "int", "org.apache.cassandra.db.marshal.Int32Type") \
  XX(W_CASS_VALUE_TYPE_TEXT,  0x000A, "text", "org.apache.cassandra.db.marshal.UTF8Type") \
  XX(W_CASS_VALUE_TYPE_TIMESTAMP,  0x000B, "timestamp", "org.apache.cassandra.db.marshal.TimestampType") \
  XX(W_CASS_VALUE_TYPE_UUID,  0x000C, "uuid", "org.apache.cassandra.db.marshal.UUIDType") \
  XX(W_CASS_VALUE_TYPE_VARCHAR,  0x000D, "varchar", "") \
  XX(W_CASS_VALUE_TYPE_VARINT,  0x000E, "varint", "org.apache.cassandra.db.marshal.IntegerType") \
  XX(W_CASS_VALUE_TYPE_TIMEUUID,  0x000F, "timeuuid", "org.apache.cassandra.db.marshal.TimeUUIDType") \
  XX(W_CASS_VALUE_TYPE_INET,  0x0010, "inet", "org.apache.cassandra.db.marshal.InetAddressType") \
  XX(W_CASS_VALUE_TYPE_DATE,  0x0011, "date", "org.apache.cassandra.db.marshal.SimpleDateType") \
  XX(W_CASS_VALUE_TYPE_TIME,  0x0012, "time", "org.apache.cassandra.db.marshal.TimeType") \
  XX(W_CASS_VALUE_TYPE_SMALL_INT,  0x0013, "smallint", "org.apache.cassandra.db.marshal.ShortType") \
  XX(W_CASS_VALUE_TYPE_TINY_INT,  0x0014, "tinyint", "org.apache.cassandra.db.marshal.ByteType") \
  XX(W_CASS_VALUE_TYPE_DURATION,  0x0015, "duration", "org.apache.cassandra.db.marshal.DurationType") \
  XX(W_CASS_VALUE_TYPE_LIST,  0x0020, "list", "org.apache.cassandra.db.marshal.ListType") \
  XX(W_CASS_VALUE_TYPE_MAP,  0x0021, "map", "org.apache.cassandra.db.marshal.MapType") \
  XX(W_CASS_VALUE_TYPE_SET,  0x0022, "set", "org.apache.cassandra.db.marshal.SetType") \
  XX(W_CASS_VALUE_TYPE_UDT,  0x0030, "", "") \
  XX(W_CASS_VALUE_TYPE_TUPLE,  0x0031, "tuple", "org.apache.cassandra.db.marshal.TupleType")

        typedef enum {
            W_CASS_VALUE_TYPE_UNKNOWN = 0xFFFF,
#define XX_VALUE_TYPE(name, type, cql, klass) name = type,
            W_CASS_VALUE_TYPE_MAPPING(XX_VALUE_TYPE)
#undef XX_VALUE_TYPE
            /* @cond IGNORE */
            W_CASS_VALUE_TYPE_LAST_ENTRY
            /* @endcond */
        } w_cass_value_type;

        typedef enum {
            W_CASS_CLUSTERING_ORDER_NONE,
            W_CASS_CLUSTERING_ORDER_ASC,
            W_CASS_CLUSTERING_ORDER_DESC
        } w_cass_clustering_order;

        typedef enum {
            W_CASS_COLLECTION_TYPE_LIST = W_CASS_VALUE_TYPE_LIST,
            W_CASS_COLLECTION_TYPE_MAP = W_CASS_VALUE_TYPE_MAP,
            W_CASS_COLLECTION_TYPE_SET = W_CASS_VALUE_TYPE_SET
        } w_cass_collection_type;

        typedef enum {
            W_CASS_BATCH_TYPE_LOGGED = 0x00,
            W_CASS_BATCH_TYPE_UNLOGGED = 0x01,
            W_CASS_BATCH_TYPE_COUNTER = 0x02
        } w_cass_batch_type;

        typedef enum {
            W_CASS_ITERATOR_TYPE_RESULT,
            W_CASS_ITERATOR_TYPE_ROW,
            W_CASS_ITERATOR_TYPE_COLLECTION,
            W_CASS_ITERATOR_TYPE_MAP,
            W_CASS_ITERATOR_TYPE_TUPLE,
            W_CASS_ITERATOR_TYPE_USER_TYPE_FIELD,
            W_CASS_ITERATOR_TYPE_META_FIELD,
            W_CASS_ITERATOR_TYPE_KEYSPACE_META,
            W_CASS_ITERATOR_TYPE_TABLE_META,
            W_CASS_ITERATOR_TYPE_TYPE_META,
            W_CASS_ITERATOR_TYPE_FUNCTION_META,
            W_CASS_ITERATOR_TYPE_AGGREGATE_META,
            W_CASS_ITERATOR_TYPE_COLUMN_META,
            W_CASS_ITERATOR_TYPE_INDEX_META,
            W_CASS_ITERATOR_TYPE_MATERIALIZED_VIEW_META
        } w_cass_iterator_type;

#define W_CASS_LOG_LEVEL_MAPPING(XX) \
  XX(W_CASS_LOG_DISABLED, "") \
  XX(W_CASS_LOG_CRITICAL, "CRITICAL") \
  XX(W_CASS_LOG_ERROR, "ERROR") \
  XX(W_CASS_LOG_WARN, "WARN") \
  XX(W_CASS_LOG_INFO, "INFO") \
  XX(W_CASS_LOG_DEBUG, "DEBUG") \
  XX(W_CASS_LOG_TRACE, "TRACE")

        /* @cond IGNORE */
#define W_CASS_LOG_LEVEL_MAP W_CASS_LOG_LEVEL_MAPPING /* Deprecated */
/* @endcond */

        typedef enum {
#define XX_LOG(log_level, _) log_level,
            W_CASS_LOG_LEVEL_MAPPING(XX_LOG)
#undef XX_LOG
            /* @cond IGNORE */
            W_CASS_LOG_LAST_ENTRY
            /* @endcond */
        } w_cass_log_level;

        typedef enum {
            W_CASS_SSL_VERIFY_NONE = 0x00,
            W_CASS_SSL_VERIFY_PEER_CERT = 0x01,
            W_CASS_SSL_VERIFY_PEER_IDENTITY = 0x02,
            W_CASS_SSL_VERIFY_PEER_IDENTITY_DNS = 0x04
        } w_cass_ssl_verify_flags;

        typedef enum  {
            W_CASS_PROTOCOL_VERSION_V1 = 0x01, /**< Deprecated */
            W_CASS_PROTOCOL_VERSION_V2 = 0x02, /**< Deprecated */
            W_CASS_PROTOCOL_VERSION_V3 = 0x03,
            W_CASS_PROTOCOL_VERSION_V4 = 0x04,
            W_CASS_PROTOCOL_VERSION_V5 = 0x05,
            W_CASS_PROTOCOL_VERSION_DSEV1 = 0x41, /**< Only supported when using the DSE
                                                     driver with DataStax Enterprise */
                                                     W_CASS_PROTOCOL_VERSION_DSEV2 = 0x42  /**< Only supported when using the DSE
                                                                                              driver with DataStax Enterprise */
        } W_cass_protocol_version;

        typedef enum {
            W_CASS_ERROR_SOURCE_NONE,
            W_CASS_ERROR_SOURCE_LIB,
            W_CASS_ERROR_SOURCE_SERVER,
            W_CASS_ERROR_SOURCE_SSL,
            W_CASS_ERROR_SOURCE_COMPRESSION
        } w_cass_error_source;

#define W_CASS_ERROR_MAPPING(XX) \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_BAD_PARAMS, 1, "Bad parameters") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_NO_STREAMS, 2, "No streams available") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_UNABLE_TO_INIT, 3, "Unable to initialize") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_MESSAGE_ENCODE, 4, "Unable to encode message") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_HOST_RESOLUTION, 5, "Unable to resolve host") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_UNEXPECTED_RESPONSE, 6, "Unexpected response from server") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_REQUEST_QUEUE_FULL, 7, "The request queue is full") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_NO_AVAILABLE_IO_THREAD, 8, "No available IO threads") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_WRITE_ERROR, 9, "Write error") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_NO_HOSTS_AVAILABLE, 10, "No hosts available") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_INDEX_OUT_OF_BOUNDS, 11, "Index out of bounds") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_INVALID_ITEM_COUNT, 12, "Invalid item count") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_INVALID_VALUE_TYPE, 13, "Invalid value type") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_REQUEST_TIMED_OUT, 14, "Request timed out") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_UNABLE_TO_SET_KEYSPACE, 15, "Unable to set keyspace") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_CALLBACK_ALREADY_SET, 16, "Callback already set") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_INVALID_STATEMENT_TYPE, 17, "Invalid statement type") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_NAME_DOES_NOT_EXIST, 18, "No value or column for name") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_UNABLE_TO_DETERMINE_PROTOCOL, 19, "Unable to find supported protocol version") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_NULL_VALUE, 20, "NULL value specified") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_NOT_IMPLEMENTED, 21, "Not implemented") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_UNABLE_TO_CONNECT, 22, "Unable to connect") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_UNABLE_TO_CLOSE, 23, "Unable to close") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_NO_PAGING_STATE, 24, "No paging state") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_PARAMETER_UNSET, 25, "Parameter unset") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_INVALID_ERROR_RESULT_TYPE, 26, "Invalid error result type") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_INVALID_FUTURE_TYPE, 27, "Invalid future type") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_INTERNAL_ERROR, 28, "Internal error") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_INVALID_CUSTOM_TYPE, 29, "Invalid custom type") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_INVALID_DATA, 30, "Invalid data") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_NOT_ENOUGH_DATA, 31, "Not enough data") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_INVALID_STATE, 32, "Invalid state") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_NO_CUSTOM_PAYLOAD, 33, "No custom payload") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_EXECUTION_PROFILE_INVALID, 34, "Invalid execution profile specified") \
  XX(W_CASS_ERROR_SOURCE_LIB, W_CASS_ERROR_LIB_NO_TRACING_ID, 35, "No tracing ID") \
  XX(W_CASS_ERROR_SOURCE_SERVER, W_CASS_ERROR_SERVER_SERVER_ERROR, 0x0000, "Server error") \
  XX(W_CASS_ERROR_SOURCE_SERVER, W_CASS_ERROR_SERVER_PROTOCOL_ERROR, 0x000A, "Protocol error") \
  XX(W_CASS_ERROR_SOURCE_SERVER, W_CASS_ERROR_SERVER_BAD_CREDENTIALS, 0x0100, "Bad credentials") \
  XX(W_CASS_ERROR_SOURCE_SERVER, W_CASS_ERROR_SERVER_UNAVAILABLE, 0x1000, "Unavailable") \
  XX(W_CASS_ERROR_SOURCE_SERVER, W_CASS_ERROR_SERVER_OVERLOADED, 0x1001, "Overloaded") \
  XX(W_CASS_ERROR_SOURCE_SERVER, W_CASS_ERROR_SERVER_IS_BOOTSTRAPPING, 0x1002, "Is bootstrapping") \
  XX(W_CASS_ERROR_SOURCE_SERVER, W_CASS_ERROR_SERVER_TRUNCATE_ERROR, 0x1003, "Truncate error") \
  XX(W_CASS_ERROR_SOURCE_SERVER, W_CASS_ERROR_SERVER_WRITE_TIMEOUT, 0x1100, "Write timeout") \
  XX(W_CASS_ERROR_SOURCE_SERVER, W_CASS_ERROR_SERVER_READ_TIMEOUT, 0x1200, "Read timeout") \
  XX(W_CASS_ERROR_SOURCE_SERVER, W_CASS_ERROR_SERVER_READ_FAILURE, 0x1300, "Read failure") \
  XX(W_CASS_ERROR_SOURCE_SERVER, W_CASS_ERROR_SERVER_FUNCTION_FAILURE, 0x1400, "Function failure") \
  XX(W_CASS_ERROR_SOURCE_SERVER, W_CASS_ERROR_SERVER_WRITE_FAILURE, 0x1500, "Write failure") \
  XX(W_CASS_ERROR_SOURCE_SERVER, W_CASS_ERROR_SERVER_SYNTAX_ERROR, 0x2000, "Syntax error") \
  XX(W_CASS_ERROR_SOURCE_SERVER, W_CASS_ERROR_SERVER_UNAUTHORIZED, 0x2100, "Unauthorized") \
  XX(W_CASS_ERROR_SOURCE_SERVER, W_CASS_ERROR_SERVER_INVALID_QUERY, 0x2200, "Invalid query") \
  XX(W_CASS_ERROR_SOURCE_SERVER, W_CASS_ERROR_SERVER_CONFIG_ERROR, 0x2300, "Configuration error") \
  XX(W_CASS_ERROR_SOURCE_SERVER, W_CASS_ERROR_SERVER_ALREADY_EXISTS, 0x2400, "Already exists") \
  XX(W_CASS_ERROR_SOURCE_SERVER, W_CASS_ERROR_SERVER_UNPREPARED, 0x2500, "Unprepared") \
  XX(W_CASS_ERROR_SOURCE_SSL, W_CASS_ERROR_SSL_INVALID_CERT, 1, "Unable to load certificate") \
  XX(W_CASS_ERROR_SOURCE_SSL, W_CASS_ERROR_SSL_INVALID_PRIVATE_KEY, 2, "Unable to load private key") \
  XX(W_CASS_ERROR_SOURCE_SSL, W_CASS_ERROR_SSL_NO_PEER_CERT, 3, "No peer certificate")  \
  XX(W_CASS_ERROR_SOURCE_SSL, W_CASS_ERROR_SSL_INVALID_PEER_CERT, 4, "Invalid peer certificate") \
  XX(W_CASS_ERROR_SOURCE_SSL, W_CASS_ERROR_SSL_IDENTITY_MISMATCH, 5, "Certificate does not match host or IP address") \
  XX(W_CASS_ERROR_SOURCE_SSL, W_CASS_ERROR_SSL_PROTOCOL_ERROR, 6, "Protocol error") \
  XX(W_CASS_ERROR_SOURCE_SSL, W_CASS_ERROR_SSL_CLOSED, 7, "Connection closed")

        /* @cond IGNORE */
#define W_CASS_ERROR_MAP W_CASS_ERROR_MAPPING /* Deprecated */
/* @endcond*/

#define CASS_ERROR(source, code) ((source << 24) | code)

        typedef enum {
            W_CASS_OK = W_SUCCESS,
#define XX_ERROR(source, name, code, _) name = CASS_ERROR(source, code),
            W_CASS_ERROR_MAPPING(XX_ERROR)
#undef XX_ERROR
            /* @cond IGNORE */
            W_CASS_ERROR_LAST_ENTRY
            /* @endcond*/
        } w_Cass_Error;

        /**
         * A callback that's notified when the future is set.
         *
         * @param[in] message
         * @param[in] data user defined data provided when the callback
         * was registered.
         *
         * @see cass_future_set_callback()
         */
        typedef void (* w_cass_future_callback)(w_cass_future* pFuture,
            void* pData);

        /**
         * Maximum size of a log message
         */
#define CASS_LOG_MAX_MESSAGE_SIZE 1024

         /**
          * A log message.
          */
        

        /**
         * A callback that's used to handle logging.
         *
         * @param[in] message
         * @param[in] data user defined data provided when the callback
         * was registered.
         *
         * @see cass_log_set_callback()
         */
        typedef void (*w_cass_log_callback)(const w_cass_log_message pMessage,
            void* pData);

        /**
         * A custom malloc function. This function should allocate "size" bytes and
         * return a pointer to that memory
         *
         * @param[in] size The size of the memory to allocate
         *
         * @see w_cass_free_function
         * @see cass_alloc_set_functions()
         */
        typedef void* (*w_cass_malloc_function)(size_t pSize);

        /**
         * A custom realloc function. This function attempts to change the size of the
         * memory pointed to by "ptr". If the memory cannot be resized then new memory
         * should be allocated and contain the contents of the original memory at "ptr".
         *
         * @param[in] ptr A pointer to the original memory. If NULL it should behave the
         * same as "CassMallocFunction"
         * @param[in] size The size of the memory to allocate/resize.
         *
         * @see w_cass_malloc_function
         * @see w_cass_free_function
         * @see cass_alloc_set_functions()
         */
        typedef void* (*w_cass_realloc_function)(void* pPtr, size_t pSize);

        /**
         * A custom free function. This function deallocates the memory pointed to by
         * "ptr" that was previously allocated by a "CassMallocFunction" or
         * "CassReallocFunction" function.
         *
         * @param[in] ptr A pointer to memory that should be deallocated. If NULL then
         * this will perform no operation.
         *
         * @see w_cass_malloc_function
         * @see w_cass_realloc_function
         * @see cass_alloc_set_functions()
         */
        typedef void (*w_cass_free_function)(void* pPtr);

        /**
         * An authenticator.
         *
         * @struct CassAuthenticator
         */
        typedef struct CassAuthenticator_ w_cass_authenticator;

        /**
         * A callback used to initiate an authentication exchange.
         *
         * Use cass_authenticator_set_response() to set the response token.
         *
         * Use w_cassandra_authenticator_set_error() if an error occurred during
         * initialization.
         *
         * @param[in] auth
         * @param[in] data
         */
        typedef void (*w_cass_authenticator_initial_callback)(w_cass_authenticator* pAuth,
            void* pData);

        /**
         * A callback used when an authentication challenge initiated
         * by the server.
         *
         * Use cass_authenticator_set_response() to set the response token.
         *
         * Use w_cassandra_authenticator_set_error() if an error occurred during the
         * challenge.
         *
         * @param[in] auth
         * @param[in] data
         * @param[in] token
         * @param[in] token_size
         */
        typedef void (*w_cass_authenticator_challenge_callback)(w_cass_authenticator* pAuth,
            void* pData,
            const char* pToken,
            size_t pTokenSize);
        /**
         * A callback used to indicate the success of the authentication
         * exchange.
         *
         * Use w_cassandra_authenticator_set_error() if an error occurred while evaluating
         * the success token.
         *
         * @param[in] auth
         * @param[in] data
         * @param[in] token
         * @param[in] token_size
         */
        typedef void (*w_cass_authenticator_success_callback)(w_cass_authenticator* pAuth,
            void* pData,
            const char* pToken,
            size_t pTokenSize);
        /**
         * A callback used to cleanup resources that were acquired during
         * the process of the authentication exchange. This is called after
         * the termination of the exchange regardless of the outcome.
         *
         * @param[in] auth
         * @param[in] data
         */
        typedef void (*w_cass_authenticator_cleanup_callback)(w_cass_authenticator* pAuth,
            void* pData);

        /**
         * A callback used to cleanup resources.
         *
         * @param[in] data
         */
        typedef void (*w_cass_authenticator_data_cleanup_callback)(void* pData);

        /**
         * Authenticator callbacks
         */
      

        typedef enum  {
            W_CASS_HOST_LISTENER_EVENT_UP,
            W_CASS_HOST_LISTENER_EVENT_DOWN,
            W_CASS_HOST_LISTENER_EVENT_ADD,
            W_CASS_HOST_LISTENER_EVENT_REMOVE
        } w_cass_host_list_ener_event;

        /**
         * A callback used to indicate the host state for a node in the cluster.
         *
         * @param[in] event
         * @param[in] address
         * @param[in] data
         * @see w_cass_cluster_set_host_listener_callback()
         */
        typedef void(* w_cass_host_listener_callback)(w_cass_host_list_ener_event event,
            const w_cass_inet pAddress,
            void* pData);

        /***********************************************************************************
         *
         * Execution Profile
         *
         ***********************************************************************************/

         /**
          * An execution profile object provides a mechanism to group together a set of
          * configuration options and reuse them across different statement executions.
          * This feature is useful when dealing with different query workloads.
          *
          * @struct w_cass_exec_profile
          */
        typedef struct CassExecProfile_ w_cass_exec_profile;

        /**
         * Creates a new execution profile.
         *
         * @public @memberof w_cass_exec_profile
         *
         * @return Returns a execution profile that must be freed.
         *
         * @see cass_execution_profile_free()
         */
        W_SYSTEM_EXPORT
            w_cass_exec_profile* w_cassandra_execution_profile_new();

        /**
         * Frees a execution profile instance.
         *
         * @public @memberof w_cass_exec_profile
         *
         * @param[in] profile
         */
        W_SYSTEM_EXPORT
            void w_cassandra_execution_profile_free(_In_ w_cass_exec_profile* pProfile);

        /**
         * Sets the timeout waiting for a response from a node.
         *
         * <b>Default:</b> Disabled (uses the cluster request timeout)
         *
         * @public @memberof w_cass_exec_profile
         *
         * @param[in] profile
         * @param[in] timeout_ms Request timeout in milliseconds. Use 0 for no timeout
         * or CASS_UINT64_MAX to disable.
         * @return W_SUCCESS if successful, otherwise an error occurred.
         *
         * @see w_cassandra_statement_set_request_timeout()
         */
        W_SYSTEM_EXPORT    w_Cass_Error
            w_cassandra_execution_profile_set_request_timeout(_In_ w_cass_exec_profile* pProfile, _In_ uint64_t pTimeoutMs);


        /**
         * Sets the consistency level.
         *
         * <b>Default:</b> Disabled (uses the default consistency)
         *
         * @public @memberof w_cass_exec_profile
         *
         * @param[in] profile
         * @param[in] consistency
         * @return W_SUCCESS if successful, otherwise an error occurred.
         *
         * @see w_cassandra_statement_set_consistency()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_execution_profile_set_consistency(_In_ w_cass_exec_profile* pProfile, _In_ w_cass_consistency pConsistency);


        /**
         * Sets the serial consistency level.
         *
         * <b>Default:</b> Disabled (uses the default serial consistency)
         *
         * @public @memberof w_cass_exec_profile
         *
         * @param[in] profile
         * @param[in] serial_consistency
         * @return W_SUCCESS if successful, otherwise an error occurred.
         *
         * @see w_cassandra_statement_set_serial_consistency()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_execution_profile_set_serial_consistency(_In_ w_cass_exec_profile* pProfile, _In_ w_cass_consistency pPserialConsistency);

        /**
         * Configures the execution profile to use round-robin load balancing.
         *
         * The driver discovers all nodes in a cluster and cycles through
         * them per request. All are considered 'local'.
         *
         * <b>Note:</b> Profile-based load balancing policy is disabled by default;
         * cluster load balancing policy is used when profile does not contain a policy.
         *
         * @public @memberof w_cass_exec_profile
         *
         * @param[in] profile
         * @return W_SUCCESS if successful, otherwise an error occurred.
         *
         * @see w_cassandra_cluster_set_load_balance_round_robin()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_execution_profile_set_load_balance_round_robin(_In_ w_cass_exec_profile* pProfile);
            /**
             * Configures the execution profile to use DC-aware load balancing.
             * For each query, all live nodes in a primary 'local' DC are tried first,
             * followed by any node from other DCs.
             *
             * <b>Note:</b> Profile-based load balancing policy is disabled by default;
             * cluster load balancing policy is used when profile does not contain a policy.
             *
             * @deprecated The remote DC settings for DC-aware are not suitable for most
             * scenarios that require DC failover. There is also unhandled gap between
             * replication factor number of nodes failing and the full cluster failing. Only
             * the remote DC settings are being deprecated.
             *
             * @public @memberof w_cass_exec_profile
             *
             * @param[in] profile
             * @param[in] local_dc The primary data center to try first
             * @param[in] used_hosts_per_remote_dc The number of hosts used in each remote
             * DC if no hosts are available in the local dc (<b>deprecated</b>)
             * @param[in] allow_remote_dcs_for_local_cl Allows remote hosts to be used if no
             * local dc hosts are available and the consistency level is LOCAL_ONE or
             * LOCAL_QUORUM (<b>deprecated</b>)
             * @return W_SUCCESS if successful, otherwise an error occurred.
             *
             * @see w_cassandra_cluster_set_load_balance_dc_aware()
             */
            W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_execution_profile_set_load_balance_dc_aware(_In_ w_cass_exec_profile* pProfile
                , _In_ const char* pLocalDc,
                _In_ unsigned pUsedHostsPerRemoteDc,
                _In_ w_bool_t pAllowRemoteCsForLocalCl);

        /**
         * Same as cass_execution_profile_set_load_balance_dc_aware(), but with lengths
         * for string parameters.
         *
         * @deprecated The remote DC settings for DC-aware are not suitable for most
         * scenarios that require DC failover. There is also unhandled gap between
         * replication factor number of nodes failing and the full cluster failing. Only
         * the remote DC settings are being deprecated.
         *
         * @public @memberof w_cass_exec_profile
         *
         * @param[in] profile
         * @param[in] local_dc
         * @param[in] local_dc_length
         * @param[in] used_hosts_per_remote_dc (<b>deprecated</b>)
         * @param[in] allow_remote_dcs_for_local_cl (<b>deprecated</b>)
         * @return same as cass_execution_profile_set_load_balance_dc_aware()
         *
         * @see w_cassandra_execution_profile_set_load_balance_dc_aware()
         * @see w_cassandra_cluster_set_load_balance_dc_aware_n()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_execution_profile_set_load_balance_dc_aware_n(_In_ w_cass_exec_profile* pProfile
                , _In_ const char* pLocalDc,
                _In_ size_t pLocalDcLength,
                _In_ unsigned pUsedHostsPerRemoteDc,
                _In_  w_bool_t pAllowRemoteDcsForLocalCl);

        /**
         * Configures the execution profile to use token-aware request routing or not.
         *
         * <b>Important:</b> Token-aware routing depends on keyspace metadata.
         * For this reason enabling token-aware routing will also enable retrieving
         * and updating keyspace schema metadata.
         *
         * <b>Default:</b> cass_true (enabled).
         *
         * This routing policy composes the base routing policy, routing
         * requests first to replicas on nodes considered 'local' by
         * the base load balancing policy.
         *
         * <b>Note:</b> Execution profiles use the cluster-level load balancing policy
         * unless enabled. This setting is not applicable unless a load balancing policy
         * is enabled on the execution profile.
         *
         * @public @memberof w_cass_exec_profile
         *
         * @param[in] profile
         * @param[in] enabled
         * @return W_SUCCESS if successful, otherwise an error occurred.
         *
         * @see w_cassandra_cluster_set_token_aware_routing()
         */
        W_SYSTEM_EXPORT 
            w_Cass_Error  w_cassandra_execution_profile_set_token_aware_routing(_In_ w_cass_exec_profile* pProfile,
                _In_ w_bool_t pEnabled);

        /**
         * Configures the execution profile's token-aware routing to randomly shuffle
         * replicas. This can reduce the effectiveness of server-side caching, but it
         * can better distribute load over replicas for a given partition key.
         *
         * <b>Note:</b> Token-aware routing must be enabled and a load balancing policy
         * must be enabled on the execution profile for the setting to be applicable.
         *
         * <b>Default:</b> cass_true (enabled).
         *
         * @public @memberof w_cass_exec_profile
         *
         * @param[in] profile
         * @param[in] enabled
         * @return W_SUCCESS if successful, otherwise an error occurred.
         *
         * @see w_cassandra_cluster_set_token_aware_routing_shuffle_replicas()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_execution_profile_set_token_aware_routing_shuffle_replicas(_In_ w_cass_exec_profile* pProfile,
                _In_ w_bool_t pEnabled);

        /**
         * Configures the execution profile to use latency-aware request routing or not.
         *
         * <b>Note:</b> Execution profiles use the cluster-level load balancing policy
         * unless enabled. This setting is not applicable unless a load balancing policy
         * is enabled on the execution profile.
         *
         * <b>Default:</b> cass_false (disabled).
         *
         * This routing policy is a top-level routing policy. It uses the
         * base routing policy to determine locality (dc-aware) and/or
         * placement (token-aware) before considering the latency.
         *
         * @public @memberof w_cass_exec_profile
         *
         * @param[in] profile
         * @param[in] enabled
         * @return W_SUCCESS if successful, otherwise an error occurred.
         *
         * @see w_cassandra_cluster_set_latency_aware_routing()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_execution_profile_set_latency_aware_routing(_In_ w_cass_exec_profile* pProfile,
                _In_ w_bool_t pEnabled);

        /**
         * Configures the execution profile's settings for latency-aware request
         * routing.
         *
         * <b>Note:</b> Execution profiles use the cluster-level load balancing policy
         * unless enabled. This setting is not applicable unless a load balancing policy
         * is enabled on the execution profile.
         *
         * <b>Defaults:</b>
         *
         * <ul>
         *   <li>exclusion_threshold: 2.0</li>
         *   <li>scale_ms: 100 milliseconds</li>
         *   <li>retry_period_ms: 10,000 milliseconds (10 seconds)</li>
         *   <li>update_rate_ms: 100 milliseconds</li>
         *   <li>min_measured: 50</li>
         * </ul>
         *
         * @public @memberof w_cass_exec_profile
         *
         * @param[in] profile
         * @param[in] exclusion_threshold Controls how much worse the latency must be
         * compared to the average latency of the best performing node before it
         * penalized.
         * @param[in] scale_ms Controls the weight given to older latencies when
         * calculating the average latency of a node. A bigger scale will give more
         * weight to older latency measurements.
         * @param[in] retry_period_ms The amount of time a node is penalized by the
         * policy before being given a second chance when the current average latency
         * exceeds the calculated threshold
         * (exclusion_threshold * best_average_latency).
         * @param[in] update_rate_ms The rate at  which the best average latency is
         * recomputed.
         * @param[in] min_measured The minimum number of measurements per-host required
         * to be considered by the policy.
         * @return W_SUCCESS if successful, otherwise an error occurred.
         *
         * @see w_cassandra_cluster_set_latency_aware_routing_settings()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_execution_profile_set_latency_aware_routing_settings(_In_ w_cass_exec_profile* pProfile,
                _In_ double pExclusionThreshold,
                _In_ uint64_t pScaleMs,
                _In_ uint64_t pRetryPeriodMs,
                _In_ uint64_t pUpdateRateMs,
                _In_ uint64_t pMinMeasured);

        /**
         * Sets/Appends whitelist hosts for the execution profile. The first call sets
         * the whitelist hosts and any subsequent calls appends additional hosts.
         * Passing an empty string will clear and disable the whitelist. White space is
         * striped from the hosts.
         *
         * This policy filters requests to all other policies, only allowing requests
         * to the hosts contained in the whitelist. Any host not in the whitelist will
         * be ignored and a connection will not be established. This policy is useful
         * for ensuring that the driver will only connect to a predefined set of hosts.
         *
         * Examples: "127.0.0.1" "127.0.0.1,127.0.0.2"
         *
         * <b>Note:</b> Execution profiles use the cluster-level load balancing policy
         * unless enabled. This setting is not applicable unless a load balancing policy
         * is enabled on the execution profile.
         *
         * @public @memberof w_cass_exec_profile
         *
         * @param[in] profile
         * @param[in] hosts A comma delimited list of addresses. An empty string will
         * clear the whitelist hosts. The string is copied into the cluster
         * configuration; the memory pointed to by this parameter can be freed after
         * this call.
         * @return W_SUCCESS if successful, otherwise an error occurred.
         *
         * @see w_cassandra_cluster_set_whitelist_filtering()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_execution_profile_set_whitelist_filtering(_In_ w_cass_exec_profile* pProfile,
                _In_ const char* pHosts);

        /**
         * Same as cass_execution_profile_set_whitelist_filtering(), but with lengths
         * for string parameters.
         *
         * @public @memberof w_cass_exec_profile
         *
         * @param[in] profile
         * @param[in] hosts
         * @param[in] hosts_length
         * @return same as w_cassandra_execution_profile_set_whitelist_filtering()
         *
         * @see w_cassandra_execution_profile_set_whitelist_filtering()
         * @see w_cassandra_cluster_set_whitelist_filtering()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_execution_profile_set_whitelist_filtering_n(_In_ w_cass_exec_profile* pProfile,
                _In_ const char* pHosts,
                _In_ size_t pHostsLength);

        /**
         * Sets/Appends blacklist hosts for the execution profile. The first call sets
         * the blacklist hosts and any subsequent calls appends additional hosts.
         * Passing an empty string will clear and disable the blacklist. White space is
         * striped from the hosts.
         *
         * This policy filters requests to all other policies, only allowing requests
         * to the hosts not contained in the blacklist. Any host in the blacklist will
         * be ignored and a connection will not be established. This policy is useful
         * for ensuring that the driver will not connect to a predefined set of hosts.
         *
         * Examples: "127.0.0.1" "127.0.0.1,127.0.0.2"
         *
         * <b>Note:</b> Execution profiles use the cluster-level load balancing policy
         * unless enabled. This setting is not applicable unless a load balancing policy
         * is enabled on the execution profile.
         *
         * @public @memberof w_cass_exec_profile
         *
         * @param[in] profile
         * @param[in] hosts A comma delimited list of addresses. An empty string will
         * clear the blacklist hosts. The string is copied into the cluster
         * configuration; the memory pointed to by this parameter can be freed after
         * this call.
         * @return W_SUCCESS if successful, otherwise an error occurred.
         *
         * @see w_cassandra_cluster_set_blacklist_filtering()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_execution_profile_set_blacklist_filtering(_In_ w_cass_exec_profile* pProfile,
                _In_ const char* pHosts);

        /**
         * Same as w_cassandra_execution_profile_set_blacklist_filtering(), but with lengths
         * for string parameters.
         *
         * @public @memberof w_cass_exec_profile
         *
         * @param[in] profile
         * @param[in] hosts
         * @param[in] hosts_length
         * @return same as w_cassandra_execution_profile_set_blacklist_filtering_hosts()
         *
         * @see w_cassandra_execution_profile_set_blacklist_filtering()
         * @see w_cassandra_cluster_set_blacklist_filtering()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_execution_profile_set_blacklist_filtering_n(_In_ w_cass_exec_profile* pProfile,
                _In_ const char* pHosts,
                _In_ size_t pHostsLength);

        /**
         * Same as w_cassandra_execution_profile_set_whitelist_filtering(), but whitelist all
         * hosts of a dc.
         *
         * Examples: "dc1", "dc1,dc2"
         *
         * @public @memberof w_cass_exec_profile
         *
         * @param[in] profile
         * @param[in] dcs A comma delimited list of dcs. An empty string will clear the
         * whitelist dcs. The string is copied into the cluster configuration; the
         * memory pointed to by this parameter can be freed after this call.
         * @return W_SUCCESS if successful, otherwise an error occurred.
         *
         * @see w_cassandra_cluster_set_whitelist_dc_filtering()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_execution_profile_set_whitelist_dc_filtering(_In_ w_cass_exec_profile* pProfile,
                _In_  const char* pDcs);

        /**
         * Same as w_cassandra_execution_profile_set_whitelist_dc_filtering(), but with lengths
         * for string parameters.
         *
         * @public @memberof w_cass_exec_profile
         *
         * @param[in] profile
         * @param[in] dcs
         * @param[in] dcs_length
         * @return same as w_cassandra_execution_profile_set_whitelist_dc_filtering()
         *
         * @see w_cassandra_execution_profile_set_whitelist_dc_filtering()
         * @see w_cassandra_cluster_set_whitelist_dc_filtering()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_execution_profile_set_whitelist_dc_filtering_n(_In_ w_cass_exec_profile* pProfile,
                _In_ const char* pDcs,
                _In_ size_t pDcsLength);

        /**
         * Same as w_cassandra_execution_profile_set_blacklist_filtering(), but blacklist all
         * hosts of a dc.
         *
         * Examples: "dc1", "dc1,dc2"
         *
         * @public @memberof w_cass_exec_profile
         *
         * @param[in] profile
         * @param[in] dcs A comma delimited list of dcs. An empty string will clear the
         * blacklist dcs. The string is copied into the cluster configuration; the
         * memory pointed to by this parameter can be freed after this call.
         * @return W_SUCCESS if successful, otherwise an error occurred.
         *
         * @see w_cassandra_execution_profile_set_blacklist_filtering()
         * @see w_cassandra_cluster_set_blacklist_dc_filtering()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_execution_profile_set_blacklist_dc_filtering(_In_ w_cass_exec_profile* pProfile,
                _In_ const char* pDcs);

        /**
         * Same as w_cassandra_execution_profile_set_blacklist_dc_filtering(), but with lengths
         * for string parameters.
         *
         * @public @memberof w_cass_exec_profile
         *
         * @param[in] profile
         * @param[in] dcs
         * @param[in] dcs_length
         * @return same as w_cassandra_execution_profile_set_blacklist_dc_filtering()
         *
         * @see w_cassandra_execution_profile_set_blacklist_dc_filtering()
         * @see w_cassandra_cluster_set_blacklist_dc_filtering()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_execution_profile_set_blacklist_dc_filtering_n(_In_ w_cass_exec_profile* pProfile,
                _In_  const char* pDcs,
                _In_   size_t pDcsLength);

        /**
         * Sets the execution profile's retry policy.
         *
         * <b>Note:</b> Profile-based retry policy is disabled by default; cluster retry
         * policy is used when profile does not contain a policy unless the retry policy
         * was explicitly set on the batch/statement request.
         *
         * @public @memberof w_cass_exec_profile
         *
         * @param[in] profile
         * @param[in] retry_policy NULL will clear retry policy from execution profile
         * @return W_SUCCESS if successful, otherwise an error occurred.
         *
         * @see w_cassandra_cluster_set_retry_policy()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_execution_profile_set_retry_policy(_In_ w_cass_exec_profile* pProfile, 
                _In_  w_cass_retry_policy* pRetryPolicy);

        /**
         * Enable constant speculative executions with the supplied settings for the
         * execution profile.
         *
         * <b>Note:</b> Profile-based speculative execution policy is disabled by
         * default; cluster speculative execution policy is used when profile does not
         * contain a policy.
         *
         * @public @memberof w_cass_exec_profile
         *
         * @param[in] profile
         * @param[in] constant_delay_ms
         * @param[in] max_speculative_executions
         * @return W_SUCCESS if successful, otherwise an error occurred
         *
         * @see w_cassandra_cluster_set_constant_speculative_execution_policy()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_execution_profile_set_constant_speculative_execution_policy(_In_ w_cass_exec_profile* pProfile,
                _In_  int64_t pConstantDelayMs,
                _In_  int pMaxSpeculativeExecutions);

        /**
         * Disable speculative executions for the execution profile.
         *
         * <b>Note:</b> Profile-based speculative execution policy is disabled by
         * default; cluster speculative execution policy is used when profile does not
         * contain a policy.
         *
         * @public @memberof w_cass_exec_profile
         *
         * @param[in] profile
         * @return W_SUCCESS if successful, otherwise an error occurred
         *
         * @see w_cassandra_cluster_set_no_speculative_execution_policy()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_execution_profile_set_no_speculative_execution_policy(_In_ w_cass_exec_profile* pProfile);
        /***********************************************************************************
         *
         * Cluster
         *
         ***********************************************************************************/

         /**
          * Creates a new cluster.
          *
          * @public @memberof w_Cass_Cluster
          *
          * @return Returns a cluster that must be freed.
          *
          * @see w_cassandra_cluster_free()
          */
        W_SYSTEM_EXPORT w_cass_cluster*
            w_cassandra_cluster_new();

        /**
         * Frees a cluster instance.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         */
        W_SYSTEM_EXPORT void  w_cassandra_cluster_free(_In_  w_cass_cluster* pCluster);

        /**
         * Sets/Appends contact points. This *MUST* be set. The first call sets
         * the contact points and any subsequent calls appends additional contact
         * points. Passing an empty string will clear the contact points. White space
         * is striped from the contact points.
         *
         * Examples: "127.0.0.1" "127.0.0.1,127.0.0.2", "server1.domain.com"
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] contact_points A comma delimited list of addresses or
         * names. An empty string will clear the contact points.
         * The string is copied into the cluster configuration; the memory pointed
         * to by this parameter can be freed after this call.
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_contact_points(_In_ w_cass_cluster* pCluster,
                _In_ const char* pContactPoints);

        /**
         * Same as w_cassandra_cluster_set_contact_points(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] contact_points
         * @param[in] contact_points_length
         * @return same as w_cassandra_cluster_set_contact_points()
         *
         * @see w_cassandra_cluster_set_contact_points()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_contact_points_n(_In_ w_cass_cluster* pCluster, 
                _In_  const char* pContactPoints,
                _In_   size_t pContactPointsLength);

        /**
         * Sets the port.
         *
         * <b>Default:</b> 9042
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] port
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_port(_In_ w_cass_cluster* pCluster,
                _In_  int pPort);

        /**
         * Sets the local address to bind when connecting to the cluster,
         * if desired.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] name IP address to bind, or empty string for no binding.
         * Only numeric addresses are supported; no resolution is done.
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_local_address(_In_ w_cass_cluster* pCluster,
                _In_ const char* pName);

        /**
         * Same as cass_cluster_set_local_address(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] name
         * @param[in] name_length
         * @return same as w_cassandra_cluster_set_local_address()
         *
         * @see cass_cluster_set_local_address()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_local_address_n(_In_ w_cass_cluster* pCluster, 
                _In_ const char* pName,
                _In_ size_t pNameLength);

        /**
         * Sets the SSL context and enables SSL.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] ssl
         *
         * @see w_cassandra_ssl_new()
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_ssl(_In_ w_cass_cluster* pCluster, 
                _In_ w_cass_ssl* pSsl);

        /**
         * Sets custom authenticator
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] exchange_callbacks
         * @param[in] cleanup_callback
         * @param[in] data
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_authenticator_callbacks(_In_ w_cass_cluster* pCluster,
                _In_ const w_cass_authenticator_callbacks pExchangeCallbacks,
                _In_ w_cass_authenticator_data_cleanup_callback pCleanupCallback,
                _In_ void* pData);

        /**
         * Sets the protocol version. The driver will automatically downgrade to the lowest
         * supported protocol version.
         *
         * <b>Default:</b> CASS_PROTOCOL_VERSION_V4 or CASS_PROTOCOL_VERSION_DSEV1 when
         * using the DSE driver with DataStax Enterprise.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] protocol_version
         * @return W_SUCCESS if successful, otherwise an error occurred.
         *
         * @see w_cassandra_cluster_set_use_beta_protocol_version()
         */
        W_SYSTEM_EXPORT 
            w_Cass_Error w_cassandra_cluster_set_protocol_version(_In_ w_cass_cluster* pCluster,
                _In_ int pProtocolVersion);

        /**
         * Use the newest beta protocol version. This currently enables the use of
         * protocol version v5 (CASS_PROTOCOL_VERSION_V5) or DSEv2 (CASS_PROTOCOL_VERSION_DSEV2)
         * when using the DSE driver with DataStax Enterprise.
         *
         * <b>Default:</b> cass_false
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] enable if false the highest non-beta protocol version will be used
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT 
            w_Cass_Error 
            w_cassandra_cluster_set_use_beta_protocol_version(_In_ w_cass_cluster* pCluster,
                _In_  w_bool_t pEnable);

        /**
         * Sets default consistency level of statement.
         *
         * <b>Default:</b> CASS_CONSISTENCY_LOCAL_ONE
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] consistency
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT 
            w_Cass_Error w_cassandra_cluster_set_consistency(_In_ w_cass_cluster* pCluster,
                _In_  w_cass_consistency pConsistency);

        /**
         * Sets default serial consistency level of statement.
         *
         * <b>Default:</b> CASS_CONSISTENCY_ANY
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] consistency
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT 
            w_Cass_Error  w_cassandra_cluster_set_serial_consistency(_In_ w_cass_cluster* pCluster, _In_ w_cass_consistency pConsistency);

        /**
         * Sets the number of IO threads. This is the number of threads
         * that will handle query requests.
         *
         * <b>Default:</b> 1
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] num_threads
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_num_threads_io(_In_ w_cass_cluster* pCluster, 
                _In_ unsigned pNumThreads);

        /**
         * Sets the size of the fixed size queue that stores
         * pending requests.
         *
         * <b>Default:</b> 8192
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] queue_size
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_queue_size_io(_In_ w_cass_cluster* pCluster,
                _In_ unsigned pQueueSize);

        /**
         * Sets the size of the fixed size queue that stores
         * events.
         *
         * <b>Default:</b> 8192
         *
         * @public @memberof w_Cass_Cluster
         *
         * @deprecated This is no longer useful and does nothing. Expect this to be
         * removed in a future release.
         *
         * @param[in] cluster
         * @param[in] queue_size
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_queue_size_event(_In_ w_cass_cluster* pCluster, 
                _In_ unsigned pQueueSize);

        /**
         * Sets the number of connections made to each server in each
         * IO thread.
         *
         * <b>Default:</b> 1
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] num_connections
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_core_connections_per_host(_In_ w_cass_cluster* pCluster, 
                _In_  unsigned pNumConnections);

        /**
         * Sets the maximum number of connections made to each server in each
         * IO thread.
         *
         * <b>Default:</b> 2
         *
         * @public @memberof w_Cass_Cluster
         *
         * @deprecated This is no longer useful and does nothing. Expect this to be
         * removed in a future release.
         *
         * @param[in] cluster
         * @param[in] num_connections
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT  w_Cass_Error 
            w_cassandra_cluster_set_max_connections_per_host(_In_ w_cass_cluster* pCluster, _In_ unsigned pNumConnections);

        /**
         * Sets the amount of time to wait before attempting to reconnect.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @deprecated This is being replaced with cass_cluster_set_constant_reconnect().
         * Expect this to be removed in a future release.
         *
         * @param[in] cluster
         * @param[in] wait_time
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_reconnect_wait_time(_In_ w_cass_cluster* pCluster, 
                _In_ unsigned pWaitTime);

        /**
         * Configures the cluster to use a reconnection policy that waits a constant
         * time between each reconnection attempt.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] delay_ms Time in milliseconds to delay attempting a reconnection;
         * 0 to perform a reconnection immediately.
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_constant_reconnect(_In_  w_cass_cluster* pCluster,
                _In_  uint64_t pDelayMs);

        /**
         * Configures the cluster to use a reconnection policy that waits exponentially
         * longer between each reconnection attempt; however will maintain a constant
         * delay once the maximum delay is reached.
         *
         * <b>Default:</b>
         * <ul>
         *   <li>2000 milliseconds base delay</li>
         *   <li>60000 milliseconds max delay</li>
         * </ul>
         *
         * <p>
         *   <b>Note:</b> A random amount of jitter (+/- 15%) will be added to the pure
         *   exponential delay value. This helps to prevent situations where multiple
         *   connections are in the reconnection process at exactly the same time. The
         *   jitter will never cause the delay to be less than the base delay, or more
         *   than the max delay.
         * </p>
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] base_delay_ms The base delay (in milliseconds) to use for
         * scheduling reconnection attempts.
         * @param[in] max_delay_ms The maximum delay to wait between two reconnection
         * attempts.
         * @return W_SUCCESS if successful, otherwise error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_exponential_reconnect(_In_ w_cass_cluster* pCluster,
                _In_ uint64_t pBaseDelayMs,
                _In_ uint64_t pMaxDelayMs);

        /**
         * Sets the amount of time, in microseconds, to wait for new requests to
         * coalesce into a single system call. This should be set to a value around
         * the latency SLA of your application's requests while also considering the
         * request's roundtrip time. Larger values should be used for throughput
         * bound workloads and lower values should be used for latency bound
         * workloads.
         *
         * <b>Default:</b> 200 us
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] delay_us
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_coalesce_delay(_In_ w_cass_cluster* pCluster,
                _In_ int64_t pDelayUs);

        /**
         * Sets the ratio of time spent processing new requests versus handling the I/O
         * and processing of outstanding requests. The range of this setting is 1 to 100,
         * where larger values allocate more time to processing new requests and smaller
         * values allocate more time to processing outstanding requests.
         *
         * <b>Default:</b> 50
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] ratio
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_new_request_ratio(_In_ w_cass_cluster* pCluster, 
                _In_ int32_t pRatio);

        /**
         * Sets the maximum number of connections that will be created concurrently.
         * Connections are created when the current connections are unable to keep up with
         * request throughput.
         *
         * <b>Default:</b> 1
         *
         * @public @memberof w_Cass_Cluster
         *
         * @deprecated This is no longer useful and does nothing. Expect this to be
         * removed in a future release.
         *
         * @param[in] cluster
         * @param[in] num_connections
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT 
            w_Cass_Error
            w_cassandra_cluster_set_max_concurrent_creation(_In_ w_cass_cluster* pCluster,
                _In_ unsigned pNumConnections);

        /**
         * Sets the threshold for the maximum number of concurrent requests in-flight
         * on a connection before creating a new connection. The number of new connections
         * created will not exceed max_connections_per_host.
         *
         * <b>Default:</b> 100
         *
         * @public @memberof w_Cass_Cluster
         *
         * @deprecated This is no longer useful and does nothing. Expect this to be
         * removed in a future release.
         *
         * @param[in] cluster
         * @param[in] num_requests
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_max_concurrent_requests_threshold(_In_ w_cass_cluster* pCluster,
                _In_  unsigned pNumConnections);

        /**
         * Sets the maximum number of requests processed by an IO worker
         * per flush.
         *
         * <b>Default:</b> 128
         *
         * @public @memberof w_Cass_Cluster
         *
         * @deprecated This is no longer useful and does nothing. Expect this to be
         * removed in a future release.
         *
         * @param[in] cluster
         * @param[in] num_requests
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_max_requests_per_flush(_In_ w_cass_cluster* cluster,
                _In_ unsigned pNumRequests);

        /**
         * Sets the high water mark for the number of bytes outstanding
         * on a connection. Disables writes to a connection if the number
         * of bytes queued exceed this value.
         *
         * <b>Default:</b> 64 KB
         *
         * @public @memberof w_Cass_Cluster
         *
         * @deprecated This is no longer useful and does nothing. Expect this to be
         * removed in a future release.
         *
         * @param[in] cluster
         * @param[in] num_bytes
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_write_bytes_high_water_mark(_In_ w_cass_cluster* pCluster,
                _In_  unsigned pNumBytes);

        /**
         * Sets the low water mark for number of bytes outstanding on a
         * connection. After exceeding high water mark bytes, writes will
         * only resume once the number of bytes fall below this value.
         *
         * <b>Default:</b> 32 KB
         *
         * @public @memberof w_Cass_Cluster
         *
         * @deprecated This is no longer useful and does nothing. Expect this to be
         * removed in a future release.
         *
         * @param[in] cluster
         * @param[in] num_bytes
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_write_bytes_low_water_mark(_In_ w_cass_cluster* pCluster,
                _In_ unsigned pNumBytes);

        /**
         * Sets the high water mark for the number of requests queued waiting
         * for a connection in a connection pool. Disables writes to a
         * host on an IO worker if the number of requests queued exceed this
         * value.
         *
         * <b>Default:</b> 256
         *
         * @public @memberof w_Cass_Cluster
         *
         * @deprecated This is no longer useful and does nothing. Expect this to be
         * removed in a future release.
         *
         * @param[in] cluster
         * @param[in] num_requests
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_pending_requests_high_water_mark(_In_ w_cass_cluster* pCluster,
                _In_   unsigned pNumRequests);

        /**
         * Sets the low water mark for the number of requests queued waiting
         * for a connection in a connection pool. After exceeding high water mark
         * requests, writes to a host will only resume once the number of requests
         * fall below this value.
         *
         * <b>Default:</b> 128
         *
         * @public @memberof w_Cass_Cluster
         *
         * @deprecated This is no longer useful and does nothing. Expect this to be
         * removed in a future release.
         *
         * @param[in] cluster
         * @param[in] num_requests
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_pending_requests_low_water_mark(_In_ w_cass_cluster* pCluster,
                _In_  unsigned pNumRequests);

        /**
         * Sets the timeout for connecting to a node.
         *
         * <b>Default:</b> 5000 milliseconds
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] timeout_ms Connect timeout in milliseconds
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_connect_timeout(_In_ w_cass_cluster* pCluster, 
                _In_  unsigned pTimeoutMs);

        /**
         * Sets the timeout for waiting for a response from a node.
         *
         * <b>Default:</b> 12000 milliseconds
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] timeout_ms Request timeout in milliseconds. Use 0 for no timeout.
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_request_timeout(_In_ w_cass_cluster* pCluster,
                _In_ unsigned pTimeoutMs);

        /**
         * Sets the timeout for waiting for DNS name resolution.
         *
         * <b>Default:</b> 2000 milliseconds
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] timeout_ms Request timeout in milliseconds
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_resolve_timeout(_In_ w_cass_cluster* pCluster, _In_ unsigned pTimeoutMs);

        /**
         * Sets the maximum time to wait for schema agreement after a schema change
         * is made (e.g. creating, altering, dropping a table/keyspace/view/index etc).
         *
         * <b>Default:</b> 10000 milliseconds
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] wait_time_ms Wait time in milliseconds
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_max_schema_wait_time(_In_ w_cass_cluster* pCluster,
                _In_ unsigned pWaitTimeMs);


        /**
         * Sets the maximum time to wait for tracing data to become available.
         *
         * <b>Default:</b> 15 milliseconds
         *
         * @param[in] cluster
         * @param[in] max_wait_time_ms
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_tracing_max_wait_time(_In_ w_cass_cluster* pCluster,
                _In_ unsigned pMaxWaitTimeMs);

        /**
         * Sets the amount of time to wait between attempts to check to see if tracing is
         * available.
         *
         * <b>Default:</b> 3 milliseconds
         *
         * @param[in] cluster
         * @param[in] retry_wait_time_ms
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_tracing_retry_wait_time(_In_ w_cass_cluster* pCluster,
                _In_ unsigned pRetryWaitTimeMs);

        /**
         * Sets the consistency level to use for checking to see if tracing data is
         * available.
         *
         * <b>Default:</b> CASS_CONSISTENCY_ONE
         *
         * @param[in] cluster
         * @param[in] consistency
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_tracing_consistency(_In_ w_cass_cluster* pCluster,
                _In_  w_cass_consistency pConsistency);


        /**
         * Sets credentials for plain text authentication.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] username
         * @param[in] password
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_credentials(_In_ w_cass_cluster* pCluster, 
                _In_ const char* pUsername, 
                _In_ const char* pPassword);

        /**
         * Same as w_cassandra_cluster_set_credentials(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] username
         * @param[in] username_length
         * @param[in] password
         * @param[in] password_length
         * @return same as w_cassandra_cluster_set_credentials()
         *
         * @see w_cassandra_cluster_set_credentials();
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_credentials_n(_In_ w_cass_cluster* pCluster,
                _In_ const char* pUsername,
                _In_ size_t pUsernameLength,
                _In_ const char* pPassword,
                _In_  size_t pPasswordLength) ;

        /**
         * Configures the cluster to use round-robin load balancing.
         *
         * The driver discovers all nodes in a cluster and cycles through
         * them per request. All are considered 'local'.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_load_balance_round_robin(_In_ w_cass_cluster * pCluster);

        /**
         * Configures the cluster to use DC-aware load balancing.
         * For each query, all live nodes in a primary 'local' DC are tried first,
         * followed by any node from other DCs.
         *
         * <b>Note:</b> This is the default, and does not need to be called unless
         * switching an existing from another policy or changing settings.
         * Without further configuration, a default local_dc is chosen from the
         * first connected contact point, and no remote hosts are considered in
         * query plans. If relying on this mechanism, be sure to use only contact
         * points from the local DC.
         *
         * @deprecated The remote DC settings for DC-aware are not suitable for most
         * scenarios that require DC failover. There is also unhandled gap between
         * replication factor number of nodes failing and the full cluster failing. Only
         * the remote DC settings are being deprecated.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] local_dc The primary data center to try first
         * @param[in] used_hosts_per_remote_dc The number of hosts used in each remote
         * DC if no hosts are available in the local dc (<b>deprecated</b>)
         * @param[in] allow_remote_dcs_for_local_cl Allows remote hosts to be used if no
         * local dc hosts are available and the consistency level is LOCAL_ONE or
         * LOCAL_QUORUM (<b>deprecated</b>)
         * @return W_SUCCESS if successful, otherwise an error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_load_balance_dc_aware(_In_ w_cass_cluster * pCluster,
                _In_ const char* pLocalDc, 
                _In_  unsigned pUsedHostsPerRemoteDc,
                _In_ w_bool_t pAllowRemoteDcsForLocalCl);


        /**
         * Same as w_cassandra_cluster_set_load_balance_dc_aware(), but with lengths for string
         * parameters.
         *
         * @deprecated The remote DC settings for DC-aware are not suitable for most
         * scenarios that require DC failover. There is also unhandled gap between
         * replication factor number of nodes failing and the full cluster failing. Only
         * the remote DC settings are being deprecated.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] local_dc
         * @param[in] local_dc_length
         * @param[in] used_hosts_per_remote_dc (<b>deprecated</b>)
         * @param[in] allow_remote_dcs_for_local_cl (<b>deprecated</b>)
         * @return same as cass_cluster_set_load_balance_dc_aware()
         *
         * @see w_cassandra_cluster_set_load_balance_dc_aware()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_load_balance_dc_aware_n(_In_ w_cass_cluster * pCluster,
                _In_ const char* pLocalDc,
                _In_ size_t pLocalDcLength, 
                _In_  unsigned pUsedHostsPerRemoteDc,
                _In_  w_bool_t pAllowRemoteDcsForLocalCl);

        /**
         * Configures the cluster to use token-aware request routing or not.
         *
         * <b>Important:</b> Token-aware routing depends on keyspace metadata.
         * For this reason enabling token-aware routing will also enable retrieving
         * and updating keyspace schema metadata.
         *
         * <b>Default:</b> cass_true (enabled).
         *
         * This routing policy composes the base routing policy, routing
         * requests first to replicas on nodes considered 'local' by
         * the base load balancing policy.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] enabled
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_token_aware_routing(_In_ w_cass_cluster* pCluster,
                _In_ w_bool_t pEnabled);


        /**
         * Configures token-aware routing to randomly shuffle replicas. This can reduce
         * the effectiveness of server-side caching, but it can better distribute load over
         * replicas for a given partition key.
         *
         * <b>Note:</b> Token-aware routing must be enabled for the setting to
         * be applicable.
         *
         * <b>Default:</b> cass_true (enabled).
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] enabled
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_token_aware_routing_shuffle_replicas(_In_ w_cass_cluster* pCluster,
                _In_  w_bool_t pEnabled);

        /**
         * Configures the cluster to use latency-aware request routing or not.
         *
         * <b>Default:</b> cass_false (disabled).
         *
         * This routing policy is a top-level routing policy. It uses the
         * base routing policy to determine locality (dc-aware) and/or
         * placement (token-aware) before considering the latency.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] enabled
         */
        W_SYSTEM_EXPORT 
            void   w_cassandra_cluster_set_latency_aware_routing(_In_ w_cass_cluster* pCluster, 
                _In_  w_bool_t pEnabled);

        /**
         * Configures the settings for latency-aware request routing.
         *
         * <b>Defaults:</b>
         *
         * <ul>
         *   <li>exclusion_threshold: 2.0</li>
         *   <li>scale_ms: 100 milliseconds</li>
         *   <li>retry_period_ms: 10,000 milliseconds (10 seconds)</li>
         *   <li>update_rate_ms: 100 milliseconds</li>
         *   <li>min_measured: 50</li>
         * </ul>
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] exclusion_threshold Controls how much worse the latency must be compared to the
         * average latency of the best performing node before it penalized.
         * @param[in] scale_ms Controls the weight given to older latencies when calculating the average
         * latency of a node. A bigger scale will give more weight to older latency measurements.
         * @param[in] retry_period_ms The amount of time a node is penalized by the policy before
         * being given a second chance when the current average latency exceeds the calculated
         * threshold (exclusion_threshold * best_average_latency).
         * @param[in] update_rate_ms The rate at  which the best average latency is recomputed.
         * @param[in] min_measured The minimum number of measurements per-host required to
         * be considered by the policy.
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_latency_aware_routing_settings(_In_ w_cass_cluster* pCluster,
                _In_  double pExclusionThreshold,
                _In_ uint64_t pScaleMs,
                _In_  uint64_t pRetryPeriodMs,
                _In_ uint64_t pUpdateRateMs,
                _In_ uint64_t pMinMeasured);

        /**
         * Sets/Appends whitelist hosts. The first call sets the whitelist hosts and
         * any subsequent calls appends additional hosts. Passing an empty string will
         * clear and disable the whitelist. White space is striped from the hosts.
         *
         * This policy filters requests to all other policies, only allowing requests
         * to the hosts contained in the whitelist. Any host not in the whitelist will
         * be ignored and a connection will not be established. This policy is useful
         * for ensuring that the driver will only connect to a predefined set of hosts.
         *
         * Examples: "127.0.0.1" "127.0.0.1,127.0.0.2"
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] hosts A comma delimited list of addresses. An empty string will
         * clear the whitelist hosts. The string is copied into the cluster
         * configuration; the memory pointed to by this parameter can be freed after
         * this call.
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_whitelist_filtering(_In_ w_cass_cluster* pCluster,
                _In_ const char* pHosts);

        /**
         * Same as w_cassandra_cluster_set_whitelist_filtering(), but with lengths for
         * string parameters.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] hosts
         * @param[in] hosts_length
         * @return same as w_cassandra_cluster_set_whitelist_filtering()
         *
         * @see w_cassandra_cluster_set_whitelist_filtering()
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_whitelist_filtering_n(_In_ w_cass_cluster* pCluster,
                _In_ const char* pHosts,
                size_t pHostsLength);

        /**
         * Sets/Appends blacklist hosts. The first call sets the blacklist hosts and
         * any subsequent calls appends additional hosts. Passing an empty string will
         * clear and disable the blacklist. White space is striped from the hosts.
         *
         * This policy filters requests to all other policies, only allowing requests
         * to the hosts not contained in the blacklist. Any host in the blacklist will
         * be ignored and a connection will not be established. This policy is useful
         * for ensuring that the driver will not connect to a predefined set of hosts.
         *
         * Examples: "127.0.0.1" "127.0.0.1,127.0.0.2"
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] hosts A comma delimited list of addresses. An empty string will
         * clear the blacklist hosts. The string is copied into the cluster
         * configuration; the memory pointed to by this parameter can be freed after
         * this call.
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_blacklist_filtering(_In_ w_cass_cluster* pCluster,
                _In_  const char* pHosts);

        /**
         * Same as w_cassandra_cluster_set_blacklist_filtering_hosts(), but with lengths for
         * string parameters.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] hosts
         * @param[in] hosts_length
         * @return same as w_cassandra_cluster_set_blacklist_filtering()
         *
         * @see w_cassandra_cluster_set_blacklist_filtering()
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_blacklist_filtering_n(_In_ w_cass_cluster* pCluster, 
                _In_  const char* pHosts, 
                _In_ size_t pHostsLength);

        /**
         * Same as w_cassandra_cluster_set_whitelist_filtering(), but whitelist all hosts of a dc
         *
         * Examples: "dc1", "dc1,dc2"
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] dcs A comma delimited list of dcs. An empty string will clear the
         * whitelist dcs. The string is copied into the cluster configuration; the
         * memory pointed to by this parameter can be freed after this call.
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_whitelist_dc_filtering(_In_ w_cass_cluster* pCluster,
                _In_ const char* pDcs);

        /**
         * Same as w_cassandra_cluster_set_whitelist_dc_filtering(), but with lengths for
         * string parameters.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] dcs
         * @param[in] dcs_length
         * @return same as w_cassandra_cluster_set_whitelist_dc_filtering()
         *
         * @see w_cassandra_cluster_set_whitelist_dc_filtering()
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_whitelist_dc_filtering_n(_In_ w_cass_cluster* pCluster,
                _In_ const char* pDcs, 
                _In_ size_t pDcsLength);

        /**
         * Same as w_cassandra_cluster_set_blacklist_filtering(), but blacklist all hosts of a dc
         *
         * Examples: "dc1", "dc1,dc2"
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] dcs A comma delimited list of dcs. An empty string will clear the
         * blacklist dcs. The string is copied into the cluster configuration; the
         * memory pointed to by this parameter can be freed after this call.
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_blacklist_dc_filtering(_In_ w_cass_cluster* pCluster,
                _In_ const char* pDcs);

        /**
         * Same as w_cassandra_cluster_set_blacklist_dc_filtering(), but with lengths for
         * string parameters.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] dcs
         * @param[in] dcs_length
         * @return same as w_cassandra_cluster_set_blacklist_dc_filtering()
         *
         * @see w_cassandra_cluster_set_blacklist_dc_filtering()
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_blacklist_dc_filtering_n(_In_ w_cass_cluster* pCluster,
                _In_  const char* pDcs,
                _In_ size_t pDcsLength);

        /**
         * Enable/Disable Nagle's algorithm on connections.
         *
         * <b>Default:</b> cass_true (disables Nagle's algorithm).
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] enabled
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_tcp_nodelay(_In_ w_cass_cluster* pCluster,
                _In_  w_bool_t pEnabled);

        /**
         * Enable/Disable TCP keep-alive
         *
         * <b>Default:</b> cass_false (disabled).
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] enabled
         * @param[in] delay_secs The initial delay in seconds, ignored when
         * `enabled` is false.
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_tcp_keepalive(_In_ w_cass_cluster* pCluster,
                _In_ w_bool_t pEnabled,
                _In_ unsigned pDelaySecs);
        /**
         * Sets the timestamp generator used to assign timestamps to all requests
         * unless overridden by setting the timestamp on a statement or a batch.
         *
         * <b>Default:</b> Monotonically increasing, client-side timestamp generator.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] timestamp_gen
         *
         * @see w_cassandra_statement_set_timestamp()
         * @see w_cassandra_batch_set_timestamp()
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_timestamp_gen(_In_ w_cass_cluster* pCluster,
                _In_ w_cass_time_stamp_gen* pTimestampGen);

        /**
         * Sets the amount of time between heartbeat messages and controls the amount
         * of time the connection must be idle before sending heartbeat messages. This
         * is useful for preventing intermediate network devices from dropping
         * connections.
         *
         * <b>Default:</b> 30 seconds
         *
         * @public @memberof w_cassandra_Cluster
         *
         * @param[in] cluster
         * @param[in] interval_secs Use 0 to disable heartbeat messages
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_connection_heartbeat_interval(_In_ w_cass_cluster* pCluster,
                _In_ unsigned pIntervalSecs);

        /**
         * Sets the amount of time a connection is allowed to be without a successful
         * heartbeat response before being terminated and scheduled for reconnection.
         *
         * <b>Default:</b> 60 seconds
         *
         * @public @memberof w_cassandra_Cluster
         *
         * @param[in] cluster
         * @param[in] timeout_secs
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_connection_idle_timeout(_In_ w_cass_cluster* pCluster,
                _In_  unsigned pTimeoutSecs);

        /**
         * Sets the retry policy used for all requests unless overridden by setting
         * a retry policy on a statement or a batch.
         *
         * <b>Default:</b> The same policy as would be created by the function:
         * w_cassandra_retry_policy_default_new(). This policy will retry on a read timeout
         * if there was enough replicas, but no data present, on a write timeout if a
         * logged batch request failed to write the batch log, and on a unavailable
         * error it retries using a new host. In all other cases the default policy
         * will return an error.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] retry_policy
         *
         * @see w_cassandra_retry_policy_default_new()
         * @see w_cassandra_statement_set_retry_policy()
         * @see w_cassandra_batch_set_retry_policy()
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_retry_policy(_In_ w_cass_cluster* pCluster,
                _In_ w_cass_retry_policy* pRetryPolicy);

        /**
         * Enable/Disable retrieving and updating schema metadata. If disabled
         * this is allows the driver to skip over retrieving and updating schema
         * metadata and w_cassandra_session_get_schema_meta() will always return an empty object.
         * This can be useful for reducing the startup overhead of short-lived sessions.
         *
         * <b>Default:</b> cass_true (enabled).
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] enabled
         *
         * @see w_cassandra_session_get_schema_meta()
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_use_schema(_In_ w_cass_cluster* pCluster, 
                _In_ w_bool_t pEnabled);

        /**
         * Enable/Disable retrieving hostnames for IP addresses using reverse IP lookup.
         *
         * This is useful for authentication (Kerberos) or encryption (SSL) services
         * that require a valid hostname for verification.
         *
         * <b>Default:</b> cass_false (disabled).
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] enabled
         * @return W_SUCCESS if successful, otherwise an error occurred
         *
         * @see w_cassandra_cluster_set_resolve_timeout()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_use_hostname_resolution(_In_ w_cass_cluster* pCluster,
                _In_  w_bool_t pEnabled);

        /**
         * Enable/Disable the randomization of the contact points list.
         *
         * <b>Default:</b> cass_true (enabled).
         *
         * <b>Important:</b> This setting should only be disabled for debugging or
         * tests.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] enabled
         * @return W_SUCCESS if successful, otherwise an error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_use_randomized_contact_points(_In_ w_cass_cluster* pCluster,
                _In_  w_bool_t pEnabled);

        /**
         * Enable constant speculative executions with the supplied settings.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] constant_delay_ms
         * @param[in] max_speculative_executions
         * @return W_SUCCESS if successful, otherwise an error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_constant_speculative_execution_policy(_In_ w_cass_cluster* pCluster,
                _In_ int64_t pConstantDelayMs,
                _In_ int pMaxSpeculativeExecutions);

        /**
         * Disable speculative executions
         *
         * <b>Default:</b> This is the default speculative execution policy.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @return W_SUCCESS if successful, otherwise an error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_no_speculative_execution_policy(_In_ w_cass_cluster* pCluster);

        /**
         * Sets the maximum number of "pending write" objects that will be
         * saved for re-use for marshalling new requests. These objects may
         * hold on to a significant amount of memory and reducing the
         * number of these objects may reduce memory usage of the application.
         *
         * The cost of reducing the value of this setting is potentially slower
         * marshalling of requests prior to sending.
         *
         * <b>Default:</b> Max unsigned integer value
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] num_objects
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_max_reusable_write_objects(_In_ w_cass_cluster* pCluster,
                _In_  unsigned pNumObjects);

        /**
         * Associates a named execution profile which can be utilized during execution.
         *
         * <b>Note:</b> Once the execution profile is added to a cluster, it is
         * immutable and any changes made to the execution profile must be re-assigned
         * to the cluster before a session connection is established in order for those
         * settings to be utilized during query execution.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] name
         * @param[in] profile
         * @return W_SUCCESS if successful, otherwise an error occurred
         *
         * @see w_cassandra_batch_set_execution_profile()
         * @see w_cassandra_statement_set_execution_profile()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_execution_profile(_In_ w_cass_cluster* pCluster,
                _In_ const char* pName,
                _In_  w_cass_exec_profile* pProfile);

        /**
         * Same as w_cassandra_cluster_add_execution_profile(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] name
         * @param[in] name_length
         * @param[in] profile
         * @return same as cass_cluster_set_execution_profile()
         *
         * @see w_cassandra_batch_set_execution_profile()
         * @see w_cassandra_statement_set_execution_profile()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_execution_profile_n(_In_ w_cass_cluster* pCluster,
                _In_ const char* pName,
                _In_ size_t pNameLength,
                _In_ w_cass_exec_profile* pProfile);

        /**
         * Prepare statements on all available hosts.
         *
         * <b>Default:</b> cass_true
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param cluster
         * @param enabled
         * @return W_SUCCESS if successful, otherwise an error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_prepare_on_all_hosts(_In_ w_cass_cluster* pCluster,
                _In_ w_bool_t pEnabled);

        /**
         * Enable pre-preparing cached prepared statements when existing hosts become
         * available again or when new hosts are added to the cluster.
         *
         * This can help mitigate request latency when executing prepared statements
         * by avoiding an extra round trip in cases where the statement is
         * unprepared on a freshly started server. The main tradeoff is extra background
         * network traffic is required to prepare the statements on hosts as they become
         * available.
         *
         * <b>Default:</b> cass_true
         *
         * @param cluster
         * @param enabled
         * @return W_SUCCESS if successful, otherwise an error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_prepare_on_up_or_add_host(_In_ w_cass_cluster* pCluster,
                _In_ w_bool_t pEnabled);

        /**
         * Enable the <b>NO_COMPACT</b> startup option.
         *
         * This can help facilitate uninterrupted cluster upgrades where tables using
         * <b>COMPACT_STORAGE</b> will operate in "compatibility mode" for
         * <b>BATCH</b>, <b>DELETE</b>, <b>SELECT</b>, and <b>UPDATE</b> CQL operations.
         *
         * <b>Default:</b> cass_false
         *
         * @cassandra{3.0.16+}
         * @cassandra{3.11.2+}
         * @cassandra{4.0+}
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] enabled
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_no_compact(_In_ w_cass_cluster* pCluster,
                _In_ w_bool_t pEnabled);

        /**
         * Sets a callback for handling host state changes in the cluster.
         *
         * <b>Note:</b> The callback is invoked only when state changes in the cluster
         * are applicable to the configured load balancing policy(s).
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] callback
         * @param[in] data
         * @return W_SUCCESS if successful, otherwise and error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_host_listener_callback(_In_ w_cass_cluster* pCluster,
                _In_ w_cass_host_listener_callback pCallback,
                _In_  void* pData);

        /**
         * Sets the secure connection bundle path for processing DBaaS credentials.
         *
         * This will pre-configure a cluster using the credentials format provided by
         * the DBaaS cloud provider.
         *
         * @param[in] cluster
         * @param[in] path Absolute path to DBaaS credentials file.
         * @return W_SUCCESS if successful, otherwise error occured.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_cloud_secure_connection_bundle(_In_ w_cass_cluster* pCluster,
                _In_ const char* pPath);

        /**
         * Same as w_cassandra_cluster_set_cloud_secure_connection_bundle(), but with lengths
         * for string parameters.
         *
         * @see w_cassandra_cluster_set_cloud_secure_connection_bundle()
         *
         * @param[in] cluster
         * @param[in] path Absolute path to DBaaS credentials file.
         * @param[in] path_length Length of path variable.
         * @return W_SUCCESS if successful, otherwise error occured.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_cloud_secure_connection_bundle_n(_In_ w_cass_cluster* pCluster, 
                _In_ const char* pPath, 
                _In_ size_t pPathLength);

        /**
         * Same as w_cassandra_cluster_set_cloud_secure_connection_bundle(), but it does not
         * initialize the underlying SSL library implementation. The SSL library still
         * needs to be initialized, but it's up to the client application to handle
         * initialization. This is similar to the function w_cassandra_ssl_new_no_lib_init(),
         * and its documentation should be used as a reference to properly initialize
         * the underlying SSL library.
         *
         * @see w_cassandra_ssl_new_no_lib_init()
         * @see w_cassandra_cluster_set_cloud_secure_connection_bundle()
         *
         * @param[in] cluster
         * @param[in] path Absolute path to DBaaS credentials file.
         * @return W_SUCCESS if successful, otherwise error occured.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_cloud_secure_connection_bundle_no_ssl_lib_init(_In_ w_cass_cluster* pCluster,
                _In_ const char* pPath);

        /**
         * Same as w_cassandra_cluster_set_cloud_secure_connection_bundle_no_ssl_lib_init(),
         * but with lengths for string parameters.
         *
         * @see w_cassandra_cluster_set_cloud_secure_connection_bundle_no_ssl_lib_init()
         *
         * @param[in] cluster
         * @param[in] path Absolute path to DBaaS credentials file.
         * @param[in] path_length Length of path variable.
         * @return W_SUCCESS if successful, otherwise error occured.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_cluster_set_cloud_secure_connection_bundle_no_ssl_lib_init_n(_In_ w_cass_cluster* pCluster, 
                _In_  const char* pPath,
                _In_ size_t pPathLength);

        /**
         * Set the application name.
         *
         * This is optional; however it provides the server with the application name
         * that can aid in debugging issues with larger clusters where there are a lot
         * of client (or application) connections.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] application_name
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_application_name(_In_ w_cass_cluster* pCluster, 
                _In_ const char* pApplicationName);

        /**
         * Same as w_cassandra_cluster_set_application_name(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] application_name
         * @param[in] application_name_length
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_application_name_n(_In_ w_cass_cluster* pCluster, 
                _In_ const char* pAplicationName,
                _In_  size_t pApplicationNameLength);

        /**
         * Set the application version.
         *
         * This is optional; however it provides the server with the application
         * version that can aid in debugging issues with large clusters where there are
         * a lot of client (or application) connections that may have different
         * versions in use.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] application_version
         */

        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_application_version(_In_ w_cass_cluster* pCluster,
                _In_ const char* pApplicationVersion);

        /**
         * Same as w_cassandra_cluster_set_application_version(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] application_version
         * @param[in] application_version_length
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_application_version_n(_In_ w_cass_cluster* pCluster,
                _In_ const char* pApplicationVersion,
                _In_ size_t pApplicationVersionLength);

        /**
         * Set the client id.
         *
         * This is optional; however it provides the server with the client ID that can
         * aid in debugging issues with large clusters where there are a lot of client
         * connections.
         *
         * Default: UUID v4 generated (@see w_cassandra_session_get_client_id())
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] client_id
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_client_id(_In_ w_cass_cluster* pCluster, 
                _In_ w_cass_uuid pClientId);

        /**
         * Sets the amount of time between monitor reporting event messages.
         *
         * <b>Default:</b> 300 seconds.
         *
         * @public @memberof w_Cass_Cluster
         *
         * @param[in] cluster
         * @param[in] interval_secs Use 0 to disable monitor reporting event messages.
         */
        W_SYSTEM_EXPORT void
            w_cassandra_cluster_set_monitor_reporting_interval(_In_ w_cass_cluster* pCluster,
                _In_   unsigned pIntervalSecs);

        /***********************************************************************************
         *
         * Session
         *
         ***********************************************************************************/

         /**
          * Creates a new session.
          *
          * @public @memberof w_Cass_Session
          *
          * @return Returns a session that must be freed.
          *
          * @see w_cassandra_session_free()
          */
        W_SYSTEM_EXPORT w_cass_session*
            w_cassandra_session_new();

        /**
         * Frees a session instance. If the session is still connected it will be synchronously
         * closed before being deallocated.
         *
         * Important: Do not free a session in a future callback. Freeing a session in a future
         * callback will cause a deadlock.
         *
         * @public @memberof w_Cass_Session
         *
         * @param[in] session
         */
        W_SYSTEM_EXPORT void
            w_cassandra_session_free(_In_ w_cass_session* pSession);

        /**
         * Connects a session.
         *
         * @public @memberof w_Cass_Session
         *
         * @param[in] session
         * @param[in] cluster The cluster configuration is copied into the session and
         * is immutable after connection.
         * @return A future that must be freed.
         *
         * @see w_cassandra_session_close()
         */
        W_SYSTEM_EXPORT w_cass_future* w_cassandra_session_connect(_In_ w_cass_session* pSession,
            _In_ const w_cass_cluster* pCluster);

        /**
         * Connects a session and sets the keyspace.
         *
         * @public @memberof w_Cass_Session
         *
         * @param[in] session
         * @param[in] cluster The cluster configuration is copied into the session and
         * is immutable after connection.
         * @param[in] keyspace
         * @return A future that must be freed.
         *
         * @see w_cassandra_session_close()
         */
        W_SYSTEM_EXPORT w_cass_future*
            w_cassandra_session_connect_keyspace(_In_ w_cass_session* pSession,
            _In_ const w_cass_cluster* pCluster,
            _In_ const char* pKeyspace);

        /**
         * Same as w_cassandra_session_connect_keyspace(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Session
         *
         * @param[in] session
         * @param[in] cluster
         * @param[in] keyspace
         * @param[in] keyspace_length
         * @return same as w_cassandra_session_connect_keyspace()
         *
         * @see w_cassandra_session_connect_keyspace()
         */
        W_SYSTEM_EXPORT w_cass_future*
            w_cassandra_session_connect_keyspace_n(_In_ w_cass_session* pSession, 
            _In_ const w_cass_cluster* pCluster,
            _In_  const char* pKeyspace, 
            _In_ size_t pKeyspaceLength);

        /**
         * Closes the session instance, outputs a close future which can
         * be used to determine when the session has been terminated. This allows
         * in-flight requests to finish.
         *
         * @public @memberof w_Cass_Session
         *
         * @param[in] session
         * @return A future that must be freed.
         */
        W_SYSTEM_EXPORT w_cass_future* 
            w_cassandra_session_close(_In_ w_cass_session* pSession);

        /**
         * Create a prepared statement.
         *
         * @public @memberof w_Cass_Session
         *
         * @param[in] session
         * @param[in] query The query is copied into the statement object; the
         * memory pointed to by this parameter can be freed after this call.
         * @return A future that must be freed.
         *
         * @see w_cassandra_future_get_prepared()
         */
        W_SYSTEM_EXPORT w_cass_future* 
            w_cassandra_session_prepare(_In_ w_cass_session* pSession,
                _In_ const char* pQuery);

        /**
         * Same as w_cassandra_session_prepare(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Session
         *
         * @param[in] session
         * @param[in] query
         * @param[in] query_length
         * @return same as w_cassandra_session_prepare()
         *
         * @see w_cassandra_session_prepare()
         */
        W_SYSTEM_EXPORT w_cass_future* 
            w_cassandra_session_prepare_n(_In_ w_cass_session* pSession,
            _In_  const char* pQuery,
                _In_  size_t pQueryLength);

        /**
         * Create a prepared statement from an existing statement.
         *
         * <b>Note:</b> Bound statements will inherit the keyspace, consistency,
         * serial consistency, request timeout and retry policy of the existing
         * statement.
         *
         * @public @memberof w_Cass_Session
         *
         * @param[in] session
         * @param[in] statement
         * @return A future that must be freed.
         *
         * @see w_cassandra_future_get_prepared()
         */
        W_SYSTEM_EXPORT w_cass_future*
            w_cassandra_session_prepare_from_existing(_In_ w_cass_session* pSession,
            _In_ w_cass_statement* pStatement);

        /**
         * Execute a query or bound statement.
         *
         * @public @memberof w_Cass_Session
         *
         * @param[in] session
         * @param[in] statement
         * @return A future that must be freed.
         *
         * @see w_cassandra_future_get_result()
         */
        W_SYSTEM_EXPORT w_cass_future* 
            w_cassandra_session_execute(_In_ w_cass_session* pSession,
            _In_  const w_cass_statement* pStatement);

        /**
         * Execute a batch statement.
         *
         * @cassandra{2.0+}
         *
         * @public @memberof w_Cass_Session
         *
         * @param[in] session
         * @param[in] batch
         * @return A future that must be freed.
         *
         * @see w_cassandra_future_get_result()
         */
        W_SYSTEM_EXPORT w_cass_future* 
            w_cassandra_session_execute_batch(_In_ w_cass_session* pSession,
            _In_ const w_cass_batch* pBatch);

        /**
         * Gets a snapshot of this session's schema metadata. The returned
         * snapshot of the schema metadata is not updated. This function
         * must be called again to retrieve any schema changes since the
         * previous call.
         *
         * @public @memberof w_Cass_Session
         *
         * @param[in] session
         * @return A schema instance that must be freed.
         *
         * @see w_cassandra_schema_meta_free()
         */
        W_SYSTEM_EXPORT const  w_cass_schema_meta* 
            w_cassandra_session_get_schema_meta(_In_ const w_cass_session* pSession);

        /**
         * Gets a copy of this session's performance/diagnostic metrics.
         *
         * @public @memberof w_Cass_Session
         *
         * @param[in] session
         * @param[out] output
         */
        W_SYSTEM_EXPORT void
            w_cassandra_session_get_metrics(_In_ const  w_cass_session* pSession,
                _Inout_ w_cass_metrics* pOutput);

        /**
         * Gets a copy of this session's speculative execution metrics.
         *
         * @public @memberof w_Cass_Session
         *
         * @param[in] session
         * @param[out] output
         */
        W_SYSTEM_EXPORT void
            w_cassandra_session_get_speculative_execution_metrics(_In_ const w_cass_session* pSession,
                _Inout_ w_cass_speculative_execution_metrics* pOutput);

        /**
         * Get the client id.
         *
         * @public @memberof w_Cass_Session
         *
         * @param[in] session
         * @return Client id.
         */
        W_SYSTEM_EXPORT w_cass_uuid
            w_cassandra_session_get_client_id(_In_ w_cass_session* pSession);

        /***********************************************************************************
         *
         * Schema Metadata
         *
         ***********************************************************************************/

         /**
          * Frees a schema metadata instance.
          *
          * @public @memberof w_Cass_Schema_Meta
          *
          * @param[in] schema_meta
          */
        W_SYSTEM_EXPORT void
            w_cassandra_schema_meta_free(_In_ const w_cass_schema_meta* pSchemaMeta);

        /**
         * Gets the version of the schema metadata snapshot.
         *
         * @public @memberof w_Cass_Schema_Meta
         *
         * @param[in] schema_meta
         *
         * @return The snapshot version.
         */
        W_SYSTEM_EXPORT uint32_t
            w_cassandra_schema_meta_snapshot_version(_In_ const w_cass_schema_meta* pSchemaMeta);

        /**
         * Gets the version of the connected Cassandra cluster.
         *
         * @public @memberof w_Cass_Schema_Meta
         *
         * @param[in] schema_meta
         *
         * @return Cassandra's version
         */
        W_SYSTEM_EXPORT w_cass_version 
            w_cassandra_schema_meta_version(_In_ const w_cass_schema_meta* pSchemaMeta);

        /**
         * Gets the keyspace metadata for the provided keyspace name.
         *
         * @public @memberof w_Cass_Schema_Meta
         *
         * @param[in] schema_meta
         * @param[in] keyspace
         *
         * @return The metadata for a keyspace. NULL if keyspace does not exist.
         */
        W_SYSTEM_EXPORT const w_cass_key_space_meta* 
            w_cassandra_schema_meta_keyspace_by_name(_In_ const w_cass_schema_meta* pSchemaMeta,
            _In_ const char* pKeyspace);

        /**
         * Same as w_cassandra_schema_meta_keyspace_by_name(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Schema_Meta
         *
         * @param[in] schema_meta
         * @param[in] keyspace
         * @param[in] keyspace_length
         * @return same as w_cassandra_schema_meta_keyspace_by_name()
         *
         * @see w_cassandra_schema_meta_keyspace_by_name()
         */
        W_SYSTEM_EXPORT const w_cass_key_space_meta*
            w_cassandra_schema_meta_keyspace_by_name_n(_In_ const w_cass_schema_meta* pSchemaMeta,
            _In_ const char* pKeyspace,
                _In_ size_t pKeyspaceLength);

        /**
         * Gets the name of the keyspace.
         *
         * @public @memberof w_Cass_Key_space_Meta
         *
         * @param[in] keyspace_meta
         * @param[out] name
         * @param[out] name_length
         */
        W_SYSTEM_EXPORT void
            w_cassandra_keyspace_meta_name(_In_ const w_cass_key_space_meta* pSchemaMeta,
                _In_ const char** pName,
                _In_ size_t* pNameLength);


        /**
         * Determine if the keyspace is a virtual keyspace.
         *
         * @public @memberof w_Cass_Key_space_Meta
         *
         * @param[in] keyspace_meta
         * @return cass_true is the keyspace is virtual, otherwise cass_false
         */
        W_SYSTEM_EXPORT w_bool_t
            w_cassandra_keyspace_meta_is_virtual(_In_ const w_cass_key_space_meta* pKeyspaceMeta);

        /**
         * Gets the table metadata for the provided table name.
         *
         * @public @memberof w_Cass_Key_space_Meta
         *
         * @param[in] keyspace_meta
         * @param[in] table
         *
         * @return The metadata for a table. NULL if table does not exist.
         */
        W_SYSTEM_EXPORT const w_cass_table_meta*
            w_cassandra_keyspace_meta_table_by_name(_In_ const w_cass_key_space_meta* pKeyspaceMeta, 
            _In_ const char* pTable);

        /**
         * Same as w_cassandra_keyspace_meta_table_by_name(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Key_space_Meta
         *
         * @param[in] keyspace_meta
         * @param[in] table
         * @param[in] table_length
         * @return same as w_cassandra_keyspace_meta_table_by_name()
         *
         * @see w_cassandra_keyspace_meta_table_by_name()
         */
        W_SYSTEM_EXPORT const w_cass_value* 
            w_cassandra_keyspace_meta_field_by_name_n(const w_cass_key_space_meta* pKeyspaceMeta,
            const char* pName,
            size_t pNameLength);

        /**
         * Gets the materialized view metadata for the provided view name.
         *
         * @cassandra{3.0+}
         *
         * @public @memberof w_Cass_Key_space_Meta
         *
         * @param[in] keyspace_meta
         * @param[in] view
         *
         * @return The metadata for a view. NULL if view does not exist.
         */
        W_SYSTEM_EXPORT const w_cass_materialized_view_meta* 
            w_cassandra_keyspace_meta_materialized_view_by_name(_In_ const w_cass_key_space_meta* pKeyspaceMeta,
            _In_ const char* pView);

        /**
         * Same as w_cassandra_keyspace_meta_materialized_view_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{3.0+}
         *
         * @public @memberof w_Cass_Key_space_Meta
         *
         * @param[in] keyspace_meta
         * @param[in] view
         * @param[in] view_length
         * @return same as w_cassandra_keyspace_meta_materialized_view_by_name()
         *
         * @see w_cassandra_keyspace_meta_materialized_view_by_name()
         */
        W_SYSTEM_EXPORT const w_cass_materialized_view_meta*
            w_cassandra_keyspace_meta_materialized_view_by_name_n(_In_ const w_cass_key_space_meta* pKeyspaceMeta, 
                _In_ const char* pView,
            _In_  size_t pViewLength);

        /**
         * Gets the data type for the provided type name.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Key_space_Meta
         *
         * @param[in] keyspace_meta
         * @param[in] type
         *
         * @return The data type for a user defined type. NULL if type does not exist.
         */
        W_SYSTEM_EXPORT const w_cass_data_type* 
            w_cassandra_keyspace_meta_user_type_by_name(_In_ const w_cass_key_space_meta* pKeyspaceMeta,
            _In_  const char* pType);

        /**
         * Same as w_cassandra_keyspace_meta_type_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Key_space_Meta
         *
         * @param[in] keyspace_meta
         * @param[in] type
         * @param[in] type_length
         * @return same as w_cassandra_keyspace_meta_type_by_name()
         *
         * @see w_cassandra_keyspace_meta_type_by_name()
         */
        W_SYSTEM_EXPORT const w_cass_data_type* 
            w_cassandra_keyspace_meta_user_type_by_name_n(_In_ const w_cass_key_space_meta* pKeyspaceMeta,
            _In_ const char* pType,
            _In_ size_t pTypelength);

        /**
         * Gets the function metadata for the provided function name.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Key_space_Meta
         *
         * @param[in] keyspace_meta
         * @param[in] name
         * @param[in] arguments A comma delimited list of CQL types (e.g "text,int,...")
         * describing the function's signature.
         *
         * @return The data function for a user defined function. NULL if function does not exist.
         */
        W_SYSTEM_EXPORT const w_cass_function_meta* w_cassandra_keyspace_meta_function_by_name(_In_ const  w_cass_key_space_meta* pKeyspaceMeta,
            _In_ const char* pName, 
            _In_ const char* pArguments);

        /**
         * Same as w_cassandra_keyspace_meta_function_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Key_space_Meta
         *
         * @param[in] keyspace_meta
         * @param[in] name
         * @param[in] name_length
         * @param[in] arguments
         * @param[in] arguments_length
         * @return same as w_cassandra_keyspace_meta_function_by_name()
         *
         * @see w_cassandra_keyspace_meta_function_by_name()
         */
        W_SYSTEM_EXPORT const w_cass_function_meta* w_cassandra_keyspace_meta_function_by_name_n(_In_ const  w_cass_key_space_meta* pKeyspaceMeta,
            _In_ const char* pName,
            _In_ size_t pNameLength,
            _In_ const char* pArguments, 
            _In_ size_t pArgumentsLength);

        /**
         * Gets the aggregate metadata for the provided aggregate name.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Key_space_Meta
         *
         * @param[in] keyspace_meta
         * @param[in] name
         * @param[in] arguments A comma delimited list of CQL types (e.g "text,int,...")
         * describing the aggregate's signature.
         *
         * @return The data aggregate for a user defined aggregate. NULL if aggregate does not exist.
         */
        W_SYSTEM_EXPORT const w_cass_aggregate_meta* w_cassandra_keyspace_meta_aggregate_by_name(_In_ const w_cass_key_space_meta* pKeyspaceMeta,
            _In_ const char* pName,
            _In_ const char* pArguments);

        /**
         * Same as w_cassandra_keyspace_meta_aggregate_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Key_space_Meta
         *
         * @param[in] keyspace_meta
         * @param[in] name
         * @param[in] name_length
         * @param[in] arguments
         * @param[in] arguments_length
         * @return same as w_cassandra_keyspace_meta_aggregate_by_name()
         *
         * @see w_cassandra_keyspace_meta_aggregate_by_name()
         */
        W_SYSTEM_EXPORT const w_cass_aggregate_meta* w_cassandra_keyspace_meta_aggregate_by_name_n(_In_ const w_cass_key_space_meta* pKeyspaceMeta,
            _In_ const char* pName,
            _In_ size_t pNameLength,
            _In_  const char* pArguments,
            _In_ size_t pArgumentsLength);

        /**
         * Gets a metadata field for the provided name. Metadata fields allow direct
         * access to the column data found in the underlying "keyspaces" metadata table.
         *
         * @public @memberof w_Cass_Key_space_Meta
         *
         * @param[in] keyspace_meta
         * @param[in] name
         * @return A metadata field value. NULL if the field does not exist.
         */
        W_SYSTEM_EXPORT const w_cass_value* w_cassandra_keyspace_meta_field_by_name(_In_ const w_cass_key_space_meta* pKeyspaceMeta, 
            _In_  const char* pName);

        /**
         * Same as w_cassandra_keyspace_meta_field_by_name(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Key_space_Meta
         *
         * @param[in] keyspace_meta
         * @param[in] name
         * @param[in] name_length
         * @return same as w_cassandra_keyspace_meta_field_by_name()
         *
         * @see w_cassandra_keyspace_meta_field_by_name()
         */
        W_SYSTEM_EXPORT const w_cass_value* w_cassandra_keyspace_meta_field_by_name_n(_In_ const w_cass_key_space_meta* pKeyspaceMeta, 
            _In_ const char* pName,
            _In_ size_t pNameLength);

        /**
         * Gets the name of the table.
         *
         * @public @memberof w_Cass_Table_Meta
         *
         * @param[in] table_meta
         * @param[out] name
         * @param[out] name_length
         */
        W_SYSTEM_EXPORT void
            w_cassandra_table_meta_name(_In_ const w_cass_table_meta* pTableMeta,
                _Inout_ const char** pName,
                _Inout_ size_t* pNameLength);

        /**
         * Determine if the table is a virtual table.
         *
         * @public @memberof w_Cass_Table_Meta
         *
         * @param[in] table_meta
         * @return w_cassandra_true is the table is virtual, otherwise cass_false
         */
        W_SYSTEM_EXPORT w_bool_t
            w_cassandra_table_meta_is_virtual(_In_ const w_cass_table_meta* pTableMeta);

        /**
         * Gets the column metadata for the provided column name.
         *
         * @public @memberof w_Cass_Table_Meta
         *
         * @param[in] table_meta
         * @param[in] column
         *
         * @return The metadata for a column. NULL if column does not exist.
         */
        W_SYSTEM_EXPORT const w_cass_column_meta* 
            w_cassandra_table_meta_column_by_name(_In_ const w_cass_table_meta* pTableMeta,
                _In_ const char* pColumn);

        /**
         * Same as w_cassandra_table_meta_column_by_name(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Table_Meta
         *
         * @param[in] table_meta
         * @param[in] column
         * @param[in] column_length
         * @return same as w_cassandra_table_meta_column_by_name()
         *
         * @see w_cassandra_table_meta_column_by_name()
         */
        W_SYSTEM_EXPORT const w_cass_column_meta* 
            w_cassandra_table_meta_column_by_name_n(_In_ const  w_cass_table_meta* pTableMeta,
            _In_ const char* pColumn,
            _In_ size_t pColumnLength);

        /**
         * Gets the total number of columns for the table.
         *
         * @public @memberof w_Cass_Table_Meta
         *
         * @param[in] table_meta
         * @return The total column count.
         */
        W_SYSTEM_EXPORT size_t
            w_cassandra_table_meta_column_count(_In_ const w_cass_table_meta* pTableMeta);

        /**
         * Gets the column metadata for the provided index.
         *
         * @public @memberof w_Cass_Table_Meta
         *
         * @param[in] table_meta
         * @param[in] index
         * @return The metadata for a column. NULL returned if the index is out of range.
         */
        W_SYSTEM_EXPORT const w_cass_column_meta*
            w_cassandra_table_meta_column(_In_ const w_cass_table_meta* pTableMeta,
            _In_  size_t pIndex);

        /**
         * Gets the index metadata for the provided index name.
         *
         * @public @memberof w_Cass_Table_Meta
         *
         * @param[in] table_meta
         * @param[in] index
         *
         * @return The metadata for a index. NULL if index does not exist.
         */
        W_SYSTEM_EXPORT const w_cass_index_meta* 
            w_cassandra_table_meta_index_by_name(_In_ const w_cass_table_meta* pTableMeta,
            _In_ const char* pIndex);

        /**
         * Same as w_cassandra_table_meta_index_by_name(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Table_Meta
         *
         * @param[in] table_meta
         * @param[in] index
         * @param[in] index_length
         * @return same as w_cassandra_table_meta_index_by_name()
         *
         * @see w_cassandra_table_meta_index_by_name()
         */
        W_SYSTEM_EXPORT const w_cass_index_meta*
            w_cassandra_table_meta_index_by_name_n(_In_ const w_cass_table_meta* pTableMeta, 
            _In_ const char* pIndex,
            _In_  size_t pIndexLength);

        /**
         * Gets the total number of indexes for the table.
         *
         * @public @memberof w_Cass_Table_Meta
         *
         * @param[in] table_meta
         * @return The total index count.
         */
        W_SYSTEM_EXPORT size_t
            w_cassandra_table_meta_index_count(_In_ const w_cass_table_meta* pTableMeta);

        /**
         * Gets the index metadata for the provided index.
         *
         * @public @memberof w_Cass_Table_Meta
         *
         * @param[in] table_meta
         * @param[in] index
         * @return The metadata for a index. NULL returned if the index is out of range.
         */
        W_SYSTEM_EXPORT const w_cass_index_meta*
            w_cassandra_table_meta_index(_In_ const w_cass_table_meta* pTableMeta,
            _In_ size_t pIndex);

        /**
         * Gets the materialized view metadata for the provided view name.
         *
         * @cassandra{3.0+}
         *
         * @public @memberof w_Cass_Table_Meta
         *
         * @param[in] table_meta
         * @param[in] view
         *
         * @return The metadata for a view. NULL if view does not exist.
         */
        W_SYSTEM_EXPORT const w_cass_materialized_view_meta*
            w_cassandra_table_meta_materialized_view_by_name(_In_ const w_cass_table_meta* pTableMeta,
            _In_ const char* pView);

        /**
         * Same as w_cassandra_table_meta_materialized_view_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{3.0+}
         *
         * @public @memberof w_Cass_Table_Meta
         *
         * @param[in] table_meta
         * @param[in] view
         * @param[in] view_length
         * @return same as w_cassandra_table_meta_materialized_view_by_name()
         *
         * @see w_cassandra_table_meta_materialized_view_by_name()
         */
        W_SYSTEM_EXPORT const w_cass_materialized_view_meta* 
            w_cassandra_table_meta_materialized_view_by_name_n(_In_ const w_cass_table_meta* pTableMeta, 
            _In_ const char* pView,
                _In_ size_t pViewLength);

        /**
         * Gets the total number of views for the table.
         *
         * @cassandra{3.0+}
         *
         * @public @memberof w_Cass_Table_Meta
         *
         * @param[in] table_meta
         * @return The total view count.
         */
        W_SYSTEM_EXPORT size_t
            w_cassandra_table_meta_materialized_view_count(_In_ const w_cass_table_meta* pTableMeta);

        /**
         * Gets the materialized view metadata for the provided index.
         *
         * @cassandra{3.0+}
         *
         * @public @memberof w_Cass_Table_Meta
         *
         * @param[in] table_meta
         * @param[in] index
         * @return The metadata for a view. NULL returned if the index is out of range.
         */
        W_SYSTEM_EXPORT const w_cass_materialized_view_meta* 
            w_cassandra_table_meta_materialized_view(_In_ const w_cass_table_meta* pTableMeta,
            _In_ size_t pIndex);

        /**
         * Gets the number of columns for the table's partition key.
         *
         * @public @memberof w_Cass_Table_Meta
         *
         * @param[in] table_meta
         * @return The count for the number of columns in the partition key.
         */
        W_SYSTEM_EXPORT size_t
            w_cassandra_table_meta_partition_key_count(_In_ const  w_cass_table_meta* pTableMeta);

        /**
         * Gets the partition key column metadata for the provided index.
         *
         * @public @memberof w_Cass_Table_Meta
         *
         * @param[in] table_meta
         * @param[in] index
         * @return The metadata for a column. NULL returned if the index is out of range.
         *
         * @see w_cassandra_table_meta_partition_key_count()
         */
        W_SYSTEM_EXPORT const w_cass_column_meta*
            w_cassandra_table_meta_partition_key(_In_ const w_cass_table_meta* pTableMeta,
            _In_ size_t pIndex);

        /**
         * Gets the number of columns for the table's clustering key.
         *
         * @public @memberof w_Cass_Table_Meta
         *
         * @param[in] table_meta
         * @return The count for the number of columns in the clustering key.
         */
        W_SYSTEM_EXPORT size_t
            w_cassandra_table_meta_clustering_key_count(_In_ const w_cass_table_meta* pTableMeta);

        /**
         * Gets the clustering key column metadata for the provided index.
         *
         * @public @memberof w_Cass_Table_Meta
         *
         * @param[in] table_meta
         * @param[in] index
         * @return The metadata for a column. NULL returned if the index is out of range.
         *
         * @see w_cassandra_table_meta_clustering_key_count()
         */
        W_SYSTEM_EXPORT const w_cass_column_meta* 
            w_cassandra_table_meta_clustering_key(_In_ const w_cass_table_meta* pTableMeta,
            _In_ size_t pIndex);

        /**
         * Gets the clustering order column metadata for the provided index.
         *
         * @public @memberof w_Cass_Table_Meta
         *
         * @param[in] table_meta
         * @param[in] index
         * @return The clustering order for a column.
         * CASS_CLUSTERING_ORDER_NONE returned if the index is out of range.
         *
         * @see w_cassandra_table_meta_clustering_key_count()
         */
        W_SYSTEM_EXPORT w_cass_clustering_order 
            w_cassandra_table_meta_clustering_key_order(_In_ const w_cass_table_meta* pTableMeta,
            _In_ size_t pIndex);

        /**
         * Gets a metadata field for the provided name. Metadata fields allow direct
         * access to the column data found in the underlying "tables" metadata table.
         *
         * @public @memberof w_Cass_Table_Meta
         *
         * @param[in] table_meta
         * @param[in] name
         * @return A metadata field value. NULL if the field does not exist.
         */
        W_SYSTEM_EXPORT const w_cass_value* 
            w_cassandra_table_meta_field_by_name(_In_ const w_cass_table_meta* pTableMeta,
            _In_ const char* pName);

        /**
         * Same as w_cassandra_table_meta_field_by_name(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Table_Meta
         *
         * @param[in] table_meta
         * @param[in] name
         * @param[in] name_length
         * @return same as w_cassandra_table_meta_field_by_name()
         *
         * @see w_cassandra_table_meta_field_by_name()
         */
        W_SYSTEM_EXPORT const w_cass_value* 
            w_cassandra_table_meta_field_by_name_n(_In_ const w_cass_table_meta* pTableMeta,
            _In_ const char* pName,
            _In_ size_t pNameLength);

        /**
         * Gets the column metadata for the provided column name.
         *
         * @cassandra{3.0+}
         *
         * @public @memberof w_Cass_Materialized_View_Meta
         *
         * @param[in] view_meta
         * @param[in] column
         *
         * @return The metadata for a column. NULL if column does not exist.
         */
        W_SYSTEM_EXPORT const w_cass_column_meta*
            w_cassandra_materialized_view_meta_column_by_name(_In_ const w_cass_materialized_view_meta* pViewMeta, 
            _In_ const char* pColumn);

        /**
         * Same as w_cassandra_materialized_view_meta_column_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{3.0+}
         *
         * @public @memberof w_Cass_Materialized_View_Meta
         *
         * @param[in] view_meta
         * @param[in] column
         * @param[in] column_length
         * @return same as w_cassandra_materialized_view_meta_column_by_name()
         *
         * @see w_cassandra_materialized_view_meta_column_by_name()
         */
        W_SYSTEM_EXPORT const w_cass_column_meta*
            w_cassandra_materialized_view_meta_column_by_name_n(_In_ const w_cass_materialized_view_meta* pViewMeta, 
            _In_ const char* pColumn,
            _In_ size_t pColumnLength);

        /**
         * Gets the name of the view.
         *
         * @cassandra{3.0+}
         *
         * @public @memberof w_Cass_Materialized_View_Meta
         *
         * @param[in] view_meta
         * @param[out] name
         * @param[out] name_length
         */
        W_SYSTEM_EXPORT void
            w_cassandra_materialized_view_meta_name(_In_ const w_cass_materialized_view_meta* pViewMeta,
                _Inout_ const char** pName,
                _Inout_ size_t* pNameLength) ;

        /**
         * Gets the base table of the view.
         *
         * @cassandra{3.0+}
         *
         * @public @memberof w_Cass_Materialized_View_Meta
         *
         * @param[in] view_meta
         *
         * @return The base table for the view.
         */
        W_SYSTEM_EXPORT const w_cass_table_meta*
            w_cassandra_materialized_view_meta_base_table(_In_ const w_cass_materialized_view_meta* pViewMeta);

        /**
         * Gets the total number of columns for the view.
         *
         * @cassandra{3.0+}
         *
         * @public @memberof w_Cass_Materialized_View_Meta
         *
         * @param[in] view_meta
         * @return The total column count.
         */
        W_SYSTEM_EXPORT size_t
            w_cassandra_materialized_view_meta_column_count(_In_ const w_cass_materialized_view_meta* pViewMeta);

        /**
         * Gets the column metadata for the provided index.
         *
         * @cassandra{3.0+}
         *
         * @public @memberof w_Cass_Materialized_View_Meta
         *
         * @param[in] view_meta
         * @param[in] index
         * @return The metadata for a column. NULL returned if the index is out of range.
         */
        W_SYSTEM_EXPORT const w_cass_column_meta* 
            w_cassandra_materialized_view_meta_column(_In_ const w_cass_materialized_view_meta* pViewMeta,
            _In_ size_t pIndex);

        /**
         * Gets the number of columns for the view's partition key.
         *
         * @cassandra{3.0+}
         *
         * @public @memberof w_Cass_Materialized_View_Meta
         *
         * @param[in] view_meta
         * @return The count for the number of columns in the partition key.
         */
        W_SYSTEM_EXPORT size_t
            w_cassandra_materialized_view_meta_partition_key_count(_In_ const w_cass_materialized_view_meta* pViewMeta);

        /**
         * Gets the partition key column metadata for the provided index.
         *
         * @cassandra{3.0+}
         *
         * @public @memberof w_Cass_Materialized_View_Meta
         *
         * @param[in] view_meta
         * @param[in] index
         * @return The metadata for a column. NULL returned if the index is out of range.
         */
        W_SYSTEM_EXPORT const w_cass_column_meta* 
            w_cassandra_materialized_view_meta_partition_key(_In_ const w_cass_materialized_view_meta* pViewMeta,
            _In_ size_t pIndex);

        /**
         * Gets the number of columns for the view's clustering key.
         *
         * @cassandra{3.0+}
         *
         * @public @memberof w_Cass_Materialized_View_Meta
         *
         * @param[in] view_meta
         * @return The count for the number of columns in the clustering key.
         */
        W_SYSTEM_EXPORT size_t
            w_cassandra_materialized_view_meta_clustering_key_count(_In_ const w_cass_materialized_view_meta* pViewMeta);

        /**
         * Gets the clustering key column metadata for the provided index.
         *
         * @cassandra{3.0+}
         *
         * @public @memberof w_Cass_Materialized_View_Meta
         *
         * @param[in] view_meta
         * @param[in] index
         * @return The metadata for a column. NULL returned if the index is out of range.
         */
        W_SYSTEM_EXPORT const w_cass_column_meta* 
            w_cassandra_materialized_view_meta_clustering_key(_In_ const w_cass_materialized_view_meta* pViewMeta,
            _In_ size_t pIndex);

        /**
         * Gets the clustering order column metadata for the provided index.
         *
         * @public @memberof w_Cass_Materialized_View_Meta
         *
         * @param[in] view_meta
         * @param[in] index
         * @return The clustering order for a column.
         * CASS_CLUSTERING_ORDER_NONE returned if the index is out of range.
         *
         * @see w_cassandra_materialized_view_meta_clustering_key_count()
         */
        W_SYSTEM_EXPORT  w_cass_clustering_order 
            w_cassandra_materialized_view_meta_clustering_key_order(_In_ const w_cass_materialized_view_meta* pViewMeta, 
                _In_ size_t pIndex) ;

        /**
         * Gets a metadata field for the provided name. Metadata fields allow direct
         * access to the column data found in the underlying "views" metadata view.
         *
         * @cassandra{3.0+}
         *
         * @public @memberof w_Cass_Materialized_View_Meta
         *
         * @param[in] view_meta
         * @param[in] name
         * @return A metadata field value. NULL if the field does not exist.
         */
        W_SYSTEM_EXPORT const w_cass_value* 
            w_cassandra_materialized_view_meta_field_by_name(_In_ const w_cass_materialized_view_meta* pViewMeta,
            _In_ const char* pName);

        /**
         * Same as w_cassandra_materialized_view_meta_field_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{3.0+}
         *
         * @public @memberof w_Cass_Materialized_View_Meta
         *
         * @param[in] view_meta
         * @param[in] name
         * @param[in] name_length
         * @return same as w_cassandra_materialized_view_meta_field_by_name()
         *
         * @see w_cassandra_materialized_view_meta_field_by_name()
         */
        W_SYSTEM_EXPORT const w_cass_value* 
            w_cassandra_materialized_view_meta_field_by_name_n(_In_ const w_cass_materialized_view_meta* pViewMeta, 
            _In_ const char* pName,
            _In_ size_t pNameLength);

        /**
         * Gets the name of the column.
         *
         * @public @memberof w_Cass_Column_Meta
         *
         * @param[in] column_meta
         * @param[out] name
         * @param[out] name_length
         */
        W_SYSTEM_EXPORT void
            w_cassandra_column_meta_name(_In_ const w_cass_column_meta* pColumnMeta,
                _Inout_ const char** pName, 
                _Inout_ size_t* pNameLength) ;

        /**
         * Gets the type of the column.
         *
         * @public @memberof w_Cass_Column_Meta
         *
         * @param[in] column_meta
         * @return The column's type.
         */
        W_SYSTEM_EXPORT w_cass_column_type
            w_cassandra_column_meta_type(_In_ const w_cass_column_meta* pColumnMeta);

        /**
         * Gets the data type of the column.
         *
         * @public @memberof w_Cass_Column_Meta
         *
         * @param[in] column_meta
         * @return The column's data type.
         */
        W_SYSTEM_EXPORT const w_cass_data_type* 
            w_cassandra_column_meta_data_type(_In_ const w_cass_column_meta* pColumnMeta);

        /**
         * Gets a metadata field for the provided name. Metadata fields allow direct
         * access to the column data found in the underlying "columns" metadata table.
         *
         * @public @memberof w_Cass_Column_Meta
         *
         * @param[in] column_meta
         * @param[in] name
         * @return A metadata field value. NULL if the field does not exist.
         */
        W_SYSTEM_EXPORT const w_cass_value* 
            w_cassandra_column_meta_field_by_name(_In_ const w_cass_column_meta* pColumnMeta,
            _In_ const char* pName);

        /**
         * Same as w_cassandra_column_meta_field_by_name(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Column_Meta
         *
         * @param[in] column_meta
         * @param[in] name
         * @param[in] name_length
         * @return same as w_cassandra_column_meta_field_by_name()
         *
         * @see w_cassandra_column_meta_field_by_name()
         */
        W_SYSTEM_EXPORT const w_cass_value* 
            w_cassandra_column_meta_field_by_name_n(_In_ const w_cass_column_meta* pColumnMeta, 
            _In_  const char* pName, 
                _In_ size_t pNameLength);

        /**
         * Gets the name of the index.
         *
         * @public @memberof w_Cass_Index_Meta
         *
         * @param[in] index_meta
         * @param[out] name
         * @param[out] name_length
         */
        W_SYSTEM_EXPORT void
            w_cassandra_index_meta_name(_In_ const w_cass_index_meta* pIndexMeta, 
                _Inout_ const char** pName,
                _Inout_ size_t* pNameLength);

        /**
         * Gets the type of the index.
         *
         * @public @memberof w_Cass_Index_Meta
         *
         * @param[in] index_meta
         * @return The index's type.
         */
        W_SYSTEM_EXPORT w_cass_index_type 
            w_cassandra_index_meta_type(_In_ const w_cass_index_meta* pIndexMeta);

        /**
         * Gets the target of the index.
         *
         * @public @memberof w_Cass_Index_Meta
         *
         * @param[in] index_meta
         * @param[out] target
         * @param[out] target_length
         */
        W_SYSTEM_EXPORT void
            w_cassandra_index_meta_target(_In_ const w_cass_index_meta* pIndexMeta, 
                _Inout_ const char** pTarget,
                _Inout_ size_t* pTargetLength);

        /**
         * Gets the options of the index.
         *
         * @public @memberof w_Cass_Index_Meta
         *
         * @param[in] index_meta
         * @return The index's options.
         */
        W_SYSTEM_EXPORT const w_cass_value* 
            w_cassandra_index_meta_options(_In_ const w_cass_index_meta* pIndexMeta);

        /**
         * Gets a metadata field for the provided name. Metadata fields allow direct
         * access to the index data found in the underlying "indexes" metadata table.
         *
         * @public @memberof w_Cass_Index_Meta
         *
         * @param[in] index_meta
         * @param[in] name
         * @return A metadata field value. NULL if the field does not exist.
         */
        W_SYSTEM_EXPORT const w_cass_value*
            w_cassandra_index_meta_field_by_name(_In_ const w_cass_index_meta* pIndexMeta,
            _In_ const char* pName);

        /**
         * Same as w_cassandra_index_meta_field_by_name(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Index_Meta
         *
         * @param[in] index_meta
         * @param[in] name
         * @param[in] name_length
         * @return same as w_cassandra_index_meta_field_by_name()
         *
         * @see w_cassandra_index_meta_field_by_name()
         */
        W_SYSTEM_EXPORT const w_cass_value*
            w_cassandra_index_meta_field_by_name_n(_In_ const  w_cass_index_meta* pIndexMeta,
            _In_ const char* pName, 
            _In_ size_t pNameLength);

        /**
         * Gets the name of the function.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Function_Meta
         *
         * @param[in] function_meta
         * @param[out] name
         * @param[out] name_length
         */
        W_SYSTEM_EXPORT void
            w_cassandra_function_meta_name(_In_ const w_cass_function_meta* pFunctionMeta,
                _Inout_ const char** pName, 
                _Inout_  size_t* pNameLength);

        /**
         * Gets the full name of the function. The full name includes the
         * function's name and the function's signature:
         * "name(type1 type2.. typeN)".
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Function_Meta
         *
         * @param[in] function_meta
         * @param[out] full_name
         * @param[out] full_name_length
         */
        W_SYSTEM_EXPORT void
            w_cassandra_function_meta_full_name(_In_ const w_cass_function_meta* pFunctionMeta,
                _Inout_ const char** pFullName,
                _Inout_ size_t* pFullNameLength);

        /**
         * Gets the body of the function.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Function_Meta
         *
         * @param[in] function_meta
         * @param[out] body
         * @param[out] body_length
         */
        W_SYSTEM_EXPORT void
            w_cassandra_function_meta_body(_In_ const w_cass_function_meta* pFunctionMeta,
                _Inout_ const char** pBody, 
                _Inout_ size_t* pBodyLength);

        /**
         * Gets the language of the function.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Function_Meta
         *
         * @param[in] function_meta
         * @param[out] language
         * @param[out] language_length
         */
        W_SYSTEM_EXPORT void
            w_cassandra_function_meta_language(_In_ const w_cass_function_meta* pFunctionMeta, 
                _Inout_ const char** pLanguage,
                _Inout_  size_t* pLanguageLength);

        /**
         * Gets whether a function is called on "null".
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Function_Meta
         *
         * @param[in] function_meta
         * @return cass_true if a function is called on null, otherwise cass_false.
         */
        W_SYSTEM_EXPORT w_bool_t
            w_cassandra_function_meta_called_on_null_input(_In_ const w_cass_function_meta* pFunctionMeta);
        /**
         * Gets the number of arguments this function takes.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Function_Meta
         *
         * @param[in] function_meta
         * @return The number of arguments.
         */
        W_SYSTEM_EXPORT size_t
            w_cassandra_function_meta_argument_count(_In_ const w_cass_function_meta* pFunctionMeta);

        /**
         * Gets the function's argument name and type for the provided index.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Function_Meta
         *
         * @param[in] function_meta
         * @param[in] index
         * @param[out] name
         * @param[out] name_length
         * @param[out] type
         * @return W_SUCCESS if successful, otherwise an error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_function_meta_argument(_In_ const w_cass_function_meta* pFunctionMeta,
                _In_ size_t pIndex, 
                _Inout_ const char** pName, 
                _Inout_ size_t* pNameLength,
                _Inout_ const w_cass_data_type** pType);

        /**
         * Gets the function's argument and type for the provided name.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Function_Meta
         *
         * @param[in] function_meta
         * @param[in] name
         * @return A data type. NULL if the argument does not exist.
         */
        W_SYSTEM_EXPORT const w_cass_data_type*
            w_cassandra_function_meta_argument_type_by_name(_In_ const w_cass_function_meta* pFunctionMeta,
            _In_ const char* pName);

        /**
         * Same as w_cassandra_function_meta_argument_type_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Function_Meta
         *
         * @param[in] function_meta
         * @param[in] name
         * @param[in] name_length
         * @return same as w_cassandra_function_meta_argument_type_by_name()
         *
         * @see w_cassandra_function_meta_argument_type_by_name()
         */
        W_SYSTEM_EXPORT const w_cass_data_type*
            w_cassandra_function_meta_argument_type_by_name_n(_In_ const w_cass_function_meta* pFunctionMeta,
            _In_ const char* pName,
            _In_ size_t pNameLength);

        /**
         * Gets the return type of the function.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Function_Meta
         *
         * @param[in] function_meta
         * @return The data type returned by the function.
         */
        W_SYSTEM_EXPORT const w_cass_data_type* 
            w_cassandra_function_meta_return_type(_In_ const w_cass_function_meta* pFunctionMeta);

        /**
         * Gets a metadata field for the provided name. Metadata fields allow direct
         * access to the column data found in the underlying "functions" metadata table.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Function_Meta
         *
         * @param[in] function_meta
         * @param[in] name
         * @return A metadata field value. NULL if the field does not exist.
         */
        W_SYSTEM_EXPORT const w_cass_value* 
            w_cassandra_function_meta_field_by_name(_In_ const w_cass_function_meta* pFunctionMeta,
            _In_ const char* pName) ;

        /**
         * Same as w_cassandra_function_meta_field_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Function_Meta
         *
         * @param[in] function_meta
         * @param[in] name
         * @param[in] name_length
         * @return same as w_cassandra_function_meta_field_by_name()
         *
         * @see w_cassandra_function_meta_field_by_name()
         */
        W_SYSTEM_EXPORT const w_cass_value* w_cassandra_function_meta_field_by_name_n(_In_ const w_cass_function_meta* pFunctionMeta, 
            _In_ const char* pName, _In_ size_t pNameLength);

        /**
         * Gets the name of the aggregate.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Aggregate_Meta
         *
         * @param[in] aggregate_meta
         * @param[out] name
         * @param[out] name_length
         */
        W_SYSTEM_EXPORT void
            w_cassandra_aggregate_meta_name(_In_ const w_cass_aggregate_meta* pAggregateMeta,
                _Inout_ const char** pName,
                _Inout_ size_t* pNameLength);

        /**
         * Gets the full name of the aggregate. The full name includes the
         * aggregate's name and the aggregate's signature:
         * "name(type1 type2.. typeN)".
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Aggregate_Meta
         *
         * @param[in] aggregate_meta
         * @param[out] full_name
         * @param[out] full_name_length
         */
        W_SYSTEM_EXPORT void
            w_cassandra_aggregate_meta_full_name(_In_ const w_cass_aggregate_meta* pAggregateMeta, 
                _Inout_ const char** pFullName,
                _Inout_ size_t* pFullNameLength);

        /**
         * Gets the number of arguments this aggregate takes.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Aggregate_Meta
         *
         * @param[in] aggregate_meta
         * @return The number of arguments.
         */
        W_SYSTEM_EXPORT size_t
            w_cassandra_aggregate_meta_argument_count(_In_ const w_cass_aggregate_meta* pAggregateMeta);

        /**
         * Gets the aggregate's argument type for the provided index.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Aggregate_Meta
         *
         * @param[in] aggregate_meta
         * @param[in] index
         * @return The data type for argument. NULL returned if the index is out of range.
         */
        W_SYSTEM_EXPORT const w_cass_data_type*
            w_cassandra_aggregate_meta_argument_type(_In_ const w_cass_aggregate_meta* pAggregateMeta,
            _In_ size_t pIndex);


        /**
         * Gets the return type of the aggregate.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Aggregate_Meta
         *
         * @param[in] aggregate_meta
         * @return The data type returned by the aggregate.
         */
        W_SYSTEM_EXPORT const w_cass_data_type* 
            w_cassandra_aggregate_meta_return_type(_In_ const w_cass_aggregate_meta* pAggregateMeta);

        /**
         * Gets the state type of the aggregate.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Aggregate_Meta
         *
         * @param[in] aggregate_meta
         * @return The data type of the aggregate's state.
         */
        W_SYSTEM_EXPORT const w_cass_data_type* 
            w_cassandra_aggregate_meta_state_type(_In_ const w_cass_aggregate_meta* pAggregateMeta);

        /**
         * Gets the function metadata for the aggregate's state function.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Aggregate_Meta
         *
         * @param[in] aggregate_meta
         * @return The function metadata for the state function.
         */
        W_SYSTEM_EXPORT const w_cass_function_meta* 
            w_cassandra_aggregate_meta_state_func(_In_ const w_cass_aggregate_meta* pAggregateMeta);

        /**
         * Gets the function metadata for the aggregates's final function.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Aggregate_Meta
         *
         * @param[in] aggregate_meta
         * @return The function metadata for the final function.
         */
        W_SYSTEM_EXPORT const w_cass_function_meta* 
            w_cassandra_aggregate_meta_final_func(_In_ const w_cass_aggregate_meta* pAggregateMeta);

        /**
         * Gets the initial condition value for the aggregate.
         *
         * @cassandra{2.2+}
         *
         * <b>Note:</b> The value of the initial condition will always be
         * a "varchar" type for Cassandra 3.0+.
         *
         * @public @memberof w_Cass_Aggregate_Meta
         *
         * @param[in] aggregate_meta
         * @return The value of the initial condition.
         */
        W_SYSTEM_EXPORT const w_cass_value* 
            w_cassandra_aggregate_meta_init_cond(_In_ const w_cass_aggregate_meta* pAggregateMeta);

        /**
         * Gets a metadata field for the provided name. Metadata fields allow direct
         * access to the column data found in the underlying "aggregates" metadata table.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Aggregate_Meta
         *
         * @param[in] aggregate_meta
         * @param[in] name
         * @return A metadata field value. NULL if the field does not exist.
         */
        W_SYSTEM_EXPORT const w_cass_value* 
            w_cassandra_aggregate_meta_field_by_name(_In_ const w_cass_aggregate_meta* pAggregateMeta,
                _In_ const char* pName);

        /**
         * Same as w_cassandra_aggregate_meta_field_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Aggregate_Meta
         *
         * @param[in] aggregate_meta
         * @param[in] name
         * @param[in] name_length
         * @return same as w_cassandra_aggregate_meta_field_by_name()
         *
         * @see w_cassandra_aggregate_meta_field_by_name()
         */
        W_SYSTEM_EXPORT const w_cass_value* 
            w_cassandra_aggregate_meta_field_by_name_n(_In_ const w_cass_aggregate_meta* pAggregateMeta,
            _In_ const char* pName,
                _In_ size_t pNameLength);

        /***********************************************************************************
         *
         * SSL
         *
         ************************************************************************************/

         /**
          * Creates a new SSL context.
          *
          * @public @memberof w_Cass_Ssl
          *
          * @return Returns a SSL context that must be freed.
          *
          * @see w_cassandra_ssl_free()
          */
        W_SYSTEM_EXPORT w_cass_ssl*
            w_cassandra_ssl_new();

        /**
         * Creates a new SSL context <b>without</b> initializing the underlying library
         * implementation. The integrating application is responsible for
         * initializing the underlying SSL implementation. The driver uses the SSL
         * implmentation from several threads concurrently so it's important that it's
         * properly setup for multithreaded use e.g. lock callbacks for OpenSSL.
         *
         * <b>Important:</b> The SSL library must be initialized before calling this
         * function.
         *
         * When using OpenSSL the following components need to be initialized:
         *
         * SSL_library_init();
         * SSL_load_error_strings();
         * OpenSSL_add_all_algorithms();
         *
         * The following thread-safety callbacks also need to be set:
         *
         * CRYPTO_set_locking_callback(...);
         * CRYPTO_set_id_callback(...);
         *
         * @public @memberof w_Cass_Ssl
         *
         * @return Returns a SSL context that must be freed.
         *
         * @see w_cassandra_ssl_new()
         * @see w_cassandra_ssl_free()
         */
        W_SYSTEM_EXPORT w_cass_ssl* 
            w_cassandra_ssl_new_no_lib_init();

        /**
         * Frees a SSL context instance.
         *
         * @public @memberof w_Cass_Ssl
         *
         * @param[in] ssl
         */
        W_SYSTEM_EXPORT void
            w_cassandra_ssl_free(_In_ w_cass_ssl* pSsl);

        /**
         * Adds a trusted certificate. This is used to verify
         * the peer's certificate.
         *
         * @public @memberof w_Cass_Ssl
         *
         * @param[in] ssl
         * @param[in] cert PEM formatted certificate string
         * @return W_SUCCESS if successful, otherwise an error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_ssl_add_trusted_cert(_In_ w_cass_ssl* pSsl, 
                _In_ const char* pCert);

        /**
         * Same as w_cassandra_ssl_add_trusted_cert(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Ssl
         *
         * @param[in] ssl
         * @param[in] cert
         * @param[in] cert_length
         * @return same as w_cassandra_ssl_add_trusted_cert()
         *
         * @see w_cassandra_ssl_add_trusted_cert()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_ssl_add_trusted_cert_n(_In_ w_cass_ssl* pSsl,
                _In_  const char* pCert,
                _In_ size_t pCertLength);

        /**
         * Sets verification performed on the peer's certificate.
         *
         * CASS_SSL_VERIFY_NONE - No verification is performed
         * CASS_SSL_VERIFY_PEER_CERT - Certificate is present and valid
         * CASS_SSL_VERIFY_PEER_IDENTITY - IP address matches the certificate's
         * common name or one of its subject alternative names. This implies the
         * certificate is also present.
         * CASS_SSL_VERIFY_PEER_IDENTITY_DNS - Hostname matches the certificate's
         * common name or one of its subject alternative names. This implies the
         * certificate is also present. Hostname resolution must also be enabled.
         *
         * <b>Default:</b> CASS_SSL_VERIFY_PEER_CERT
         *
         * @public @memberof w_Cass_Ssl
         *
         * @param[in] ssl
         * @param[in] flags
         * @return W_SUCCESS if successful, otherwise an error occurred
         *
         * @see w_cassandra_cluster_set_use_hostname_resolution()
         */
        W_SYSTEM_EXPORT void
            w_cassandra_ssl_set_verify_flags(_In_ w_cass_ssl* pSsl, 
                _In_ int pFlags);

        /**
         * Set client-side certificate chain. This is used to authenticate
         * the client on the server-side. This should contain the entire
         * Certificate chain starting with the certificate itself.
         *
         * @public @memberof w_Cass_Ssl
         *
         * @param[in] ssl
         * @param[in] cert PEM formatted certificate string
         * @return W_SUCCESS if successful, otherwise an error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_ssl_set_cert(_In_ w_cass_ssl* pSsl, _In_ const char* pCert);

        /**
         * Same as w_cassandra_ssl_set_cert(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Ssl
         *
         * @param[in] ssl
         * @param[in] cert
         * @param[in] cert_length
         * @return same as cass_ssl_set_cert()
         *
         * @see w_cassandra_ssl_set_cert()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_ssl_set_cert_n(_In_ w_cass_ssl* pSsl,
                _In_  const char* pCert,
                _In_ size_t pCertLength);

        /**
         * Set client-side private key. This is used to authenticate
         * the client on the server-side.
         *
         * @public @memberof w_Cass_Ssl
         *
         * @param[in] ssl
         * @param[in] key PEM formatted key string
         * @param[in] password used to decrypt key
         * @return W_SUCCESS if successful, otherwise an error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_ssl_set_private_key(_In_ w_cass_ssl* pSsl, 
                _In_ const char* pKey,
                _In_ const char* pPassword);

        /**
         * Same as w_cassandra_ssl_set_private_key(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Ssl
         *
         * @param[in] ssl
         * @param[in] key
         * @param[in] key_length
         * @param[in] password
         * @param[in] password_length
         * @return same as w_cassandra_ssl_set_private_key()
         *
         * @see w_cassandra_ssl_set_private_key()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_ssl_set_private_key_n(_In_ w_cass_ssl* pSsl,
                _In_ const char* pKey, 
                _In_ size_t pKeyLength,
                _In_  const char* pPassword,
                _In_ size_t pPasswordLength);

        /***********************************************************************************
         *
         * Authenticator
         *
         ************************************************************************************/

         /**
          * Gets the IP address of the host being authenticated.
          *
          * @param[in] auth
          * @param[out] address
          *
          * @public @memberof W_Cass_Authenticator
          */
        W_SYSTEM_EXPORT void
            w_cassandra_authenticator_address(_In_ const w_cass_authenticator* pAuth,
                _Inout_ w_cass_inet* pAddress);

        /**
         * Gets the hostname of the host being authenticated.
         *
         * @public @memberof CassAuthenticator
         *
         * @param[in] auth
         * @param[out] length
         * @return A null-terminated string.
         */
        W_SYSTEM_EXPORT const char*
            w_cassandra_authenticator_hostname(_In_ const w_cass_authenticator* pAuth,
                _Inout_ size_t* pLength);

        /**
         * Gets the class name for the server-side IAuthentication implementation.
         *
         * @public @memberof CassAuthenticator
         *
         * @param[in] auth
         * @param[out] length
         * @return A null-terminated string.
         */
        W_SYSTEM_EXPORT const char*
            w_cassandra_authenticator_class_name(_In_ const w_cass_authenticator* pAuth,
                _Inout_ size_t* pLength);

        /**
         * Gets the user data created during the authenticator exchange. This
         * is set using w_cassandra_authenticator_set_exchange_data().
         *
         * @public @memberof CassAuthenticator
         *
         * @param[in] auth
         * @return User specified exchange data previously set by
         * w_cassandra_authenticator_set_exchange_data().
         *
         * @see w_cassandra_authenticator_set_exchange_data()
         */
        W_SYSTEM_EXPORT void*
            w_cassandra_authenticator_exchange_data(_In_ w_cass_authenticator* pAuth);
        /**
         * Sets the user data to be used during the authenticator exchange.
         *
         * @public @memberof CassAuthenticator
         *
         * @param[in] auth
         * @param[in] exchange_data
         *
         * @see w_cassandra_authenticator_exchange_data()
         */
        W_SYSTEM_EXPORT void
            w_cassandra_authenticator_set_exchange_data(_In_ w_cass_authenticator* pAuth,
                _In_ void* pExchangeData);

        /**
         * Gets a response token buffer of the provided size.
         *
         * @public @memberof CassAuthenticator
         *
         * @param[in] auth
         * @param[in] size
         * @return A buffer to copy the response token.
         */
        W_SYSTEM_EXPORT char*
            w_cassandra_authenticator_response(_In_ w_cass_authenticator* pAuth, _In_ size_t pSize);

        /**
         * Sets the response token.
         *
         * @public @memberof CassAuthenticator
         *
         * @param[in] auth
         * @param[in] response
         * @param[in] response_size
         */
        W_SYSTEM_EXPORT void
            w_cassandra_authenticator_set_response(_In_ w_cass_authenticator* pAuth,
                _In_ const char* pResponse, 
                _In_ size_t pResponseSize);

        /**
         * Sets an error for the authenticator exchange.
         *
         * @public @memberof CassAuthenticator
         *
         * @param[in] auth
         * @param[in] message
         */
        W_SYSTEM_EXPORT void
            w_cassandra_authenticator_set_error(_In_ w_cass_authenticator* pAuth,
                _In_ const char* pMessage);

        /**
         * Same as w_cassandra_authenticator_set_error(), but with lengths for string
         * parameters.
         *
         * @public @memberof CassAuthenticator
         *
         * @param[in] auth
         * @param[in] message
         * @param[in] message_length
         *
         * @see w_cassandra_authenticator_set_error()
         */
        W_SYSTEM_EXPORT void
            w_cassandra_authenticator_set_error_n(_In_ w_cass_authenticator* pAuth, 
                _In_ const char* pMessage,
                _In_ size_t pMessageLength);

        /***********************************************************************************
         *
         * Future
         *
         ***********************************************************************************/

         /**
          * Frees a future instance. A future can be freed anytime.
          *
          * @public @memberof w_Cass_Future
          */
        W_SYSTEM_EXPORT void
            w_cassandra_future_free(_In_ w_cass_future* pFuture);

        /**
         * Sets a callback that is called when a future is set
         *
         * @public @memberof w_Cass_Future
         *
         * @param[in] future
         * @param[in] callback
         * @param[in] data
         * @return W_SUCCESS if successful, otherwise an error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_future_set_callback(_In_ w_cass_future* pFuture,
                _In_ w_cass_future_callback pCallback,
                _In_ void* pData);

        /**
         * Gets the set status of the future.
         *
         * @public @memberof w_Cass_Future
         *
         * @param[in] future
         * @return true if set
         */
        W_SYSTEM_EXPORT w_bool_t
            w_cassandra_future_ready(_In_ w_cass_future* pFuture);

        /**
         * Wait for the future to be set with either a result or error.
         *
         * <b>Important:</b> Do not wait in a future callback. Waiting in a future
         * callback will cause a deadlock.
         *
         * @public @memberof w_Cass_Future
         *
         * @param[in] future
         */
        W_SYSTEM_EXPORT void
            w_cassandra_future_wait(_In_ w_cass_future* pFuture);

        /**
         * Wait for the future to be set or timeout.
         *
         * @public @memberof w_Cass_Future
         *
         * @param[in] future
         * @param[in] timeout_us wait time in microseconds
         * @return false if returned due to timeout
         */
        W_SYSTEM_EXPORT w_bool_t
            w_cassandra_future_wait_timed(_In_ w_cass_future* pFuture, 
                _In_ w_duration_t pTimeoutUs);

        /**
         * Gets the result of a successful future. If the future is not ready this method will
         * wait for the future to be set.
         *
         * @public @memberof w_Cass_Future
         *
         * @param[in] future
         * @return w_Cass_Result instance if successful, otherwise NULL for error. The return instance
         * must be freed using w_cassandra_result_free().
         *
         * @see w_cassandra_session_execute() and w_cassandra_session_execute_batch()
         */
        W_SYSTEM_EXPORT const w_cass_result* 
            w_cassandra_future_get_result (_In_ w_cass_future* pFuture);

        /**
         * Gets the error result from a future that failed as a result of a server error. If the
         * future is not ready this method will wait for the future to be set.
         *
         * @public @memberof w_Cass_Future
         *
         * @param[in] future
         * @return w_Cass_ErrorResult instance if the request failed with a server error,
         * otherwise NULL if the request was successful or the failure was not caused by
         * a server error. The return instance must be freed using w_cassandra_error_result_free().
         *
         * @see w_cassandra_session_execute() and w_cassandra_session_execute_batch()
         */
        W_SYSTEM_EXPORT const w_cass_error_result*
            w_cassandra_future_get_error_result(_In_ w_cass_future* pFuture);

        /**
         * Gets the result of a successful future. If the future is not ready this method will
         * wait for the future to be set. The first successful call consumes the future, all
         * subsequent calls will return NULL.
         *
         * @public @memberof w_Cass_Future
         *
         * @param[in] future
         * @return w_Cass_Prepared instance if successful, otherwise NULL for error. The return instance
         * must be freed using w_cassandra_prepared_free().
         *
         * @see w_cassandra_session_prepare()
         */
        W_SYSTEM_EXPORT const w_cass_prepared* 
            w_cassandra_future_get_prepared(_In_ w_cass_future* pFuture);

        /**
         * Gets the error code from future. If the future is not ready this method will
         * wait for the future to be set.
         *
         * @public @memberof w_Cass_Future
         *
         * @param[in] future
         * @return CASS_OK if successful, otherwise an error occurred.
         *
         * @see w_cassandra_error_desc()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_future_error_code(_In_ w_cass_future* pFuture);

        /**
         * Gets the error message from future. If the future is not ready this method will
         * wait for the future to be set.
         *
         * @public @memberof w_Cass_Future
         *
         * @param[in] future
         * @param[out] message Empty string returned if successful, otherwise
         * a message describing the error is returned.
         * @param[out] message_length
         */
        W_SYSTEM_EXPORT void
            w_cassandra_future_error_message(_In_ w_cass_future* pFuture,
                _Inout_ const char** pMessage,
                _Inout_ size_t* pMessageLength);

        /**
         * Gets the tracing ID associated with the request.
         *
         * @public @memberof w_Cass_Future
         *
         * @param[in] future
         * @param[out] tracing_id
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_future_tracing_id(_In_ w_cass_future* pFuture, 
                _Inout_ w_cass_uuid* pTracingId);

        /**
         * Gets a the number of custom payload items from a response future. If the future is not
         * ready this method will wait for the future to be set.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Future
         *
         * @param[in] future
         * @return the number of custom payload items.
         */
        W_SYSTEM_EXPORT size_t
            w_cassandra_future_custom_payload_item_count(_In_ w_cass_future* pFuture);

        /**
         * Gets a custom payload item from a response future at the specified index. If the future is not
         * ready this method will wait for the future to be set.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Future
         *
         * @param[in] future
         * @param[in] index
         * @param[out] name
         * @param[out] name_length
         * @param[out] value
         * @param[out] value_size
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_future_custom_payload_item(_In_ w_cass_future* pFuture,
                _In_ size_t pIndex, 
                _Inout_ const char** pName, 
                _Inout_ size_t* pNameLength,
                _Inout_ const w_byte_t** pValue,
                _Inout_ size_t* pValueSize);

        /***********************************************************************************
         *
         * Statement
         *
         ***********************************************************************************/

         /**
          * Creates a new query statement.
          *
          * @public @memberof w_Cass_Statement
          *
          * @param[in] query The query is copied into the statement object; the
          * memory pointed to by this parameter can be freed after this call.
          * @param[in] parameter_count The number of bound parameters.
          * @return Returns a statement that must be freed.
          *
          * @see w_cassandra_statement_free()
          */
        W_SYSTEM_EXPORT w_cass_statement*
            w_cassandra_statement_new(_In_ const char* pQuery, 
            _In_ size_t pParameterCount);

        /**
         * Same as w_cassandra_statement_new(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] query
         * @param[in] query_length
         * @param[in] parameter_count
         * @return same as w_cassandra_statement_new()
         *
         * @see w_cassandra_statement_new()
         */
        W_SYSTEM_EXPORT w_cass_statement* 
            w_cassandra_statement_new_n(_In_ const char* pQuery, 
            _In_ size_t pQueryLength,
            _In_ size_t pParameterCount);

        /**
         * Clear and/or resize the statement's parameters.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] count
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_reset_parameters(_In_ w_cass_statement* pStatement,
                _In_ size_t pCount);

        /**
         * Frees a statement instance. Statements can be immediately freed after
         * being prepared, executed or added to a batch.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         */
        W_SYSTEM_EXPORT void
            w_cassandra_statement_free(_In_ w_cass_statement* pStatement);

        /**
         * Adds a key index specifier to this a statement.
         * When using token-aware routing, this can be used to tell the driver which
         * parameters within a non-prepared, parameterized statement are part of
         * the partition key.
         *
         * Use consecutive calls for composite partition keys.
         *
         * This is not necessary for prepared statements, as the key
         * parameters are determined in the metadata processed in the prepare phase.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] index
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_add_key_index(_In_ w_cass_statement* pStatement, 
            _In_ size_t pIndex);

        /**
         * Sets the statement's keyspace. This is used for token-aware routing and when
         * using protocol v5 or greater it also overrides the session's current
         * keyspace for the statement.
         *
         * This is not necessary and will not work for bound statements, as the keyspace
         * is determined by the prepared statement metadata.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] keyspace
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_set_keyspace(_In_ w_cass_statement* pStatement,
                _In_ const char* pKeyspace);

        /**
         * Same as cass_statement_set_keyspace(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] keyspace
         * @param[in] keyspace_length
         * @return same as cass_statement_set_keyspace()
         *
         * @see w_cassandra_statement_set_keyspace()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_set_keyspace_n(_In_ w_cass_statement* pStatement,
                _In_ const char* pKeyspace,
                _In_ size_t pKeyspaceLength);

        /**
         * Sets the statement's consistency level.
         *
         * <b>Default:</b> CASS_CONSISTENCY_LOCAL_ONE
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] consistency
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_set_consistency(_In_ w_cass_statement* pStatement,
                _In_ w_cass_consistency pConsistency);

        /**
         * Sets the statement's serial consistency level.
         *
         * @cassandra{2.0+}
         *
         * <b>Default:</b> Not set
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] serial_consistency
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_set_serial_consistency(_In_ w_cass_statement* pStatement, 
                _In_ w_cass_consistency pSerialConsistency);

        /**
         * Sets the statement's page size.
         *
         * @cassandra{2.0+}
         *
         * <b>Default:</b> -1 (Disabled)
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] page_size
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_set_paging_size(_In_ w_cass_statement* pStatement,
                _In_ int pPageSize);

        /**
         * Sets the statement's paging state. This can be used to get the next page of
         * data in a multi-page query.
         *
         * @cassandra{2.0+}
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] result
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_set_paging_state(_In_ w_cass_statement* pStatement, 
                _In_  const w_cass_result* pResult);

        /**
         * Sets the statement's paging state. This can be used to get the next page of
         * data in a multi-page query.
         *
         * @cassandra{2.0+}
         *
         * <b>Warning:</b> The paging state should not be exposed to or come from
         * untrusted environments. The paging state could be spoofed and potentially
         * used to gain access to other data.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] paging_state
         * @param[in] paging_state_size
         * @return W_SUCCESS if successful, otherwise an error occurred.
         *
         * @see w_cassandra_result_paging_state_token()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_set_paging_state_token(_In_ w_cass_statement* pStatement, 
                _In_ const char* pPagingState,
                _In_ size_t pPagingStateSize);

        /**
         * Sets the statement's timestamp.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] timestamp
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_set_timestamp(_In_ w_cass_statement* pStatement, 
                _In_ int64_t pTimestamp);

        /**
         * Sets the statement's timeout for waiting for a response from a node.
         *
         * <b>Default:</b> Disabled (use the cluster-level request timeout)
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] timeout_ms Request timeout in milliseconds. Use 0 for no timeout
         * or CASS_UINT64_MAX to disable (to use the cluster-level request timeout).
         * @return W_SUCCESS if successful, otherwise an error occurred.
         *
         * @see w_cassandra_cluster_set_request_timeout()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_set_request_timeout(_In_ w_cass_statement* pStatement,
                _In_ uint64_t pTimeoutMs);

        /**
         * Sets whether the statement is idempotent. Idempotent statements are able to be
         * automatically retried after timeouts/errors and can be speculatively executed.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] is_idempotent
         * @return W_SUCCESS if successful, otherwise an error occurred.
         *
         * @see w_cassandra_cluster_set_constant_speculative_execution_policy()
         * @see w_cassandra_execution_profile_set_constant_speculative_execution_policy()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_set_is_idempotent(_In_ w_cass_statement* pStatement, _In_ w_bool_t pIsIdempotent);
                
        /**
         * Sets the statement's retry policy.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] retry_policy
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_set_retry_policy(_In_ w_cass_statement* pStatement, 
                _In_ w_cass_retry_policy* pRetryPolicy);

        /**
         * Sets the statement's custom payload.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] payload
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_set_custom_payload(_In_ w_cass_statement* pStatement,
                _In_ const w_cass_custom_payload* pPayload);

        /**
         * Sets the execution profile to execute the statement with.
         *
         * <b>Note:</b> NULL or empty string will clear execution profile from statement
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @return W_SUCCESS if successful, otherwise an error occurred.
         *
         * @see w_cassandra_cluster_set_execution_profile()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_set_execution_profile(_In_ w_cass_statement* pStatement,
                _In_ const char* pName);

        /**
         * Same as w_cassandra_statement_set_execution_profile(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] name_length
         * @return W_SUCCESS if successful, otherwise an error occurred.
         *
         * @see w_cassandra_statement_set_execution_profile()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_set_execution_profile_n(_In_ w_cass_statement* pStatement, 
                _In_ const char* pName,
                _In_ size_t pNameLength);

        /**
         * Sets whether the statement should use tracing.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] enabled
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_set_tracing(_In_ w_cass_statement* pStatement,
                _In_ w_bool_t pEnabled);

        /**
         * Sets a specific host that should run the query.
         *
         * In general, this should not be used, but it can be useful in the following
         * situations:
         * * To query node-local tables such as system and virtual tables.
         * * To apply a sequence of schema changes where it makes sense for all the
         *   changes to be applied on a single node.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] host
         * @param[in] port
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_set_host(_In_ w_cass_statement* pStatement,
                _In_  const char* pHost,
                _In_ int pPort);

        /**
         * Same as w_cassandra_statement_set_host(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] host
         * @param[in] host_length
         * @param[in] port
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_set_host_n(_In_ w_cass_statement* pStatement, 
                _In_ const char* pHost,
                _In_ size_t pHostLength,
                _In_ int pPort);

        /**
         * Same as w_cassandra_statement_set_host(), but with the `w_cass_inet` type
         * for the host instead of a string.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] host
         * @param[in] port
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_set_host_inet(_In_ w_cass_statement* pStatement,
                _In_ const w_cass_inet* pHost,
                _In_ int pPort);

        /**
         * Binds null to a query or bound statement at the specified index.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] index
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_null(_In_ w_cass_statement* pStatement,
                _In_ size_t pIndex);

        /**
         * Binds a null to all the values with the specified name.
         *
         * This can only be used with statements created by
         * w_cassandra_prepared_bind() when using Cassandra 2.0 or earlier.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_null_by_name(_In_ w_cass_statement* pStatement,
                _In_ const char* pName);

        /**
         * Same as w_cassandra_statement_bind_null_by_name(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] name_length
         * @return same as w_cassandra_statement_bind_null_by_name()
         *
         * @see w_cassandra_statement_bind_null_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_null_by_name_n(_In_ w_cass_statement* pStatement,
                _In_ const char* pName,
                _In_ size_t pNameLength);

        /**
         * Binds a "tinyint" to a query or bound statement at the specified index.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_int8(_In_ w_cass_statement* pStatement,
                _In_  size_t pIndex,
                _In_ int8_t pValue);

        /**
         * Binds a "tinyint" to all the values with the specified name.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_int8_by_name(_In_ w_cass_statement* pStatement,
                _In_ const char* pName,
                _In_ int8_t pValue);

        /**
         * Same as w_cassandra_statement_bind_int8_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] name_length
         * @param[in] value
         * @return same as w_cassandra_statement_bind_int8_by_name()
         *
         * @see w_cassandra_statement_bind_int8_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_int8_by_name_n(_In_ w_cass_statement* pStatement,
                _In_ const char* pName,
                _In_ size_t pNameLength,
                _In_ int8_t pValue);

        /**
         * Binds an "smallint" to a query or bound statement at the specified index.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_int16(_In_ w_cass_statement* pStatement,
                _In_ size_t pIndex,
                _In_ int16_t pValue);

        /**
         * Binds an "smallint" to all the values with the specified name.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_int16_by_name(_In_ w_cass_statement* pStatement,
                _In_ const char* pName, 
                _In_ int16_t pValue);

        /**
         * Same as w_cassandra_statement_bind_int16_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] name_length
         * @param[in] value
         * @return same as w_cassandra_statement_bind_int16_by_name()
         *
         * @see w_cassandra_statement_bind_int16_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_int16_by_name_n(_In_ w_cass_statement* pStatement,
                _In_ const char* pName,
                _In_  size_t pNameLength, 
                _In_ int16_t pValue);

        /**
         * Binds an "int" to a query or bound statement at the specified index.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_int32(_In_ w_cass_statement* pStatement,
                _In_  size_t pIndex,
                _In_ int32_t pValue);

        /**
         * Binds an "int" to all the values with the specified name.
         *
         * This can only be used with statements created by
         * w_cassandra_prepared_bind() when using Cassandra 2.0 or earlier.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_int32_by_name(_In_ w_cass_statement* pStatement, 
                _In_ const char* pName,
                _In_  int32_t pValue);

        /**
         * Same as w_cassandra_statement_bind_int32_by_name(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] name_length
         * @param[in] value
         * @return same as w_cassandra_statement_bind_int32_by_name()
         *
         * @see w_cassandra_statement_bind_int32_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_int32_by_name_n(_In_ w_cass_statement* pStatement, 
                _In_ const char* pName, 
                _In_ size_t pNameLength, 
                _In_ int32_t pValue);

        /**
         * Binds a "date" to a query or bound statement at the specified index.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_int64(_In_ w_cass_statement* pStatement,
                _In_ size_t pIndex,
                _In_ int64_t pValue);

        /**
         * Binds a "date" to all the values with the specified name.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_uint32_by_name(_In_ w_cass_statement* pStatement,
                _In_ const char* pName, 
                _In_ uint32_t pValue);

        /**
         * Same as w_cassandra_statement_bind_uint32_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] name_length
         * @param[in] value
         * @return same as w_cassandra_statement_bind_uint32_by_name()
         *
         * @see w_cassandra_statement_bind_uint32_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_uint32_by_name_n(_In_ w_cass_statement* pStatement, 
                _In_ const char* pName,
                _In_ size_t pNameLength, 
                _In_ uint32_t pValue);

        /**
         * Binds a "bigint", "counter", "timestamp" or "time" to a query or
         * bound statement at the specified index.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_int64(w_cass_statement* pStatement,
                size_t pIndex,
                int64_t pValue);

        /**
         * Binds a "bigint", "counter", "timestamp" or "time" to all values
         * with the specified name.
         *
         * This can only be used with statements created by
         * w_cassandra_prepared_bind() when using Cassandra 2.0 or earlier.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_int64_by_name(_In_ w_cass_statement* pStatement,
                _In_ const char* pName,
                _In_ uint64_t pValue);

        /**
         * Same as w_cassandra_statement_bind_int64_by_name(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] name_length
         * @param[in] value
         * @return same as w_cassandra_statement_bind_int64_by_name(0
         *
         * @see w_cassandra_statement_bind_int64_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_int64_by_name_n(_In_ w_cass_statement* pStatement,
                _In_ const char* pName, 
                _In_ size_t pNameLength,
                _In_  int64_t pValue);

        /**
         * Binds a "float" to a query or bound statement at the specified index.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_float(_In_ w_cass_statement* pStatement,
                _In_  size_t pIndex,
                _In_  float pValue);

        /**
         * Binds a "float" to all the values with the specified name.
         *
         * This can only be used with statements created by
         * w_cassandra_prepared_bind() when using Cassandra 2.0 or earlier.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_float_by_name(_In_ w_cass_statement* pStatement,
                _In_  const char* pName,
                _In_  float pValue);

        /**
         * Same as w_cassandra_statement_bind_float_by_name(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] name_length
         * @param[in] value
         * @return same as w_cassandra_statement_bind_float_by_name()
         *
         * @see w_cassandra_statement_bind_float_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_float_by_name_n(_In_ w_cass_statement* pStatement,
                _In_ const char* pName,
                _In_ size_t pNameLength,
                _In_ float pValue);

        /**
         * Binds a "double" to a query or bound statement at the specified index.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_double(_In_ w_cass_statement* pStatement,
                _In_ size_t pIndex,
                _In_ double pValue);

        /**
         * Binds a "double" to all the values with the specified name.
         *
         * This can only be used with statements created by
         * w_cassandra_prepared_bind() when using Cassandra 2.0 or earlier.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_double_by_name(_In_ w_cass_statement* pStatement,
                _In_ const char* pName,
                _In_  double pValue);

        /**
         * Same as w_cassandra_statement_bind_double_by_name(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] name_length
         * @param[in] value
         * @return same as w_cassandra_statement_bind_double_by_name()
         *
         * @see w_cassandra_statement_bind_double_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_double_by_name_n(_In_ w_cass_statement* pStatement,
                _In_ const char* pName,
                _In_ size_t pNameLength, 
                _In_ double pValue);

        /**
         * Binds a "boolean" to a query or bound statement at the specified index.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_bool(_In_ w_cass_statement* pStatement,
                _In_  size_t pIndex, 
                _In_  w_bool_t pValue);

        /**
         * Binds a "boolean" to all the values with the specified name.
         *
         * This can only be used with statements created by
         * w_cassandra_prepared_bind() when using Cassandra 2.0 or earlier.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_bool_by_name(_In_ w_cass_statement* pStatement,
                _In_ const char* pName,
                _In_ w_bool_t pValue);

        /**
         * Same as w_cassandra_statement_bind_bool_by_name(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] name_length
         * @param[in] value
         * @return same as w_cassandra_statement_bind_bool_by_name()
         *
         * @see w_cassandra_statement_bind_bool_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_bool_by_name_n(_In_ w_cass_statement* pStatement, 
                _In_ const char* pName, 
                _In_ size_t pNameLength, 
                _In_  w_bool_t pValue);

        /**
         * Binds an "ascii", "text" or "varchar" to a query or bound statement
         * at the specified index.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] index
         * @param[in] value The value is copied into the statement object; the
         * memory pointed to by this parameter can be freed after this call.
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_string(_In_ w_cass_statement* pStatement,
                _In_ size_t pIndex, 
                _In_  const char* pValue);

        /**
         * Same as w_cassandra_statement_bind_string(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] index
         * @param[in] value
         * @param[in] value_length
         * @return same as cass_statement_bind_string()
         *
         * @see w_cassandra_statement_bind_string()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_string_n(_In_ w_cass_statement* pStatement, 
                _In_ size_t pIndex, 
                _In_  const char* pValue, 
                _In_ size_t pValueLength);

        /**
         * Binds an "ascii", "text" or "varchar" to all the values
         * with the specified name.
         *
         * This can only be used with statements created by
         * w_cassandra_prepared_bind() when using Cassandra 2.0 or earlier.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] value The value is copied into the statement object; the
         * memory pointed to by this parameter can be freed after this call.
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_string_by_name(_In_ w_cass_statement* pStatement,
                _In_ const char* pName,
                _In_ const char* pValue);

        /**
         * Same as w_cassandra_statement_bind_string_by_name(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] name_length
         * @param[in] value
         * @param[in] value_length
         * @return same as w_cassandra_statement_bind_string_by_name()
         *
         * @see w_cassandra_statement_bind_string_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_string_by_name_n(_In_ w_cass_statement* pStatement,
                _In_ const char* pName,
                _In_  size_t pNameLength,
                _In_ const char* pValue, 
                _In_  size_t pValueLength);

        /**
         * Binds a "blob", "varint" or "custom" to a query or bound statement at the specified index.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] index
         * @param[in] value The value is copied into the statement object; the
         * memory pointed to by this parameter can be freed after this call.
         * @param[in] value_size
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_bytes(_In_ w_cass_statement* pStatement,
                _In_ size_t pIndex, 
                _In_  const w_byte_t* pValue,
                _In_ size_t pValueSize) ;

        /**
         * Binds a "blob", "varint" or "custom" to all the values with the
         * specified name.
         *
         * This can only be used with statements created by
         * w_cassandra_prepared_bind() when using Cassandra 2.0 or earlier.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] value The value is copied into the statement object; the
         * memory pointed to by this parameter can be freed after this call.
         * @param[in] value_size
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_bytes_by_name(_In_ w_cass_statement* pStatement,
                _In_ const char* pName,
                _In_ const w_byte_t* pValue,
                _In_  size_t pValueSize);

        /**
         * Same as w_cassandra_statement_bind_bytes_by_name(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] name_length
         * @param[in] value
         * @param[in] value_size
         * @return same as w_cassandra_statement_bind_bytes_by_name()
         *
         * @see w_cassandra_statement_bind_bytes_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_bytes_by_name_n(_In_ w_cass_statement* pStatement,
                _In_  const char* pName,
                _In_ size_t pNameLength, 
                _In_ const w_byte_t* pValue,
                _In_ size_t pValueSize);

        /**
         * Binds a "custom" to a query or bound statement at the specified index.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] index
         * @param[in] class_name
         * @param[in] value The value is copied into the statement object; the
         * memory pointed to by this parameter can be freed after this call.
         * @param[in] value_size
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_custom(_In_ w_cass_statement* pStatement,
                _In_ size_t pIndex,
                _In_ const char* pClassName,
                _In_ const w_byte_t* pValue, 
                _In_ size_t pValueSize);
        /**
         * Same as w_cassandra_statement_bind_custom(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] index
         * @param[in] class_name
         * @param[in] class_name_length
         * @param[in] value The value is copied into the statement object; the
         * memory pointed to by this parameter can be freed after this call.
         * @param[in] value_size
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_custom_n(_In_ w_cass_statement* pStatement,
                _In_  size_t pIndex,
                _In_ const char* pClassName,
                _In_ size_t pClassNameLength,
                _In_ const w_byte_t* pValue,
                _In_ size_t pValueSize);

        /**
         * Binds a "custom" to all the values with the specified name.
         *
         * This can only be used with statements created by
         * w_cassandra_prepared_bind() when using Cassandra 2.0 or earlier.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] class_name
         * @param[in] value The value is copied into the statement object; the
         * memory pointed to by this parameter can be freed after this call.
         * @param[in] value_size
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_custom_by_name(_In_ w_cass_statement* pStatement,
                _In_ const char* pName,
                _In_ const char* pClassName,
                _In_ const w_byte_t* pValue,
                _In_ size_t pValueSize);

        /**
         * Same as w_cassandra_statement_bind_custom_by_name(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] name_length
         * @param[in] class_name
         * @param[in] class_name_length
         * @param[in] value
         * @param[in] value_size
         * @return same as w_cassandra_statement_bind_custom_by_name()
         *
         * @see w_cassandra_statement_bind_custom_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_custom_by_name_n(_In_ w_cass_statement* pStatement, 
                _In_ const char* pName,
                _In_ size_t pNameLength,
                _In_ const char* pClassName, 
                _In_ size_t pClassNameLength,
                _In_ const w_byte_t* pValue,
                _In_ size_t pValueSize);

        /**
         * Binds a "uuid" or "timeuuid" to a query or bound statement at the specified index.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_uuid(_In_ w_cass_statement* pStatement,
                _In_ size_t pIndex,
                _In_ w_cass_uuid pValue);

        /**
         * Binds a "uuid" or "timeuuid" to all the values
         * with the specified name.
         *
         * This can only be used with statements created by
         * w_cassandra_prepared_bind() when using Cassandra 2.0 or earlier.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_uuid_by_name(_In_ w_cass_statement* pStatement,
                _In_ const char* pName, 
                _In_ w_cass_uuid pValue);

        /**
         * Same as w_cassandra_statement_bind_uuid_by_name(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] name_length
         * @param[in] value
         * @return same as w_cassandra_statement_bind_uuid_by_name()
         *
         * @see w_cassandra_statement_bind_uuid_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_uuid_by_name_n(_In_ w_cass_statement* pStatement,
                _In_  const char* pName,
                _In_ size_t pNameLength,
                _In_  w_cass_uuid pValue);

        /**
         * Binds an "inet" to a query or bound statement at the specified index.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_inet(_In_ w_cass_statement* pStatement,
                _In_ size_t pIndex, 
                _In_ w_cass_inet pValue);

        /**
         * Binds an "inet" to all the values with the specified name.
         *
         * This can only be used with statements created by
         * w_cassandra_prepared_bind() when using Cassandra 2.0 or earlier.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_inet_by_name(_In_ w_cass_statement* pStatement,
                _In_ const char* pName,
                _In_ w_cass_inet pValue);

        /**
         * Same as w_cassandra_statement_bind_inet_by_name(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] name_length
         * @param[in] value
         * @return same as w_cassandra_statement_bind_inet_by_name()
         *
         * @see w_cassandra_statement_bind_inet_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_inet_by_name_n(_In_ w_cass_statement* pStatement,
                _In_ const char* pName,
                _In_ size_t pNameLength, 
                _In_ w_cass_inet pValue);

        /**
         * Bind a "decimal" to a query or bound statement at the specified index.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] index
         * @param[in] varint The value is copied into the statement object; the
         * memory pointed to by this parameter can be freed after this call.
         * @param[in] varint_size
         * @param[in] scale
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_decimal(_In_ w_cass_statement* pStatement,
                _In_ size_t pIndex,
                _In_ const w_byte_t* pVarint,
                _In_ size_t pVarintSize,
                _In_ int32_t pScale);

        /**
         * Binds a "decimal" to all the values with the specified name.
         *
         * This can only be used with statements created by
         * w_cassandra_prepared_bind() when using Cassandra 2.0 or earlier.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] varint The value is copied into the statement object; the
         * memory pointed to by this parameter can be freed after this call.
         * @param[in] varint_size
         * @param[in] scale
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_decimal_by_name(_In_ w_cass_statement* pStatement,
                _In_ const char* pName,
                _In_ const w_byte_t* pVarint,
                _In_ size_t pVarintSize,
                _In_ int32_t pScale);

        /**
         * Same as w_cassandra_statement_bind_decimal_by_name(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] name_length
         * @param[in] varint
         * @param[in] varint_size
         * @param[in] scale
         * @return same as w_cassandra_statement_bind_decimal_by_name()
         *
         * @see w_cassandra_statement_bind_decimal_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_decimal_by_name_n(_In_ w_cass_statement* pStatement,
                _In_ const char* pName,
                _In_ size_t pNameLength,
                _In_  const w_byte_t* pVarint, 
                _In_ size_t pVarintSize,
                _In_  int32_t pScale);

        /**
         * Binds a "duration" to a query or bound statement at the specified index.
         *
         * @cassandra{3.10+}
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] index
         * @param[in] months
         * @param[in] days
         * @param[in] nanos
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_duration(_In_ w_cass_statement* pStatement,
                _In_ size_t pIndex,
                _In_ int32_t pMonths,
                _In_ int32_t pDays,
                _In_ int64_t pNanos);

        /**
         * Binds a "duration" to all the values with the specified name.
         *
         * @cassandra{3.10+}
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] months
         * @param[in] days
         * @param[in] nanos
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_duration_by_name(_In_ w_cass_statement* pStatement,
                _In_  const char* pName, 
                _In_ int32_t pMonths,
                _In_  int32_t pDays, 
                _In_ int64_t pNanos) ;

        /**
         * Same as w_cassandra_statement_bind_duration_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{3.10+}
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] name_length
         * @param[in] months
         * @param[in] days
         * @param[in] nanos
         * @return same as w_cassandra_statement_bind_duration_by_name()
         *
         * @see w_cassandra_statement_bind_duration_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_duration_by_name_n(_In_ w_cass_statement* pStatement,
                _In_ const char* pName,
                _In_ size_t pNameLength,
                _In_ int32_t pMonths,
                _In_ int32_t pDays, 
                _In_ int64_t pNanos);

        /**
         * Bind a "list", "map" or "set" to a query or bound statement at the
         * specified index.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] index
         * @param[in] collection The collection can be freed after this call.
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_collection(_In_ w_cass_statement* pStatement,
                _In_ size_t pIndex, 
                _In_ const w_cass_collection* pCollection);

        /**
         * Bind a "list", "map" or "set" to all the values with the
         * specified name.
         *
         * This can only be used with statements created by
         * w_cassandra_prepared_bind() when using Cassandra 2.0 or earlier.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] collection The collection can be freed after this call.
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_collection_by_name(_In_ w_cass_statement* pStatement,
                _In_ const char* pName,
                _In_ const w_cass_collection* pCollection);

        /**
         * Same as w_cassandra_statement_bind_collection_by_name(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] name_length
         * @param[in] collection
         * @return same as w_cassandra_statement_bind_collection_by_name()
         *
         * @see w_cassandra_statement_bind_collection_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_collection_by_name_n(_In_ w_cass_statement* pStatement,
                _In_ const char* pName, 
                _In_ size_t pNameLength, 
                _In_ const w_cass_collection* pCollection);

        /**
         * Bind a "tuple" to a query or bound statement at the specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] index
         * @param[in] tuple The tuple can be freed after this call.
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_tuple(_In_ w_cass_statement* pStatement,
                _In_ size_t pIndex,
                _In_ const w_cass_tuple* pTuple);

        /**
         * Bind a "tuple" to all the values with the specified name.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] tuple The tuple can be freed after this call.
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_tuple_by_name(_In_ w_cass_statement* pStatement,
                _In_ const char* pName,
                _In_ const w_cass_tuple* pTuple);

        /**
         * Same as w_cassandra_statement_bind_tuple_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] name_length
         * @param[in] tuple
         * @return same as w_cassandra_statement_bind_tuple_by_name()
         *
         * @see w_cassandra_statement_bind_tuple_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_tuple_by_name_n(_In_ w_cass_statement* pStatement,
                _In_  const char* pName, 
                _In_ size_t pNameLength,
                _In_ const w_cass_tuple* pTuple);

        /**
         * Bind a user defined type to a query or bound statement at the
         * specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] index
         * @param[in] user_type The user type can be freed after this call.
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_user_type(_In_ w_cass_statement* pStatement, 
                _In_ size_t pIndex,
                _In_ const w_cass_user_type* pUserType);
        /**
         * Bind a user defined type to a query or bound statement with the
         * specified name.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] user_type The user type can be freed after this call.
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_user_type_by_name(_In_ w_cass_statement* pStatement,
                _In_  const char* pName,
                _In_ const w_cass_user_type* pUserType);

        /**
         * Same as w_cassandra_statement_bind_user_type_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] statement
         * @param[in] name
         * @param[in] name_length
         * @param[in] user_type
         * @return same as w_cassandra_statement_bind_user_type_by_name()
         *
         * @see w_cassandra_statement_bind_collection_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_statement_bind_user_type_by_name_n(_In_ w_cass_statement* pStatement, 
                _In_ const char* pName,
                _In_ size_t pNameLength,
                _In_ const w_cass_user_type* pUserType);

        /***********************************************************************************
         *
         * Prepared
         *
         ***********************************************************************************/

         /**
          * Frees a prepared instance.
          *
          * @public @memberof w_Cass_Prepared
          *
          * @param[in] prepared
          */
        W_SYSTEM_EXPORT void
            w_cassandra_prepared_free(_In_ const w_cass_prepared* pPrepared);

        /**
         * Creates a bound statement from a pre-prepared statement.
         *
         * @public @memberof w_Cass_Prepared
         *
         * @param[in] prepared
         * @return Returns a bound statement that must be freed.
         *
         * @see w_cassandra_statement_free()
         */
        W_SYSTEM_EXPORT w_cass_statement* 
            w_cassandra_prepared_bind(_In_ const w_cass_prepared* pPrepared);

        /**
         * Gets the name of a parameter at the specified index.
         *
         * @public @memberof w_Cass_Prepared
         *
         * @param[in] prepared
         * @param[in] index
         * @param[out] name
         * @param[out] name_length
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_prepared_parameter_name(_In_ const w_cass_prepared* pPrepared,
                _In_  size_t pIndex, 
                _Inout_ const char** pName,
                _Inout_ size_t* pNameLength);

        /**
         * Gets the data type of a parameter at the specified index.
         *
         * @public @memberof w_Cass_Prepared
         *
         * @param[in] prepared
         * @param[in] index
         * @return Returns a reference to the data type of the parameter. Do not free
         * this reference as it is bound to the lifetime of the prepared.
         */
        W_SYSTEM_EXPORT const w_cass_data_type*
            w_cassandra_prepared_parameter_data_type(_In_ const  w_cass_prepared* pPrepared,
            _In_  size_t pIndex);

        /**
         * Gets the data type of a parameter for the specified name.
         *
         * @public @memberof w_Cass_Prepared
         *
         * @param[in] prepared
         * @param[in] name
         * @return Returns a reference to the data type of the parameter. Do not free
         * this reference as it is bound to the lifetime of the prepared.
         */
        W_SYSTEM_EXPORT const w_cass_data_type*
            w_cassandra_prepared_parameter_data_type_by_name(_In_ const w_cass_prepared* pPrepared,
                _In_ const char* pName);

        /**
         * Same as w_cassandra_prepared_parameter_data_type_by_name(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Prepared
         *
         * @param[in] prepared
         * @param[in] name
         * @param[in] name_length
         * @return Returns a reference to the data type of the parameter. Do not free
         * this reference as it is bound to the lifetime of the prepared.
         *
         * @see w_cassandra_prepared_parameter_data_type_by_name()
         */
        W_SYSTEM_EXPORT const w_cass_data_type*
            w_cassandra_prepared_parameter_data_type_by_name_n(_In_ const w_cass_prepared* pPrepared, 
            _In_  const char* pName,
            _In_ size_t pNameLength);

        /***********************************************************************************
         *
         * Batch
         *
         ***********************************************************************************/

         /**
          * Creates a new batch statement with batch type.
          *
          * @cassandra{2.0+}
          *
          * @public @memberof w_Cass_Batch
          *
          * @param[in] type
          * @return Returns a batch statement that must be freed.
          *
          * @see w_cassandra_batch_free()
          */
        W_SYSTEM_EXPORT w_cass_batch* 
            w_cassandra_batch_new(_In_ w_cass_batch_type pType);

        /**
         * Frees a batch instance. Batches can be immediately freed after being
         * executed.
         *
         * @cassandra{2.0+}
         *
         * @public @memberof w_Cass_Batch
         *
         * @param[in] batch
         */
        W_SYSTEM_EXPORT void
            w_cassandra_batch_free(_In_ w_cass_batch* pBatch);

        /**
         * Sets the batch's keyspace. When using protocol v5 or greater it overrides
         * the session's keyspace for the batch.
         *
         * <b>Note:</b> If not set explicitly then the batch will inherit the keyspace
         * of the first child statement with a non-empty keyspace.
         *
         * @public @memberof w_Cass_Batch
         *
         * @param[in] batch
         * @param[in] keyspace
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cass_batch_set_keyspace(_In_ w_cass_batch* pBatch,
                _In_ const char* pKeyspace);

        /**
         * Same as cass_batch_set_keyspace(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Batch
         *
         * @param[in] batch
         * @param[in] keyspace
         * @param[in] keyspace_length
         * @return same as cass_batch_set_keyspace()
         *
         * @see w_cassandra_batch_set_keyspace()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_batch_set_keyspace_n(_In_ w_cass_batch* pBatch, 
                _In_  const char* pKeyspace,
                _In_ size_t pKeyspaceLength);

        /**
         * Sets the batch's consistency level
         *
         * @cassandra{2.0+}
         *
         * @public @memberof w_Cass_Batch
         *
         * @param[in] batch
         * @param[in] consistency The batch's write consistency.
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_batch_set_consistency(_In_ w_cass_batch* pBatch,
                _In_ w_cass_consistency pConsistency);

        /**
         * Sets the batch's serial consistency level.
         *
         * @cassandra{2.0+}
         *
         * <b>Default:</b> Not set
         *
         * @public @memberof w_Cass_Batch
         *
         * @param[in] batch
         * @param[in] serial_consistency
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_batch_set_serial_consistency(_In_ w_cass_batch* pBatch, 
                _In_  w_cass_consistency pSerialConsistency);

        /**
         * Sets the batch's timestamp.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Batch
         *
         * @param[in] batch
         * @param[in] timestamp
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_batch_set_timestamp(_In_ w_cass_batch* pBatch, 
                _In_ int64_t pTimeStamp);

        /**
         * Sets the batch's timeout for waiting for a response from a node.
         *
         * <b>Default:</b> Disabled (use the cluster-level request timeout)
         *
         * @public @memberof w_Cass_Batch
         *
         * @param[in] batch
         * @param[in] timeout_ms Request timeout in milliseconds. Use 0 for no timeout
         * or CASS_UINT64_MAX to disable (to use the cluster-level request timeout).
         * @return W_SUCCESS if successful, otherwise an error occurred.
         *
         * @see w_cassandra_cluster_set_request_timeout()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_batch_set_request_timeout(_In_ w_cass_batch* pBatch,
                _In_ uint64_t pTimeOutms);

        /**
         * Sets whether the statements in a batch are idempotent. Idempotent batches
         * are able to be automatically retried after timeouts/errors and can be
         * speculatively executed.
         *
         * @public @memberof w_Cass_Batch
         *
         * @param[in] batch
         * @param[in] is_idempotent
         * @return W_SUCCESS if successful, otherwise an error occurred.
         *
         * @see w_cassandra_cluster_set_constant_speculative_execution_policy()
         * @see w_cassandra_execution_profile_set_constant_speculative_execution_policy()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_batch_set_is_idempotent(_In_ w_cass_batch* pBatch,
                _In_  w_bool_t pIsIdempotent);

        /**
         * Sets the batch's retry policy.
         *
         * @cassandra{2.0+}
         *
         * @public @memberof w_Cass_Batch
         *
         * @param[in] batch
         * @param[in] retry_policy
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_batch_set_retry_policy(_In_ w_cass_batch* pBatch,
                _In_ w_cass_retry_policy* pRetryPolicy);

        /**
         * Sets the batch's custom payload.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Batch
         *
         * @param[in] batch
         * @param[in] payload
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_batch_set_custom_payload(_In_ w_cass_batch* pBatch,
                _In_ const w_cass_custom_payload* pPayload);

        /**
         * Sets whether the batch should use tracing.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Statement
         *
         * @param[in] batch
         * @param[in] enabled
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_batch_set_tracing(_In_ w_cass_batch* pBatch,
                _In_ w_bool_t pEnabled);

        /**
         * Adds a statement to a batch.
         *
         * @cassandra{2.0+}
         *
         * @public @memberof w_Cass_Batch
         *
         * @param[in] batch
         * @param[in] statement
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_batch_add_statement(_In_ w_cass_batch* pBatch, 
                _In_ w_cass_statement* pStatement);

        /**
         * Sets the execution profile to execute the batch with.
         *
         * <b>Note:</b> NULL or empty string will clear execution profile from batch
         *
         * @public @memberof w_Cass_Batch
         *
         * @param[in] batch
         * @param[in] name
         * @return W_SUCCESS if successful, otherwise an error occurred.
         *
         * @see w_cassandra_cluster_set_execution_profile()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_batch_set_execution_profile(_In_ w_cass_batch* pBatch, 
                _In_ const char* pName);

        /**
         * Same as w_cassandra_batch_set_execution_profile(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Batch
         *
         * @param[in] batch
         * @param[in] name
         * @param[in] name_length
         * @return W_SUCCESS if successful, otherwise an error occurred.
         *
         * @see cass_batch_set_execution_profile()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_batch_set_execution_profile_n(_In_ w_cass_batch* pBatch,
                _In_ const char* pName, 
                _In_ size_t pNameLength);

        /***********************************************************************************
         *
         * Data type
         *
         ***********************************************************************************/

         /**
          * Creates a new data type with value type.
          *
          * @public @memberof w_Cass_Data_Type
          *
          * @param[in] type
          * @return Returns a data type that must be freed.
          *
          * @see w_cassandra_data_type_free()
          */
        W_SYSTEM_EXPORT w_cass_data_type* 
            w_cassandra_data_type_new(_In_ w_cass_value_type pType);

        /**
         * Creates a new data type from an existing data type.
         *
         * @public @memberof w_Cass_Data_Type
         *
         * @param[in] data_type
         * @return Returns a data type that must be freed.
         *
         * @see w_cassandra_data_type_free()
         */
        W_SYSTEM_EXPORT w_cass_data_type* 
            w_cassandra_data_type_new_from_existing(_In_ const w_cass_data_type* pDataType);

        /**
         * Creates a new tuple data type.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Data_Type
         *
         * @param[in] item_count The number of items in the tuple
         * @return Returns a data type that must be freed.
         *
         * @see w_cassandra_data_type_free()
         */
        W_SYSTEM_EXPORT w_cass_data_type* 
            w_cassandra_data_type_new_tuple(_In_ size_t ptemCount);

        /**
         * Creates a new UDT (user defined type) data type.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Data_Type
         *
         * @param[in] field_count The number of fields in the UDT
         * @return Returns a data type that must be freed.
         *
         * @see w_cassandra_data_type_free()
         */
        W_SYSTEM_EXPORT w_cass_data_type*
            w_cassandra_data_type_new_udt(_In_ size_t pFieldCount);

        /**
         * Frees a data type instance.
         *
         * @public @memberof w_Cass_Data_Type
         *
         * @param[in] data_type
         */
        W_SYSTEM_EXPORT void
            w_cassandra_data_type_free(_In_ w_cass_data_type* pDataType);

        /**
         * Gets the value type of the specified data type.
         *
         * @param[in] data_type
         * @return The value type
         */
        W_SYSTEM_EXPORT w_cass_value_type
            w_cassandra_data_type_type(_In_ const w_cass_data_type* pDataType);

        /**
         * Gets whether a data type is frozen.
         *
         * @cassandra{2.1+}
         *
         * @param[in] data_type
         * @return cass_true if the data type is frozen, otherwise cass_false.
         */
        W_SYSTEM_EXPORT w_bool_t
            w_cassandra_data_type_is_frozen(_In_ const w_cass_data_type* pDataType);

        /**
         * Gets the type name of a UDT data type.
         *
         * <b>Note:</b> Only valid for UDT data types.
         *
         * @param[in] data_type
         * @param[out] type_name
         * @param[out] type_name_length
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_data_type_type_name(_In_ const w_cass_data_type* pDataType,
                _Inout_  const char** pTypeName,
                _Inout_ size_t* pTypeNameLength);

        /**
         * Sets the type name of a UDT data type.
         *
         * <b>Note:</b> Only valid for UDT data types.
         *
         * @param[in] data_type
         * @param[in] type_name
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_data_type_set_type_name(_In_ w_cass_data_type* pDataType,
                _In_ const char* pTypeName);

        /**
         * Same as w_cassandra_data_type_set_type_name(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Data_Type
         *
         * @param[in] data_type
         * @param[in] type_name
         * @param[in] type_name_length
         * @return Returns a data type that must be freed.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_data_type_set_type_name_n(_In_ w_cass_data_type* pDataType, 
                _In_ const char* pTypeName, 
                _In_ size_t pTypeNameLength);

        /**
         * Gets the type name of a UDT data type.
         *
         * <b>Note:</b> Only valid for UDT data types.
         *
         * @cassandra{2.1+}
         *
         * @param[in] data_type
         * @param[out] keyspace
         * @param[out] keyspace_length
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_data_type_keyspace(_In_ const w_cass_data_type* pDataType, 
                _Inout_ const char** pKeyspace,
                _Inout_ size_t* pKeypaceLength);

        /**
         * Sets the keyspace of a UDT data type.
         *
         * <b>Note:</b> Only valid for UDT data types.
         *
         * @cassandra{2.1+}
         *
         * @param[in] data_type
         * @param[in] keyspace
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_data_type_set_keyspace(_In_ w_cass_data_type* pDataType,
                _In_ const char* pKeyspace);

        /**
         * Same as w_cassandra_data_type_set_keyspace(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Data_Type
         *
         * @param[in] data_type
         * @param[in] keyspace
         * @param[in] keyspace_length
         * @return Returns a data type that must be freed.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_data_type_set_keyspace_n(_In_ w_cass_data_type* pDataType, 
                _In_ const char* pKeypace,
                _In_ size_t pKeypaceLength);

        /**
         * Gets the class name of a custom data type.
         *
         * <b>Note:</b> Only valid for custom data types.
         *
         * @param[in] data_type
         * @param[out] class_name
         * @param[out] class_name_length
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_data_type_class_name(_In_ const w_cass_data_type* pDataType,
                _Inout_ const char** pClassName,
                _Inout_ size_t* pClassNameLength);

        /**
         * Sets the class name of a custom data type.
         *
         * <b>Note:</b> Only valid for custom data types.
         *
         * @param[in] data_type
         * @param[in] class_name
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_data_type_set_class_name(_In_ w_cass_data_type* pDataType,
                _In_ const char* pClassName);

        /**
         * Same as w_cassandra_data_type_set_class_name(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Data_Type
         *
         * @param[in] data_type
         * @param[in] class_name
         * @param[in] class_name_length
         * @return Returns a data type that must be freed.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_data_type_set_class_name_n(_In_ w_cass_data_type* pDataType, 
                _In_ const char* pClassName, 
                _In_ size_t pClassNameLength);

        /**
         * Gets the sub-data type count of a UDT (user defined type), tuple
         * or collection.
         *
         * <b>Note:</b> Only valid for UDT, tuple and collection data types.
         *
         * @param[in] data_type
         * @return Returns the number of sub-data types
         */
        W_SYSTEM_EXPORT size_t
            w_cassandra_data_type_sub_type_count(_In_ const w_cass_data_type* pDataType);

        /**
         * @deprecated Use w_cassandra_data_type_sub_type_count()
         */
        W_SYSTEM_EXPORT size_t
            w_cassandra_data_sub_type_count(const w_cass_data_type* data_type);

        /**
         * Gets the sub-data type count of a UDT (user defined type), tuple
         * or collection.
         *
         * <b>Note:</b> Only valid for UDT, tuple and collection data types.
         *
         * @param[in] data_type
         * @return Returns the number of sub-data types
         */

         /**
          * Gets the sub-data type of a UDT (user defined type), tuple or collection at
          * the specified index.
          *
          * <b>Note:</b> Only valid for UDT, tuple and collection data types.
          *
          * @param[in] data_type
          * @param[in] index
          * @return Returns a reference to a child data type. Do not free this
          * reference as it is bound to the lifetime of the parent data type. NULL
          * is returned if the index is out of range.
          */
        W_SYSTEM_EXPORT const w_cass_data_type* 
            w_cassandra_data_type_sub_data_type(_In_ const w_cass_data_type* pDataType,
            _In_ size_t pIndex);

        /**
         * Gets the sub-data type of a UDT (user defined type) at the specified index.
         *
         * <b>Note:</b> Only valid for UDT data types.
         *
         * @cassandra{2.1+}
         *
         * @param[in] data_type
         * @param[in] name
         * @return Returns a reference to a child data type. Do not free this
         * reference as it is bound to the lifetime of the parent data type. NULL
         * is returned if the name doesn't exist.
         */
        W_SYSTEM_EXPORT const w_cass_data_type*
            w_cassandra_data_type_sub_data_type_by_name(_In_ const w_cass_data_type* pDataType,
            _In_ const char* pName);

        /**
         * Same as w_cassandra_data_type_sub_data_type_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Data_Type
         *
         * @param[in] data_type
         * @param[in] name
         * @param[in] name_length
         * @return Returns a reference to a child data type. Do not free this
         * reference as it is bound to the lifetime of the parent data type. NULL
         * is returned if the name doesn't exist.
         */
        W_SYSTEM_EXPORT const w_cass_data_type* w_cassandra_data_type_sub_data_type_by_name_n(_In_ const w_cass_data_type* pDataType,
            _In_ const char* pName,
            _In_ size_t pNameLength);

        /**
         * Gets the sub-type name of a UDT (user defined type) at the specified index.
         *
         * @cassandra{2.1+}
         *
         * <b>Note:</b> Only valid for UDT data types.
         *
         * @param[in] data_type
         * @param[in] index
         * @param[out] name
         * @param[out] name_length
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_data_type_sub_type_name(_In_ const w_cass_data_type* pDataType, 
                _In_ size_t pIndex,
                _In_  const char** pName, 
                _In_  size_t* pNameLength);

        /**
         * Adds a sub-data type to a tuple or collection.
         *
         * <b>Note:</b> Only valid for tuple and collection data types.
         *
         * @param[in] data_type
         * @param[in] sub_data_type
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_data_type_add_sub_type(_In_ w_cass_data_type* pDataType, 
                _In_ const w_cass_data_type* pSubDataType);

        /**
         * Adds a sub-data type to a UDT (user defined type).
         *
         * <b>Note:</b> Only valid for UDT data types.
         *
         * @cassandra{2.1+}
         *
         * @param[in] data_type
         * @param[in] name
         * @param[in] sub_data_type
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_data_type_add_sub_type_by_name(_In_ w_cass_data_type* pDataType,
                _In_ const char* pName, 
                _In_ const w_cass_data_type* pSubDataType);

        /**
         * Same as w_cassandra_data_type_add_sub_type_by_name(), but with lengths for string
         * parameters.
         *
         * <b>Note:</b> Only valid for UDT data types.
         *
         * @cassandra{2.1+}
         *
         * @param[in] data_type
         * @param[in] name
         * @param[in] name_length
         * @param[in] sub_data_type
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_data_type_add_sub_type_by_name_n(_In_ w_cass_data_type* pDataType,
                _In_  const char* pName, 
                _In_  size_t pNameLength,
                _In_  const w_cass_data_type* pSubDataType);

        /**
         * Adds a sub-data type to a tuple or collection using a value type.
         *
         * <b>Note:</b> Only valid for tuple and collection data types.
         *
         * @param[in] data_type
         * @param[in] sub_value_type
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_data_type_add_sub_value_type(_In_ w_cass_data_type* pDataType, 
                _In_ w_cass_value_type pSubValueType);


        /**
         * Adds a sub-data type to a UDT (user defined type) using a value type.
         *
         * <b>Note:</b> Only valid for UDT data types.
         *
         * @cassandra{2.1+}
         *
         * @param[in] data_type
         * @param[in] name
         * @param[in] sub_value_type
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_data_type_add_sub_value_type_by_name(_In_ w_cass_data_type* pDataType, 
                _In_  const char* pName, 
                _In_  w_cass_value_type pSubValueType);

        /**
         * Same as cass_data_type_add_sub_value_type_by_name(), but with lengths for string
         * parameters.
         *
         * <b>Note:</b> Only valid for UDT data types.
         *
         * @cassandra{2.1+}
         *
         * @param[in] data_type
         * @param[in] name
         * @param[in] name_length
         * @param[in] sub_value_type
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_data_type_add_sub_value_type_by_name_n(_In_ w_cass_data_type* pDataType,
                _In_  const char* pName, 
                _In_  size_t pNameLength,
                _In_  w_cass_value_type sub_value_type);

        /***********************************************************************************
         *
         * Collection
         *
         ***********************************************************************************/

         /**
          * Creates a new collection.
          *
          * @public @memberof w_Cass_Collection
          *
          * @param[in] type
          * @param[in] item_count The approximate number of items in the collection.
          * @return Returns a collection that must be freed.
          *
          * @see w_cassandra_collection_free()
          */
        W_SYSTEM_EXPORT w_cass_collection*
            w_cassandra_collection_new(_In_ w_cass_collection_type pType,
                _In_ size_t pItemCount);

        /**
         * Creates a new collection from an existing data type.
         *
         * @public @memberof w_Cass_Collection
         *
         * @param[in] data_type
         * @param[in] item_count The approximate number of items in the collection.
         * @return Returns a collection that must be freed.
         *
         * @see w_cassandra_collection_free();
         */
        W_SYSTEM_EXPORT w_cass_collection*
            w_cassandra_collection_new_from_data_type(_In_ const w_cass_data_type* pDataType,
            _In_ size_t pItemCount);

        /**
         * Frees a collection instance.
         *
         * @public @memberof w_Cass_Collection
         *
         * @param[in] collection
         */
        W_SYSTEM_EXPORT void
            w_cassandra_collection_free(_In_ w_cass_collection* pCollection);

        /**
         * Gets the data type of a collection.
         *
         * @param[in] collection
         * @return Returns a reference to the data type of the collection. Do not free
         * this reference as it is bound to the lifetime of the collection.
         */
        W_SYSTEM_EXPORT const w_cass_data_type* 
            w_cassandra_collection_data_type(_In_ const w_cass_collection* pCollection);

        /**
         * Appends a "tinyint" to the collection.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Collection
         *
         * @param[in] collection
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_collection_append_int8(_In_ w_cass_collection* pCollection, 
                _In_ int8_t pValue);

        /**
         * Appends an "smallint" to the collection.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Collection
         *
         * @param[in] collection
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_collection_append_int16(_In_ w_cass_collection* pCollection,
                _In_ int16_t pValue);

        /**
         * Appends an "int" to the collection.
         *
         * @public @memberof w_Cass_Collection
         *
         * @param[in] collection
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_collection_append_int32(_In_ w_cass_collection* pCollection,
                _In_ int32_t pValue);

        /**
         * Appends a "date" to the collection.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Collection
         *
         * @param[in] collection
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_collection_append_uint32(_In_ w_cass_collection* pCollection, 
                _In_ uint32_t pValue);


        /**
         * Appends a "bigint", "counter", "timestamp" or "time" to the
         * collection.
         *
         * @public @memberof w_Cass_Collection
         *
         * @param[in] collection
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_collection_append_int64(_In_ w_cass_collection* pCollection,
                _In_ int64_t pValue);

        /**
         * Appends a "float" to the collection.
         *
         * @public @memberof w_Cass_Collection
         *
         * @param[in] collection
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_collection_append_float(_In_ w_cass_collection* pCollection, 
                _In_ float pValue);

        /**
         * Appends a "double" to the collection.
         *
         * @public @memberof w_Cass_Collection
         *
         * @param[in] collection
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_collection_append_double(_In_ w_cass_collection* pCollection,
                _In_ double pValue);

        /**
         * Appends a "boolean" to the collection.
         *
         * @public @memberof w_Cass_Collection
         *
         * @param[in] collection
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_collection_append_bool(_In_ w_cass_collection* pCollection,
                _In_ w_bool_t pValue);

        /**
         * Appends an "ascii", "text" or "varchar" to the collection.
         *
         * @public @memberof w_Cass_Collection
         *
         * @param[in] collection
         * @param[in] value The value is copied into the collection object; the
         * memory pointed to by this parameter can be freed after this call.
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_collection_append_string(_In_ w_cass_collection* pCollection, 
                _In_  const char* pValue);


        /**
         * Same as w_cassandra_collection_append_string(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Collection
         *
         * @param[in] collection
         * @param[in] value
         * @param[in] value_length
         * @return same as w_cassandra_collection_append_string()
         *
         * @see w_cassandra_collection_append_string();
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_collection_append_string_n(_In_ w_cass_collection* pCollection,
                _In_  const char* pValue, 
                _In_ size_t pValueLength);

        /**
         * Appends a "blob", "varint" or "custom" to the collection.
         *
         * @public @memberof w_Cass_Collection
         *
         * @param[in] collection
         * @param[in] value The value is copied into the collection object; the
         * memory pointed to by this parameter can be freed after this call.
         * @param[in] value_size
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_collection_append_bytes(_In_ w_cass_collection* pCollection, 
                _In_  const w_byte_t* pValue, 
                _In_ size_t pValueSize);

        /**
         * Appends a "custom" to the collection.
         *
         * @public @memberof w_Cass_Collection
         *
         * @param[in] collection
         * @param[in] class_name
         * @param[in] value The value is copied into the collection object; the
         * memory pointed to by this parameter can be freed after this call.
         * @param[in] value_size
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_collection_append_custom(_In_ w_cass_collection* pCollection, 
                _In_ const char* pClassName, 
                _In_  const w_byte_t* pValue,
                _In_ size_t pValueSize);

        /**
         * Same as w_cassandra_collection_append_custom(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Collection
         *
         * @param[in] collection
         * @param[in] class_name
         * @param[in] class_name_length
         * @param[in] value
         * @param[in] value_size
         * @return same as w_cassandra_collection_append_custom()
         *
         * @see w_cassandra_collection_append_custom()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_collection_append_custom_n(_In_ w_cass_collection* pCollection,
                _In_  const char* pClassName,
                _In_ size_t pClassNameLength,
                _In_ const w_byte_t* pValue,
                _In_ size_t pValueSize);

        /**
         * Appends a "uuid" or "timeuuid"  to the collection.
         *
         * @public @memberof w_Cass_Collection
         *
         * @param[in] collection
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_collection_append_uuid(_In_ w_cass_collection* pCollection, 
                _In_ w_cass_uuid pValue);

        /**
         * Appends an "inet" to the collection.
         *
         * @public @memberof w_Cass_Collection
         *
         * @param[in] collection
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_collection_append_inet(_In_ w_cass_collection* pCollection,
                _In_ w_cass_inet pValue);

        /**
         * Appends a "decimal" to the collection.
         *
         * @public @memberof w_Cass_Collection
         *
         * @param[in] collection
         * @param[in] varint The value is copied into the collection object; the
         * memory pointed to by this parameter can be freed after this call.
         * @param[in] varint_size
         * @param[in] scale
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_collection_append_decimal(_In_ w_cass_collection* pCollection,
                _In_ const w_byte_t* pVarint,
                _In_ size_t pVarintSize,
                _In_ int32_t pScale);

        /**
         * Appends a "duration" to the collection.
         *
         * @cassandra{3.10+}
         *
         * @public @memberof w_Cass_Collection
         *
         * @param[in] collection
         * @param[in] months
         * @param[in] days
         * @param[in] nanos
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_collection_append_duration(_In_ w_cass_collection* pCollection, _In_ int32_t pMonths, _In_ int32_t pDays, _In_ int64_t pNanos);

        /**
         * Appends a "list", "map" or "set" to the collection.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Collection
         *
         * @param[in] collection
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_collection_append_collection(_In_ w_cass_collection* pCollection, 
                _In_ const w_cass_collection* pValue);

        /**
         * Appends a "tuple" to the collection.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Collection
         *
         * @param[in] collection
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_collection_append_tuple(_In_ w_cass_collection* pCollection,
                _In_ const w_cass_tuple* pValue);

        /**
         * Appends a "udt" to the collection.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Collection
         *
         * @param[in] collection
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_collection_append_user_type(_In_ w_cass_collection* pCollection,
                _In_ const w_cass_user_type* pValue);

        /***********************************************************************************
         *
         * Tuple
         *
         ***********************************************************************************/

         /**
          * Creates a new tuple.
          *
          * @cassandra{2.1+}
          *
          * @public @memberof w_Cass_Tuple
          *
          * @param[in] item_count The number of items in the tuple.
          * @return Returns a tuple that must be freed.
          *
          * @see w_cassandra_tuple_free()
          */
        W_SYSTEM_EXPORT w_cass_tuple* 
            w_cassandra_tuple_new(_In_ size_t pItemCount);

        /**
         * Creates a new tuple from an existing data type.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Tuple
         *
         * @param[in] data_type
         * @return Returns a tuple that must be freed.
         *
         * @see w_cassandra_tuple_free();
         */
        W_SYSTEM_EXPORT w_cass_tuple*
            w_cassandra_tuple_new_from_data_type(_In_ const w_cass_data_type* pDataType);

        /**
         * Frees a tuple instance.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Tuple
         *
         * @param[in] tuple
         */
        W_SYSTEM_EXPORT void
            w_cassandra_tuple_free(_In_ w_cass_tuple* pTuple);

        /**
         * Gets the data type of a tuple.
         *
         * @cassandra{2.1+}
         *
         * @param[in] tuple
         * @return Returns a reference to the data type of the tuple. Do not free
         * this reference as it is bound to the lifetime of the tuple.
         */
        W_SYSTEM_EXPORT  const w_cass_data_type* 
            w_cassandra_tuple_data_type(_In_ const w_cass_tuple* pTuple);

        /**
         * Sets an null in a tuple at the specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Tuple
         *
         * @param[in] tuple
         * @param[in] index
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_tuple_set_null(_In_ w_cass_tuple* pTuple, _In_  size_t pIndex);

        /**
         * Sets a "tinyint" in a tuple at the specified index.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Tuple
         *
         * @param[in] tuple
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_tuple_set_int8(_In_ w_cass_tuple* pTuple,
                _In_ size_t pIndex,
                int8_t pValue);

        /**
         * Sets an "smallint" in a tuple at the specified index.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Tuple
         *
         * @param[in] tuple
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_tuple_set_int16(_In_ w_cass_tuple* pTuple,
                _In_ size_t pIndex, 
                _In_ int16_t pValue);

        /**
         * Sets an "int" in a tuple at the specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Tuple
         *
         * @param[in] tuple
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_tuple_set_int32(_In_ w_cass_tuple* pTuple,
                _In_ size_t pIndex,
                _In_ int32_t pValue);

        /**
         * Sets a "date" in a tuple at the specified index.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Tuple
         *
         * @param[in] tuple
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_tuple_set_uint32(_In_ w_cass_tuple* pTuple,
                _In_ size_t pIndex,
                _In_  uint32_t pValue);

        /**
         * Sets a "bigint", "counter", "timestamp" or "time" in a tuple at the
         * specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Tuple
         *
         * @param[in] tuple
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_tuple_set_int64(_In_ w_cass_tuple* pTuple,
                _In_ size_t pIndex,
                _In_ int64_t pValue);

        /**
         * Sets a "float" in a tuple at the specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Tuple
         *
         * @param[in] tuple
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_tuple_set_float(_In_ w_cass_tuple* pTuple,
                _In_ size_t pIndex,
                _In_ float pValue);

        /**
         * Sets a "double" in a tuple at the specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Tuple
         *
         * @param[in] tuple
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_tuple_set_double(_In_ w_cass_tuple* pTuple, 
                _In_ size_t pIndex, 
                _In_ double pValue);

        /**
         * Sets a "boolean" in a tuple at the specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Tuple
         *
         * @param[in] tuple
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_tuple_set_bool(_In_ w_cass_tuple* pTuple,
                _In_ size_t pIndex,
                _In_ w_bool_t pValue);

        /**
         * Sets an "ascii", "text" or "varchar" in a tuple at the specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Tuple
         *
         * @param[in] tuple
         * @param[in] index
         * @param[in] value The value is copied into the tuple object; the
         * memory pointed to by this parameter can be freed after this call.
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_tuple_set_string(_In_ w_cass_tuple* pTuple,
                _In_ size_t pIndex, 
                _In_ const char* pValue);

        /**
         * Same as w_cassandra_tuple_set_string(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Tuple
         *
         * @param[in] tuple
         * @param[in] index
         * @param[in] value
         * @param[in] value_length
         * @return same as w_cassandra_tuple_set_string()
         *
         * @see w_cassandra_tuple_set_string();
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_tuple_set_string_n(_In_ w_cass_tuple* pTuple,
                _In_  size_t pIndex,
                _In_ const char* pValue, 
                _In_ size_t pValueLength);

        /**
         * Sets a "blob", "varint" or "custom" in a tuple at the specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Tuple
         *
         * @param[in] tuple
         * @param[in] index
         * @param[in] value The value is copied into the tuple object; the
         * memory pointed to by this parameter can be freed after this call.
         * @param[in] value_size
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_tuple_set_bytes(_In_ w_cass_tuple* pTuple,
                _In_ size_t pIndex,
                _In_ const w_byte_t* pValue, 
                _In_ size_t pValueSize);

        /**
         * Sets a "custom" in a tuple at the specified index.
         *
         * @public @memberof w_Cass_Tuple
         *
         * @param[in] tuple
         * @param[in] index
         * @param[in] class_name
         * @param[in] value The value is copied into the tuple object; the
         * memory pointed to by this parameter can be freed after this call.
         * @param[in] value_size
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_tuple_set_custom(_In_ w_cass_tuple* pTuple,
                _In_ size_t pIndex,
                _In_ const char* pClassName, 
                _In_ const w_byte_t* pValue,
                _In_ size_t pValueSize);

        /**
         * Same as w_cassandra_tuple_set_custom(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Tuple
         *
         * @param[in] tuple
         * @param[in] index
         * @param[in] class_name
         * @param[in] class_name_length
         * @param[in] value
         * @param[in] value_size
         * @return same as w_cassandra_tuple_set_custom()
         *
         * @see w_cassandra_tuple_set_custom()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_tuple_set_custom_n(_In_ w_cass_tuple* pTuple, 
                _In_ size_t pIndex, 
                _In_ const char* pClassName, 
                _In_  size_t pClassNameLength,
                _In_ const w_byte_t* pValue,
                _In_ size_t pValueSize);

        /**
         * Sets a "uuid" or "timeuuid" in a tuple at the specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Tuple
         *
         * @param[in] tuple
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_tuple_set_uuid(_In_ w_cass_tuple* pTuple,
                _In_ size_t pIndex,
                _In_ w_cass_uuid pValue);

        /**
         * Sets an "inet" in a tuple at the specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Tuple
         *
         * @param[in] tuple
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_tuple_set_inet(_In_ w_cass_tuple* pTuple, 
                _In_ size_t pIndex,
                _In_ w_cass_inet pValue);

        /**
         * Sets a "decimal" in a tuple at the specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Tuple
         *
         * @param[in] tuple
         * @param[in] index
         * @param[in] varint The value is copied into the tuple object; the
         * memory pointed to by this parameter can be freed after this call.
         * @param[in] varint_size
         * @param[in] scale
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_tuple_set_decimal(_In_ w_cass_tuple* pTuple, 
                _In_ size_t pIndex,
                _In_ const w_byte_t* pVarint,
                _In_ size_t pVarintSize,
                _In_ int32_t pScale);

        /**
         * Sets a "duration" in a tuple at the specified index.
         *
         * @cassandra{3.10+}
         *
         * @public @memberof w_Cass_Tuple
         *
         * @param[in] tuple
         * @param[in] index
         * @param[in] months
         * @param[in] days
         * @param[in] nanos
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_tuple_set_duration(_In_ w_cass_tuple* pTuple, 
                _In_  size_t pIndex, 
                _In_ int32_t pMonths,
                _In_ int32_t pDays,
                _In_ int64_t pNanos); ;

        /**
         * Sets a "list", "map" or "set" in a tuple at the specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Tuple
         *
         * @param[in] tuple
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_tuple_set_collection(_In_ w_cass_tuple* pTuple,
                _In_ size_t pIndex,
                _In_ const w_cass_collection* pValue); ;

        /**
         * Sets a "tuple" in a tuple at the specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Tuple
         *
         * @param[in] tuple
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_tuple_set_tuple(_In_ w_cass_tuple* pTuple,
                _In_ size_t pIndex, 
                _In_ const w_cass_tuple* pValue);

        /**
         * Sets a "udt" in a tuple at the specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Tuple
         *
         * @param[in] tuple
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_tuple_set_user_type(_In_ w_cass_tuple* pTuple, 
                _In_ size_t pIndex,
                _In_ const w_cass_user_type* pValue);

        /***********************************************************************************
         *
         * User defined type
         *
         ***********************************************************************************/

         /**
          * Creates a new user defined type from existing data type;
          *
          * @cassandra{2.1+}
          *
          * @public @memberof w_Cass_User_Type
          *
          * @param[in] data_type
          * @return Returns a user defined type that must be freed. NULL is returned if
          * the data type is not a user defined type.
          *
          * @see w_cassandra_user_type_free()
          */
        W_SYSTEM_EXPORT w_cass_user_type* 
            w_cassandra_user_type_new_from_data_type(_In_ const w_cass_data_type* pDataType);

        /**
         * Frees a user defined type instance.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         */
        W_SYSTEM_EXPORT void
            w_cassandra_user_type_free(_In_ w_cass_user_type* pUserType);

        /**
         * Gets the data type of a user defined type.
         *
         * @cassandra{2.1+}
         *
         * @param[in] user_type
         * @return Returns a reference to the data type of the user defined type.
         * Do not free this reference as it is bound to the lifetime of the
         * user defined type.
         */
        W_SYSTEM_EXPORT const w_cass_data_type* 
            w_cassandra_user_type_data_type(_In_ const w_cass_user_type* pUserType);

        /**
         * Sets a null in a user defined type at the specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] index
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_null(_In_ w_cass_user_type* pUserType, _In_ size_t pIndex);

        /**
         * Sets a null in a user defined type at the specified name.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_null_by_name(_In_ w_cass_user_type* pUserType, 
                _In_ const char* pName);

        /**
         * Same as w_cassandra_user_type_set_null_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] name_length
         * @return same as w_cassandra_user_type_set_null_by_name()
         *
         * @see w_cassandra_user_type_set_null_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_null_by_name_n(_In_ w_cass_user_type* pUserType,
                _In_ const char* pName, 
                _In_ size_t pNameLength);

        /**
         * Sets a "tinyint" in a user defined type at the specified index.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_int8(_In_ w_cass_user_type* pUserType,
                _In_ size_t pIndex, 
                _In_ int8_t pValue);

        /**
         * Sets a "tinyint" in a user defined type at the specified name.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_int8_by_name(_In_ w_cass_user_type* pUserType, 
                _In_ const char* pName,
                _In_ int8_t pValue);

        /**
         * Same as w_cassandra_user_type_set_int8_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] name_length
         * @param[in] value
         * @return same as w_cassandra_user_type_set_int8_by_name()
         *
         * @see w_cassandra_user_type_set_int8_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_int8_by_name_n(_In_ w_cass_user_type* pUserType,
                _In_  const char* pName, 
                _In_  size_t pNameLength,
                _In_ int8_t pValue);

        /**
         * Sets an "smallint" in a user defined type at the specified index.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_int16(_In_ w_cass_user_type* pUserType, 
                _In_ size_t pIndex,
                _In_ int16_t pValue);

        /**
         * Sets an "smallint" in a user defined type at the specified name.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_int16_by_name(_In_ w_cass_user_type* pUserType, 
                _In_ const char* pName,
                _In_ int16_t pValue);

        /**
         * Same as w_cassandra_user_type_set_int16_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] name_length
         * @param[in] value
         * @return same as w_cassandra_user_type_set_int16_by_name()
         *
         * @see w_cassandra_user_type_set_int16_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_int16_by_name_n(_In_ w_cass_user_type* pUserType,
                _In_ const char* pName,
                _In_ size_t pNameLength,
                _In_ int16_t pValue);

        /**
         * Sets an "int" in a user defined type at the specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_int32(_In_ w_cass_user_type* pUserType, 
                _In_ size_t pIndex,
                _In_ int32_t pValue);

        /**
         * Sets an "int" in a user defined type at the specified name.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_int32_by_name(_In_ w_cass_user_type* pUserType,
                _In_ const char* pName,
                _In_ int32_t pValue);

        /**
         * Same as w_cassandra_user_type_set_int32_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] name_length
         * @param[in] value
         * @return same as w_cassandra_user_type_set_int32_by_name()
         *
         * @see w_cassandra_user_type_set_int32_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_int32_by_name_n(_In_ w_cass_user_type* pUserType, 
                _In_ const char* pName,
                _In_ size_t pNameLength,
                int32_t pValue);

        /**
         * Sets a "date" in a user defined type at the specified index.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_uint32(_In_ w_cass_user_type* pUserType, 
                _In_ size_t pIndex, 
                _In_ uint32_t pValue);

        /**
         * Sets a "date" in a user defined type at the specified name.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_uint32_by_name(_In_ w_cass_user_type* pUserType,
                _In_ const char* pName, 
                _In_ uint32_t pValue);

        /**
         * Same as w_cassandra_user_type_set_uint32_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] name_length
         * @param[in] value
         * @return same as w_cassandra_user_type_set_uint32_by_name()
         *
         * @see w_cassandra_user_type_set_uint32_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_uint32_by_name_n(_In_ w_cass_user_type* pUserType, 
                _In_ const char* pName, 
                _In_ size_t pNameLength,
                _In_ uint32_t pValue);


        /**
         * Sets an "bigint", "counter", "timestamp" or "time" in a
         * user defined type at the specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_int64(_In_ w_cass_user_type* pUserType, 
                _In_ size_t pIndex,
                _In_ int64_t pValue);

        /**
         * Sets an "bigint", "counter", "timestamp" or "time" in a
         * user defined type at the specified name.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_int64_by_name(_In_ w_cass_user_type* pUserType, 
                _In_ const char* pName,
                _In_ int64_t pValue);

        /**
         * Same as w_cassandra_user_type_set_int64_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] name_length
         * @param[in] value
         * @return same as w_cassandra_user_type_set_int64_by_name()
         *
         * @see w_cassandra_user_type_set_int64_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_int64_by_name_n(_In_ w_cass_user_type* pUserType,
                _In_ const char* pName,
                _In_ size_t pNameLength,
                _In_ int64_t pValue);

        /**
         * Sets a "float" in a user defined type at the specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_float_by_name(_In_ w_cass_user_type* pUserType,
                _In_ const char* pName,
                _In_ float pValue);

        /**
         * Sets a "float" in a user defined type at the specified name.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_float_by_name_n(_In_ w_cass_user_type* pUserType,
                _In_ const char* pName, 
                _In_ size_t pNameLength, 
                _In_ float pValue);

        /**
         * Same as w_cassandra_user_type_set_float_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] name_length
         * @param[in] value
         * @return same as w_cassandra_user_type_set_float_by_name()
         *
         * @see w_cassandra_user_type_set_float_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_float_by_name_n(_In_ w_cass_user_type* pUserType,
                _In_ const char* pName,
                _In_ size_t pNameLength,
                _In_ float pValue);

        /**
         * Sets an "double" in a user defined type at the specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_double(_In_ w_cass_user_type* pUserType, 
                _In_ size_t pIndex, _In_ double pValue);

        /**
         * Sets an "double" in a user defined type at the specified name.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_double_by_name(_In_ w_cass_user_type* pUserType,
                _In_ const char* pName, 
                _In_ double pValue);

        /**
         * Same as w_cassandra_user_type_set_double_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] name_length
         * @param[in] value
         * @return same as w_cassandra_user_type_set_double_by_name()
         *
         * @see w_cassandra_user_type_set_double_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cass_user_type_set_double_by_name_n(_In_ w_cass_user_type* pUserType,
                _In_ const char* pName, 
                _In_ size_t pNameLength, 
                _In_ double pValue);

        /**
         * Sets a "boolean" in a user defined type at the specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_bool(_In_ w_cass_user_type* pUserType,
                _In_ size_t pIndex,
                _In_ w_bool_t pValue);

        /**
         * Sets a "boolean" in a user defined type at the specified name.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_bool_by_name(_In_ w_cass_user_type* pUserType,
                _In_  const char* pName, 
                _In_ w_bool_t pValue);

        /**
         * Same as w_cassandra_user_type_set_double_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] name_length
         * @param[in] value
         * @return same as w_cassandra_user_type_set_double_by_name()
         *
         * @see w_cassandra_user_type_set_double_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_bool_by_name_n(_In_ w_cass_user_type* pUserType, 
                _In_ const char* pName, 
                _In_ size_t pNameLength,
                _In_ w_bool_t pValue);


        /**
         * Sets an "ascii", "text" or "varchar" in a user defined type at the
         * specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_string(_In_ w_cass_user_type* pUserType,
                _In_ size_t pIndex,
                _In_ const char* pValue);

        /**
         * Same as w_cassandra_user_type_set_string(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] index
         * @param[in] value
         * @param[in] value_length
         * @return same as w_cassandra_user_type_set_string()
         *
         * @see w_cassandra_user_type_set_string()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_string_n(_In_ w_cass_user_type* pUserType, 
                _In_ size_t pIndex, 
                _In_ const char* pValue, 
                _In_ size_t pValueLength);

        /**
         * Sets an "ascii", "text" or "varchar" in a user defined type at the
         * specified name.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_string_by_name(_In_ w_cass_user_type* pUserType, 
                _In_  const char* pName,
                _In_ const char* pValue);

        /**
         * Same as w_cassandra_user_type_set_string_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] name_length
         * @param[in] value
         * @param[in] value_length
         * @return same as w_cassandra_user_type_set_string_by_name()
         *
         * @see w_cassandra_user_type_set_string_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_string_by_name_n(_In_ w_cass_user_type* pUserType,
                _In_ const char* pName,
                _In_ size_t pNameLength, 
                _In_ const char* pValue,
                _In_  size_t pValueLength);

        /**
         * Sets a "blob" "varint" or "custom" in a user defined type at the specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] index
         * @param[in] value
         * @param[in] value_size
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_bytes(_In_ w_cass_user_type* pUserType,
                _In_ size_t pIndex,
                _In_ const w_byte_t* pValue, 
                _In_ size_t pValueSize);

        /**
         * Sets a "blob", "varint" or "custom" in a user defined type at the specified name.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] value
         * @param[in] value_size
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_bytes_by_name(_In_ w_cass_user_type* pUserType,
                _In_ const char* pName,
                _In_ const w_byte_t* pValue,
                _In_ size_t pValueSize);

        /**
         * Same as w_cassandra_user_type_set_bytes_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] name_length
         * @param[in] value
         * @param[in] value_size
         * @return same as w_cassandra_user_type_set_bytes_by_name()
         *
         * @see w_cassandra_user_type_set_bytes_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_bytes_by_name_n(_In_ w_cass_user_type* pUserType, 
                _In_ const char* pName, _In_ size_t pNameLength, 
                _In_ const w_byte_t* pValue,
                _In_ size_t pValueSize);

        /**
         * Sets a "custom" in a user defined type at the specified index.
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] index
         * @param[in] class_name
         * @param[in] value
         * @param[in] value_size
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_custom(_In_ w_cass_user_type* pUserType, 
                _In_ size_t pIndex,
                _In_  const char* pClassName,
                _In_ const w_byte_t* pValue,
                _In_ size_t pValueSize);

        /**
         * Same as w_cassandra_user_type_set_custom(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] index
         * @param[in] class_name
         * @param[in] class_name_length
         * @param[in] value
         * @param[in] value_size
         * @return same as w_cassandra_user_type_set_custom()
         *
         * @see w_cassandra_user_type_set_custom()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_custom_n(_In_ w_cass_user_type* pUserType,
                _In_ size_t pIndex,
                _In_ const char* pClassName, 
                _In_ size_t pClassNameLength, 
                _In_  const w_byte_t* pValue,
                _In_ size_t pValueSize);

        /**
         * Sets a "custom" in a user defined type at the specified name.
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] class_name
         * @param[in] value
         * @param[in] value_size
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_custom_by_name(_In_ w_cass_user_type* pUserType,
                _In_ const char* pName,
                _In_ const char* pClassName,
                _In_ const w_byte_t* pValue,
                _In_ size_t pValueSize);

        /**
         * Same as w_cassandra_user_type_set_custom_by_name(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] name_length
         * @param[in] class_name
         * @param[in] class_name_length
         * @param[in] value
         * @param[in] value_size
         * @return same as w_cassandra_user_type_set_custom_by_name()
         *
         * @see w_cassandra_user_type_set_custom_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_custom_by_name_n(_In_ w_cass_user_type* pUserType,
                _In_ const char* pName, 
                _In_ size_t pNameLength, 
                _In_ const char* pClassName,
                _In_ size_t pClassNameLength, 
                _In_ const w_byte_t* pValue, 
                _In_ size_t pValueSize);

        /**
         * Sets a "uuid" or "timeuuid" in a user defined type at the specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_uuid(_In_ w_cass_user_type* pUserType,
                _In_ size_t pIndex,
                _In_ w_cass_uuid pValue);

        /**
         * Sets a "uuid" or "timeuuid" in a user defined type at the specified name.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_uuid_by_name(_In_ w_cass_user_type* pUserType, 
                _In_ const char* pName, 
                _In_ w_cass_uuid pValue);

        /**
         * Same as w_cassandra_user_type_set_uuid_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] name_length
         * @param[in] value
         * @return same as w_cassandra_user_type_set_uuid_by_name()
         *
         * @see w_cassandra_user_type_set_uuid_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_uuid_by_name_n(_In_ w_cass_user_type* pUserType, 
                _In_ const char* pName, 
                _In_ size_t pNameLength,
                _In_ w_cass_uuid pValue);

        /**
         * Sets a "inet" in a user defined type at the specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cass_user_type_set_inet(_In_ w_cass_user_type* pUserType,
                _In_  size_t pIndex, 
                _In_ w_cass_inet pValue);

        /**
         * Sets a "inet" in a user defined type at the specified name.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cass_user_type_set_inet_by_name(_In_ w_cass_user_type* pUserType, 
                _In_ const char* pName,
                _In_ w_cass_inet pValue);

        /**
         * Same as w_cassandra_user_type_set_inet_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] name_length
         * @param[in] value
         * @return same as w_cassandra_user_type_set_inet_by_name()
         *
         * @see w_cassandra_user_type_set_inet_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_inet_by_name_n(_In_ w_cass_user_type* pUserType,
                _In_ const char* pName, 
                _In_ size_t pNameLength, 
                _In_ w_cass_inet pValue);

        /**
         * Sets an "decimal" in a user defined type at the specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] index
         * @param[in] varint
         * @param[in] varint_size
         * @param[in] scale
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_decimal(_In_ w_cass_user_type* pUserType, 
                _In_ size_t pIndex, 
                _In_ const w_byte_t* pVarint, 
                _In_ size_t pVarintSize, 
                _In_  int pScale);

        /**
         * Sets "decimal" in a user defined type at the specified name.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] varint
         * @param[in] varint_size
         * @param[in] scale
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_decimal_by_name(_In_ w_cass_user_type* pUserType, 
                _In_ const char* pName, 
                _In_ const w_byte_t* pVarint, 
                _In_ size_t pVarintSize, 
                _In_ int pScale);

        /**
         * Same as w_cassandra_user_type_set_decimal_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] name_length
         * @param[in] varint
         * @param[in] varint_size
         * @param[in] scale
         * @return same as w_cassandra_user_type_set_decimal_by_name()
         *
         * @see w_cassandra_user_type_set_decimal_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_decimal_by_name_n(_In_ w_cass_user_type* pUserType, 
                _In_ const char* pName, 
                _In_ size_t pNameLength, 
                _In_ const w_byte_t* pVarint,
                _In_ size_t pVarintSize, 
                _In_ int pScale);

        /**
         * Sets a "duration" in a user defined type at the specified index.
         *
         * @cassandra{3.10+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] index
         * @param[in] months
         * @param[in] days
         * @param[in] nanos
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_duration(_In_ w_cass_user_type* pUserType,
                _In_ size_t pIndex,
                _In_  int32_t pMonths,
                _In_ int32_t pDays, 
                _In_ int64_t pNanos);

        /**
         * Sets "duration" in a user defined type at the specified name.
         *
         * @cassandra{3.10+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] months
         * @param[in] days
         * @param[in] nanos
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_duration_by_name(_In_ w_cass_user_type* pUserType,
                _In_  const char* pName,
                _In_ int32_t pMonths, 
                _In_  int32_t pDays,
                _In_  int64_t pNanos);

        /**
         * Same as w_cassandra_user_type_set_duration_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{3.10+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] name_length
         * @param[in] months
         * @param[in] days
         * @param[in] nanos
         * @return same as w_cassandra_user_type_set_duration_by_name()
         *
         * @see w_cassandra_user_type_set_duration_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_duration_by_name_n(_In_ w_cass_user_type* pUserType,
                _In_  const char* pName, 
                _In_   size_t pNameLength,
                _In_ int32_t pMonths,
                _In_ int32_t pDays, 
                _In_ int64_t pNanos);

        /**
         * Sets a "list", "map" or "set" in a user defined type at the
         * specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_collection(_In_ w_cass_user_type* pUserType, 
                _In_ size_t pIndex, 
                _In_ const w_cass_collection* pValue);

        /**
         * Sets a "list", "map" or "set" in a user defined type at the
         * specified name.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_collection_by_name(_In_ w_cass_user_type* pUserType, 
                _In_ const char* pName, 
                _In_ const w_cass_collection* pValue);

        /**
         * Same as w_cassandra_user_type_set_collection_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] name_length
         * @param[in] value
         * @return same as w_cassandra_user_type_set_collection_by_name()
         *
         * @see w_cassandra_user_type_set_collection_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_collection_by_name_n(_In_ w_cass_user_type* pUserType, 
                _In_ const char* pName,
                _In_ size_t pNameLength,
                _In_ const w_cass_collection* pValue);

        /**
         * Sets a "tuple" in a user defined type at the specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_tuple(_In_ w_cass_user_type* pUserType,
                _In_ size_t pIndex, 
                _In_ const w_cass_tuple* pValue);

        /**
         * Sets a "tuple" in a user defined type at the specified name.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_tuple_by_name(_In_ w_cass_user_type* pUserType,
                _In_ const char* pName, 
                _In_ const w_cass_tuple* pValue);

        /**
         * Same as w_cassandra_user_type_set_tuple_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] name_length
         * @param[in] value
         * @return same as w_cassandra_user_type_set_tuple_by_name()
         *
         * @see w_cassandra_user_type_set_tuple_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_tuple_by_name_n(_In_ w_cass_user_type* pUserType, 
                _In_ const char* pName, 
                _In_ size_t pNameLength, 
                _In_ const w_cass_tuple* pValue);

        /**
         * Sets a user defined type in a user defined type at the specified index.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] index
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_user_type(_In_ w_cass_user_type* pUserType, 
                _In_ size_t pIndex, 
                _In_ const w_cass_user_type* pValue);

        /**
         * Sets a user defined type in a user defined type at the specified name.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] value
         * @return W_SUCCESS if successful, otherwise an error occurred.
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cass_user_type_set_user_type_by_name(_In_ w_cass_user_type* pUserType,
                _In_ const char* pName,
                _In_ const w_cass_user_type* pValue);

        /**
         * Same as w_cassandra_user_type_set_user_type_by_name(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_User_Type
         *
         * @param[in] user_type
         * @param[in] name
         * @param[in] name_length
         * @param[in] value
         * @return same as w_cassandra_user_type_set_user_type_by_name()
         *
         * @see w_cassandra_user_type_set_user_type_by_name()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_user_type_set_user_type_by_name_n(_In_ w_cass_user_type* pUserType, 
                _In_ const char* pName, 
                _In_ size_t pNameLength,
                _In_ const w_cass_user_type* pValue);

        /***********************************************************************************
         *
         * Result
         *
         ***********************************************************************************/

         /**
          * Frees a result instance.
          *
          * This method invalidates all values, rows, and
          * iterators that were derived from this result.
          *
          * @public @memberof w_Cass_Result
          *
          * @param[in] result
          */
        W_SYSTEM_EXPORT void
            w_cassandra_result_free(_In_ const w_cass_result* pResult);

        /**
         * Gets the number of rows for the specified result.
         *
         * @public @memberof w_Cass_Result
         *
         * @param[in] result
         * @return The number of rows in the result.
         */
        W_SYSTEM_EXPORT size_t
            w_cassandra_result_row_count(_In_ const w_cass_result* pResult);

        /**
         * Gets the number of columns per row for the specified result.
         *
         * @public @memberof w_Cass_Result
         *
         * @param[in] result
         * @return The number of columns per row in the result.
         */
        W_SYSTEM_EXPORT size_t
            w_cassandra_result_column_count(_In_ const w_cass_result* pResult);

        /**
         * Gets the column name at index for the specified result.
         *
         * @public @memberof w_Cass_Result
         *
         * @param[in] result
         * @param[in] index
         * @param[out] name The column name at the specified index.
         * @param[out] name_length
         * @return W_SUCCESS if successful, otherwise error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_result_column_name(_In_ const w_cass_result* pResult,
                _In_  size_t pIndex, 
                _Inout_ const char** pName,
                _Inout_  size_t* pNameLength);

        /**
         * Gets the column type at index for the specified result.
         *
         * @public @memberof w_Cass_Result
         *
         * @param[in] result
         * @param[in] index
         * @return The column type at the specified index. CASS_VALUE_TYPE_UNKNOWN
         * is returned if the index is out of bounds.
         */
        W_SYSTEM_EXPORT w_cass_value_type 
            w_cassandra_result_column_type(_In_ const w_cass_result* pResult,
                _In_ size_t pIndex);

        /**
         * Gets the column data type at index for the specified result.
         *
         * @public @memberof w_Cass_Result
         *
         * @param[in] result
         * @param[in] index
         * @return The column type at the specified index. NULL is returned if the
         * index is out of bounds.
         */
        W_SYSTEM_EXPORT const w_cass_data_type* w_cassandra_result_column_data_type(_In_ const w_cass_result* pResult,
            _In_ size_t pIndex);

        /**
         * Gets the first row of the result.
         *
         * @public @memberof w_Cass_Result
         *
         * @param[in] result
         * @return The first row of the result. NULL if there are no rows.
         */
        W_SYSTEM_EXPORT const w_cass_row* 
            w_cassandra_result_first_row(_In_ const w_cass_result* pResult);

        /**
         * Returns true if there are more pages.
         *
         * @cassandra{2.0+}
         *
         * @public @memberof w_Cass_Result
         *
         * @param[in] result
         * @return cass_true if there are more pages
         */
        W_SYSTEM_EXPORT w_bool_t
            w_cassandra_result_has_more_pages(_In_ const w_cass_result* pResult);

        /**
         * Gets the raw paging state from the result. The paging state is bound to the
         * lifetime of the result object. If paging state needs to live beyond the
         * lifetime of the result object it must be copied.
         *
         * <b>Warning:</b> The paging state should not be exposed to or come from
         * untrusted environments. The paging state could be spoofed and potentially
         * used to gain access to other data.
         *
         * @cassandra{2.0+}
         *
         * @public @memberof w_Cass_Result
         *
         * @param[in] result
         * @param[out] paging_state
         * @param[out] paging_state_size
         * @return W_SUCCESS if successful, otherwise error occurred
         *
         * @see w_cassandra_statement_set_paging_state_token()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_result_paging_state_token(_In_ const w_cass_result* pResult, 
                _Inout_ const char** pPagingState,
                _Inout_ size_t* pPagingStateSize);

        /***********************************************************************************
         *
         * Error result
         *
         ***********************************************************************************/

         /**
          * Frees an error result instance.
          *
          * @public @memberof w_Cass_Error_Result
          *
          * @param[in] error_result
          */
        W_SYSTEM_EXPORT void
            w_cassandra_error_result_free(_In_ const w_cass_error_result* pErrorResult);

        /**
         * Gets error code for the error result. This error code will always
         * have an server error source.
         *
         * @public @memberof w_Cass_Error_Result
         *
         * @param[in] error_result
         * @return The server error code
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_error_result_code(_In_ const w_cass_error_result* pErrorResult);

        /**
         * Gets consistency that triggered the error result of the
         * following types:
         *
         * <ul>
         *   <li>CASS_ERROR_SERVER_READ_TIMEOUT</li>
         *   <li>CASS_ERROR_SERVER_WRITE_TIMEOUT</li>
         *   <li>CASS_ERROR_SERVER_READ_FAILURE</li>
         *   <li>CASS_ERROR_SERVER_WRITE_FAILURE</li>
         *   <li>CASS_ERROR_SERVER_UNAVAILABLE</li>
         * </ul>
         *
         * @public @memberof w_Cass_Error_Result
         *
         * @param[in] error_result
         * @return The consistency that triggered the error for a read timeout,
         * write timeout or an unavailable error result. Undefined for other
         * error result types.
         */
        W_SYSTEM_EXPORT w_cass_consistency
            w_cassandra_error_result_consistency(_In_ const w_cass_error_result* pErrorResult);

        /**
         * Gets the actual number of received responses, received acknowledgments
         * or alive nodes for following error result types, respectively:
         *
         * <ul>
         *   <li>CASS_ERROR_SERVER_READ_TIMEOUT</li>
         *   <li>CASS_ERROR_SERVER_WRITE_TIMEOUT</li>
         *   <li>CASS_ERROR_SERVER_READ_FAILURE</li>
         *   <li>CASS_ERROR_SERVER_WRITE_FAILURE</li>
         *   <li>CASS_ERROR_SERVER_UNAVAILABLE</li>
         * </ul>
         *
         * @public @memberof w_Cass_Error_Result
         *
         * @param[in] error_result
         * @return The actual received responses for a read timeout, actual received
         * acknowledgments for a write timeout or actual alive nodes for a unavailable
         * error. Undefined for other error result types.
         */
        W_SYSTEM_EXPORT int32_t
            w_cass_error_result_responses_received(_In_ const w_cass_error_result* pErrorResult);

        /**
         * Gets required responses, required acknowledgments or required alive nodes
         * needed to successfully complete the request for following error result types,
         * respectively:
         *
         * <ul>
         *   <li>CASS_ERROR_SERVER_READ_TIMEOUT</li>
         *   <li>CASS_ERROR_SERVER_WRITE_TIMEOUT</li>
         *   <li>CASS_ERROR_SERVER_READ_FAILURE</li>
         *   <li>CASS_ERROR_SERVER_WRITE_FAILURE</li>
         *   <li>CASS_ERROR_SERVER_UNAVAILABLE</li>
         * </ul>
         *
         * @public @memberof w_Cass_Error_Result
         *
         * @param[in] error_result
         * @return The required responses for a read time, required acknowledgments
         * for a write timeout or required alive nodes for an unavailable error result.
         * Undefined for other error result types.
         */
        W_SYSTEM_EXPORT int32_t
            w_cassandra_error_result_responses_required(_In_ const w_cass_error_result* pErrorResult);

        /**
         * Gets the number of nodes that experienced failures for the following error types:
         *
         * <ul>
         *   <li>CASS_ERROR_SERVER_READ_FAILURE</li>
         *   <li>CASS_ERROR_SERVER_WRITE_FAILURE</li>
         * </ul>
         *
         * @public @memberof w_Cass_Error_Result
         *
         * @param[in] error_result
         * @return The number of nodes that failed during a read or write request.
         */
        W_SYSTEM_EXPORT int32_t
            w_cassandra_error_result_num_failures(_In_ const w_cass_error_result* pErrorResult);

        /**
         * Determines whether the actual data was present in the responses from the
         * replicas for the following error result types:
         *
         * <ul>
         *   <li>CASS_ERROR_SERVER_READ_TIMEOUT</li>
         *   <li>CASS_ERROR_SERVER_READ_FAILURE</li>
         * </ul>
         *
         * @public @memberof w_Cass_Error_Result
         *
         * @param[in] error_result
         * @return cass_true if the data was present in the received responses when the
         * read timeout occurred. Undefined for other error result types.
         */
        W_SYSTEM_EXPORT w_bool_t
            w_cassandra_error_result_data_present(_In_ const w_cass_error_result* pErrorResult);

        /**
         * Gets the write type of a request for the following error result types:
         *
         * <ul>
         *   <li>CASS_ERROR_SERVER_WRITE_TIMEOUT</li>
         *   <li>CASS_ERROR_SERVER_WRITE_FAILURE</li>
         * </ul>
         *
         * @public @memberof w_Cass_Error_Result
         *
         * @param[in] error_result
         * @return The type of the write that timed out. Undefined for
         * other error result types.
         */
        W_SYSTEM_EXPORT w_cass_writetype
            w_cassandra_error_result_write_type(_In_ const w_cass_error_result* pErrorResult);

        /**
         * Gets the affected keyspace for the following error result types:
         *
         * <ul>
         *   <li>CASS_ERROR_SERVER_ALREADY_EXISTS</li>
         *   <li>CASS_ERROR_SERVER_FUNCTION_FAILURE</li>
         * </ul>
         *
         * @public @memberof w_Cass_Error_Result
         *
         * @param[in] error_result
         * @param[out] keyspace
         * @param[out] keyspace_length
         * @return W_SUCCESS if successful, otherwise error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_error_result_keyspace(_In_ const w_cass_error_result* pErrorResult, 
                _Inout_ const char** pKeyspace,
                _Inout_ size_t* pKeyspaceLength);

        /**
         * Gets the affected table for the already exists error
         * (CASS_ERROR_SERVER_ALREADY_EXISTS) result type.
         *
         * @public @memberof w_Cass_Error_Result
         *
         * @param[in] error_result
         * @param[out] table
         * @param[out] table_length
         * @return W_SUCCESS if successful, otherwise error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_error_result_table(_In_ const w_cass_error_result* pErrorResult, 
                _Inout_ const char** pTable,
                _Inout_ size_t* pTableLength);

        /**
         * Gets the affected function for the function failure error
         * (CASS_ERROR_SERVER_FUNCTION_FAILURE) result type.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Error_Result
         *
         * @param[in] error_result
         * @param[out] function
         * @param[out] function_length
         * @return W_SUCCESS if successful, otherwise error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_error_result_function(_In_ const w_cass_error_result* pErrorResult,
                _Inout_  const char** pFunction,
                _Inout_ size_t* pFunctionLength);

        /**
         * Gets the number of argument types for the function failure error
         * (CASS_ERROR_SERVER_FUNCTION_FAILURE) result type.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Error_Result
         *
         * @param[in] error_result
         * @return The number of arguments for the affected function.
         */
        W_SYSTEM_EXPORT size_t
            w_cassandra_error_num_arg_types(_In_ const w_cass_error_result* pErrorResult);

        /**
         * Gets the argument type at the specified index for the function failure
         * error (CASS_ERROR_SERVER_FUNCTION_FAILURE) result type.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Error_Result
         *
         * @param[in] error_result
         * @param[in] index
         * @param[out] arg_type
         * @param[out] arg_type_length
         * @return W_SUCCESS if successful, otherwise error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_error_result_arg_type(_In_ const w_cass_error_result* pErrorResult,
                _In_  size_t pIndex, 
                _Inout_ const char** pArgType, 
                _Inout_ size_t* pArgTypeLength);

        /***********************************************************************************
         *
         * Iterator
         *
         ***********************************************************************************/

         /**
          * Frees an iterator instance.
          *
          * @public @memberof w_Cass_Iterator
          *
          * @param[in] iterator
          */
        W_SYSTEM_EXPORT void
            w_cassandra_iterator_free(_In_ w_cass_iterator* pIterator);

        /**
         * Gets the type of the specified iterator.
         *
         * @public @memberof w_Cass_Iterator
         *
         * @param[in] iterator
         * @return The type of the iterator.
         */
        W_SYSTEM_EXPORT w_cass_iterator_type 
            w_cassandra_iterator_type(_In_ w_cass_iterator* pIterator);

        /**
         * Creates a new iterator for the specified result. This can be
         * used to iterate over rows in the result.
         *
         * @public @memberof w_Cass_Result
         *
         * @param[in] result
         * @return A new iterator that must be freed.
         *
         * @see w_cassandra_iterator_free()
         */
        W_SYSTEM_EXPORT w_cass_iterator* 
            w_cassandra_iterator_from_result(_In_ const w_cass_result* pResult);

        /**
         * Creates a new iterator for the specified row. This can be
         * used to iterate over columns in a row.
         *
         * @public @memberof w_Cass_Row
         *
         * @param[in] row
         * @return A new iterator that must be freed.
         *
         * @see w_cassandra_iterator_free()
         */
        W_SYSTEM_EXPORT w_cass_iterator* 
            w_cassandra_iterator_from_row(_In_ const w_cass_row* pRow);

        /**
         * Creates a new iterator for the specified collection. This can be
         * used to iterate over values in a collection.
         *
         * @public @memberof w_Cass_Value
         *
         * @param[in] value
         * @return A new iterator that must be freed. NULL returned if the
         * value is not a collection.
         *
         * @see w_cassandra_iterator_free()
         */
        W_SYSTEM_EXPORT  w_cass_iterator* 
            w_cassandra_iterator_from_collection(_In_ const w_cass_value* pValue);

        /**
         * Creates a new iterator for the specified map. This can be
         * used to iterate over key/value pairs in a map.
         *
         * @public @memberof w_Cass_Value
         *
         * @param[in] value
         * @return A new iterator that must be freed. NULL returned if the
         * value is not a map.
         *
         * @see w_cassandra_iterator_free()
         */
        W_SYSTEM_EXPORT w_cass_iterator* 
            w_cassandra_iterator_from_map(_In_ const w_cass_value* pValue);

        /**
         * Creates a new iterator for the specified tuple. This can be
         * used to iterate over values in a tuple.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Value
         *
         * @param[in] value
         * @return A new iterator that must be freed. NULL returned if the
         * value is not a tuple.
         *
         * @see w_cassandra_iterator_free()
         */
        W_SYSTEM_EXPORT w_cass_iterator*
            w_cassandra_iterator_from_tuple(_In_ const w_cass_value* pValue);

        /**
         * Creates a new iterator for the specified user defined type. This can be
         * used to iterate over fields in a user defined type.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Value
         *
         * @param[in] value
         * @return A new iterator that must be freed. NULL returned if the
         * value is not a user defined type.
         *
         * @see w_cassandra_iterator_free()
         */
        W_SYSTEM_EXPORT  w_cass_iterator* 
            w_cassandra_iterator_fields_from_user_type(_In_ const w_cass_value* pValue);

        /**
         * Creates a new iterator for the specified schema metadata.
         * This can be used to iterate over keyspace.
         *
         * @public @memberof w_Cass_Schema_Meta
         *
         * @param[in] schema_meta
         * @return A new iterator that must be freed.
         *
         * @see w_cassandra_iterator_get_keyspace_meta()
         * @see w_cassandra_iterator_free()
         */
        W_SYSTEM_EXPORT w_cass_iterator* 
            w_cassandra_iterator_keyspaces_from_schema_meta(_In_ const w_cass_schema_meta* pSchemaMeta);

        /**
         * Creates a new iterator for the specified keyspace metadata.
         * This can be used to iterate over tables.
         *
         * @public @memberof w_Cass_Key_space_Meta
         *
         * @param[in] keyspace_meta
         * @return A new iterator that must be freed.
         *
         * @see w_cassandra_iterator_get_table_meta()
         * @see w_cassandra_iterator_free()
         */
        W_SYSTEM_EXPORT  w_cass_iterator*
            w_cassandra_iterator_tables_from_keyspace_meta(_In_ const w_cass_key_space_meta* pKeyspaceMeta);

        /**
         * Creates a new iterator for the specified keyspace metadata.
         * This can be used to iterate over views.
         *
         * @cassandra{3.0+}
         *
         * @public @memberof w_Cass_Key_space_Meta
         *
         * @param[in] keyspace_meta
         * @return A new iterator that must be freed.
         *
         * @see w_cassandra_iterator_get_materialized_view_meta()
         * @see w_cassandra_iterator_free()
         */
        W_SYSTEM_EXPORT w_cass_iterator* 
            w_cassandra_iterator_materialized_views_from_keyspace_meta(_In_ const w_cass_key_space_meta* pKeyspaceMeta);

        /**
         * Creates a new iterator for the specified keyspace metadata.
         * This can be used to iterate over types.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Key_space_Meta
         *
         * @param[in] keyspace_meta
         * @return A new iterator that must be freed.
         *
         * @see w_cassandra_iterator_get_user_type()
         * @see w_cassandra_iterator_free()
         */
        W_SYSTEM_EXPORT w_cass_iterator* 
            w_cassandra_iterator_user_types_from_keyspace_meta(_In_ const w_cass_key_space_meta* pKeyspaceMeta);

        /**
         * Creates a new iterator for the specified keyspace metadata.
         * This can be used to iterate over functions.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Key_space_Meta
         *
         * @param[in] keyspace_meta
         * @return A new iterator that must be freed.
         *
         * @see w_cassandra_iterator_get_function_meta()
         * @see w_cassandra_iterator_free()
         */
        W_SYSTEM_EXPORT w_cass_iterator* 
            w_cassandra_iterator_functions_from_keyspace_meta(_In_ const w_cass_key_space_meta* pKeyspaceMeta);

        /**
         * Creates a new iterator for the specified keyspace metadata.
         * This can be used to iterate over aggregates.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Key_space_Meta
         *
         * @param[in] keyspace_meta
         * @return A new iterator that must be freed.
         *
         * @see w_cassandra_iterator_get_aggregate_meta()
         * @see w_cassandra_iterator_free()
         */
        W_SYSTEM_EXPORT w_cass_iterator* 
            w_cassandra_iterator_aggregates_from_keyspace_meta(_In_ const w_cass_key_space_meta* pKeyspaceMeta);

        /**
         * Creates a new fields iterator for the specified keyspace metadata. Metadata
         * fields allow direct access to the column data found in the underlying
         * "keyspaces" metadata table. This can be used to iterate those metadata
         * field entries.
         *
         * @public @memberof w_Cass_Key_space_Meta
         *
         * @param[in] keyspace_meta
         * @return A new iterator that must be freed.
         *
         * @see w_cassandra_iterator_get_meta_field_name()
         * @see w_cassandra_iterator_get_meta_field_value()
         * @see w_cassandra_iterator_free()
         */
        W_SYSTEM_EXPORT w_cass_iterator* 
            w_cassandra_iterator_fields_from_keyspace_meta(_In_ const w_cass_key_space_meta* pKeyspaceMeta);

        /**
         * Creates a new iterator for the specified table metadata.
         * This can be used to iterate over columns.
         *
         * @public @memberof w_Cass_Table_Meta
         *
         * @param[in] table_meta
         * @return A new iterator that must be freed.
         *
         * @see w_cassandra_iterator_get_column_meta()
         * @see w_cassandra_iterator_free()
         */
        W_SYSTEM_EXPORT w_cass_iterator* 
            w_cassandra_iterator_columns_from_table_meta(_In_ const w_cass_table_meta* pTableMeta);

        /**
         * Creates a new iterator for the specified table metadata.
         * This can be used to iterate over indexes.
         *
         * @public @memberof w_Cass_Table_Meta
         *
         * @param[in] table_meta
         * @return A new iterator that must be freed.
         *
         * @see w_cassandra_iterator_get_index_meta()
         * @see w_cassandra_iterator_free()
         */
        W_SYSTEM_EXPORT w_cass_iterator* 
            w_cassandra_iterator_indexes_from_table_meta(_In_ const w_cass_table_meta* pTableMeta);

        /**
         * Creates a new iterator for the specified materialized view metadata.
         * This can be used to iterate over columns.
         *
         * @cassandra{3.0+}
         *
         * @public @memberof w_Cass_Table_Meta
         *
         * @param[in] table_meta
         * @return A new iterator that must be freed.
         *
         * @see w_cassandra_iterator_get_materialized_view_meta()
         * @see w_cassandra_iterator_free()
         */
        W_SYSTEM_EXPORT w_cass_iterator*
            w_cassandra_iterator_materialized_views_from_table_meta(_In_ const w_cass_table_meta* pTableMeta);

        /**
         * Creates a new fields iterator for the specified table metadata. Metadata
         * fields allow direct access to the column data found in the underlying
         * "tables" metadata table. This can be used to iterate those metadata
         * field entries.
         *
         * @public @memberof w_Cass_Table_Meta
         *
         * @param[in] table_meta
         * @return A new iterator that must be freed.
         *
         * @see w_cassandra_iterator_get_meta_field_name()
         * @see w_cassandra_iterator_get_meta_field_value()
         * @see w_cassandra_iterator_free()
         */
        W_SYSTEM_EXPORT w_cass_iterator*
            w_cassandra_iterator_fields_from_table_meta(_In_ const w_cass_table_meta* pTableMeta);

        /**
         * Creates a new iterator for the specified materialized view metadata.
         * This can be used to iterate over columns.
         *
         * @cassandra{3.0+}
         *
         * @public @memberof w_Cass_Materialized_View_Meta
         *
         * @param[in] view_meta
         * @return A new iterator that must be freed.
         *
         * @see w_cassandra_iterator_get_column_meta()
         * @see w_cassandra_iterator_free()
         */
        W_SYSTEM_EXPORT w_cass_iterator*
            w_cassandra_iterator_columns_from_materialized_view_meta(_In_ const w_cass_materialized_view_meta* pViewMe);

        /**
         * Creates a new fields iterator for the specified materialized view metadata.
         * Metadata fields allow direct access to the column data found in the
         * underlying "views" metadata view. This can be used to iterate those metadata
         * field entries.
         *
         * @cassandra{3.0+}
         *
         * @public @memberof w_Cass_Materialized_View_Meta
         *
         * @param[in] view_meta
         * @return A new iterator that must be freed.
         *
         * @see w_cassandra_iterator_get_meta_field_name()
         * @see w_cassandra_iterator_get_meta_field_value()
         * @see w_cassandra_iterator_free()
         */
        W_SYSTEM_EXPORT w_cass_iterator*
            w_cassandra_iterator_fields_from_materialized_view_meta(_In_ const w_cass_materialized_view_meta* pViewMe);

        /**
         * Creates a new fields iterator for the specified column metadata. Metadata
         * fields allow direct access to the column data found in the underlying
         * "columns" metadata table. This can be used to iterate those metadata
         * field entries.
         *
         * @public @memberof w_Cass_Column_Meta
         *
         * @param[in] column_meta
         * @return A new iterator that must be freed.
         *
         * @see w_cassandra_iterator_get_meta_field_name()
         * @see w_cassandra_iterator_get_meta_field_value()
         * @see w_cassandra_iterator_free()
         */
        W_SYSTEM_EXPORT w_cass_iterator*
            w_cassandra_iterator_fields_from_column_meta(_In_ const w_cass_column_meta* pColumnMeta);

        /**
         * Creates a new fields iterator for the specified index metadata. Metadata
         * fields allow direct access to the index data found in the underlying
         * "indexes" metadata table. This can be used to iterate those metadata
         * field entries.
         *
         * @public @memberof w_Cass_Index_Meta
         *
         * @param[in] index_meta
         * @return A new iterator that must be freed.
         *
         * @see w_cassandra_iterator_get_meta_field_name()
         * @see w_cassandra_iterator_get_meta_field_value()
         * @see w_cassandra_iterator_free()
         */
        W_SYSTEM_EXPORT w_cass_iterator* 
            w_cassandra_iterator_fields_from_index_meta(_In_ const w_cass_index_meta* pIndexMeta);

        /**
         * Creates a new fields iterator for the specified function metadata. Metadata
         * fields allow direct access to the column data found in the underlying
         * "functions" metadata table. This can be used to iterate those metadata
         * field entries.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Function_Meta
         *
         * @param[in] function_meta
         * @return A new iterator that must be freed.
         *
         * @see w_cassandra_iterator_get_meta_field()
         * @see w_cassandra_iterator_free()
         */
        W_SYSTEM_EXPORT w_cass_iterator* 
            w_cassandra_iterator_fields_from_function_meta(_In_ const w_cass_function_meta* pFunctionMeta);

        /**
         * Creates a new fields iterator for the specified aggregate metadata. Metadata
         * fields allow direct access to the column data found in the underlying
         * "aggregates" metadata table. This can be used to iterate those metadata
         * field entries.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Aggregate_Meta
         *
         * @param[in] aggregate_meta
         * @return A new iterator that must be freed.
         *
         * @see w_cassandra_iterator_get_meta_field()
         * @see w_cassandra_iterator_free()
         */
        W_SYSTEM_EXPORT w_cass_iterator* 
            w_cassandra_iterator_fields_from_aggregate_meta(_In_ const w_cass_aggregate_meta* pAggregateMeta);

        /**
         * Advance the iterator to the next row, column or collection item.
         *
         * @public @memberof w_Cass_Iterator
         *
         * @param[in] iterator
         * @return false if no more rows, columns or items, otherwise true
         */
        W_SYSTEM_EXPORT w_bool_t
            w_cassandra_iterator_next(_In_ w_cass_iterator* pIterator);

        /**
         * Gets the row at the result iterator's current position.
         *
         * Calling w_cassandra_iterator_next() will invalidate the previous
         * row returned by this method.
         *
         * @public @memberof w_Cass_Iterator
         *
         * @param[in] iterator
         * @return A row
         */
        W_SYSTEM_EXPORT const w_cass_row* 
            w_cassandra_iterator_get_row(_In_ const w_cass_iterator* pIterator);

        /**
         * Gets the column value at the row iterator's current position.
         *
         * Calling w_cassandra_iterator_next() will invalidate the previous
         * column returned by this method.
         *
         * @public @memberof w_Cass_Iterator
         *
         * @param[in] iterator
         * @return A value
         */
        W_SYSTEM_EXPORT const w_cass_value* 
            w_cassandra_iterator_get_column(_In_ const w_cass_iterator* pIterator);

        /**
         * Gets the value at a collection or tuple iterator's current position.
         *
         * Calling w_cassandra_iterator_next() will invalidate the previous
         * value returned by this method.
         *
         * @public @memberof w_Cass_Iterator
         *
         * @param[in] iterator
         * @return A value
         */
        W_SYSTEM_EXPORT const w_cass_value*
            w_cassandra_iterator_get_value(_In_ const w_cass_iterator* pIterator);

        /**
         * Gets the key at the map iterator's current position.
         *
         * Calling w_cassandra_iterator_next() will invalidate the previous
         * value returned by this method.
         *
         * @public @memberof w_Cass_Iterator
         *
         * @param[in] iterator
         * @return A value
         */
        W_SYSTEM_EXPORT const w_cass_value* 
            w_cassandra_iterator_get_map_key(_In_ const w_cass_iterator* pIterator);


        /**
         * Gets the value at the map iterator's current position.
         *
         * Calling w_cassandra_iterator_next() will invalidate the previous
         * value returned by this method.
         *
         * @public @memberof w_Cass_Iterator
         *
         * @param[in] iterator
         * @return A value
         */
        W_SYSTEM_EXPORT const w_cass_value* 
            w_cassandra_iterator_get_map_value(_In_ const w_cass_iterator* pIterator);

        /**
         * Gets the field name at the user type defined iterator's current position.
         *
         * Calling w_cassandra_iterator_next() will invalidate the previous
         * name returned by this method.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Iterator
         *
         * @param[in] iterator
         * @param[out] name
         * @param[out] name_length
         * @return W_SUCCESS if successful, otherwise error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_iterator_get_user_type_field_name(_In_ const w_cass_iterator* pIterator,
                _Inout_ const char** pName,
                _Inout_ size_t* pNameLength);

        /**
         * Gets the field value at the user type defined iterator's current position.
         *
         * Calling w_cassandra_iterator_next() will invalidate the previous
         * value returned by this method.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Iterator
         *
         * @param[in] iterator
         * @return A value
         */
        W_SYSTEM_EXPORT const w_cass_value* 
            w_cassandra_iterator_get_user_type_field_value(_In_ const w_cass_iterator* pIterator);

        /**
         * Gets the keyspace metadata entry at the iterator's current position.
         *
         * Calling w_cassandra_iterator_next() will invalidate the previous
         * value returned by this method.
         *
         * @public @memberof w_Cass_Iterator
         *
         * @param[in] iterator
         * @return A keyspace metadata entry
         */
        W_SYSTEM_EXPORT const w_cass_key_space_meta*
            w_cassandra_iterator_get_keyspace_meta(_In_ const w_cass_iterator* pIterator);

        /**
         * Gets the table metadata entry at the iterator's current position.
         *
         * Calling w_cassandra_iterator_next() will invalidate the previous
         * value returned by this method.
         *
         * @public @memberof w_Cass_Iterator
         *
         * @param[in] iterator
         * @return A table metadata entry
         */
        W_SYSTEM_EXPORT const w_cass_table_meta* 
            w_cassandra_iterator_get_table_meta(_In_ const w_cass_iterator* pIterator);

        /**
         * Gets the materialized view metadata entry at the iterator's current position.
         *
         * Calling w_cassandra_iterator_next() will invalidate the previous
         * value returned by this method.
         *
         * @cassandra{3.0+}
         *
         * @public @memberof w_Cass_Iterator
         *
         * @param[in] iterator
         * @return A materialized view metadata entry
         */
        W_SYSTEM_EXPORT const w_cass_materialized_view_meta* 
            w_cassandra_iterator_get_materialized_view_meta(_In_ const w_cass_iterator* pIterator);

        /**
         * Gets the type metadata entry at the iterator's current position.
         *
         * Calling w_cassandra_iterator_next() will invalidate the previous
         * value returned by this method.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Iterator
         *
         * @param[in] iterator
         * @return A type metadata entry
         */
        W_SYSTEM_EXPORT const w_cass_data_type* 
            w_cassandra_iterator_get_user_type(_In_ const w_cass_iterator* pIterator);

        /**
         * Gets the function metadata entry at the iterator's current position.
         *
         * Calling w_cassandra_iterator_next() will invalidate the previous
         * value returned by this method.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Iterator
         *
         * @param[in] iterator
         * @return A function metadata entry
         */
        W_SYSTEM_EXPORT const w_cass_function_meta* 
            w_cassandra_iterator_get_function_meta(_In_ const w_cass_iterator* pIterator);

        /**
         * Gets the aggregate metadata entry at the iterator's current position.
         *
         * Calling w_cassandra_iterator_next() will invalidate the previous
         * value returned by this method.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Iterator
         *
         * @param[in] iterator
         * @return A aggregate metadata entry
         */
        W_SYSTEM_EXPORT const w_cass_aggregate_meta* 
            w_cassandra_iterator_get_aggregate_meta(_In_ const w_cass_iterator* pIterator);

        /**
         * Gets the column metadata entry at the iterator's current position.
         *
         * Calling w_cassandra_iterator_next() will invalidate the previous
         * value returned by this method.
         *
         * @public @memberof w_Cass_Iterator
         *
         * @param[in] iterator
         * @return A column metadata entry
         */
        W_SYSTEM_EXPORT const w_cass_column_meta* 
            w_cassandra_iterator_get_column_meta(_In_ const w_cass_iterator* pIterator);

        /**
         * Gets the index metadata entry at the iterator's current position.
         *
         * Calling w_cassandra_iterator_next() will invalidate the previous
         * value returned by this method.
         *
         * @public @memberof w_Cass_Iterator
         *
         * @param[in] iterator
         * @return A index metadata entry
         */
        W_SYSTEM_EXPORT const w_cass_index_meta* 
            w_cassandra_iterator_get_index_meta(_In_ const w_cass_iterator* pIterator);

        /**
         * Gets the metadata field name at the iterator's current position.
         *
         * Calling w_cassandra_iterator_next() will invalidate the previous
         * value returned by this method.
         *
         * @public @memberof w_Cass_Iterator
         *
         * @param[in] iterator
         * @param[out] name
         * @param[out] name_length
         * @return W_SUCCESS if successful, otherwise error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_iterator_get_meta_field_name(_In_ const w_cass_iterator* pIterator,
                _Inout_ const char** pName,
                _Inout_ size_t* pNameLength);

        /**
         * Gets the metadata field value at the iterator's current position.
         *
         * Calling w_cassandra_iterator_next() will invalidate the previous
         * value returned by this method.
         *
         * @public @memberof w_Cass_Iterator
         *
         * @param[in] iterator
         * @return A metadata field value
         */
        W_SYSTEM_EXPORT const w_cass_value* 
            w_cassandra_iterator_get_meta_field_value(_In_ const w_cass_iterator* pIterator);

        /***********************************************************************************
         *
         * Row
         *
         ***********************************************************************************/

         /**
          * Get the column value at index for the specified row.
          *
          * @public @memberof w_Cass_Row
          *
          * @param[in] row
          * @param[in] index
          * @return The column value at the specified index. NULL is
          * returned if the index is out of bounds.
          */
        W_SYSTEM_EXPORT const w_cass_value*
            w_cassandra_row_get_column(_In_ const w_cass_row* pRow,
                _In_ size_t pIndex);


        /**
         * Get the column value by name for the specified row.
         *
         * @public @memberof w_Cass_Row
         *
         * @param[in] row
         * @param[in] name
         * @return The column value for the specified name. NULL is
         * returned if the column does not exist.
         */
        W_SYSTEM_EXPORT const w_cass_value* 
            w_cassandra_row_get_column_by_name(_In_ const w_cass_row* pRow, 
                _In_ const char* pName);

        /**
         * Same as w_cassandra_row_get_column_by_name(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_Cass_Row
         *
         * @param[in] row
         * @param[in] name
         * @param[in] name_length
         * @return same as w_cassandra_row_get_column_by_name()
         *
         * @see w_cassandra_row_get_column_by_name()
         */
        W_SYSTEM_EXPORT const w_cass_value* 
            w_cassandra_row_get_column_by_name_n(_In_ const w_cass_row* pRow, 
            _In_  const char* pName,
                _In_ size_t pNameLength);

        /***********************************************************************************
         *
         * Value
         *
         ***********************************************************************************/

         /**
          * Gets the data type of a value.
          *
          * @public @memberof w_Cass_Value
          *
          * @param[in] value
          * @return Returns a reference to the data type of the value.
          * Do not free this reference as it is bound to the lifetime of the value.
          */
        W_SYSTEM_EXPORT const w_cass_data_type* 
            w_cassandra_value_data_type(_In_ const w_cass_value* pValue);

        /**
         * Gets an int8 for the specified value.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Value
         *
         * @param[in] value
         * @param[out] output
         * @return W_SUCCESS if successful, otherwise error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_value_get_int8(_In_ const w_cass_value* pValue,
                _Inout_ int8_t* pOutput);

        /**
         * Gets an int16 for the specified value.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Value
         *
         * @param[in] value
         * @param[out] output
         * @return W_SUCCESS if successful, otherwise error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_value_get_int16(_In_ const w_cass_value* pValue,
                _Inout_ int16_t* pOutput);

        /**
         * Gets an int32 for the specified value.
         *
         * @public @memberof w_Cass_Value
         *
         * @param[in] value
         * @param[out] output
         * @return W_SUCCESS if successful, otherwise error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_value_get_int32(_In_ const w_cass_value* pValue,
                _Inout_ int32_t* pOutput);

        /**
         * Gets an uint32 for the specified value.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Value
         *
         * @param[in] value
         * @param[out] output
         * @return W_SUCCESS if successful, otherwise error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_value_get_uint32(_In_ const w_cass_value* pValue, 
                _Inout_  uint32_t* pOutput);

        /**
         * Gets an int64 for the specified value.
         *
         * @public @memberof w_Cass_Value
         *
         * @param[in] value
         * @param[out] output
         * @return W_SUCCESS if successful, otherwise error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_value_get_int64(_In_ const w_cass_value* pValue,
                _Inout_ int64_t* pOutput);

        /**
         * Gets a float for the specified value.
         *
         * @public @memberof w_Cass_Value
         *
         * @param[in] value
         * @param[out] output
         * @return W_SUCCESS if successful, otherwise error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_value_get_float(_In_ const w_cass_value* pValue, 
                _Inout_  float* pOutput);

        /**
         * Gets a double for the specified value.
         *
         * @public @memberof w_Cass_Value
         *
         * @param[in] value
         * @param[out] output
         * @return W_SUCCESS if successful, otherwise error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_value_get_double(_In_ const w_cass_value* pValue,
                _Inout_ double* pOutput);

        /**
         * Gets a bool for the specified value.
         *
         * @public @memberof w_Cass_Value
         *
         * @param[in] value
         * @param[out] output
         * @return W_SUCCESS if successful, otherwise error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_value_get_bool(_In_ const w_cass_value* pValue,
                _Inout_ w_bool_t* pOutput);

        /**
         * Gets a UUID for the specified value.
         *
         * @public @memberof w_Cass_Value
         *
         * @param[in] value
         * @param[out] output
         * @return W_SUCCESS if successful, otherwise error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_value_get_uuid(_In_ const w_cass_value* pValue, 
                _Inout_  w_cass_uuid* pOutput);

        /**
         * Gets an INET for the specified value.
         *
         * @public @memberof w_Cass_Value
         *
         * @param[in] value
         * @param[out] output
         * @return W_SUCCESS if successful, otherwise error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_value_get_inet(_In_ const w_cass_value* pValue, 
                _Inout_ w_cass_inet* pOutput);

        /**
         * Gets a string for the specified value.
         *
         * @public @memberof w_Cass_Value
         *
         * @param[in] value
         * @param[out] output
         * @param[out] output_size
         * @return W_SUCCESS if successful, otherwise error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_value_get_string(_In_ const w_cass_value* pValue, 
                _Inout_ const char** pOutput,
                _Inout_ size_t* pOutputSize);

        /**
         * Gets the bytes of the specified value.
         *
         * @public @memberof w_Cass_Value
         *
         * @param[in] value
         * @param[out] output
         * @param[out] output_size
         * @return W_SUCCESS if successful, otherwise error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_value_get_bytes(_In_ const w_cass_value* pValue, 
                _Inout_ const w_byte_t** pOutput, 
                _Inout_ size_t* pOutputSize);

        /**
         * Gets a decimal for the specified value.
         *
         * @public @memberof w_Cass_Value
         *
         * @param[in] value
         * @param[out] varint
         * @param[out] varint_size
         * @param[out] scale
         * @return W_SUCCESS if successful, otherwise error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_value_get_decimal(_In_ const w_cass_value* pValue,
                _Inout_ const w_byte_t** pVarint,
                _Inout_ size_t* pVarintSize, 
                _Inout_ int32_t* pScale);

        /**
         * Gets a duration for the specified value.
         *
         * @cassandra{3.10+}
         *
         * @public @memberof w_Cass_Value
         *
         * @param[in] value
         * @param[out] months
         * @param[out] days
         * @param[out] nanos
         * @return W_SUCCESS if successful, otherwise error occurred
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_value_get_duration(_In_ const w_cass_value* pValue, 
                _Inout_ int32_t* pMonths,
                _Inout_ int32_t* pDays,
                _Inout_ int64_t* pNanos);

        /**
         * Gets the type of the specified value.
         *
         * @public @memberof w_Cass_Value
         *
         * @param[in] value
         * @return The type of the specified value.
         */
        W_SYSTEM_EXPORT w_cass_value_type 
            w_cassandra_value_type(_In_ const w_cass_value* pValue);

        /**
         * Returns true if a specified value is null.
         *
         * @public @memberof w_Cass_Value
         *
         * @param[in] value
         * @return true if the value is null, otherwise false.
         */
        W_SYSTEM_EXPORT w_bool_t
            w_cassandra_value_is_null(_In_ const w_cass_value* pValue);

        /**
         * Returns true if a specified value is a collection.
         *
         * @public @memberof w_Cass_Value
         *
         * @param[in] value
         * @return true if the value is a collection, otherwise false.
         */
        W_SYSTEM_EXPORT w_bool_t
            w_cassandra_value_is_collection(_In_ const w_cass_value* pValue);

        /**
         * Returns true if a specified value is a duration.
         *
         * @public @memberof w_Cass_Value
         *
         * @param[in] value
         * @return true if the value is a duration, otherwise false.
         */
        W_SYSTEM_EXPORT w_bool_t
            w_cassandra_value_is_duration(_In_ const w_cass_value* pValue);

        /**
         * Get the number of items in a collection. Works for all collection types.
         *
         * @public @memberof w_Cass_Value
         *
         * @param[in] collection
         * @return Count of items in a collection. 0 if not a collection.
         */
        W_SYSTEM_EXPORT size_t
            w_cassandra_value_item_count(_In_ const w_cass_value* pCollection);

        /**
         * Get the primary sub-type for a collection. This returns the sub-type for a
         * list or set and the key type for a map.
         *
         * @public @memberof w_Cass_Value
         *
         * @param[in] collection
         * @return The type of the primary sub-type. CASS_VALUE_TYPE_UNKNOWN
         * returned if not a collection.
         */
        W_SYSTEM_EXPORT  w_cass_value_type  
            w_cassandra_value_primary_sub_type(_In_ const w_cass_value* pCollection);

        /**
         * Get the secondary sub-type for a collection. This returns the value type for a
         * map.
         *
         * @public @memberof w_Cass_Value
         *
         * @param[in] collection
         * @return The type of the primary sub-type. CASS_VALUE_TYPE_UNKNOWN
         * returned if not a collection or not a map.
         */
        W_SYSTEM_EXPORT w_cass_value_type
            w_cassandra_value_secondary_sub_type(_In_ const  w_cass_value* pCollection);


        /***********************************************************************************
         *
         * UUID
         *
         ************************************************************************************/

         /**
          * Creates a new UUID generator.
          *
          * <b>Note:</b> This object is thread-safe. It is best practice to create and reuse
          * a single object per application.
          *
          * <b>Note:</b> If unique node information (IP address) is unable to be determined
          * then random node information will be generated.
          *
          * @public @memberof w_Cass_Uuid_Gen
          *
          * @return Returns a UUID generator that must be freed.
          *
          * @see w_cassandra_uuid_gen_free()
          * @see w_cassandra_uuid_gen_new_with_node()
          */
        W_SYSTEM_EXPORT w_cass_uuid_gen*
            w_cassandra_uuid_gen_new();

        /**
         * Creates a new UUID generator with custom node information.
         *
         * <b>Note:</b> This object is thread-safe. It is best practice to create and reuse
         * a single object per application.
         *
         * @public @memberof w_Cass_Uuid_Gen
         *
         * @return Returns a UUID generator that must be freed.
         *
         * @see w_cassandra_uuid_gen_free()
         */
        W_SYSTEM_EXPORT w_cass_uuid_gen* 
            w_cassandra_uuid_gen_new_with_node(_In_ uint64_t pNode);

        /**
         * Frees a UUID generator instance.
         *
         * @public @memberof w_Cass_Uuid_Gen
         *
         * @param[in] uuid_gen
         */
        W_SYSTEM_EXPORT void
            w_cassandra_uuid_gen_free(_In_ w_cass_uuid_gen* pUuiGen);

        /**
         * Generates a V1 (time) UUID.
         *
         * <b>Note:</b> This method is thread-safe
         *
         * @public @memberof w_Cass_Uuid_Gen
         *
         * @param[in] uuid_gen
         * @param[out] output A V1 UUID for the current time.
         */
        W_SYSTEM_EXPORT void
            w_cassandra_uuid_gen_time(_In_ w_cass_uuid_gen* pUuiGen, 
                _Inout_ w_cass_uuid* pOutput);

        /**
         * Generates a new V4 (random) UUID
         *
         * <b>Note:</b>: This method is thread-safe
         *
         * @public @memberof w_Cass_Uuid_Gen
         *
         * @param[in] uuid_gen
         * @param output A randomly generated V4 UUID.
         */
        W_SYSTEM_EXPORT void
            w_cassandra_uuid_gen_random(_In_ w_cass_uuid_gen* pUuiGen, 
                _Inout_  w_cass_uuid* pOutput);

        /**
         * Generates a V1 (time) UUID for the specified time.
         *
         * <b>Note:</b>: This method is thread-safe
         *
         * @public @memberof w_Cass_Uuid_Gen
         *
         * @param[in] uuid_gen
         * @param[in] timestamp
         * @param[out] output A V1 UUID for the specified time.
         */
        W_SYSTEM_EXPORT void
            w_cassandra_uuid_gen_from_time(_In_ w_cass_uuid_gen* pUuidGen,
                _In_ uint64_t pTimeStamp, 
                _Inout_ w_cass_uuid* pOutput);

        /**
         * Sets the UUID to the minimum V1 (time) value for the specified time.
         *
         * @public @memberof w_cass_uuid
         *
         * @param[in] time
         * @param[out] output A minimum V1 UUID for the specified time.
         */
        W_SYSTEM_EXPORT void
            w_cassandra_uuid_min_from_time(_In_ uint64_t pTime, 
                _Inout_ w_cass_uuid* pOutput);

        /**
         * Sets the UUID to the maximum V1 (time) value for the specified time.
         *
         * @public @memberof w_cass_uuid
         *
         * @param[in] time
         * @param[out] output A maximum V1 UUID for the specified time.
         */
        W_SYSTEM_EXPORT void
            w_cassandra_uuid_max_from_time(_In_ uint64_t pTime, 
                _Inout_ w_cass_uuid* pOutput);

        /**
         * Gets the timestamp for a V1 UUID
         *
         * @public @memberof w_cass_uuid
         *
         * @param[in] uuid
         * @return The timestamp in milliseconds since the Epoch
         * (00:00:00 UTC on 1 January 1970). 0 returned if the UUID
         * is not V1.
         */
        W_SYSTEM_EXPORT uint64_t
            w_cassandra_uuid_timestamp(_In_ w_cass_uuid pUuid)  ;

        /**
         * Gets the version for a UUID
         *
         * @public @memberof w_cass_uuid
         *
         * @param[in] uuid
         * @return The version of the UUID (1 or 4)
         */
        W_SYSTEM_EXPORT uint8_t
            w_cassandra_uuid_version(_In_ w_cass_uuid pUuid) ;

        /**
         * Returns a null-terminated string for the specified UUID.
         *
         * @public @memberof w_cass_uuid
         *
         * @param[in] uuid
         * @param[out] output A null-terminated string of length CASS_UUID_STRING_LENGTH.
         */
        W_SYSTEM_EXPORT void
            w_cassandra_uuid_string(_In_ w_cass_uuid pUuid, 
                _Inout_ char* pOutput);

        /**
         * Returns a UUID for the specified string.
         *
         * Example: "550e8400-e29b-41d4-a716-446655440000"
         *
         * @public @memberof w_cass_uuid
         *
         * @param[in] str
         * @param[out] output
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_uuid_from_string(_In_ const char* pStr,
                _Inout_ w_cass_uuid* pOutput);

        /**
         * Same as w_cassandra_uuid_from_string(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_cass_uuid
         *
         * @param[in] str
         * @param[in] str_length
         * @param[out] output
         * @return same as w_cassandra_uuid_from_string()
         *
         * @see w_cassandra_uuid_from_string()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_uuid_from_string_n(_In_ const char* pStr,
                _In_ size_t pStrLength, 
                _Inout_ w_cass_uuid* pOutput);

        /***********************************************************************************
         *
         * Timestamp generators
         *
         ***********************************************************************************/

         /**
          * Creates a new server-side timestamp generator. This generator allows Cassandra
          * to assign timestamps server-side.
          *
          * <b>Note:</b> This is the default timestamp generator.
          *
          * @cassandra{2.1+}
          *
          * @public @memberof w_Cass_Time_stamp_Gen
          *
          * @return Returns a timestamp generator that must be freed.
          *
          * @see w_cassandra_timestamp_gen_free()
          */
        W_SYSTEM_EXPORT w_cass_time_stamp_gen* 
            w_cassandra_timestamp_gen_server_side_new();

        /**
         * Creates a new monotonically increasing timestamp generator with microsecond
         * precision.
         *
         * This implementation guarantees a monotonically increasing timestamp. If the
         * timestamp generation rate exceeds one per microsecond or if the clock skews
         * into the past the generator will artificially increment the previously
         * generated timestamp until the request rate decreases or the clock skew
         * is corrected.
         *
         * By default, this timestamp generator will generate warnings if more than
         * 1 second of clock skew is detected. It will print an error every second until
         * the clock skew is resolved. These settings can be changed by using
         * `w_cassandra_timestamp_gen_monotonic_new_with_settings()` to create the generator
         * instance.
         *
         * <b>Note:</b> This generator is thread-safe and can be shared by multiple
         * sessions.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Time_stamp_Gen
         *
         * @return Returns a timestamp generator that must be freed.
         *
         * @see w_cassandra_timestamp_gen_monotonic_new_with_settings();
         * @see w_cassandra_timestamp_gen_free()
         */
        W_SYSTEM_EXPORT w_cass_time_stamp_gen* 
            w_cassandra_timestamp_gen_monotonic_new();

        /**
         * Same as w_cassandra_timestamp_gen_monotonic_new(), but with settings for controlling
         * warnings about clock skew.
         *
         * @param warning_threshold_us The amount of clock skew, in microseconds, that
         * must be detected before a warning is triggered. A threshold less than 0 can
         * be used to disable warnings.
         * @param warning_interval_ms The amount of time, in milliseconds, to wait before
         * warning again about clock skew. An interval value less than or equal to 0 allows
         * the warning to be triggered every millisecond.
         * @return Returns a timestamp generator that must be freed.
         */
        W_SYSTEM_EXPORT w_cass_time_stamp_gen*
            w_cassandra_timestamp_gen_monotonic_new_with_settings(_In_ int64_t pWarningThresholdUs,
                _In_ int64_t pWarningIntervalMs);

        /**
         * Frees a timestamp generator instance.
         *
         * @cassandra{2.1+}
         *
         * @public @memberof w_Cass_Time_stamp_Gen
         *
         * @param[in] timestamp_gen
         */
        W_SYSTEM_EXPORT void
            w_cassandra_timestamp_gen_free(_In_ w_cass_time_stamp_gen* pTimestampGen);


        /***********************************************************************************
         *
         * Retry policies
         *
         ***********************************************************************************/

         /**
          * Creates a new default retry policy.
          *
          * This policy retries queries in the following cases:
          * <ul>
          *   <li>On a read timeout, if enough replicas replied but data was not received.</li>
          *   <li>On a write timeout, if a timeout occurs while writing the distributed batch log</li>
          *   <li>On unavailable, it will move to the next host</li>
          * </ul>
          *
          * In all other cases the error will be returned.
          *
          * This policy always uses the query's original consistency level.
          *
          * @public @memberof w_Cass_Retry_Policy
          *
          * @return Returns a retry policy that must be freed.
          *
          * @see w_cassandra_retry_policy_free()
          */
        W_SYSTEM_EXPORT w_cass_retry_policy* 
            w_cassandra_retry_policy_default_new();

        /**
         * Creates a new downgrading consistency retry policy.
         *
         * <b>Important:</b> This policy may attempt to retry requests with a lower
         * consistency level. Using this policy can break consistency guarantees.
         *
         * This policy will retry in the same scenarios as the default policy, but
         * it will also retry in the following cases:
         * <ul>
         *   <li>On a read timeout, if some replicas responded but is lower than
         *   required by the current consistency level then retry with a lower
         *   consistency level.</li>
         *   <li>On a write timeout, Retry unlogged batches at a lower consistency level
         *   if at least one replica responded. For single queries and batch if any
         *    replicas responded then consider the request successful and swallow the
         *    error.</li>
         *   <li>On unavailable, retry at a lower consistency if at lease one replica
         *   responded.</li>
         * </ul>
         *
         * This goal of this policy is to attempt to save a request if there's any
         * chance of success. A writes succeeds as long as there's a single copy
         * persisted and a read will succeed if there's some data available even
         * if it increases the risk of reading stale data.
         *
         * @deprecated This still works, but should not be used in new applications. It
         * can lead to unexpected behavior when the cluster is in a degraded state.
         * Instead, applications should prefer using the lowest consistency level on
         * statements that can be tolerated by a specific use case.
         *
         * @public @memberof w_Cass_Retry_Policy
         *
         * @return Returns a retry policy that must be freed.
         *
         * @see w_cassandra_retry_policy_free()
         */
        W_SYSTEM_EXPORT w_cass_retry_policy*
            w_cassandra_retry_policy_downgrading_consistency_new();

        /**
         * Creates a new fallthrough retry policy.
         *
         * This policy never retries or ignores a server-side failure. The error
         * is always returned.
         *
         * @public @memberof w_Cass_Retry_Policy
         *
         * @return Returns a retry policy that must be freed.
         *
         * @see w_cassandra_retry_policy_free()
         */
        W_SYSTEM_EXPORT w_cass_retry_policy* 
            w_cassandra_retry_policy_fallthrough_new();

        /**
         * Creates a new logging retry policy.
         *
         * This policy logs the retry decision of its child policy. Logging is
         * done using CASS_LOG_INFO.
         *
         * @public @memberof w_Cass_Retry_Policy
         *
         * @param[in] child_retry_policy
         * @return Returns a retry policy that must be freed. NULL is returned if
         * the child_policy is a logging retry policy.
         *
         * @see w_cassandra_retry_policy_free()
         */
        W_SYSTEM_EXPORT  w_cass_retry_policy* 
            w_cassandra_retry_policy_logging_new(_In_ w_cass_retry_policy* pChildRetryPolicy);

        /**
         * Frees a retry policy instance.
         *
         * @public @memberof w_Cass_Retry_Policy
         *
         * @param[in] policy
         */
        W_SYSTEM_EXPORT void
            w_cassandra_retry_policy_free(_In_ w_cass_retry_policy* pPolicy);

        /***********************************************************************************
         *
         * Custom payload
         *
         ***********************************************************************************/

         /**
          * Creates a new custom payload.
          *
          * @public @memberof w_Cass_Custom_Payload
          *
          * @cassandra{2.2+}
          *
          * @return Returns a custom payload that must be freed.
          *
          * @see w_cassandra_custom_payload_free()
          */
        W_SYSTEM_EXPORT w_cass_custom_payload* 
            w_cassandra_custom_payload_new();

        /**
         * Frees a custom payload instance.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Custom_Payload
         *
         * @param[in] payload
         */
        W_SYSTEM_EXPORT void
            w_cassandra_custom_payload_free(_In_ w_cass_custom_payload* pPayload);

        /**
         * Sets an item to the custom payload.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Custom_Payload
         *
         * @param[in] payload
         * @param[in] name
         * @param[in] value
         * @param[in] value_size
         */
        W_SYSTEM_EXPORT void
            w_cassandra_custom_payload_set(_In_ w_cass_custom_payload* pPayload,
                _In_  const char* pName,
                _In_ const w_byte_t* pValue,
                _In_ size_t pValueSize);

        /**
         * Same as w_cassandra_custom_payload_set(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Custom_Payload
         *
         * @param[in] payload
         * @param[in] name
         * @param[in] name_length
         * @param[in] value
         * @param[in] value_size
         */
        W_SYSTEM_EXPORT void
            w_cassandra_custom_payload_set_n(_In_ w_cass_custom_payload* pPayload,
                _In_ const char* pName, 
                _In_  size_t pNameLength, 
                _In_ const w_byte_t* pValue,
                _In_ size_t pValueSize);

        /**
         * Removes an item from the custom payload.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Custom_Payload
         *
         * @param[in] payload
         * @param[in] name
         */
        W_SYSTEM_EXPORT void
            w_cassandra_custom_payload_remove(_In_ w_cass_custom_payload* pPayload, 
                _In_ const char* pName);

        /**
         * Same as w_cassandra_custom_payload_set(), but with lengths for string
         * parameters.
         *
         * @cassandra{2.2+}
         *
         * @public @memberof w_Cass_Custom_Payload
         *
         * @param[in] payload
         * @param[in] name
         * @param[in] name_length
         */
        W_SYSTEM_EXPORT void
            w_cassandra_custom_payload_remove_n(_In_ w_cass_custom_payload* pPayload, 
                _In_ const char* pName,
                _In_ size_t pNameLength);


        /***********************************************************************************
         *
         * Consistency
         *
         ***********************************************************************************/

         /**
          * Gets the string for a consistency.
          *
          * @param[in] consistency
          * @return A null-terminated string for the consistency.
          * Example: "ALL", "ONE", "QUORUM", etc.
          */
        W_SYSTEM_EXPORT const char*
            w_cassandra_consistency_string(_In_ w_cass_consistency pConsistency);

        /***********************************************************************************
         *
         * Write type
         *
         ***********************************************************************************/
         /**
          * Gets the string for a write type.
          *
          * @param[in] write_type
          * @return A null-terminated string for the write type.
          * Example: "BATCH", "SIMPLE", "COUNTER", etc.
          */
        W_SYSTEM_EXPORT const char*
            w_cassandra_write_type_string(_In_ w_cass_writetype pWriteType);

        /***********************************************************************************
         *
         * Error
         *
         ***********************************************************************************/

         /**
          * Gets a description for an error code.
          *
          * @param[in] error
          * @return A null-terminated string describing the error.
          */
        W_SYSTEM_EXPORT const char*
            w_cassandra_error_desc(_In_ w_Cass_Error error);

        /***********************************************************************************
         *
         * Log
         *
         ***********************************************************************************/

         /**
          * Explicitly wait for the log to flush and deallocate resources.
          * This *MUST* be the last call using the library. It is an error
          * to call any w_cassandra_*() functions after this call.
          *
          * @deprecated This is no longer useful and does nothing. Expect this to be
          * removed in a future release.
          */
        W_SYSTEM_EXPORT void
            w_cassandra_log_cleanup();

        /**
         * Sets the log level.
         *
         * <b>Note:</b> This needs to be done before any call that might log, such as
         * any of the w_cassandra_cluster_*() or w_cassandra_ssl_*() functions.
         *
         * <b>Default:</b> CASS_LOG_WARN
         *
         * @param[in] log_level
         */
        W_SYSTEM_EXPORT void
            w_cassandra_log_set_level(_In_ w_cass_log_level pLogLevel);

        /**
         * Sets a callback for handling logging events.
         *
         * <b>Note:</b> This needs to be done before any call that might log, such as
         * any of the w_cassandra_cluster_*() or w_cassandra_ssl_*() functions.
         *
         * <b>Default:</b> An internal callback that prints to stderr
         *
         * @param[in] data An opaque data object passed to the callback.
         * @param[in] callback A callback that handles logging events. This is
         * called in a separate thread so access to shared data must be synchronized.
         */
        W_SYSTEM_EXPORT void
            w_cassandra_log_set_callback(_In_ w_cass_log_callback callback,
                _In_  void* data);

        /**
         * Sets the log queue size.
         *
         * <b>Note:</b> This needs to be done before any call that might log, such as
         * any of the w_cassandra_cluster_*() or w_cassandra_ssl_*() functions.
         *
         * <b>Default:</b> 2048
         *
         * @deprecated This is no longer useful and does nothing. Expect this to be
         * removed in a future release.
         *
         * @param[in] queue_size
         */
        W_SYSTEM_EXPORT void
            w_cassandra_log_set_queue_size(size_t queue_size);

        /**
         * Gets the string for a log level.
         *
         * @param[in] log_level
         * @return A null-terminated string for the log level.
         * Example: "ERROR", "WARN", "INFO", etc.
         */
        W_SYSTEM_EXPORT const char*
            w_cassandra_log_level_string(_In_ w_cass_log_level pLogLevel);

        /***********************************************************************************
         *
         * Inet
         *
         ************************************************************************************/

         /**
          * Constructs an inet v4 object.
          *
          * @public @memberof w_cass_inet
          *
          * @param[in] address An address of size CASS_INET_V4_LENGTH
          * @return An inet object.
          */
        W_SYSTEM_EXPORT w_cass_inet  
            w_cassandra_inet_init_v4(_In_ const uint8_t* pAddress);

        /**
         * Constructs an inet v6 object.
         *
         * @public @memberof w_cass_inet
         *
         * @param[in] address An address of size CASS_INET_V6_LENGTH
         * @return An inet object.
         */
        W_SYSTEM_EXPORT w_cass_inet 
            w_cassandra_inet_init_v6(_In_ const uint8_t* pAddress);

        /**
         * Returns a null-terminated string for the specified inet.
         *
         * @public @memberof w_cass_inet
         *
         * @param[in] inet
         * @param[out] output A null-terminated string of length CASS_INET_STRING_LENGTH.
         */
        W_SYSTEM_EXPORT void
            w_cassandra_inet_string(_In_ w_cass_inet pInet, 
                _Inout_ char* pOutput);

        /**
         * Returns an inet for the specified string.
         *
         * Examples: "127.0.0.1" or "::1"
         *
         * @public @memberof w_cass_inet
         *
         * @param[in] str
         * @param[out] output
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_inet_from_string(_In_ const char* pStr,
                _Inout_ w_cass_inet* pOutput);

        /**
         * Same as w_cassandra_inet_from_string(), but with lengths for string
         * parameters.
         *
         * @public @memberof w_cass_inet
         *
         * @param[in] str
         * @param[in] str_length
         * @param[out] output
         * @return same as w_cassandra_inet_from_string()
         *
         * @see w_cassandra_inet_from_string()
         */
        W_SYSTEM_EXPORT w_Cass_Error
            w_cassandra_inet_from_string_n(_In_ const char* pStr,
                _In_  size_t pStrLength,
                _Inout_  w_cass_inet* pOutput);
     
        /***********************************************************************************
         *
         * Date/Time
         *
         ************************************************************************************/

         /**
          * Converts a unix timestamp (in seconds) to the Cassandra "date" type. The "date" type
          * represents the number of days since the Epoch (1970-01-01) with the Epoch centered at
          * the value 2^31.
          *
          * @cassandra{2.2+}
          *
          * @param[in] epoch_secs
          * @return the number of days since the date -5877641-06-23
          */
        W_SYSTEM_EXPORT uint32_t
            w_cassandra_date_from_epoch(_In_ int64_t pEpochSecs);

        /**
         * Converts a unix timestamp (in seconds) to the Cassandra "time" type. The "time" type
         * represents the number of nanoseconds since midnight (range 0 to 86399999999999).
         *
         * @cassandra{2.2+}
         *
         * @param[in] epoch_secs
         * @return nanoseconds since midnight
         */
        W_SYSTEM_EXPORT int64_t
            w_cassandra_time_from_epoch(_In_ int64_t pEpochSecs);

        /**
         * Combines the Cassandra "date" and "time" types to Epoch time in seconds.
         *
         * @cassandra{2.2+}
         *
         * @param[in] date
         * @param[in] time
         * @return Epoch time in seconds. Negative times are possible if the date
         * occurs before the Epoch (1970-1-1).
         */
        W_SYSTEM_EXPORT int64_t
            w_cassandra_date_time_to_epoch(_In_ uint32_t pDate,
                _In_ int64_t pTime);

        /***********************************************************************************
         *
         * Allocator
         *
         ************************************************************************************/

         /**
          * Set custom allocation functions.
          *
          * <b>Note:</b> This is not thread-safe. The allocation functions must be set
          * before any other library function is called.
          *
          * <b>Default:</b> The C runtime's malloc(), realloc() and free()
          *
          * <b>Important:</b> The C runtime's malloc(), realloc() and free() will be
          * used by libuv when using versions 1.5 or earlier.
          *
          * @param[in] malloc_func
          * @param[in] realloc_func
          * @param[in] free_func
          */
        W_SYSTEM_EXPORT void
            w_cassandra_alloc_set_functions(_In_ w_cass_malloc_function pMallocFunc,
                _In_  w_cass_realloc_function RpeallocFunc,
                _In_ w_cass_free_function pFreeFunc);

#ifdef __cplusplus
    } /* extern "C" */
#endif


