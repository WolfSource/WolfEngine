/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <gtest.hpp> 
#include <system/socket/w_tcp_server.hpp>
#include <system/socket/w_tcp_client.hpp>
#include <system/w_timer.hpp>

TEST(tcp, tcp_server_timeout_test)
{
	const wolf::system::w_leak_detector _detector = {};

	using tcp = boost::asio::ip::tcp;
	using w_tcp_server = wolf::system::socket::w_tcp_server;
	using w_socket_options = wolf::system::socket::w_socket_options;

	auto _io = boost::asio::io_context();
	w_socket_options _opts = {};
	tcp::endpoint _endpoint = { tcp::v4(), 8080 };
	auto timeout = std::chrono::milliseconds(3000);

	auto t1 = std::jthread([&]()
		{
			//stop server
			std::this_thread::sleep_for(std::chrono::seconds(5));
	_io.stop();
		});

	w_tcp_server::init(
		_io,
		std::move(_endpoint),
		std::move(timeout),
		std::move(_opts),
		[](const std::string& p_conn_id, gsl::span<char> p_data, size_t p_read_bytes) -> auto
		{
			std::cout << "tcp server just read: " << p_read_bytes << " bytes" << std::endl;
	return boost::system::errc::connection_aborted;
		},
		[](const std::string& p_conn_id)
		{
			std::cout << "timeout for connection: " << p_conn_id << std::endl;
		});
	_io.run();
}

TEST(tcp, tcp_client_connect_timeout_test)
{
	const wolf::system::w_leak_detector _detector = {};

	using tcp = boost::asio::ip::tcp;
	using w_tcp_client = wolf::system::socket::w_tcp_client;
	using w_socket_options = wolf::system::socket::w_socket_options;
	using w_timer = wolf::system::w_timer;

	auto _io = boost::asio::io_context();

	w_socket_options _opts = {};
	tcp::endpoint _endpoint = { tcp::v4(), 8000 };

	boost::asio::co_spawn(
		_io,
		[&]() -> boost::asio::awaitable<void>
		{
			auto client = w_tcp_client(_io, _opts);
	auto _timer = w_timer(_io);
	_timer.expires_after(std::chrono::nanoseconds(1));

	auto _ret = co_await(
		_timer.async_wait(boost::asio::use_awaitable) || client.async_resolve(_endpoint));
	// expect timeout
	EXPECT_EQ(_ret.index(), 0);

	_timer.cancel();
	_timer.expires_after(std::chrono::seconds(5));
	_ret = co_await(
		_timer.async_wait(boost::asio::use_awaitable) || client.async_resolve("google.com", "443"));
	// expect resolving
	EXPECT_EQ(_ret.index(), 1);

	_io.stop();

	co_return;
		},
		boost::asio::detached);

	_io.run();
}