/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#if defined(WOLF_TEST) && defined(WOLF_MEDIA_STB)

#pragma once

#include <boost/test/included/unit_test.hpp>
#include <system/w_leak_detector.hpp>

#include <media/w_image.hpp>

BOOST_AUTO_TEST_CASE(image_load_save_test) {
  const wolf::system::w_leak_detector _detector = {};

  std::cout << "image_load_save_test just done" << std::endl;

  boost::leaf::try_handle_all(
      [&]() -> boost::leaf::result<void> {
        using w_image = wolf::media::w_image;
        using w_image_data = wolf::media::w_image_data;

        auto _rgb_file = std::filesystem::current_path().append(
            "../../content/texture/rgb.png");
        BOOST_LEAF_AUTO(_src_image, w_image::load(_rgb_file, 0));

        const auto _bmp_path =
            std::filesystem::current_path().append("/rgb_bmp.png");
        BOOST_LEAF_CHECK(w_image::save_bmp(_bmp_path, _src_image));

        constexpr auto _quality = 90;
        const auto _jpg_path =
            std::filesystem::current_path().append("/rgb_jpg.png");
        BOOST_LEAF_CHECK(w_image::save_jpg(_jpg_path, _src_image, _quality));

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

  std::cout << "image_load_save_test just done" << std::endl;
}

#endif