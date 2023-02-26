#ifdef WOLF_TEST

#pragma once

#ifdef WOLF_ML_OCR

#define BOOST_TEST_MODULE ml_soccer

#include <ml/referee_ocr/w_soccer.hpp>

#include <boost/test/included/unit_test.hpp>
#include <filesystem>
#include <new>
#include <opencv2/opencv.hpp>

#include <ml/referee_ocr/w_ocr_engine.hpp>
#include <ml/referee_ocr/w_read_video_frames.hpp>
#include <ml/referee_ocr/w_referee.hpp>
#include <ml/referee_ocr/w_utilities.hpp>

namespace fs = std::filesystem;
fs::path soccer_asset_path = "../wolf/ml/test/common_test_asset/soccer";

using namespace wolf::ml::ocr;

BOOST_AUTO_TEST_CASE(set_config_function) {
  fs::path env_file_path = soccer_asset_path / ".set_config";

  set_env(env_file_path.string().c_str());

  char *type = new char[50];
  snprintf(type, 50, "SOCCER_SCREEN_IDENTITY");
  w_ocr_engine::config_struct screen_identity = w_soccer::set_config(type);

  delete[] type;

  BOOST_TEST(!screen_identity.config_for_ocr.verbose);
  BOOST_TEST(screen_identity.config_for_ocr.restrictions.max_area == 105);
  BOOST_TEST(screen_identity.name.compare("window_name_test") == 0);
}

std::vector<w_referee::match_result_struct> video_result;
typedef void ocr_callback(char *, int, uint8_t *, int, int);

void ocr_result(char *pResultBuffer, int pResultBufferSize,
                uint8_t *pImageBuffer, int pImageWidth, int pImageHeight) {
  w_referee::match_result_struct match_result_temp;
  std::string temp(pResultBuffer);
  std::vector<std::string> result_vec = split_string(temp, ',');

  match_result_temp.stat = result_vec[0];
  match_result_temp.home_name.text = result_vec[1];
  match_result_temp.home_result.text = result_vec[2];
  match_result_temp.away_name.text = result_vec[3];
  match_result_temp.away_result.text = result_vec[4];

  cv::Mat image = cv::Mat(pImageHeight, pImageWidth, CV_8UC3, pImageBuffer);

  match_result_temp.result_image = image;
  video_result.push_back(match_result_temp);
}

BOOST_AUTO_TEST_CASE(single_image_result_extraction_function) {
  fs::path json_file_path =
      soccer_asset_path / ".single_image_result_extraction";
  fs::path video_path =
      soccer_asset_path / "single_image_result_extraction.mp4";

  set_env(json_file_path.string().c_str());

  ocr_callback *_callback = &ocr_result;

  w_soccer *referee_obj = new w_soccer();

  // This is an example of using the Project
  w_read_video_frames player(video_path.string());
  int number_of_frames = int(player.get_frame_amount());

  cv::Mat frame;
  for (int i = 0; i < number_of_frames; i++) {
    frame = player.read_video_frame_by_frame();
    if (frame.empty()) {
      break;
    }

    char *res_str = new char[256];
    referee_obj->single_image_result_extraction(frame.data, frame.rows,
                                                frame.cols, _callback);
  }
  frame.release();

  std::cout << "Results: " << video_result[0].stat << " "
            << video_result[0].home_name.text << " "
            << video_result[0].away_name.text << " "
            << video_result[0].home_result.text << " "
            << video_result[0].away_result.text << std::endl;

  BOOST_TEST(video_result[0].stat.compare("90 00") == 0);
  BOOST_TEST(video_result[0].home_name.text.compare("ARSENAL") == 0);
  BOOST_TEST(video_result[0].away_name.text.compare("FC BAYERN") == 0);
  BOOST_TEST(video_result[0].home_result.text.compare("0") == 0);
  BOOST_TEST(video_result[0].away_result.text.compare("8") == 0);
}

BOOST_AUTO_TEST_CASE(fill_stat_map_function) {
  fs::path json_file_path = soccer_asset_path / ".fill_stat_map";

  set_env(json_file_path.string().c_str());

  w_soccer referee_obj;

  std::map<std::string, std::string> stat_map = referee_obj.get_stat_map();

  BOOST_TEST(stat_map["45 00"].compare("first_half") == 0);
  BOOST_TEST(stat_map["90 00"].compare("second_half") == 0);
  // BOOST_TEST(referee_obj->stat_extra_first_half.compare("105 00") == 0);
  // BOOST_TEST(referee_obj->stat_extra_second_half.compare("120 00") == 0);
  // BOOST_TEST(referee_obj->stat_penalty.compare("penalty") == 0);
}

