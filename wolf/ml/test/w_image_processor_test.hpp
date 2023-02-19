#ifdef WOLF_TEST

#pragma once

#ifdef WOLF_ML_OCR

#define BOOST_TEST_MODULE referee_ocr_image_processor

#include <ml/referee_ocr/w_image_processor.hpp>

#include <boost/test/included/unit_test.hpp>
#include <filesystem>
#include <opencv2/opencv.hpp>

namespace fs = std::filesystem;
fs::path image_processor_path = "../ml/test/common_test_asset/image_processor";

using namespace wolf::ml::ocr;

BOOST_AUTO_TEST_CASE(prepare_image_for_contour_detection_with_default_arguments) {
  fs::path source_image_path = image_processor_path / "test_image.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  fs::path desired_image_path =
      image_processor_path / "prepare_image_for_contour_detection_default.png";
  cv::Mat desired_image_temp = cv::imread(desired_image_path.string());
  cv::Mat desired_image;
  cv::cvtColor(desired_image_temp, desired_image, cv::COLOR_BGR2GRAY);
  config_for_ocr_struct config_for_ocr_struct;
  cv::Mat processed_image =
      prepare_image_for_contour_detection(source_image, config_for_ocr_struct);
  cv::Mat diff = processed_image != desired_image;
  BOOST_TEST(cv::countNonZero(diff) == 0);
}

BOOST_AUTO_TEST_CASE(negative_image_with_default_arguments) {
  fs::path source_image_path = image_processor_path / "test_image.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  fs::path desired_image_path =
      image_processor_path / "negative_image_default.png";
  cv::Mat desired_image = cv::imread(desired_image_path.string());
  negative_image(source_image);
  BOOST_TEST((sum(source_image != desired_image) == cv::Scalar(0, 0, 0, 0)));
}

BOOST_AUTO_TEST_CASE(make_white_background_with_default_arguments) {
  fs::path source_image_path = image_processor_path / "test_image.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  fs::path desired_image_temp =
      image_processor_path / "make_white_background_default.png";
  cv::Mat desired_image = cv::imread(desired_image_temp.string());
  config_for_ocr_struct config_for_ocr_struct;
  config_for_ocr_struct.make_white_background = true;
  make_contour_white_background(source_image, config_for_ocr_struct);
  BOOST_TEST((sum(source_image != desired_image) == cv::Scalar(0, 0, 0, 0)));
}

BOOST_AUTO_TEST_CASE(make_white_background_with_threshold_50) {
  fs::path source_image_path = image_processor_path / "test_image.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  fs::path desired_image_temp =
      image_processor_path / "make_white_background_50.png";
  cv::Mat desired_image = cv::imread(desired_image_temp.string());
  config_for_ocr_struct config_for_ocr_struct;
  config_for_ocr_struct.make_white_background = true;
  config_for_ocr_struct.white_background_threshold = 50;
  make_contour_white_background(source_image, config_for_ocr_struct);
  BOOST_TEST((sum(source_image != desired_image) == cv::Scalar(0, 0, 0, 0)));
}

BOOST_AUTO_TEST_CASE(find_all_contours_on_image_with_contours) {
  fs::path source_image_path = image_processor_path / "test_image.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  config_for_ocr_struct config_for_ocr_struct;
  cv::Mat filtered_image =
      prepare_image_for_contour_detection(source_image, config_for_ocr_struct);
  std::vector<std::vector<cv::Point>> contours =
      find_all_countors(filtered_image);
  BOOST_TEST(contours.size() == 475);
}

BOOST_AUTO_TEST_CASE(find_all_contours_on_image_without_contours) {
  fs::path source_image_path =
      image_processor_path / "find_all_contours_solid_black.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  config_for_ocr_struct config_for_ocr_struct;
  cv::Mat filtered_image =
      prepare_image_for_contour_detection(source_image, config_for_ocr_struct);
  std::vector<std::vector<cv::Point>> contours =
      find_all_countors(filtered_image);
  BOOST_TEST(contours.size() == 0);
}

BOOST_AUTO_TEST_CASE(resize_function_with_only_height_argument) {
  fs::path source_image_path = image_processor_path / "test_image.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  resize_image(source_image, 40);
  BOOST_TEST(source_image.rows == 40);
}

BOOST_AUTO_TEST_CASE(resize_function_with_only_width_argument) {
  fs::path source_image_path = image_processor_path / "test_image.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  resize_image(source_image, -1, 60);
  BOOST_TEST(source_image.cols == 60);
}

BOOST_AUTO_TEST_CASE(resize_function_without_argument) {
  fs::path source_image_path = image_processor_path / "test_image.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  int cols = source_image.cols;
  int rows = source_image.rows;
  resize_image(source_image);
  BOOST_TEST(source_image.cols == cols);
  BOOST_TEST(source_image.rows == rows);
}

BOOST_AUTO_TEST_CASE(gaussian_filter_with_default_argument) {
  fs::path source_image_path = image_processor_path / "test_image.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  fs::path desired_image_path =
      image_processor_path / "gaussian_filter_default.png";
  cv::Mat desired_image = cv::imread(desired_image_path.string());
  config_for_ocr_struct config_for_ocr_struct;
  gaussian_blur(source_image, config_for_ocr_struct);
  BOOST_TEST((sum(source_image != desired_image) == cv::Scalar(0, 0, 0, 0)));
}

BOOST_AUTO_TEST_CASE(gaussian_filter_with_win_size_7) {
  fs::path source_image_path = image_processor_path / "test_image.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  fs::path desired_image_path = image_processor_path / "gaussian_filter_7.png";
  cv::Mat desired_image = cv::imread(desired_image_path.string());
  config_for_ocr_struct config_for_ocr_struct;
  config_for_ocr_struct.gaussian_blur_win_size = 7;
  gaussian_blur(source_image, config_for_ocr_struct);
  BOOST_TEST((sum(source_image != desired_image) == cv::Scalar(0, 0, 0, 0)));
}

BOOST_AUTO_TEST_CASE(threshold_filter_with_default_argument) {
  fs::path source_image_path = image_processor_path / "test_image.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  fs::path desired_image_path =
      image_processor_path / "threshold_filter_default.png";
  cv::Mat desired_image_temp = cv::imread(desired_image_path.string());
  cv::Mat desired_image;
  cv::cvtColor(desired_image_temp, desired_image, cv::COLOR_BGR2GRAY);
  config_for_ocr_struct config_for_ocr_struct;
  threshold_image(source_image, config_for_ocr_struct);
  cv::Mat diff = source_image != desired_image;
  BOOST_TEST(cv::countNonZero(diff) == 0);
}

BOOST_AUTO_TEST_CASE(threshold_filter_with_threshold_value_180) {
  fs::path source_image_path = image_processor_path / "test_image.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  fs::path desired_image_path =
      image_processor_path / "threshold_filter_180.png";
  cv::Mat desired_image_temp = cv::imread(desired_image_path.string());
  cv::Mat desired_image;
  cv::cvtColor(desired_image_temp, desired_image, cv::COLOR_BGR2GRAY);
  config_for_ocr_struct config_for_ocr_struct;
  config_for_ocr_struct.threshold_value = 180;
  threshold_image(source_image, config_for_ocr_struct);
  cv::Mat diff = desired_image != desired_image;
  BOOST_TEST(cv::countNonZero(diff) == 0);
}

#endif // WOLF_ML_OCR

#endif // WOLF_TEST
