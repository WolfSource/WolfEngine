//  modified by Pooya Eimandar base on boost fiber
//  Copyright 2003-2013 Christopher M. Kohlhoff
//  Copyright Oliver Kowalke, Nat Goodspeed 2015.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifdef WOLF_ENABLE_FIBER

#include "fiber_client_server.h"

#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <map>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <sstream>
#include <thread>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/system/error_code.hpp>

#include <boost/fiber/all.hpp>
#include "imp/round_robin.hpp"
#include "imp/yield.hpp"

#include <log/w_log.h>

using boost::asio::ip::tcp;
typedef boost::shared_ptr<tcp::socket> socket_ptr;

static std::unordered_map<int, std::shared_ptr<boost::asio::io_context>> s_context;
static std::mutex s_context_mutex;

#pragma region thread names

const char* const ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
class thread_names 
{
private:
    std::map<std::thread::id, std::string> _names{};
    const char* _next{ ALPHABET };
    std::mutex _mtx{};
public:
    thread_names() = default;    
    std::string lookup()
    {
        std::unique_lock<std::mutex> _lk(_mtx);

        auto _this_id(std::this_thread::get_id());
        auto found = this->_names.find(_this_id);
        if (found != this->_names.end())
        {
            return found->second;
        }

        BOOST_ASSERT(*_next);
        std::string _name(1, *_next++);
        this->_names[_this_id] = _name;
        return _name;
    }
};
static thread_names s_thread_names;

#pragma endregion

#pragma region fiber names

class fiber_names 
{
private:
    std::map<boost::fibers::fiber::id, std::string> _names{};
    unsigned _next{ 0 };
    boost::fibers::mutex _mtx{};

public:
    fiber_names() = default;

    std::string lookup()
    {
        std::unique_lock<boost::fibers::mutex> _lk(_mtx);
        auto _this_id(boost::this_fiber::get_id());
        auto _found = this->_names.find(_this_id);
        if (_found != this->_names.end())
        {
            return _found->second;
        }
        std::ostringstream _out;
        // Bake into the fiber's name the thread name on which we first
        // lookup() its ID, to be able to spot when a fiber hops between
        // threads.
        _out << s_thread_names.lookup() << _next++;
        std::string _name(_out.str());
        this->_names[_this_id] = _name;
        return _name;
    }
};
static fiber_names s_fiber_names;

#pragma endregion

static std::string s_thread_fiber_info()
{
    std::ostringstream _out;
    _out << "Thread " << s_thread_names.lookup() << ": "
        << std::setw(4) << s_fiber_names.lookup() << std::setw(0);
    return _out.str();
}

static boost::system::error_code s_asio_write_async(
    _In_ socket_ptr pSocket,
    _In_ w_buffer pBuffer)
{
    using namespace boost;
    using namespace boost::system;
    using namespace boost::system::errc;

    error_code _ec;
    if (!pSocket || !pBuffer)
    {
        return make_error_code(invalid_argument);
    }
    asio::async_write(
        *pSocket,
        asio::buffer(pBuffer->data, pBuffer->len),
        fibers::asio::yield[_ec]);
    return _ec;
}

static boost::system::error_code s_asio_read_async(
    _In_ socket_ptr pSocket,
    _Inout_ w_buffer* pBuffer,
    _Inout_ size_t* pReplyLength)
{
    using namespace boost;
    using namespace boost::system;
    using namespace boost::system::errc;

    error_code _ec;
    if (!pSocket || !pBuffer || !pReplyLength)
    {
        return make_error_code(invalid_argument);
    }
    *pReplyLength = pSocket->async_read_some(
        asio::buffer((*pBuffer)->data, (*pBuffer)->len),
        fibers::asio::yield[_ec]);
    return _ec;
}

W_RESULT fiber_async_write(
    _In_ void* pSocket,
    _In_ w_buffer pBuffer)
{
    const char* _trace_info = "fiber_async_write";

    if (!pSocket || !pBuffer)
    {
        return W_BAD_ARG;
    }

    auto _socket = *reinterpret_cast<socket_ptr*>(pSocket);
    if (!_socket)
    {
        return W_BAD_ARG;
    }

    auto _ec = s_asio_write_async(_socket, pBuffer);
    if (_ec)
    {
        auto _info = s_thread_fiber_info();
        if (_ec == boost::asio::error::eof)
        {
            return W_EOF;
        }
        else
        {
            LOG_P(W_LOG_DEBUG, "%s : caught exception: %s . trace info: %s",
                _info.c_str(),
                _ec.message().c_str(),
                _trace_info);
            return _ec.value();
        }
    }
    return W_SUCCESS;
}

W_RESULT fiber_async_read(
    _In_ void* pSocket,
    _Inout_ w_buffer* pBuffer,
    _Inout_ size_t* pReplyLength)
{
    const char* _trace_info = "fiber_async_read";

    if (!pSocket || !pBuffer || !pReplyLength)
    {
        return W_BAD_ARG;
    }

    auto _socket = *reinterpret_cast<socket_ptr*>(pSocket);
    if (!_socket)
    {
        return W_BAD_ARG;
    }

    auto _ec = s_asio_read_async(_socket, pBuffer, pReplyLength);
    if (_ec)
    {
        auto _info = s_thread_fiber_info();
        if (_ec == boost::asio::error::eof)
        {
            return W_EOF;
        }
        else
        {
            LOG_P(W_LOG_DEBUG, "%s : caught exception: %s . trace info: %s",
                _info.c_str(),
                _ec.message().c_str(),
                _trace_info);
            return _ec.value();
        }
    }
    return W_SUCCESS;
}

