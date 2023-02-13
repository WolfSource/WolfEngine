#include "w_image_processor.h"

#include <catch2/catch_all.hpp>
#include <filesystem>
#include <opencv2/opencv.hpp>

namespace fs = std::filesystem;
fs::path image_processor_path =
    "../../../ocr/common_test_asset/image_processor";

TEST_CASE("prepare_image_for_contour_detection with default arguments",
          "[w_image_processor_test.cpp]") {
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
  CHECK(cv::countNonZero(diff) == 0);
}

TEST_CASE("negative_image with default arguments",
          "[w_image_processor_test.cpp]") {
  fs::path source_image_path = image_processor_path / "test_image.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  fs::path desired_image_path =
      image_processor_path / "negative_image_default.png";
  cv::Mat desired_image = cv::imread(desired_image_path.string());
  negative_image(source_image);
  CHECK((sum(source_image != desired_image) == cv::Scalar(0, 0, 0, 0)));
}

TEST_CASE("make_white_background with default arguments",
          "[w_image_processor_test.cpp]") {
  fs::path source_image_path = image_processor_path / "test_image.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  fs::path desired_image_temp =
      image_processor_path / "make_white_background_default.png";
  cv::Mat desired_image = cv::imread(desired_image_temp.string());
  config_for_ocr_struct config_for_ocr_struct;
  config_for_ocr_struct.make_white_background = true;
  make_contour_white_background(source_image, config_for_ocr_struct);
  CHECK((sum(source_image != desired_image) == cv::Scalar(0, 0, 0, 0)));
}

TEST_CASE("make_white_background with threshold 50",
          "[w_image_processor_test.cpp]") {
  fs::path source_image_path = image_processor_path / "test_image.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  fs::path desired_image_temp =
      image_processor_path / "make_white_background_50.png";
  cv::Mat desired_image = cv::imread(desired_image_temp.string());
  config_for_ocr_struct config_for_ocr_struct;
  config_for_ocr_struct.make_white_background = true;
  config_for_ocr_struct.white_background_threshold = 50;
  make_contour_white_background(source_image, config_for_ocr_struct);
  CHECK((sum(source_image != desired_image) == cv::Scalar(0, 0, 0, 0)));
}

TEST_CASE("find_all_contours on image with contours",
          "[w_image_processor_test.cpp]") {
  fs::path source_image_path = image_processor_path / "test_image.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  config_for_ocr_struct config_for_ocr_struct;
  cv::Mat filtered_image =
      prepare_image_for_contour_detection(source_image, config_for_ocr_struct);
  std::vector<std::vector<cv::Point>> contours =
      find_all_countors(filtered_image);
  CHECK(contours.size() == 475);
}

TEST_CASE("find_all_contours on image without contours",
          "[w_image_processor_test.cpp]") {
  fs::path source_image_path =
      image_processor_path / "find_all_contours_solid_black.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  config_for_ocr_struct config_for_ocr_struct;
  cv::Mat filtered_image =
      prepare_image_for_contour_detection(source_image, config_for_ocr_struct);
  std::vector<std::vector<cv::Point>> contours =
      find_all_countors(filtered_image);
  CHECK(contours.size() == 0);
}

TEST_CASE("resize_function with only height argument",
          "[w_image_processor_test.cpp]") {
  fs::path source_image_path = image_processor_path / "test_image.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  resize_image(source_image, 40);
  CHECK(source_image.rows == 40);
}

TEST_CASE("resize_function with only width argument",
          "[w_image_processor_test.cpp]") {
  fs::path source_image_path = image_processor_path / "test_image.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  resize_image(source_image, -1, 60);
  CHECK(source_image.cols == 60);
}

TEST_CASE("resize_function without argument", "[w_image_processor_test.cpp]") {
  fs::path source_image_path = image_processor_path / "test_image.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  int cols = source_image.cols;
  int rows = source_image.rows;
  resize_image(source_image);
  CHECK(source_image.cols == cols);
  CHECK(source_image.rows == rows);
}

TEST_CASE("gaussian_filter with default argument",
          "[w_image_processor_test.cpp]") {
  fs::path source_image_path = image_processor_path / "test_image.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  fs::path desired_image_path =
      image_processor_path / "gaussian_filter_default.png";
  cv::Mat desired_image = cv::imread(desired_image_path.string());
  config_for_ocr_struct config_for_ocr_struct;
  gaussian_blur(source_image, config_for_ocr_struct);
  CHECK((sum(source_image != desired_image) == cv::Scalar(0, 0, 0, 0)));
}

TEST_CASE("gaussian_filter with win size 7", "[w_image_processor_test.cpp]") {
  fs::path source_image_path = image_processor_path / "test_image.png";
  cv::Mat source_image = cv::imread(source_image_path.string());
  fs::path desired_image_path = image_processor_path / "gaussian_filter_7.png";
  cv::Mat desired_image = cv::imread(desired_image_path.string());
  config_for_ocr_struct config_for_ocr_struct;
  config_for_ocr_struct.gaussian_blur_win_size = 7;
  gaussian_blur(source_image, config_for_ocr_struct);
  CHECK((sum(source_image != desired_image) == cv::Scalar(0, 0, 0, 0)));
}

TEST_CASE("threshold_filter with default argument ",
          "[w_image_processor_test.cpp]") {
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
  CHECK(cv::countNonZero(diff) == 0);
}

TEST_CASE("threshold_filter with threshold value 180 ",
          "[w_image_processor_test.cpp]") {
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
  CHECK(cv::countNonZero(diff) == 0);
}
