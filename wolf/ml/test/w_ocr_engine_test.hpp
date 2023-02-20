#ifdef WOLF_TEST

#pragma once

#ifdef WOLF_ML_OCR

#define BOOST_TEST_MODULE ml_ocr_engine

#include <ml/referee_ocr/w_ocr_engine.hpp>

#include <boost/test/included/unit_test.hpp>
#include <filesystem>
#include <opencv2/opencv.hpp>

#include <ml/referee_ocr/w_image_processor.hpp>

using namespace wolf::ml::ocr;

namespace fs = std::filesystem;
w_ocr_engine ocr_object;
fs::path ocr_engine_asset_path = "../ml/test/common_test_asset/ocr_engine";

BOOST_AUTO_TEST_CASE(check_if_overlapped_with_default_arguments_on_overlapped_rects) {
  cv::Rect rect1(0, 0, 10, 10);
  cv::Rect rect2(0, 0, 10, 10);
  config_for_ocr_struct config_for_ocr_struct;
  BOOST_TEST(ocr_object.check_if_overlapped(rect1, rect2, config_for_ocr_struct) ==
        true);
}

BOOST_AUTO_TEST_CASE(check_if_overlapped_with_default_arguments_on_non_overlapped_rects) {
  cv::Rect rect1(0, 0, 10, 10);
  cv::Rect rect2(10, 10, 10, 10);
  config_for_ocr_struct config_for_ocr_struct;
  BOOST_TEST(ocr_object.check_if_overlapped(rect1, rect2, config_for_ocr_struct) ==
        false);
}

BOOST_AUTO_TEST_CASE(check_if_overlapped_with_threshold_0_01) {
  cv::Rect rect1(0, 0, 10, 10);
  cv::Rect rect2(9, 9, 10, 10);
  config_for_ocr_struct config_for_ocr_struct;
  config_for_ocr_struct.overlapped_threshold = 0.01;
  BOOST_TEST(ocr_object.check_if_overlapped(rect1, rect2, config_for_ocr_struct) ==
        false);
}

BOOST_AUTO_TEST_CASE(check_if_overlapped_with_threshold_0_009) {
  cv::Rect rect1(0, 0, 10, 10);
  cv::Rect rect2(9, 9, 10, 10);
  config_for_ocr_struct config_for_ocr_struct;
  config_for_ocr_struct.overlapped_threshold = 0.009;
  BOOST_TEST(ocr_object.check_if_overlapped(rect1, rect2, config_for_ocr_struct) ==
        true);
}

BOOST_AUTO_TEST_CASE(contours_to_char_structs_with_default_arguments) {
  fs::path desired_image_path =
      ocr_engine_asset_path / "contours_to_char_structs_default.png";
  cv::Mat desired_image_temp = cv::imread(desired_image_path.string());
  cv::Mat desired_image;
  cv::cvtColor(desired_image_temp, desired_image, cv::COLOR_BGR2GRAY);
  std::vector<std::vector<cv::Point>> contours =
      find_all_countors(desired_image);
  std::vector<w_ocr_engine::characters_struct> characters =
      ocr_object.contours_to_char_structs(contours);
  BOOST_TEST(characters.size() == 475);
}

BOOST_AUTO_TEST_CASE(contours_to_char_structs_with_default_arguments_on_empty_contour_vector) {
  fs::path desired_image_path =
      ocr_engine_asset_path / "contours_to_char_structs_Solid_black.png";
  cv::Mat desired_image_temp = cv::imread(desired_image_path.string());
  cv::Mat desired_image;
  cv::cvtColor(desired_image_temp, desired_image, cv::COLOR_BGR2GRAY);
  std::vector<std::vector<cv::Point>> contours =
      find_all_countors(desired_image);
  std::vector<w_ocr_engine::characters_struct> characters =
      ocr_object.contours_to_char_structs(contours);
  BOOST_TEST(characters.size() == 0);
}

BOOST_AUTO_TEST_CASE(enhance_contour_image_for_modelwithdefaultarguments) {
  fs::path source_image_path = ocr_engine_asset_path / "test_image.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  cv::Mat processed_image = source_image.clone();
  config_for_ocr_struct config_for_ocr_struct;
  ocr_object.enhance_contour_image_for_model(processed_image,
                                             config_for_ocr_struct);
  BOOST_TEST((sum(source_image != processed_image) == cv::Scalar(0, 0, 0, 0)));
}