//fiber function per server connection
static void s_session(socket_ptr pSocket, w_fiber_server_callback_fn pOnReceivedCallback)
{
    const char* _trace_info = "fiber_server::s_session";
    constexpr int _max_length = 1024;
    
    try 
    {
        auto _info = s_thread_fiber_info();
        for (;;)
        {
            char _data[_max_length];
            boost::system::error_code _ec;
            auto _length = pSocket->async_read_some(
                boost::asio::buffer(_data),
                boost::fibers::asio::yield[_ec]);
            if (_ec == boost::asio::error::eof)
            {
                LOG_P(W_LOG_DEBUG, "%s : connection closed cleanly by peer. trace info: %s",
                    _info.c_str(),
                    _trace_info);
                break;
            }
            else if (_ec)
            {
                throw boost::system::system_error(_ec); //some other error
            }

            auto _str = std::string(_data, _length);
            LOG_P(W_LOG_DEBUG, "%s : handled: %s",
                _info.c_str(),
                _str.c_str());

            if (pOnReceivedCallback && _length)
            {
                w_buffer_t _b;
                _b.data = (uint8_t*)&_data[0];
                _b.len = _length;

                auto _void_ptr = reinterpret_cast<void*>(&pSocket);
                auto _ret = pOnReceivedCallback(_void_ptr, &_b, _info.c_str());
                if (_ret != W_SUCCESS)
                {
                    if (_ret == W_EOF)
                    {
                        LOG_P(W_LOG_DEBUG,
                            "%s : connection closed cleanly by peer. trace info: %s",
                            _info.c_str(),
                            _trace_info);
                    }
                    else
                    {
                        LOG_P(W_LOG_DEBUG,
                            "%s : connection closed unexpectedly. trace info: %s",
                            _info.c_str(),
                            _trace_info);
                    }
                    break;
                }
            }
        }

        LOG_P(W_LOG_DEBUG, "%s : connection closed",
            _info.c_str());
    }
    catch (std::exception const& ex)
    {
        auto _info = s_thread_fiber_info();
        LOG_P(W_LOG_DEBUG, "%s : caught exception: %s",
            _info.c_str(),
            ex.what());
    }
}

//listening server
static W_RESULT s_server(
    std::shared_ptr< boost::asio::io_context> const& pIOContext, 
    tcp::acceptor& pAcceptor,
    w_fiber_server_callback_fn pOnReceivedCallback)
{
    W_RESULT _ret = W_SUCCESS;

    auto _info = s_thread_fiber_info();
    LOG_P(W_LOG_INFO, "%s: fiber-server started", _info.c_str());

    try
    {
        for (;;)
        {
            socket_ptr _socket(new tcp::socket(*pIOContext));
            boost::system::error_code _ec;
            pAcceptor.async_accept(
                *_socket,
                boost::fibers::asio::yield[_ec]);
            if (_ec)
            {
                throw boost::system::system_error(_ec); //some other error
            }
            else
            {
                boost::fibers::fiber(s_session, _socket, pOnReceivedCallback).detach();
            }
        }
    }
    catch (std::exception const& ex)
    {
        _ret = W_FAILURE;
        LOG_P(W_LOG_ERROR, "%s: caught exception : %s", _info.c_str(), ex.what());
    }
    pIOContext->stop();
    LOG_P(W_LOG_INFO, "%s: fiber-server stopped", _info.c_str());

    return _ret;
}

static W_RESULT s_client(
    std::shared_ptr< boost::asio::io_context > const& pIOContext,
    boost::fibers::barrier& pBarrier,
    _In_ const int pIPV4_OR_IPV6,
    _In_ const char* pEndPoint,
    _In_ const uint16_t pPort,
    _In_ w_fiber_client_callback_fn pOnSendReceiveCallback)
{
    const char* _trace_info = "s_client";
    W_RESULT _ret = W_SUCCESS;

    auto _info = s_thread_fiber_info();
    try
    {
        tcp::resolver _resolver(*pIOContext);
        tcp::resolver::query _query(pIPV4_OR_IPV6 == 4 ? tcp::v6() : tcp::v4(), pEndPoint, std::to_string(pPort));
        tcp::resolver::iterator _iterator = _resolver.resolve(_query);
        socket_ptr _socket(new tcp::socket(*pIOContext));

        boost::asio::connect(*_socket, _iterator);
       
        if (pOnSendReceiveCallback)
        {
            auto _void_ptr = reinterpret_cast<void*>(&_socket);
            if (pOnSendReceiveCallback(_void_ptr, _info.c_str()) != W_SUCCESS)
            {
                //close the connection
            }
        }
    }
    catch (const std::exception const& ex)
    {
        _ret = W_FAILURE;
        LOG_P(W_LOG_ERROR, "%s: caught exception : %s", _info.c_str(), ex.what());
    }
    return _ret;//connection closed cleanly by peer
}

