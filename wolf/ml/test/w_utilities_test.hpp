#ifdef WOLF_TEST

#pragma once

#ifdef WOLF_ML_OCR

#define BOOST_TEST_MODULE ml_utilities

#include <ml/referee_ocr/w_utilities.hpp>

#include <boost/test/included/unit_test.hpp>
#include <filesystem>
#include <opencv2/opencv.hpp>

namespace fs = std::filesystem;
fs::path utilities_asset_path = "../wolf/ml/test/common_test_asset/utilities";

using namespace wolf::ml::ocr;

BOOST_AUTO_TEST_CASE(get_value_from_json_file_by_key_gives_value_by_key) {
  fs::path json_file_path =
      utilities_asset_path / "get_value_from_json_file_by_key.json";
  std::string key = "cpp-test-json";
  std::string value =
      get_value_from_json_file_by_key(json_file_path.string(), key);
  BOOST_TEST(value.compare("ready") == 0);
}

BOOST_AUTO_TEST_CASE(string_2_boolean_on_capitalized_characters) {
  std::vector<std::string> list_string;
  list_string.push_back("true");
  list_string.push_back("FalSe");
  bool temp;
  temp = string_2_boolean(list_string[0]);
  BOOST_TEST(temp);
  temp = string_2_boolean(list_string[1]);
  BOOST_TEST(!temp);
}

BOOST_AUTO_TEST_CASE(line_of_numbers_in_string_to_vector_of_integers_function) {
  std::string temp = "100,200,300,400";
  std::vector<int> integer_list =
      line_of_numbers_in_string_to_vector_of_integers(temp);
  BOOST_TEST(integer_list[0] == 100);
  BOOST_TEST(integer_list[1] == 200);
  BOOST_TEST(integer_list[2] == 300);
  BOOST_TEST(integer_list[3] == 400);
}

BOOST_AUTO_TEST_CASE(store_image_in_folder_function) {
  std::vector<w_referee::match_result_struct> temp_video_result;
  w_referee::match_result_struct temp;
  w_referee::frame_result_struct frame_result;

  frame_result.home_name.text = "Home";
  frame_result.home_result.text = "2";
  frame_result.away_name.text = "Away";
  frame_result.away_result.text = "1";
  frame_result.frame_number = 3;
  frame_result.stat = "first_half";
  int height = 50;
  int width = 50;
  temp.result_image = cv::Mat::zeros(height, width, CV_8UC3);
  temp.all_frames_results.push_back(frame_result);
  temp_video_result.push_back(temp);

  std::string output_folder_path = ".";
  fs::path temp_video_path = "../test/123.mp4";
  store_image_in_folder(temp_video_result, output_folder_path,
                        temp_video_path.string());

  fs::path file_path = "./123_0.png";

  bool is_file_exist = false;
  if (std::filesystem::remove(file_path.string())) {
    is_file_exist = true;
  }
  BOOST_TEST(is_file_exist);
}

BOOST_AUTO_TEST_CASE(write_results_in_file_function) {
  std::vector<w_referee::match_result_struct> temp_video_result;
  w_referee::match_result_struct temp;
  w_referee::frame_result_struct frame_result;

  frame_result.home_name.text = "Home";
  frame_result.home_result.text = "2";
  frame_result.away_name.text = "Away";
  frame_result.away_result.text = "1";
  frame_result.frame_number = 3;
  frame_result.stat = "first_half";
  int height = 50;
  int width = 50;
  temp.result_image = cv::Mat::zeros(height, width, CV_8UC3);
  temp.all_frames_results.push_back(frame_result);
  temp_video_result.push_back(temp);

  fs::path temp_output_text_file = "./test_write_results_in_file.txt";

  write_results_in_file(temp_video_result, temp_output_text_file.string());

  std::vector<std::string> lines =
      read_text_file_line_by_line(temp_output_text_file.string());

  if (!std::filesystem::remove(temp_output_text_file.string())) {
    BOOST_TEST(false);
  }

  BOOST_TEST(lines[0].compare(temp_video_result[0].stat + "," +
                           temp_video_result[0].home_name.text + "," +
                           temp_video_result[0].home_result.text + "," +
                           temp_video_result[0].away_name.text + "," +
                           temp_video_result[0].away_result.text + "," +
                           std::to_string(temp_video_result[0].frame_number)) ==
          0);
}

