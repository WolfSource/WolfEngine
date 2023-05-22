#ifdef WOLF_SYSTEM_POSTGRESQL

#pragma once

#include <memory>
#include <string>
#include <type_traits>

#include <libpq-fe.h>

#include <wolf.hpp>

namespace wolf::system::pgdb {

namespace internal {

/**
 * @brief make a default instantiable delter callable out of compile-time known callable objects.
 *
 * intended to be used for C deleter functions, like `free`.
 * to be used with `std::unique_ptr` or similars.
 *
 * @tparam T type of the pointer which this deleter will be called with.
 * @tparam Func a callable object that takes a pointer of `T` as argument.
 */
template <typename T, auto Func>
struct c_type_deleter {
    using pointer = std::remove_cvref_t<T>*;

    constexpr auto operator()(pointer p_ptr) noexcept {
        return Func(p_ptr);
    }
    constexpr auto operator()(pointer p_ptr) const noexcept {
        return Func(p_ptr);
    }
};

template <typename T, auto DeleterFunc>
using c_type_raii = std::unique_ptr<T, c_type_deleter<T, DeleterFunc>>;

using dbconn = c_type_raii<PGconn, PQfinish>;
using dbresult = c_type_raii<PGresult, PQclear>;
using dbcancel = c_type_raii<PGcancel, PQfreeCancel>;

/**
 * @brief check whether the given connection pointer is in error state.
 * @param p_conn internal connection pointer.
 * @return error or nothing.
 */
inline auto dbconn_error_check(const dbconn& p_conn) -> boost::leaf::result<void>
{
    if (!p_conn) {
        return W_FAILURE(std::errc::invalid_argument, "connection is null.");
    }

    switch (PQstatus(p_conn.get())) {
    case CONNECTION_BAD:
        return W_FAILURE(PQstatus(p_conn.get()), PQerrorMessage(p_conn.get()));

    default:
        return {};
    }
}

/**
 * @brief check whether the given result pointer is in error state.
 * @param p_res internal result pointer.
 * @return error or nothing.
 */
inline auto dbresult_error_check(const dbresult& p_res) -> boost::leaf::result<void>
{
    if (!p_res) {
        return W_FAILURE(std::errc::invalid_argument,
                         "result is null.");
    }

    switch (PQresultStatus(p_res.get())) {
    case PGRES_FATAL_ERROR:
    case PGRES_NONFATAL_ERROR:
    case PGRES_BAD_RESPONSE:
        return W_FAILURE(PQresultStatus(p_res.get()), PQresultErrorMessage(p_res.get()));

    default:
        return {};
    }
}

/// basic converters

template <typename T> requires std::is_integral_v<T>
inline auto to_string(T p_value) -> std::string
{
    return std::to_string(p_value);
}

inline auto to_string(const char* const p_value) -> std::string
{
    return std::string(p_value);
}

inline auto to_string(const std::string& p_value) -> std::string
{
    return p_value;
}

inline auto to_string(std::string&& p_value) -> std::string
{
    return p_value;
}

template <typename T>
    requires std::is_integral_v<T>
inline auto from_string(T* p_ptr, const char* p_str) -> boost::leaf::result<void>
{
    auto [errptr, errcode] = std::from_chars(p_str, p_str + strlen(p_str), *p_ptr);
    if (errcode != std::errc()) {
        return W_FAILURE(errcode, "could not convert given string to given integral type.");
    }

    return {};
}

inline auto from_string(std::string* p_ptr, const char* p_str) -> boost::leaf::result<void>
{
    *p_ptr = std::string(p_str);
    return {};
}

}  // namespace internal

// forward declares mostly for friend statements.
class w_pgdb_conection;
class w_pgdb_execresult;
class w_pgdb_queryresult;

/**
 * @brief single convertible record/row.
 */
class w_pgdb_record {
    friend w_pgdb_queryresult;

public:
    /**
     * @brief get raw string at given field index.
     * @return raw c string value at given field index, it might be null.
     */
    auto at(std::size_t p_field_index) const noexcept -> const char*
    {
        return PQgetvalue(_res_ptr.get(), static_cast<int>(_record_index), static_cast<int>(p_field_index));
    }

