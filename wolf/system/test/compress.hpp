/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#if defined(WOLF_TESTS)

#pragma once

#include <boost/test/included/unit_test.hpp>
#include <system/w_leak_detector.hpp>
#include <wolf.hpp>

#include <system/compression/w_lz4.hpp>
#include <system/compression/w_lzma.hpp>

#ifdef WOLF_SYSTEM_LZ4

BOOST_AUTO_TEST_CASE(compress_lz4) {
  const wolf::system::w_leak_detector _detector = {};

  boost::leaf::try_handle_all(
      [&]() -> boost::leaf::result<void> {
        using lz4 = wolf::system::compression::w_lz4;

        const auto _mock_compression_data = gsl::ensure_z(
            "HELLO WOLF\r\nHELLO WOLF!*&%!HELLO WOLF!07*&%!\r\nThe quick "
            "brown fox jumps over the lazy dog!");

        const auto _bytes = gsl::as_bytes(_mock_compression_data);
        const auto _bytes_len = _bytes.size();

        constexpr auto _max_retry = 10;
        BOOST_LEAF_AUTO(_compress_default, lz4::compress_default(_bytes));

        BOOST_LEAF_AUTO(_decompress_default,
                        lz4::decompress(_compress_default, _max_retry));
        BOOST_REQUIRE_EQUAL(_decompress_default.size(), _bytes_len);

        constexpr auto _acceleration = 1000;
        BOOST_LEAF_AUTO(_compress_fast,
                        lz4::compress_fast(_bytes, _acceleration));

        BOOST_LEAF_AUTO(_decompress_fast,
                        lz4::decompress(_compress_fast, _max_retry));
        BOOST_REQUIRE_EQUAL(_decompress_fast.size(), _bytes_len);

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

  boost::leaf::try_handle_all(
      [&]() -> boost::leaf::result<void> {
        using lzma = wolf::system::compression::w_lzma;

        const auto _mock_compression_data = gsl::ensure_z(
            "HELLO WOLF\r\nHELLO WOLF!*&%!HELLO WOLF!07*&%!\r\nThe quick "
            "brown fox jumps over the lazy dog!");

        BOOST_LEAF_AUTO(
            _compress_lzm1,
            lzma::compress_lzma1(gsl::as_bytes(_mock_compression_data), 7));

        BOOST_LEAF_AUTO(_decompress_lzm1,
                        lzma::decompress_lzma1(_compress_lzm1));

        BOOST_LEAF_AUTO(
            _compress_lzm2,
            lzma::compress_lzma2(gsl::as_bytes(_mock_compression_data), 7));

        BOOST_LEAF_AUTO(_decompress_lzm2,
                        lzma::decompress_lzma2(_compress_lzm2));

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

#endif //WOLF_SYSTEM_LZMA

#endif // WOLF_TESTS