BOOST_AUTO_TEST_CASE(read_text_file_line_by_line_function) {
  fs::path file_path = "./test_read_text_file_line_by_line.txt";
  std::string content = "This is a test!";

  write_in_file_append(file_path.string(), content);
  std::vector<std::string> lines =
      read_text_file_line_by_line(file_path.string());

  if (!std::filesystem::remove(file_path.string())) {
    BOOST_TEST(false);
  }

  BOOST_TEST(lines[0].compare(content) == 0);
}

BOOST_AUTO_TEST_CASE(normalized_levenshtein_similarity_on_one_empty_argument) {
  float similarity = normalized_levenshtein_similarity("", "text");
  BOOST_TEST(similarity == 0);
}

BOOST_AUTO_TEST_CASE(normalized_levenshtein_similarity_on_two_empty_argument) {
  float similarity = normalized_levenshtein_similarity("", "");
  BOOST_TEST(similarity == 0);
}

BOOST_AUTO_TEST_CASE(normalized_levenshtein_similarity_on_two_equal_string) {
  float similarity = normalized_levenshtein_similarity("test", "test");
  BOOST_TEST(similarity == 1);
}

BOOST_AUTO_TEST_CASE(normalized_levenshtein_similarity_with_similarity_0_5_on_strings) {
  float similarity = normalized_levenshtein_similarity("test", "mast");
  BOOST_TEST(similarity == 0.5);
}

BOOST_AUTO_TEST_CASE(normalized_levenshtein_similarity_on_same_numbers) {
  float similarity = normalized_levenshtein_similarity("1234", "1234");
  BOOST_TEST(similarity == 1);
}

BOOST_AUTO_TEST_CASE(normalized_levenshtein_similarity_on_strings_with_different_length) {
  float similarity = normalized_levenshtein_similarity("test", "tst");
  BOOST_TEST(similarity == 0.75);
}

BOOST_AUTO_TEST_CASE(get_nearest_string_with_threshold_0_5_returns_similar_string) {
  fs::path similar_strings_path =
      utilities_asset_path / ".get_nearest_string_0_5";

  set_env(similar_strings_path.string().c_str());

  std::string path = get_env_string("SIMILAR_STRINGS_FILE_PATH");
  std::string output = get_nearest_string("tst1", path);
  BOOST_TEST(output.compare("test1") == 0);
}

BOOST_AUTO_TEST_CASE(get_nearest_string_with_threshold_0_9_returns_input) {
  fs::path similar_strings_path =
      utilities_asset_path / ".get_nearest_string_0_9";

  set_env(similar_strings_path.string().c_str());

  std::string path = get_env_string("SIMILAR_STRINGS_FILE_PATH");
  std::string output = get_nearest_string("tst1", path);
  BOOST_TEST(output.compare("tst1") == 0);
}

BOOST_AUTO_TEST_CASE(get_nearest_string_on_empty_string_returns_empty_string) {
  fs::path similar_strings_path =
      utilities_asset_path / ".get_nearest_string_0_5";

  set_env(similar_strings_path.string().c_str());

  std::string path = get_env_string("SIMILAR_STRINGS_FILE_PATH");
  std::string output = get_nearest_string("", path);
  BOOST_TEST(output.compare("") == 0);
}

BOOST_AUTO_TEST_CASE(replace_string_first_phrase_exists_in_text) {
  std::string text = "hello hamed";
  replace_string(text, "hamed", "bagher");
  BOOST_TEST(text.compare("hello bagher") == 0);
}