    /**
     * @brief count of fields.
     */
    auto size() const noexcept -> std::size_t
    {
        return PQnfields(_res_ptr.get());
    }

    /**
     * @brief convert fields into given types and store into pointed objects.
     * @param ptrs pointers to objects to read.
     */
    template <typename ...Ts>
    auto scan(Ts* ...p_ptrs) -> boost::leaf::result<void>
    {
        if (sizeof...(Ts) != PQnfields(_res_ptr.get())) {
            return W_FAILURE(std::errc::invalid_argument,
                             "given params to scan are more than available record fields.");
        }

        if constexpr (sizeof...(Ts) == 0) {
            return {};
        } else {
            return scan(0, p_ptrs...);
        }
    }

private:
    template <typename T>
    auto scan(std::size_t p_field_index, T* p_ptr) -> boost::leaf::result<void>
    {
        return scan_field_into(p_field_index, p_ptr);
    }

    template <typename T, typename U, typename ...Rest>
    auto scan(std::size_t p_field_index, T* p_first, U* p_second, Rest* ...p_rest)
        -> boost::leaf::result<void>
    {
        BOOST_LEAF_CHECK(scan_field_into(p_field_index, p_first));

        return scan(p_field_index + 1, p_second, p_rest...);
    }

    template <typename T>
    auto scan_field_into(std::size_t p_field_index, T* p_ptr) -> boost::leaf::result<void>
    {
        auto str_value = PQgetvalue(_res_ptr.get(), _record_index, p_field_index);
        if (!str_value) {
            return W_FAILURE(std::errc::invalid_argument, "field could not be retrieved.");
        }

        BOOST_LEAF_CHECK(internal::from_string(p_ptr, str_value));

        return {};
    }

    w_pgdb_record(const internal::dbresult& p_res, std::size_t p_index) noexcept
        : _res_ptr(p_res)
        , _record_index(p_index)
    {}

    const internal::dbresult& _res_ptr;
    const std::size_t _record_index = 0;
};

/**
 * @brief sql query result from sql command that does yield back values.
 */
class w_pgdb_queryresult {
    friend w_pgdb_execresult;

public:
    ~w_pgdb_queryresult() { done(); }

    // move-only
    w_pgdb_queryresult(const w_pgdb_queryresult&) = delete;
    w_pgdb_queryresult(w_pgdb_queryresult&& p_other) noexcept
        : _conn_ptr(p_other._conn_ptr)
        , _res_ptr(p_other._res_ptr.release())
        , _row_index(std::exchange(p_other._row_index, 0))
    {}

    w_pgdb_queryresult& operator=(const w_pgdb_queryresult&) = delete;
    w_pgdb_queryresult& operator=(w_pgdb_queryresult&& p_other) noexcept
    {
        std::swap(_conn_ptr, p_other._conn_ptr);
        std::swap(_res_ptr, p_other._res_ptr);
        std::swap(_row_index, p_other._row_index);
        return *this;
    };

    /**
     * @brief count of records in the result.
     */
    auto size() const noexcept -> std::size_t
    {
        return PQntuples(_res_ptr.get());
    }

    /**
     * @brief count of fields in each record.
     */
    auto field_count() const noexcept -> std::size_t
    {
        return PQnfields(_res_ptr.get());
    }

    /**
     * @brief name of the field at given index.
     * @param p_index field index.
     */
    auto field_name_at(std::size_t p_index) const noexcept -> const char*
    {
        return PQfname(_res_ptr.get(), static_cast<int>(p_index));
    }

