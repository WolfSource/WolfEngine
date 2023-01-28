#include "w_ocr_engine.h"

#include <catch2/catch_all.hpp>
#include <filesystem>
#include <opencv2/opencv.hpp>

#include "w_image_processor.h"
namespace fs = std::filesystem;
OCR ocr_object;
fs::path ocr_engine_asset_path = "../../../ocr/common_test_asset/ocr_engine";

TEST_CASE("check_if_overlapped with default arguments on overlapped rects",
          "[w_ocr_engine_test.cpp]") {
  cv::Rect rect1(0, 0, 10, 10);
  cv::Rect rect2(0, 0, 10, 10);
  config_for_ocr_struct config_for_ocr_struct;
  CHECK(ocr_object.check_if_overlapped(rect1, rect2, config_for_ocr_struct) ==
        true);
}

TEST_CASE("check_if_overlapped with default arguments on non overlapped rects",
          "[w_ocr_engine_test.cpp]") {
  cv::Rect rect1(0, 0, 10, 10);
  cv::Rect rect2(10, 10, 10, 10);
  config_for_ocr_struct config_for_ocr_struct;
  CHECK(ocr_object.check_if_overlapped(rect1, rect2, config_for_ocr_struct) ==
        false);
}

TEST_CASE("check_if_overlapped with threshold 0.01 ",
          "[w_ocr_engine_test.cpp]") {
  cv::Rect rect1(0, 0, 10, 10);
  cv::Rect rect2(9, 9, 10, 10);
  config_for_ocr_struct config_for_ocr_struct;
  config_for_ocr_struct.overlapped_threshold = 0.01;
  CHECK(ocr_object.check_if_overlapped(rect1, rect2, config_for_ocr_struct) ==
        false);
}

TEST_CASE("check_if_overlapped with threshold 0.009 ",
          "[w_ocr_engine_test.cpp]") {
  cv::Rect rect1(0, 0, 10, 10);
  cv::Rect rect2(9, 9, 10, 10);
  config_for_ocr_struct config_for_ocr_struct;
  config_for_ocr_struct.overlapped_threshold = 0.009;
  CHECK(ocr_object.check_if_overlapped(rect1, rect2, config_for_ocr_struct) ==
        true);
}

TEST_CASE("contours_to_char_structs with default arguments ",
          "[w_ocr_engine_test.cpp]") {
  fs::path desired_image_path =
      ocr_engine_asset_path / "contours_to_char_structs_default.png";
  cv::Mat desired_image_temp = cv::imread(desired_image_path.string());
  cv::Mat desired_image;
  cv::cvtColor(desired_image_temp, desired_image, cv::COLOR_BGR2GRAY);
  std::vector<std::vector<cv::Point>> contours =
      find_all_countors(desired_image);
  std::vector<OCR::characters_struct> characters =
      ocr_object.contours_to_char_structs(contours);
  CHECK(characters.size() == 475);
}

TEST_CASE(
    "contours_to_char_structs with default arguments on empty contour vector ",
    "[w_ocr_engine_test.cpp]") {
  fs::path desired_image_path =
      ocr_engine_asset_path / "contours_to_char_structs_Solid_black.png";
  cv::Mat desired_image_temp = cv::imread(desired_image_path.string());
  cv::Mat desired_image;
  cv::cvtColor(desired_image_temp, desired_image, cv::COLOR_BGR2GRAY);
  std::vector<std::vector<cv::Point>> contours =
      find_all_countors(desired_image);
  std::vector<OCR::characters_struct> characters =
      ocr_object.contours_to_char_structs(contours);
  CHECK(characters.size() == 0);
}

TEST_CASE("enhance_contour_image_for_model with default arguments ",
          "[w_ocr_engine_test.cpp]") {
  fs::path source_image_path = ocr_engine_asset_path / "test_image.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  cv::Mat processed_image = source_image.clone();
  config_for_ocr_struct config_for_ocr_struct;
  ocr_object.enhance_contour_image_for_model(processed_image,
                                             config_for_ocr_struct);
  CHECK((sum(source_image != processed_image) == cv::Scalar(0, 0, 0, 0)));
}

TEST_CASE("enhance_contour_image_for_model with resize ",
          "[w_ocr_engine_test.cpp]") {
  fs::path source_image_path = ocr_engine_asset_path / "test_image.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  fs::path desired_image_path =
      ocr_engine_asset_path / "enhance_contour_image_for_model_resize.png";
  cv::Mat desired_image = cv::imread(desired_image_path.string());
  config_for_ocr_struct config_for_ocr_struct;
  config_for_ocr_struct.do_resize_contour = true;
  ocr_object.enhance_contour_image_for_model(source_image,
                                             config_for_ocr_struct);
  CHECK((sum(source_image != desired_image) == cv::Scalar(0, 0, 0, 0)));
}

