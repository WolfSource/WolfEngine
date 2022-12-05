/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#if defined(WOLF_TESTS)

#pragma once

#ifdef WOLF_SYSTEM_LZ4

#include <boost/test/included/unit_test.hpp>
#include <system/w_leak_detector.hpp>
#include <wolf.hpp>

#include <system/compression/w_lz4.hpp>

BOOST_AUTO_TEST_CASE(compress_lz4) {
  const wolf::system::w_leak_detector _detector = {};

  boost::leaf::try_handle_all(
      [&]() -> boost::leaf::result<void> {
        using lz4 = wolf::system::compression::w_lz4;

        const auto _mock_compression_data = gsl::ensure_z(
            "HELLO WOLF\r\nHELLO WOLF!*&%!HELLO WOLF!07*&%!\r\nThe quick "
            "brown fox jumps over the lazy dog!");

        constexpr auto _max_retry = 10;
        BOOST_LEAF_AUTO(_compress_default,
                        lz4::compress_default(_mock_compression_data));

        BOOST_LEAF_AUTO(_decompress_default,
                        lz4::decompress(_compress_default, _max_retry));
        BOOST_REQUIRE_EQUAL(_decompress_default.size(),
                            _mock_compression_data.size());

        constexpr auto _acceleration = 1000;
        BOOST_LEAF_AUTO(
            _compress_fast,
            lz4::compress_fast(_mock_compression_data, _acceleration));

        BOOST_LEAF_AUTO(_decompress_fast,
                        lz4::decompress(_compress_fast, _max_retry));
        BOOST_REQUIRE_EQUAL(_decompress_fast.size(),
                            _mock_compression_data.size());

        return {};
      },
      [](const w_trace &p_trace) {
        std::cout << "got error :" << p_trace << std::endl;
        BOOST_REQUIRE(false);
      },
      [] {
        std::cout << "got an error!" << std::endl;
        BOOST_ERROR(false);
      });
}

#endif //WOLF_SYSTEM_LZ4

#ifdef WOLF_SYSTEM_LZMA

BOOST_AUTO_TEST_CASE(compress_lzma) {
  const wolf::system::w_leak_detector _detector = {};
}

#endif //WOLF_SYSTEM_LZMA

#endif // WOLF_TESTS