    /**
     * @brief whether the result contains or doesn't contain any records.
     */
    auto is_empty() -> bool
    {
        auto status = PQresultStatus(_res_ptr.get());
        return (status != PGRES_TUPLES_OK && status != PGRES_SINGLE_TUPLE) || size() == 0;
    }

    /**
     * @brief process each record by given handler.
     *
     * until the handler return error or there wouldn't be any more result.
     *
     * @param p_handler callable to pass each record to and returns a boost leaf result.
     * @return count of processed records.
     */
    template <typename F>
    auto on_each(F&& p_handler) -> boost::leaf::result<std::size_t>
    {
        if (!_res_ptr) {
            return W_FAILURE(std::errc::operation_canceled, "result doesn't exist or has been consumed already.");
        }

        std::size_t row_index = 0;

        const auto size = PQntuples(_res_ptr.get());

        while (row_index < size) {
            auto record = w_pgdb_record(_res_ptr, row_index);

            BOOST_LEAF_CHECK(p_handler(record));

            if (PQresultStatus(_res_ptr.get()) == PGRES_SINGLE_TUPLE) {
                _res_ptr.reset(PQgetResult(_conn_ptr.get()));
            }

            ++row_index;
        }

        return row_index;
    }

    /**
     * @brief process a single record by given handler.
     * @param p_handler callable to pass each record to and returns a boost leaf result.
     * @return error or nothing.
     */
    template <typename F>
    auto on_single(F&& p_handler) -> boost::leaf::result<void>
    {
        if (!_res_ptr) {
            return W_FAILURE(std::errc::operation_canceled,
                             "result doesn't exist or has been consumed already.");
        }

        auto record = w_pgdb_record(_res_ptr, _row_index);

        BOOST_LEAF_CHECK(p_handler(record));

        if (PQresultStatus(_res_ptr.get()) == PGRES_SINGLE_TUPLE) {
            _res_ptr.reset(PQgetResult(_conn_ptr.get()));
        }

        _row_index++;

        return {};
    }

    /**
     * @brief cancel the ongoing query.
     * @return error or nothing.
     */
    auto cancel() -> boost::leaf::result<void>
    {
        auto cancel_req = internal::dbcancel(PQgetCancel(_conn_ptr.get()));
        if (!cancel_req) {
            return W_FAILURE(PQstatus(_conn_ptr.get()), "could not create cancellation object.");
        }

        // stack is much faster and safer, heap alloc isn't required if we don't hit error.
        constexpr auto errbuf_size = 255;
        char errbuf[errbuf_size + 1] = { 0 };

        if (PQcancel(cancel_req.get(), errbuf, errbuf_size) <= 0) {
            return W_FAILURE(PQstatus(_conn_ptr.get()), std::string(errbuf));
        }

        return {};
    }

    /**
     * @brief consume the remaining of the result,
     *        so new command/queries on the connection can be made.
     */
    void done()
    {
        while (_res_ptr) {
            _res_ptr.reset(PQgetResult(_conn_ptr.get()));
        }
    }

private:
    explicit w_pgdb_queryresult(internal::dbconn& p_conn, internal::dbresult&& p_result)
        : _conn_ptr(p_conn)
        , _res_ptr(std::move(p_result))
    {}

    auto raw() noexcept -> internal::dbresult& { return _res_ptr; }
    auto raw() const noexcept -> const internal::dbresult& { return _res_ptr; }

    internal::dbconn& _conn_ptr;
    internal::dbresult _res_ptr{nullptr};
    std::size_t _row_index = 0; //< count of so far read rows.
};

/**
 * @brief sql execution result from sql command that doesn't yield back values.
 */
class w_pgdb_execresult {
    friend w_pgdb_conection;

public:
    ~w_pgdb_execresult() { done(); }

    // move-only
    w_pgdb_execresult(const w_pgdb_execresult&) = delete;
    w_pgdb_execresult(w_pgdb_execresult&& p_other) noexcept
        : _conn_ptr(p_other._conn_ptr)
        , _res_ptr(p_other._res_ptr.release())
    {}

