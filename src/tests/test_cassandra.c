#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include<wolf.h>
#include"db/w_cassandra.h"

struct Basic_ {
    w_bool_t bln;
    float flt;
    double dbl;
    int32_t i32;
    int64_t i64;
};

typedef struct Basic_ Basic;

void print_error(w_Cass_Future* future) {
    const char* message;
    size_t message_length;
    w_cassandra_future_error_message(future, &message, &message_length);
    fprintf(stderr, "Error: %.*s\n", (int)message_length, message);
}

w_Cass_Cluster* create_cluster(const char* hosts) {
    w_Cass_Cluster* cluster = w_cassandra_cluster_new();
    w_cassandra_cluster_set_protocol_version(cluster, W_CASS_PROTOCOL_VERSION_V4);
    w_cassandra_cluster_set_contact_points(cluster, hosts);
    return cluster;
}

w_Cass_Error connect_session(w_Cass_Session* session, const w_Cass_Cluster* cluster) {
    w_Cass_Error rc = W_CASS_OK;
    w_Cass_Future* future = w_cassandra_session_connect(session, cluster);

    w_cassandra_future_wait(future);
    rc = w_cassandra_future_error_code(future);
    if (rc != W_CASS_OK) {
        print_error(future);
    }
    w_cassandra_future_free(future);

    return rc;
}

w_Cass_Error execute_query(w_Cass_Session* session, const char* query) {
    w_Cass_Error rc = W_CASS_OK;
    w_Cass_Future* future = NULL;
    w_Cass_Statement* statement = w_cassandra_statement_new(query, 0);

    future = w_cassandra_session_execute(session, statement);
    w_cassandra_future_wait(future);

    rc = w_cassandra_future_error_code(future);
    if (rc != W_CASS_OK) {
        print_error(future);
    }

    w_cassandra_future_free(future);
    w_cassandra_statement_free(statement);

    return rc;
}

w_Cass_Error insert_into_basic(w_Cass_Session* session, const char* key, const Basic* basic) {
    w_Cass_Error rc = W_CASS_OK;
    w_Cass_Statement* statement = NULL;
    w_Cass_Future* future = NULL;
    const char* query =
        "INSERT INTO hero.playpod (key, bln, flt, dbl, i32, i64) VALUES (?, ?, ?, ?, ?, ?);";

    statement = w_cassandra_statement_new(query, 6);

    w_cassandra_statement_bind_string(statement, 0, key);
    w_cassandra_statement_bind_bool(statement, 1, basic->bln);
    w_cassandra_statement_bind_float(statement, 2, basic->flt);
    w_cassandra_statement_bind_double(statement, 3, basic->dbl);
    w_cassandra_statement_bind_int32(statement, 4, basic->i32);
    w_cassandra_statement_bind_int64(statement, 5, basic->i64);

    future = w_cassandra_session_execute(session, statement);
    w_cassandra_future_wait(future);

    rc = w_cassandra_future_error_code(future);
    if (rc != W_CASS_OK) {
        print_error(future);
    }

    w_cassandra_future_free(future);
    w_cassandra_statement_free(statement);

    return rc;
}

w_Cass_Error select_from_basic(w_Cass_Session* session, const char* key, Basic* basic) {
    w_Cass_Error rc = W_CASS_OK;
    w_Cass_Statement* statement = NULL;
    w_Cass_Future* future = NULL;
    const char* query = "SELECT * FROM hero.playpod WHERE key = ?";

    statement = w_cassandra_statement_new(query, 1);

    w_cassandra_statement_bind_string(statement, 0, key);

    future = w_cassandra_session_execute(session, statement);
    w_cassandra_future_wait(future);

    rc = w_cassandra_future_error_code(future);
    if (rc != W_CASS_OK) {
        print_error(future);
    }
    else {
        const w_Cass_Result* result = w_cassandra_future_get_result(future);
        w_Cass_Iterator* iterator = w_cassandra_iterator_from_result(result);

        if (w_cassandra_iterator_next(iterator)) {
            const w_Cass_Row* row = w_cassandra_iterator_get_row(iterator);
            w_cassandra_value_get_bool(w_cassandra_row_get_column(row, 1), &basic->bln);
            w_cassandra_value_get_double(w_cassandra_row_get_column(row, 2), &basic->dbl);
            w_cassandra_value_get_float(w_cassandra_row_get_column(row, 3), &basic->flt);
            w_cassandra_value_get_int32(w_cassandra_row_get_column(row, 4), &basic->i32);
            w_cassandra_value_get_int64(w_cassandra_row_get_column(row, 5), &basic->i64);
        }

        w_cassandra_result_free(result);
        w_cassandra_iterator_free(iterator);
    }

    w_cassandra_future_free(future);
    w_cassandra_statement_free(statement);

    return rc;
}

int main(int argc, char* argv[]) {
    wolf_init();
    w_Cass_Cluster* cluster = NULL;
    w_Cass_Session* session = w_cassandra_session_new();
    char* hosts = "10.56.132.42";

    Basic input = { w_true, 0.001f, 0.0002, 1, 2 };
    Basic output;

    if (argc > 1) {
        hosts = argv[1];
    }
    cluster = create_cluster(hosts);

    if (connect_session(session, cluster) != W_CASS_OK) {
        w_cassandra_cluster_free(cluster);
        w_cassandra_session_free(session);
        return -1;
    }

    execute_query(session, "CREATE KEYSPACE hero WITH replication = { \
                           'class': 'SimpleStrategy', 'replication_factor': '3' };");

    execute_query(session, "CREATE TABLE hero.playpod (key text, \
                                              bln boolean, \
                                              flt float, dbl double,\
                                              i32 int, i64 bigint, \
                                              PRIMARY KEY (key));");

    insert_into_basic(session, "test", &input);
    select_from_basic(session, "test", &output);
    execute_query(session, "UPDATE hero.playpod SET dbl = 0.0002  WHERE key = 'test'");
    execute_query(session, "DELETE dbl FROM hero.playpod WHERE key = 'test'");

    assert(input.bln == output.bln);
    assert(input.flt == output.flt);
    assert(input.dbl == output.dbl);
    assert(input.i32 == output.i32);
    assert(input.i64 == output.i64);

    w_cassandra_session_free(session);
    w_cassandra_cluster_free(cluster);
    wolf_fini();
    return 0;
}
