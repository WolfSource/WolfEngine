/*
	Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
	https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <DISABLE_ANALYSIS_BEGIN>
#include <catch2/catch.hpp>
#include <DISABLE_ANALYSIS_END>

#include "w_tcp_server.hpp" 

TEST_CASE("wolf::system::socket::w_tcp_server", "w_tcp_server")
{
	wolf::system::w_leak_detector _detector = {};

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
		[](gsl::span<char> p_data, size_t p_read_bytes) -> boost::leaf::result<void>
		{
			std::cout << "read: " << p_read_bytes << std::endl;
			return boost::leaf::new_error(boost::system::errc::connection_aborted);
		});
	_io.run();
}