    w_pgdb_execresult& operator=(const w_pgdb_execresult&) = delete;
    w_pgdb_execresult& operator=(w_pgdb_execresult&& p_other) noexcept
    {
        std::swap(_conn_ptr, p_other._conn_ptr);
        std::swap(_res_ptr, p_other._res_ptr);
        return *this;
    };

    // since two internal::dbresult can't exist at the same time,
    // create w_pgdb_queryresult and dispose *this.
    auto as_query() && -> w_pgdb_queryresult
    {
        return w_pgdb_queryresult(_conn_ptr, std::move(_res_ptr));
    }

    /**
     * @brief consume the remaining of the result,
     *        so new command/queries on the connection can be made.
     */
    void done()
    {
        while (_res_ptr) {
            _res_ptr.reset(PQgetResult(_conn_ptr.get()));
        }
    }

private:
    explicit w_pgdb_execresult(internal::dbconn& p_conn, internal::dbresult&& p_res)
        : _conn_ptr(p_conn)
        , _res_ptr(std::move(p_res))
    {}

    auto raw() noexcept -> internal::dbresult& { return _res_ptr; }
    auto raw() const noexcept -> const internal::dbresult& { return _res_ptr; }

    internal::dbconn& _conn_ptr;
    internal::dbresult _res_ptr{nullptr};
};

/**
 * a single connection to database to execute/query one at a time.
 *
 * create a connection by calling one of static factory methods like `make`.
 */
class w_pgdb_conection {
public:
    w_pgdb_conection(const w_pgdb_conection&) = delete;
    w_pgdb_conection(w_pgdb_conection&& other) noexcept
        : _conn_ptr(other._conn_ptr.release())
    {}

    w_pgdb_conection& operator=(const w_pgdb_conection&) = delete;
    w_pgdb_conection& operator=(w_pgdb_conection&& other) noexcept
    {
        std::swap(_conn_ptr, other._conn_ptr);
        return *this;
    };

    ~w_pgdb_conection() { _conn_ptr.reset(); }

    /**
     * @brief make a connection by given key-value parameters.
     *
     * @note each pair's key and value must be convertible to `const char*`.
     * @note valid param details can be found at: https://www.postgresql.org/docs/current/libpq-connect.html
     *
     * @param p_params pairs of key-value parameters. (e.g. {"host", "localhost"}, {"port", "26257"})
     * @return database connection or error.
     */
    template <typename ...Ts, typename ...Us>
    static auto make(const std::pair<Ts, Us>& ...p_params) -> boost::leaf::result<w_pgdb_conection>
    {
        const auto params_count = sizeof...(Ts);

        const char* keywords[params_count + 1] = { static_cast<const char*>(p_params.first)..., nullptr };
        const char* values[params_count + 1] = { static_cast<const char*>(p_params.second)..., nullptr };

        auto dbconn = internal::dbconn(PQconnectdbParams(keywords, values, 0));

        BOOST_LEAF_CHECK(internal::dbconn_error_check(dbconn));

        return w_pgdb_conection(std::move(dbconn));
    }

    /**
     * @brief make a connection by given connection string url.
     *
     * for example: postgre://admin:admin@localhost:26257/defaultdb
     *
     * @param p_conn_str connection string url.
     * @return database connection or error.
     */
    static auto make(const char* p_conn_str) -> boost::leaf::result<w_pgdb_conection>
    {
        auto conn = internal::dbconn(PQconnectdb(p_conn_str));

        BOOST_LEAF_CHECK(internal::dbconn_error_check(conn));

        return w_pgdb_conection(std::move(conn));
    }