BOOST_AUTO_TEST_CASE(replace_team_names_with_most_similar_string_finds_similar_team_to_REALMDRD) {
  std::vector<w_referee::match_result_struct> results;
  w_referee::match_result_struct result;

  fs::path similar_strings_path =
      soccer_asset_path / ".replace_team_names_with_most_similar_string";

  set_env(similar_strings_path.string().c_str());

  result.away_name.text = "REALMDRD";
  results.push_back(result);

  w_soccer::replace_team_names_with_most_similar_string(results);

  BOOST_TEST(results[0].away_name.text.compare("REAL MADRID") == 0);
}

BOOST_AUTO_TEST_CASE(initial_match_result_struct_function) {
  fs::path json_file_path = soccer_asset_path / ".initial_match_result_struct";

  set_env(json_file_path.string().c_str());

  w_referee::frame_result_struct frame_data;
  frame_data.away_name.text = "IRAN";
  frame_data.away_result.text = "3";
  frame_data.home_name.text = "ENGLAND";
  frame_data.home_result.text = "3";
  frame_data.frame_number = 114;
  frame_data.stat = "first_half";
  cv::Mat image = cv::Mat::zeros(50, 50, CV_8UC3);

  w_soccer soccer_object;

  w_referee::match_result_struct match_struct_temp;
  match_struct_temp =
      soccer_object.initial_match_result_struct(frame_data, image);

  image.release();
  BOOST_TEST(match_struct_temp.all_frames_results[0].away_name.text.compare(
              "IRAN") == 0);
  BOOST_TEST(match_struct_temp.stat.compare("first_half") == 0);
  BOOST_TEST(match_struct_temp.all_frames_results[0].away_result.text.compare(
              "3") == 0);
}

BOOST_AUTO_TEST_CASE(update_match_data_function) {
  fs::path json_file_path = soccer_asset_path / ".update_match_data";

  set_env(json_file_path.string().c_str());

  w_referee::frame_result_struct frame_data;
  frame_data.away_name.text = "IRAN";
  frame_data.away_result.text = "3";
  frame_data.home_name.text = "ENGLAND";
  frame_data.home_result.text = "3";
  frame_data.frame_number = 114;
  frame_data.stat = "first_half";
  cv::Mat image = cv::Mat::zeros(50, 50, CV_8UC3);

  w_soccer soccer_object;

  std::vector<w_referee::match_result_struct> match_data_temp;
  match_data_temp = soccer_object.get_matches_data();
  BOOST_TEST(match_data_temp.size() == 0);

  soccer_object.update_match_data(frame_data, image);
  frame_data.frame_number = 116;
  soccer_object.update_match_data(frame_data, image);
  frame_data.frame_number = 117;
  soccer_object.update_match_data(frame_data, image);

  match_data_temp = soccer_object.get_matches_data();
  BOOST_TEST(match_data_temp.size() == 1);

  frame_data.frame_number = 210;
  soccer_object.update_match_data(frame_data, image);

  match_data_temp = soccer_object.get_matches_data();
  BOOST_TEST(match_data_temp.size() == 2);
  BOOST_TEST(match_data_temp[1].all_frames_results[0].frame_number == 210);
}

BOOST_AUTO_TEST_CASE(get_matches_data_function) {
  fs::path json_file_path = soccer_asset_path / ".update_match_data";

  set_env(json_file_path.string().c_str());

  w_referee::frame_result_struct frame_data;
  frame_data.away_name.text = "IRAN";
  frame_data.away_result.text = "3";
  frame_data.home_name.text = "ENGLAND";
  frame_data.home_result.text = "3";
  frame_data.frame_number = 114;
  frame_data.stat = "first_half";
  cv::Mat image = cv::Mat::zeros(50, 50, CV_8UC3);

  w_soccer soccer_object;

  std::vector<w_referee::match_result_struct> match_data_temp;
  soccer_object.update_match_data(frame_data, image);
  match_data_temp = soccer_object.get_matches_data();
  BOOST_TEST(match_data_temp.size() == 1);
  BOOST_TEST(match_data_temp[0].all_frames_results[0].frame_number == 114);
}

BOOST_AUTO_TEST_CASE(replace_team_names_with_most_similar_string_finds_similar_team_to_empty_string) {
  std::vector<w_referee::match_result_struct> results;
  w_referee::match_result_struct result;

  fs::path similar_strings_path =
      soccer_asset_path / ".replace_team_names_with_most_similar_string";

  set_env(similar_strings_path.string().c_str());

  result.away_name.text = "";
  results.push_back(result);

  w_soccer::replace_team_names_with_most_similar_string(results);

  BOOST_TEST(results[0].away_name.text.compare("") == 0);
}


#endif // WOLF_ML_OCR

#endif // WOLF_TEST