BOOST_AUTO_TEST_CASE(enhance_contour_image_for_model_with_resize) {
  fs::path source_image_path = ocr_engine_asset_path / "test_image.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  fs::path desired_image_path =
      ocr_engine_asset_path / "enhance_contour_image_for_model_resize.png";
  cv::Mat desired_image = cv::imread(desired_image_path.string());
  config_for_ocr_struct config_for_ocr_struct;
  config_for_ocr_struct.do_resize_contour = true;
  ocr_object.enhance_contour_image_for_model(source_image,
                                             config_for_ocr_struct);
  BOOST_TEST((sum(source_image != desired_image) == cv::Scalar(0, 0, 0, 0)));
}

BOOST_AUTO_TEST_CASE(enhance_contour_image_for_modelwithmake_white) {
  fs::path source_image_path = ocr_engine_asset_path / "test_image.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  fs::path desired_image_path =
      ocr_engine_asset_path / "enhance_contour_image_for_model_make_white.png";
  cv::Mat desired_image = cv::imread(desired_image_path.string());
  config_for_ocr_struct config_for_ocr_struct;
  config_for_ocr_struct.make_white_background = true;
  ocr_object.enhance_contour_image_for_model(source_image,
                                             config_for_ocr_struct);
  BOOST_TEST((sum(source_image != desired_image) == cv::Scalar(0, 0, 0, 0)));
}

BOOST_AUTO_TEST_CASE(euclidean_distance_with_two_non_overlapped_chars) {
  w_ocr_engine::characters_struct char1;
  w_ocr_engine::characters_struct char2;
  char1.center.x = 1;
  char1.center.y = 1;
  char2.center.x = 1;
  char2.center.y = 2;
  double distance = ocr_object.euclidean_distance(char1, char2);
  BOOST_TEST(distance == 1);
}

BOOST_AUTO_TEST_CASE(euclidean_distance_with_two_overlapped_chars) {
  w_ocr_engine::characters_struct char1;
  w_ocr_engine::characters_struct char2;
  char1.center.x = 1;
  char1.center.y = 1;
  char2.center.x = 1;
  char2.center.y = 1;
  double distance = ocr_object.euclidean_distance(char1, char2);
  BOOST_TEST(distance == 0);
}

BOOST_AUTO_TEST_CASE(euclidean_distance_with_two_empty_chars) {
  w_ocr_engine::characters_struct char1;
  w_ocr_engine::characters_struct char2;
  double distance = ocr_object.euclidean_distance(char1, char2);
  BOOST_TEST(distance == 0);
}

BOOST_AUTO_TEST_CASE(filter_chars_by_contour_size_without_restriction) {
  fs::path source_image_path =
      ocr_engine_asset_path / "filter_chars_by_contour_size.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  config_for_ocr_struct config_for_ocr_struct;
  config_for_ocr_struct.restrictions.max_area = 99999999;
  config_for_ocr_struct.restrictions.min_area = 1;
  config_for_ocr_struct.restrictions.max_height = 99999999;
  config_for_ocr_struct.restrictions.min_height = 1;
  config_for_ocr_struct.restrictions.max_width = 99999999;
  config_for_ocr_struct.restrictions.min_width = 1;
  cv::Mat prepared =
      prepare_image_for_contour_detection(source_image, config_for_ocr_struct);
  std::vector<std::vector<cv::Point>> contours = find_all_countors(prepared);
  std::vector<w_ocr_engine::characters_struct> characters =
      ocr_object.contours_to_char_structs(contours);
  std::vector<w_ocr_engine::characters_struct> filtered_characters =
      ocr_object.filter_chars_by_contour_size(characters,
                                              config_for_ocr_struct);
  BOOST_TEST(filtered_characters.size() == 3);
}

BOOST_AUTO_TEST_CASE(filter_chars_by_contour_size_by_area_restriction) {
  fs::path source_image_path =
      ocr_engine_asset_path / "filter_chars_by_contour_size.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  config_for_ocr_struct config_for_ocr_struct;
  config_for_ocr_struct.restrictions.max_area = 500000;
  config_for_ocr_struct.restrictions.min_area = 1;
  config_for_ocr_struct.restrictions.max_height = 99999999;
  config_for_ocr_struct.restrictions.min_height = 1;
  config_for_ocr_struct.restrictions.max_width = 99999999;
  config_for_ocr_struct.restrictions.min_width = 1;
  cv::Mat prepared =
      prepare_image_for_contour_detection(source_image, config_for_ocr_struct);
  std::vector<std::vector<cv::Point>> contours = find_all_countors(prepared);
  std::vector<w_ocr_engine::characters_struct> characters =
      ocr_object.contours_to_char_structs(contours);
  std::vector<w_ocr_engine::characters_struct> filtered_characters =
      ocr_object.filter_chars_by_contour_size(characters,
                                              config_for_ocr_struct);
  BOOST_TEST(filtered_characters.size() == 2);
}