BOOST_AUTO_TEST_CASE(replace_string_returns_text_if_first_phrase_not_exists_in_text) {
  std::string text = "hello hamed";
  replace_string(text, "shahoo", "bagher");
  BOOST_TEST(text.compare("hello hamed") == 0);
}

BOOST_AUTO_TEST_CASE(replace_string_returns_empty_text_if_input_text_was_empty) {
  std::string text = "";
  replace_string(text, "shahoo", "bagher");
  BOOST_TEST(text.compare("") == 0);
}

BOOST_AUTO_TEST_CASE(is_line_contains_variable_function_find_empty_line) {
  const std::string content = "";

  BOOST_TEST(!is_line_contains_variable(content));
}

BOOST_AUTO_TEST_CASE(is_line_contains_variable_function_find_line_started_by_sharp) {
  const std::string content = "# This is a commented line!";

  BOOST_TEST(!is_line_contains_variable(content));
}

BOOST_AUTO_TEST_CASE(set_env_function) {
  fs::path dot_env_file_path = utilities_asset_path / ".set_env";

  set_env(dot_env_file_path.string().c_str());

  if (const char *env_p1 = getenv("V1")) {
    std::string temp(env_p1);
    BOOST_TEST(temp.compare("v1") == 0);
  }
  if (const char *env_p2 = getenv("V2")) {
    std::string temp(env_p2);
    BOOST_TEST(temp.compare("v2") == 0);
  }
  if (const char *env_p3 = getenv("V3")) {
    std::string temp(env_p3);
    BOOST_TEST(temp.compare("v3") == 0);
  }
  if (const char *env_p4 = getenv("V4")) {
    std::string temp(env_p4);
    BOOST_TEST(temp.compare("v4") == 0);
  }
}

BOOST_AUTO_TEST_CASE(get_env_int_function) {
  fs::path dot_env_file_path = utilities_asset_path / ".get_env_int";

  set_env(dot_env_file_path.string().c_str());

  std::string key = "INT_VALUE";
  int value = get_env_int(key.c_str());

  BOOST_TEST(value == 7);
}

BOOST_AUTO_TEST_CASE(get_env_float_function) {
  fs::path dot_env_file_path = utilities_asset_path / ".get_env_float";

  set_env(dot_env_file_path.string().c_str());

  std::string key = "FLOAT_VALUE";
  float value = get_env_float(key.c_str());

  BOOST_TEST(value == 4.5);
}

BOOST_AUTO_TEST_CASE(get_env_boolean_function) {
  fs::path dot_env_file_path = utilities_asset_path / ".get_env_boolean";

  set_env(dot_env_file_path.string().c_str());

  std::string key = "BOOLEAN_VALUE";
  bool value = get_env_boolean(key.c_str());

  BOOST_TEST(value);
}

BOOST_AUTO_TEST_CASE(get_env_string_function) {
  fs::path dot_env_file_path = utilities_asset_path / ".get_env_string";

  set_env(dot_env_file_path.string().c_str());

  std::string key = "STRING_VALUE";
  std::string value = get_env_string(key.c_str());

  BOOST_TEST(value.compare("this is a test! == 0"));
}

BOOST_AUTO_TEST_CASE(get_env_cv_rect_function) {
  fs::path dot_env_file_path = utilities_asset_path / ".get_env_cv_rect";

  set_env(dot_env_file_path.string().c_str());

  std::string key = "CV_RECT_VALUE";
  cv::Rect value = get_env_cv_rect(key.c_str());

  BOOST_TEST(value.x == 313);
  BOOST_TEST(value.y == 110);
  BOOST_TEST(value.width == 72);
  BOOST_TEST(value.height == 14);
}

BOOST_AUTO_TEST_CASE(get_relative_path_to_root_function) {
  std::string temp = get_relative_path_to_root();

  bool result = (temp.compare("../") == 0 || temp.compare("../../../") == 0);

  BOOST_TEST(result);
}

#endif // WOLF_ML_OCR

#endif // WOLF_TEST
