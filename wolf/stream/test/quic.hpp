#if defined(WOLF_TEST) && defined(WOLF_STREAM_QUIC)

#pragma once

#include <wolf.hpp>
#include <system/w_leak_detector.hpp>
#include <boost/test/included/unit_test.hpp>

#include <stream/quic/w_quic.hpp>

BOOST_AUTO_TEST_CASE(quic)
{
    std::cout << "entering test case 'quic'" << std::endl;

    const wolf::system::w_leak_detector detector{};

    BOOST_REQUIRE(foo());

    std::cout << "leaving test case 'quic'" << std::endl;
}

#endif  // defined(WOLF_TEST) && defined(WOLF_STREAM_QUIC)
