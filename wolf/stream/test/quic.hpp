#if defined(WOLF_TEST) && defined(WOLF_STREAM_QUIC)

#pragma once

#include <wolf.hpp>
#include <system/w_leak_detector.hpp>
#include <boost/test/included/unit_test.hpp>

#include <stream/quic/w_quic.hpp>

namespace quic = wolf::stream::quic;

BOOST_AUTO_TEST_CASE(msquic_base_types) {
    BOOST_REQUIRE(quic::w_status(quic::w_status_code::Success).succeeded());
    BOOST_REQUIRE(quic::w_status(quic::w_status_code::Aborted).failed());

    {
        auto status_code = quic::w_status_code::Success;
        auto status = quic::w_status(status_code);
        BOOST_REQUIRE(quic::status_to_str(status_code) == quic::status_to_str(status));
    }
}

#endif  // defined(WOLF_TEST) && defined(WOLF_STREAM_QUIC)