    /**
     * @brief execute the given sql command with given sql parameters.
     *
     * @note params will be converted to string by an internal converter for simple types.
     *       if your type isn't supported, convert to string then pass it to this method.
     *
     * @param p_sqlstr sql command.
     * @param p_params sql command's parameters. (if there are any)
     * @return command result or error.
     */
    template <typename ...Args>
    auto execute(const char* p_sqlstr, Args&& ...p_params) -> boost::leaf::result<w_pgdb_execresult>
    {
        constexpr std::size_t arg_count = sizeof...(Args);

        // avoid complicated process of making params when none are given.
        if constexpr (arg_count == 0) {
            if (PQsendQuery(_conn_ptr.get(), p_sqlstr) <= 0) {
                return W_FAILURE(PQstatus(_conn_ptr.get()), PQerrorMessage(_conn_ptr.get()));
            }
        } else {
            // currently raw data isn't supported, and it's only text/string.
            // IIFE for compile-time stack array allocation and loop-free initialization.
            int res = [&](std::same_as<std::string> auto&& ...p_str_params) {
                const char* string_array[arg_count] = { (p_str_params.c_str())... };

                int length_array[arg_count] = { static_cast<int>(p_str_params.size())... };

                return PQsendQueryParams(
                            _conn_ptr.get(),
                            p_sqlstr,
                            arg_count,
                            nullptr,  // infer types, they're not determined.
                            string_array,
                            length_array,
                            nullptr,  // only nul-terminated c strings (text), binary isn't supported.
                            0
                       );
            }(internal::to_string(std::forward<Args>(p_params))...);

            if (res <= 0) {
                return W_FAILURE(PQstatus(_conn_ptr.get()), PQerrorMessage(_conn_ptr.get()));
            }
        }

        // to make sure the result will be terminated/disposed correctly on error,
        // otherwise the connection will still hold the result and next execute won't work.
        auto dbres = w_pgdb_execresult(_conn_ptr, internal::dbresult(PQgetResult(_conn_ptr.get())));
        BOOST_LEAF_CHECK(internal::dbresult_error_check(dbres.raw()));

        return dbres;
    }

    /**
     * @brief execute the given sql query with given sql parameters.
     *
     * @note params will be converted to string by an internal converter for simple types.
     *       if your type isn't supported, convert to string then pass it to this method.
     *
     * @param p_sqlstr sql query.
     * @param p_params sql query's parameters. (if there are any)
     * @return query result or error.
     */
    template <typename ...Args>
    auto query(const char* p_sqlstr, Args&& ...p_args) -> boost::leaf::result<w_pgdb_queryresult>
    {
        BOOST_LEAF_AUTO(res, execute(p_sqlstr, std::forward<Args>(p_args)...));
        return std::move(res).as_query();
    }

    /**
     * @brief execute the given sql query with given sql parameters in streaming mode.
     *
     * the result records will be fetched one at a time to be processed,
     * instead of waiting for all records to be retrieved.
     *
     * @note params will be converted to string by an internal converter for simple types.
     *       if your type isn't supported, convert to string then pass it to this method.
     *
     * @param p_sqlstr sql query.
     * @param p_params sql query's parameters. (if there are any)
     * @return query result or error.
     */
    template <typename ...Args>
    auto stream_query(const char* p_sqlstr, Args&& ...p_args) -> boost::leaf::result<w_pgdb_queryresult>
    {
        BOOST_LEAF_AUTO(res, execute(p_sqlstr, std::forward<Args>(p_args)...));

        if (PQsetSingleRowMode(_conn_ptr.get()) <= 0) {
            return W_FAILURE(PQstatus(_conn_ptr.get()), PQerrorMessage(_conn_ptr.get()));
        }

        return std::move(res).as_query();
    }

private:
    explicit w_pgdb_conection(internal::dbconn&& p_conn) : _conn_ptr(std::move(p_conn)) {}

    auto raw() noexcept -> internal::dbconn& { return _conn_ptr; }
    auto raw() const noexcept -> const internal::dbconn& { return _conn_ptr; }

    internal::dbconn _conn_ptr;
};

}  // namespace wolf::system::pgdb

#endif  // WOLF_SYSTEM_POSTGRESQL