TEST_CASE("enhance_contour_image_for_model with make_white ",
          "[w_ocr_engine_test.cpp]") {
  fs::path source_image_path = ocr_engine_asset_path / "test_image.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  fs::path desired_image_path =
      ocr_engine_asset_path / "enhance_contour_image_for_model_make_white.png";
  cv::Mat desired_image = cv::imread(desired_image_path.string());
  config_for_ocr_struct config_for_ocr_struct;
  config_for_ocr_struct.make_white_background = true;
  ocr_object.enhance_contour_image_for_model(source_image,
                                             config_for_ocr_struct);
  CHECK((sum(source_image != desired_image) == cv::Scalar(0, 0, 0, 0)));
}

TEST_CASE("euclidean_distance with two non overlapped chars ",
          "[w_ocr_engine_test.cpp]") {
  OCR::characters_struct char1;
  OCR::characters_struct char2;
  char1.center.x = 1;
  char1.center.y = 1;
  char2.center.x = 1;
  char2.center.y = 2;
  double distance = ocr_object.euclidean_distance(char1, char2);
  CHECK(distance == 1);
}

TEST_CASE("euclidean_distance with two overlapped chars ",
          "[w_ocr_engine_test.cpp]") {
  OCR::characters_struct char1;
  OCR::characters_struct char2;
  char1.center.x = 1;
  char1.center.y = 1;
  char2.center.x = 1;
  char2.center.y = 1;
  double distance = ocr_object.euclidean_distance(char1, char2);
  CHECK(distance == 0);
}

TEST_CASE("euclidean_distance with two empty chars ",
          "[w_ocr_engine_test.cpp]") {
  OCR::characters_struct char1;
  OCR::characters_struct char2;
  double distance = ocr_object.euclidean_distance(char1, char2);
  CHECK(distance == 0);
}

TEST_CASE("filter_chars_by_contour_size without restriction ",
          "[w_ocr_engine_test.cpp]") {
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
  std::vector<OCR::characters_struct> characters =
      ocr_object.contours_to_char_structs(contours);
  std::vector<OCR::characters_struct> filtered_characters =
      ocr_object.filter_chars_by_contour_size(characters,
                                              config_for_ocr_struct);
  CHECK(filtered_characters.size() == 3);
}

TEST_CASE("filter_chars_by_contour_size by area restriction ",
          "[w_ocr_engine_test.cpp]") {
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
  std::vector<OCR::characters_struct> characters =
      ocr_object.contours_to_char_structs(contours);
  std::vector<OCR::characters_struct> filtered_characters =
      ocr_object.filter_chars_by_contour_size(characters,
                                              config_for_ocr_struct);
  CHECK(filtered_characters.size() == 2);
}

TEST_CASE("filter_chars_by_contour_size by width restriction ",
          "[w_ocr_engine_test.cpp]") {
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
  std::vector<OCR::characters_struct> characters =
      ocr_object.contours_to_char_structs(contours);
  std::vector<OCR::characters_struct> filtered_characters =
      ocr_object.filter_chars_by_contour_size(characters,
                                              config_for_ocr_struct);
  CHECK(filtered_characters.size() == 1);
}

TEST_CASE("merge_overlapped_contours on two overlapped contours ",
          "[w_ocr_engine_test.cpp]") {
  config_for_ocr_struct config_for_ocr_struct;
  OCR::characters_struct char1;
  char1.bound_rect.x = 0;
  char1.bound_rect.y = 0;
  char1.bound_rect.width = 10;
  char1.bound_rect.height = 10;
  OCR::characters_struct char2;
  char2.bound_rect.x = 0;
  char2.bound_rect.y = 0;
  char2.bound_rect.width = 10;
  char2.bound_rect.height = 10;
  std::vector<OCR::characters_struct> characters;
  characters.push_back(char1);
  characters.push_back(char2);
  ocr_object.merge_overlapped_contours(characters, config_for_ocr_struct);
  CHECK(characters.size() == 1);
}

TEST_CASE("merge_overlapped_contours on two non overlapped contours ",
          "[w_ocr_engine_test.cpp]") {
  config_for_ocr_struct config_for_ocr_struct;
  OCR::characters_struct char1;
  char1.bound_rect.x = 0;
  char1.bound_rect.y = 0;
  char1.bound_rect.width = 10;
  char1.bound_rect.height = 10;
  OCR::characters_struct char2;
  char2.bound_rect.x = 0;
  char2.bound_rect.y = 10;
  char2.bound_rect.width = 10;
  char2.bound_rect.height = 10;
  std::vector<OCR::characters_struct> characters;
  characters.push_back(char1);
  characters.push_back(char2);
  ocr_object.merge_overlapped_contours(characters, config_for_ocr_struct);
  CHECK(characters.size() == 2);
}

TEST_CASE("mask_contour works", "[w_ocr_engine_test.cpp]") {
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
  std::vector<OCR::characters_struct> characters =
      ocr_object.contours_to_char_structs(contours);
  cv::Mat processed = ocr_object.mask_contour(source_image, characters[1]);
  CHECK((sum(processed != desired_image) == cv::Scalar(0, 0, 0, 0)));
}