W_RESULT fiber_server_run(
    _In_ const int pIPV4_OR_IPV6, 
    _In_ const uint16_t pPort,
    _In_ int** pID,
    _In_ w_fiber_server_callback_fn pOnReceivedCallback)
{
    const char* _trace_info = "w_fiber_server_run";
    
    if (!pID)
    {
        W_ASSERT_P(false, "invalid parameter! pID is NULL. trace info: %s", _trace_info);
        return W_BAD_ARG;
    }

    W_RESULT _ret = W_SUCCESS;
    try
    {
        //create io context
        auto _io_ctx = std::make_shared<boost::asio::io_context>();
        boost::fibers::use_scheduling_algorithm<boost::fibers::asio::round_robin>(_io_ctx);

        auto _t_name = s_thread_names.lookup();
        LOG_P(W_LOG_DEBUG, "thread %s just started. trace info: %s", _t_name.c_str(), _trace_info);

        /* 
            W_SOCKET_FAMILY_IPV4 = 3,
            W_SOCKET_FAMILY_IPV6 = 4,
        */
        tcp::acceptor _acceptor(
            *_io_ctx,
            tcp::endpoint(pIPV4_OR_IPV6 == 4 ? tcp::v6() : tcp::v4(),
                pPort));

        //launch server based on fibers
        boost::fibers::fiber(s_server, _io_ctx, std::ref(_acceptor), pOnReceivedCallback).detach();
        
        //try for 10 times to find a random ID
        constexpr auto _max = INT_MAX;
        constexpr auto _min = 1;
        int _id = 0;

        for (size_t i = 0; i < 10; ++i)
        {
            _id = rand() % (_max - _min + 1) + _min;
            if (s_context.find(_id) == s_context.end())
            {
                break;
            }
        }
        if (_id == 0)
        {
            LOG_P(
                W_LOG_ERROR,
                "could not find random id for fiber server. trace info: %s",
                _trace_info);
            return W_FAILURE;
        }

        {
            std::scoped_lock<std::mutex> _lck(s_context_mutex);
            *pID = &_id;
            //store io context
            s_context[_id] = _io_ctx;
        }

        //run io context
        _io_ctx->run();

        if (_io_ctx->stopped())
        {
            auto _info = s_thread_fiber_info();
            LOG_P(W_LOG_DEBUG, "%s : io context just stopped. trace info: %s", _info.c_str(), _trace_info);
        }

        _t_name = s_thread_names.lookup();
        LOG_P(W_LOG_DEBUG, "thread %s : done. trace info: %s", _t_name.c_str(), _trace_info);
    }
    catch (std::exception const& e)
    {
        _ret = W_FAILURE;
        LOG_P(W_LOG_ERROR, "exception: %s . trace info: %s", e.what(), _trace_info);
    }

    return _ret;
}

W_RESULT fiber_server_stop(_In_ const int pID)
{
    const char* _trace_info = "fiber_server_stop";

    W_RESULT _ret = W_SUCCESS;

    {
        std::scoped_lock<std::mutex> _lck(s_context_mutex);
        auto _iter = s_context.find(pID);
        if (_iter == s_context.end())
        {
            LOG_P(W_LOG_ERROR, "invalid parameters! trace info: %s", _trace_info);
            return (_ret = W_BAD_ARG);
        }

        try
        {
            _iter->second->stop();
            _iter->second.reset();
            s_context.erase(_iter);
        }
        catch (std::exception const& e)
        {
            _ret = W_FAILURE;
            LOG_P(W_LOG_ERROR,
                "could not stop io context with id: %d . error: %s . trace info: %s",
                pID,
                e.what(),
                _trace_info);
        }
    }

    return _ret;
}

W_RESULT fiber_clients_connect(
    _In_ const int pIPV4_OR_IPV6,
    _In_ const char* pEndPoint,
    _In_ const uint16_t pPort,
    _In_ const int pNumberOfClients,
    _In_ w_fiber_client_callback_fn pOnSendReceiveCallback)
{
    const char* _trace_info = "fiber_clients_connect";

    W_RESULT _ret = W_SUCCESS;

    auto _io_ctx = std::make_shared<boost::asio::io_context>();
    boost::fibers::use_scheduling_algorithm<boost::fibers::asio::round_robin>(_io_ctx);

    try
    {
        boost::fibers::barrier _barrier(pNumberOfClients);
        for (auto i = 0; i < pNumberOfClients; ++i)
        {
            boost::fibers::fiber(
                s_client,
                _io_ctx,
                std::ref(_barrier),
                pIPV4_OR_IPV6,
                pEndPoint,
                pPort,
                pOnSendReceiveCallback).detach();
        }

        //run io context
        _io_ctx->run();
    }
    catch (std::exception const& e)
    {
        _ret = W_FAILURE;
        LOG_P(W_LOG_ERROR, "exception: %s . trace info: %s", e.what(), _trace_info);
    }

    return _ret;
}

#endif
