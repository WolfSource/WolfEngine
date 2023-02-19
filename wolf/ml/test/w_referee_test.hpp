#ifdef WOLF_TEST

#pragma once

#ifdef WOLF_ML_OCR

#define BOOST_TEST_MODULE referee_ocr_referee

#include <ml/referee_ocr/w_referee.hpp>

#include <boost/test/included/unit_test.hpp>
#include <filesystem>
#include <opencv2/opencv.hpp>

#include <ml/referee_ocr/w_ocr_engine.hpp>

namespace fs = std::filesystem;
fs::path w_referee_path = "../ml/test/common_test_asset/w_referee";

using namespace wolf::ml::ocr;

BOOST_AUTO_TEST_CASE(concatenate_name_result_function) {
  w_referee ocr_referee_object;

  std::vector<w_ocr_engine::character_and_center> cluster_temp;

  w_ocr_engine::character_and_center char_center_temp;
  char_center_temp.text = "T";
  char_center_temp.center = cv::Point(5, 20);
  cluster_temp.push_back(char_center_temp);
  char_center_temp.text = "e";
  char_center_temp.center = cv::Point(10, 20);
  cluster_temp.push_back(char_center_temp);
  char_center_temp.text = "s";
  char_center_temp.center = cv::Point(15, 20);
  cluster_temp.push_back(char_center_temp);
  char_center_temp.text = "t";
  char_center_temp.center = cv::Point(20, 20);
  cluster_temp.push_back(char_center_temp);

  w_ocr_engine::character_and_center temp_result =
      ocr_referee_object.concatenate_name_result(cluster_temp);

  BOOST_TEST(temp_result.text.compare("T e s t") == 0);
}

BOOST_AUTO_TEST_CASE(if_the_string_is_in_the_vector_function) {
  w_referee ocr_referee_object;

  std::vector<w_referee::vote_over_string_vector> vote_vector_temp;
  w_referee::vote_over_string_vector vote_var_temp;
  vote_var_temp.already_voted = false;
  vote_var_temp.center = cv::Point(70, 77);
  vote_var_temp.str = "Test";
  vote_vector_temp.push_back(vote_var_temp);

  w_ocr_engine::character_and_center char_temp;
  char_temp.center = cv::Point(70, 77);
  char_temp.text = "Test";

  BOOST_TEST(ocr_referee_object.if_the_string_is_in_the_vector(char_temp,
                                                            vote_vector_temp));
}

BOOST_AUTO_TEST_CASE(voting_over_results_and_names_function) {
  w_referee ocr_referee_object;

  std::vector<w_referee::frame_result_struct> all_results_temp;
  w_referee::frame_result_struct frame_date_temp;

  frame_date_temp.frame_number = 110;
  frame_date_temp.away_name.text = "ENGLAND";
  frame_date_temp.away_result.text = "1";
  frame_date_temp.home_name.text = "IRAN";
  frame_date_temp.home_result.text = "2";
  frame_date_temp.stat = w_referee::final_result;
  all_results_temp.push_back(frame_date_temp);
  all_results_temp.push_back(frame_date_temp);
  all_results_temp.push_back(frame_date_temp);
  all_results_temp.push_back(frame_date_temp);
  all_results_temp.push_back(frame_date_temp);
  all_results_temp.push_back(frame_date_temp);
  all_results_temp.push_back(frame_date_temp);
  all_results_temp.push_back(frame_date_temp);
  all_results_temp.push_back(frame_date_temp);
  all_results_temp.push_back(frame_date_temp);
  frame_date_temp.away_result.text = "3";
  all_results_temp.push_back(frame_date_temp);
  all_results_temp.push_back(frame_date_temp);
  all_results_temp.push_back(frame_date_temp);
  all_results_temp.push_back(frame_date_temp);
  all_results_temp.push_back(frame_date_temp);
  all_results_temp.push_back(frame_date_temp);
  all_results_temp.push_back(frame_date_temp);
  all_results_temp.push_back(frame_date_temp);
  all_results_temp.push_back(frame_date_temp);

  w_referee::frame_result_struct frame_date_result;

  ocr_referee_object.voting_over_results_and_names(frame_date_result,
                                                   all_results_temp);

  BOOST_TEST(frame_date_result.away_result.text.compare("1") == 0);
}

#endif // WOLF_ML_OCR

#endif // WOLF_TEST