BOOST_AUTO_TEST_CASE(filter_chars_by_contour_size_by_width_restriction) {
  fs::path source_image_path =
      ocr_engine_asset_path / "filter_chars_by_contour_size.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  config_for_ocr_struct config_for_ocr_struct;
  config_for_ocr_struct.restrictions.max_area = 500000;
  config_for_ocr_struct.restrictions.min_area = 1;
  config_for_ocr_struct.restrictions.max_height = 99999999;
  config_for_ocr_struct.restrictions.min_height = 1;
  config_for_ocr_struct.restrictions.max_width = 300;
  config_for_ocr_struct.restrictions.min_width = 1;
  cv::Mat prepared =
      prepare_image_for_contour_detection(source_image, config_for_ocr_struct);
  std::vector<std::vector<cv::Point>> contours = find_all_countors(prepared);
  std::vector<w_ocr_engine::characters_struct> characters =
      ocr_object.contours_to_char_structs(contours);
  std::vector<w_ocr_engine::characters_struct> filtered_characters =
      ocr_object.filter_chars_by_contour_size(characters,
                                              config_for_ocr_struct);
  BOOST_TEST(filtered_characters.size() == 1);
}

BOOST_AUTO_TEST_CASE(merge_overlapped_contours_on_two_overlapped_contours) {
  config_for_ocr_struct config_for_ocr_struct;
  w_ocr_engine::characters_struct char1;
  char1.bound_rect.x = 0;
  char1.bound_rect.y = 0;
  char1.bound_rect.width = 10;
  char1.bound_rect.height = 10;
  w_ocr_engine::characters_struct char2;
  char2.bound_rect.x = 0;
  char2.bound_rect.y = 0;
  char2.bound_rect.width = 10;
  char2.bound_rect.height = 10;
  std::vector<w_ocr_engine::characters_struct> characters;
  characters.push_back(char1);
  characters.push_back(char2);
  ocr_object.merge_overlapped_contours(characters, config_for_ocr_struct);
  BOOST_TEST(characters.size() == 1);
}

BOOST_AUTO_TEST_CASE(merge_overlapped_contours_on_two_non_overlapped_contours) {
  config_for_ocr_struct config_for_ocr_struct;
  w_ocr_engine::characters_struct char1;
  char1.bound_rect.x = 0;
  char1.bound_rect.y = 0;
  char1.bound_rect.width = 10;
  char1.bound_rect.height = 10;
  w_ocr_engine::characters_struct char2;
  char2.bound_rect.x = 0;
  char2.bound_rect.y = 10;
  char2.bound_rect.width = 10;
  char2.bound_rect.height = 10;
  std::vector<w_ocr_engine::characters_struct> characters;
  characters.push_back(char1);
  characters.push_back(char2);
  ocr_object.merge_overlapped_contours(characters, config_for_ocr_struct);
  BOOST_TEST(characters.size() == 2);
}

BOOST_AUTO_TEST_CASE(mask_contour_works) {
  fs::path contours_image_path =
      ocr_engine_asset_path / "filter_chars_by_contour_size.png";
  fs::path source_image_path = ocr_engine_asset_path / "test_image.png";
  fs::path desired_image_path = ocr_engine_asset_path / "mask_contour.png";
  cv::Mat contours_image = cv::imread(contours_image_path.string());
  cv::Mat source_image = cv::imread(source_image_path.string());
  cv::Mat desired_image = cv::imread(desired_image_path.string());
  config_for_ocr_struct config_for_ocr_struct;
  config_for_ocr_struct.restrictions.max_area = 500000;
  cv::Mat prepared = prepare_image_for_contour_detection(contours_image,
                                                         config_for_ocr_struct);
  std::vector<std::vector<cv::Point>> contours = find_all_countors(prepared);
  std::vector<w_ocr_engine::characters_struct> characters =
      ocr_object.contours_to_char_structs(contours);
  cv::Mat processed = ocr_object.mask_contour(source_image, characters[1]);
  BOOST_TEST((sum(processed != desired_image) == cv::Scalar(0, 0, 0, 0)));
}

#endif // WOLF_ML_OCR

#endif // WOLF_TEST
