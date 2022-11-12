/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <gtest.hpp> 
#include <system/socket/w_tcp_server.hpp>

TEST(tcp, tcp_server_timeout_test)
{
	const wolf::system::w_leak_detector _detector = {};

	using namespace wolf::system::socket;
	using namespace boost::asio;
	using tcp = boost::asio::ip::tcp;

	auto _io = boost::asio::io_context();
	w_socket_options _opts = {};
	_opts.timeout = std::chrono::milliseconds(3000);

	tcp::endpoint _endpoint = { tcp::v4(), 8080 };

	auto t1 = std::jthread([&]()
		{
			//stop server
			std::this_thread::sleep_for(std::chrono::seconds(5));
	_io.stop();
		});

	w_tcp_server::init(
		_io,
		std::move(_endpoint),
		std::move(_opts),
		[](const std::string& p_conn_id, gsl::span<char> p_data, size_t p_read_bytes) -> auto
		{
			EXPECT_TRUE(false) << "tcp server just read: " << p_read_bytes << " bytes" << std::endl;
	return boost::system::errc::connection_aborted;
		},
		[](const std::string& p_conn_id)
		{
			EXPECT_TRUE(false) << "timeout for connection: " << p_conn_id << std::endl;
		});
	_io.run();
}
