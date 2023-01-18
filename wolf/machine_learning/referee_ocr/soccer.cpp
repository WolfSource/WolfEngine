#include "soccer.h"

#include "DISABLE_ANALYSIS_BEGIN"

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <string>

#include "DISABLE_ANALYSIS_END"

#include "ocr_referee_class.h"
#include "utilities.h"

SOCCER::SOCCER() {
  // LOG_P(w_log_type::W_LOG_INFO, "creating SOCCER object ...");

  char *type_1 = new char[30];
  snprintf(type_1, 30, "SOCCER_SCREEN_IDENTITY");
  screen_identity = set_config(type_1);

  char *type_2 = new char[30];
  snprintf(type_2, 30, "SOCCER_RESULT_HOME");
  result_home = set_config(type_2);
  char *type_3 = new char[30];
  snprintf(type_3, 30, "SOCCER_RESULT_AWAY");
  result_away = set_config(type_3);

  char *type_4 = new char[30];
  snprintf(type_4, 30, "SOCCER_NAME_HOME");
  name_home = set_config(type_4);
  char *type_5 = new char[30];
  snprintf(type_5, 30, "SOCCER_NAME_AWAY");
  name_away = set_config(type_5);

  char *type_6 = new char[30];
  snprintf(type_6, 30, "SOCCER_PLATFORM_FREE");
  platform_free = set_config_for_ocr(type_6);

  char *type_7 = new char[30];
  snprintf(type_7, 30, "SOCCER_PENALTY");
  penalty = set_config_for_ocr(type_7);

  delete[] type_1;
  delete[] type_2;
  delete[] type_3;
  delete[] type_4;
  delete[] type_5;
  delete[] type_6;
  delete[] type_7;

  fill_stat_map();
}

SOCCER::~SOCCER() {}

auto SOCCER::set_config(_In_ char *pType) -> OCR::config_struct {
  std::string type(pType);

  OCR::config_struct config;

  config.name = get_env_string((type + "_WINDOW_NAME").c_str());
  config.window = get_env_cv_rect((type + "_WINDOW").c_str());
  config.is_time = get_env_boolean((type + "_IS_TIME").c_str());
  config.config_for_ocr.do_resize_contour =
      get_env_boolean((type + "_DO_RESIZE_CONTOUR").c_str());
  config.config_for_ocr.gaussian_blur_win_size =
      get_env_int((type + "_GAUSSIAN_BLUR_WIN_SIZE").c_str());
  config.config_for_ocr.if_store_image_boxes =
      get_env_boolean((type + "_IF_STORE_IMAGE_BOXES").c_str());
  config.config_for_ocr.is_white =
      get_env_boolean((type + "_IS_WHITE").c_str());
  config.config_for_ocr.is_digit =
      get_env_boolean((type + "_IS_DIGIT").c_str());
  config.config_for_ocr.make_white_background =
      get_env_boolean((type + "_MAKE_WHITE_BACKGROUND").c_str());
  config.config_for_ocr.margin = get_env_int((type + "_MARGIN").c_str());
  config.config_for_ocr.verbose = get_env_boolean((type + "_VERBOSE").c_str());
  config.config_for_ocr.threshold_value =
      get_env_int((type + "_THRESHOLD").c_str());
  config.config_for_ocr.white_background_threshold =
      get_env_int((type + "_WHITE_BACKGROUND_THRESHOLD").c_str());
  config.config_for_ocr.restrictions.max_area =
      get_env_int((type + "_RESTRICTIONS_MAX_AREA").c_str());
  config.config_for_ocr.restrictions.min_area =
      get_env_int((type + "_RESTRICTIONS_MIN_AREA").c_str());
  config.config_for_ocr.restrictions.max_width =
      get_env_int((type + "_RESTRICTIONS_MAX_WIDTH").c_str());
  config.config_for_ocr.restrictions.min_width =
      get_env_int((type + "_RESTRICTIONS_MIN_WIDTH").c_str());
  config.config_for_ocr.restrictions.max_height =
      get_env_int((type + "_RESTRICTIONS_MAX_HEIGHT").c_str());
  config.config_for_ocr.restrictions.min_height =
      get_env_int((type + "_RESTRICTIONS_MIN_HEIGHT").c_str());

  return config;
}

auto SOCCER::set_config_for_ocr(_In_ char *pType) -> config_for_ocr_struct {
  std::string type(pType);
  config_for_ocr_struct config;

  config.restrictions.max_area =
      get_env_int((type + "_RESTRICTIONS_MAX_AREA").c_str());
  config.restrictions.min_area =
      get_env_int((type + "_RESTRICTIONS_MIN_AREA").c_str());
  config.restrictions.max_width =
      get_env_int((type + "_RESTRICTIONS_MAX_WIDTH").c_str());
  config.restrictions.min_width =
      get_env_int((type + "_RESTRICTIONS_MIN_WIDTH").c_str());
  config.restrictions.max_height =
      get_env_int((type + "_RESTRICTIONS_MAX_HEIGHT").c_str());
  config.restrictions.min_height =
      get_env_int((type + "_RESTRICTIONS_MIN_HEIGHT").c_str());
  config.fraction = get_env_float((type + "_FRACTION").c_str());

  return config;
}

auto SOCCER::fill_stat_map() -> void {
  stat_first_half = get_env_string("SOCCER_STAT_FIRST_HALF_STRING");
  stat_second_half = get_env_string("SOCCER_STAT_SECOND_HALF_STRING");
  stat_extra_first_half = get_env_string("SOCCER_STAT_EXTRA_FIRST_HALF_STRING");
  stat_extra_second_half =
      get_env_string("SOCCER_STAT_EXTRA_SECOND_HALF_STRING");
  stat_penalty = get_env_string("SOCCER_STAT_PENALTY_STRING");

  bool is_platform_free = get_env_boolean("SOCCER_GLOBAL_PLATFORM_FREE");

  stat_map.insert(std::pair<std::string, std::string>(
      get_first_character_of_string(stat_first_half, is_platform_free),
      "first_half"));
  stat_map.insert(std::pair<std::string, std::string>(
      get_first_character_of_string(stat_second_half, is_platform_free),
      "second_half"));
  stat_map.insert(std::pair<std::string, std::string>(
      get_first_character_of_string(stat_extra_first_half, is_platform_free),
      "extra_first_half"));
  stat_map.insert(std::pair<std::string, std::string>(
      get_first_character_of_string(stat_extra_second_half, is_platform_free),
      "extra_second_half"));
  stat_map.insert(std::pair<std::string, std::string>(
      get_first_character_of_string(stat_penalty, is_platform_free),
      "penalty"));
}

auto SOCCER::extract_result_from_frame_boxes(
    _In_ cv::Mat &frame, _Inout_ frame_result_struct &frame_data) -> void {
  std::vector<OCR::character_and_center> temp_words, temp_team_names;

  cv::Mat frame_box = frame(screen_identity.window);
  temp_words =
      ocr_object.image_to_string(frame_box, screen_identity.config_for_ocr);

  if (temp_words.size() == 1) {
    if (temp_words[0].text.c_str()[0] == stat_first_half.c_str()[0] ||
        temp_words[0].text.c_str()[0] == stat_second_half.c_str()[0]) {
      frame_data.stat = stat_map[temp_words[0].text];
      temp_words.clear();

      frame_box = frame(result_home.window);
      temp_words =
          ocr_object.image_to_string(frame_box, result_home.config_for_ocr);
      if (temp_words.size() != 0) {
        frame_data.home_result = temp_words[0];

        frame_box = frame(result_away.window);
        temp_words =
            ocr_object.image_to_string(frame_box, result_away.config_for_ocr);
        if (temp_words.size() != 0) {
          frame_data.away_result = temp_words[0];

          temp_team_names.clear();

          frame_box = frame(name_home.window);
          temp_team_names =
              ocr_object.image_to_string(frame_box, name_home.config_for_ocr);
          if (temp_team_names.size() == 0) {
            frame_box.release();
            return;
          }
          frame_data.home_name = concatenate_name_result(temp_team_names);

          frame_box = frame(name_away.window);
          temp_team_names =
              ocr_object.image_to_string(frame_box, name_away.config_for_ocr);
          if (temp_team_names.size() == 0) {
            frame_box.release();
            return;
          }
          frame_data.away_name = concatenate_name_result(temp_team_names);
        }
      }
    }
  }
  frame_box.release();
}

auto SOCCER::extract_result_based_on_clusters_symmetricity(
    _In_ cv::Mat &frame, _Inout_ frame_result_struct &frame_data) -> void {
  cv::Mat cloned_image = frame.clone();
  int image_width = frame.cols;

  if (get_env_boolean("CONFIG_STORE_LATEST_FRAME")) {
    std::string image_data_file_name =
        get_env_string("SOCCER_GLOBAL_LOG_FILE") + "_" +
        std::to_string(frame_number % 10);
    std::ofstream ofs(image_data_file_name.c_str(), std::ofstream::out);
    ofs.write((char *)frame.data,
              frame.total() * frame.channels() * sizeof(uint8_t));
    ofs.close();
  }

  std::vector<std::vector<OCR::characters_struct>> digits_candidates;
  std::vector<std::vector<OCR::characters_struct>> words_candidates;
  std::vector<std::vector<OCR::characters_struct>> time_candidates;

  extract_all_image_char_clusters(cloned_image, digits_candidates,
                                  words_candidates, time_candidates);

  if (digits_candidates.size() == 2 && words_candidates.size() == 2 &&
      time_candidates.size() == 1) {
    for (int i = 0; i < words_candidates.size(); i++) {
      std::vector<OCR::character_and_center> text_of_cluster =
          ocr_object.char_vec_to_string(words_candidates[i], frame,
                                        name_home.config_for_ocr);

      if (text_of_cluster.size() > 0) {
        if (text_of_cluster[0].center.x < image_width / 2) {
          frame_data.home_name = text_of_cluster[0];
        } else {
          frame_data.away_name = text_of_cluster[0];
        }
      }
    }
    for (int i = 0; i < digits_candidates.size(); i++) {
      std::vector<OCR::character_and_center> text_of_cluster =
          ocr_object.char_vec_to_string(digits_candidates[i], frame,
                                        result_home.config_for_ocr);
      if (text_of_cluster.size() > 0) {
        if (text_of_cluster[0].center.x < image_width / 2) {
          frame_data.home_result = text_of_cluster[0];
        } else {
          frame_data.away_result = text_of_cluster[0];
        }
      }
    }
    for (int i = 0; i < time_candidates.size(); i++) {
      std::vector<OCR::character_and_center> text_of_cluster =
          ocr_object.char_vec_to_string(time_candidates[i], frame,
                                        screen_identity.config_for_ocr);
      if (text_of_cluster.size() > 0) {
        frame_data.stat = get_nearest_string(text_of_cluster[0].text, stat_map);
        if (frame_data.stat.compare("") == 0) {
          OCR::config_struct temp_screen_identity = screen_identity;
          temp_screen_identity.config_for_ocr.is_digit = false;
          text_of_cluster = ocr_object.char_vec_to_string(
              time_candidates[i], frame, temp_screen_identity.config_for_ocr);
          std::vector<std::string> temp_result =
              split_string(text_of_cluster[0].text, ' ');
          if (temp_result.size() > 1) {
            frame_data.stat = get_nearest_string(temp_result[1], stat_map);
          }
        }
        if (frame_data.stat.compare("") != 0) {
          extract_penalty_result_symmetricity(frame, digits_candidates,
                                              words_candidates, time_candidates,
                                              frame_data);
          // frame_data.stat = stat_map[frame_data.stat];
        }
      }
    }
  }
  cloned_image.release();
}

auto SOCCER::extract_penalty_result_symmetricity(
    _In_ cv::Mat &frame,
    _In_ std::vector<std::vector<OCR::characters_struct>> digits_candidates,
    _In_ std::vector<std::vector<OCR::characters_struct>> words_candidates,
    _In_ std::vector<std::vector<OCR::characters_struct>> time_candidates,
    _Inout_ frame_result_struct &frame_data) -> void {
  if (frame_data.stat.compare(get_env_string("SOCCER_STAT_CHECK_PENALTY")) !=
          0 ||
      digits_candidates.size() != 2 || time_candidates.size() != 1) {
    return;
  }
  cv::Rect result_box_bound_rect;

  if (stat_second_half.compare(get_env_string("SOCCER_STAT_CHECK_PENALTY")) ==
      0) {
    OCR::characters_struct left, right;
    left = (digits_candidates[0][0].bound_rect.x >
            digits_candidates[1][0].bound_rect.x)
               ? digits_candidates[1][digits_candidates[1].size() - 1]
               : digits_candidates[0][digits_candidates[0].size() - 1];
    right = (digits_candidates[0][0].bound_rect.x >
             digits_candidates[1][0].bound_rect.x)
                ? digits_candidates[0][0]
                : digits_candidates[1][0];

    result_box_bound_rect.x = left.bound_rect.x + left.bound_rect.width;
    result_box_bound_rect.width = right.bound_rect.x - result_box_bound_rect.x;

    if (result_box_bound_rect.width < 0) {
      std::cout << "something went wrong!!!" << std::endl;
      return;
    }

    if (result_box_bound_rect.width > frame.cols / 3) {
      return;
    }

    result_box_bound_rect.y =
        left.bound_rect.y + left.bound_rect.height / 2 + 4;
    result_box_bound_rect.height = left.bound_rect.height / 2;
  } else if (stat_penalty.compare(
                 get_env_string("SOCCER_STAT_CHECK_PENALTY")) == 0) {
    result_box_bound_rect.x = time_candidates[0][0].bound_rect.x - 50;
    result_box_bound_rect.width = 50 + 10;
    // (time_candidates[0][time_candidates[0].size() - 1].bound_rect.x +
    //  time_candidates[0][time_candidates[0].size() - 1].bound_rect.width -
    //  result_box_bound_rect.x) /
    // 2;
    result_box_bound_rect.y = time_candidates[0][0].bound_rect.y;
    result_box_bound_rect.height = time_candidates[0][0].bound_rect.height;
  }

  cv::Mat result_box;
  frame(result_box_bound_rect).copyTo(result_box);

  // cv::imshow("image", result_box);
  // cv::waitKey();

  std::vector<OCR::character_and_center> temp_words;

  temp_words =
      ocr_object.image_to_string(result_box, result_away.config_for_ocr);

  if (temp_words.size() == 2 && stat_second_half.compare(get_env_string(
                                    "SOCCER_STAT_CHECK_PENALTY")) == 0) {
    frame_data.home_penalty_result =
        (temp_words[0].center.x < temp_words[1].center.x) ? temp_words[0]
                                                          : temp_words[1];
    frame_data.away_penalty_result =
        (temp_words[0].center.x < temp_words[1].center.x) ? temp_words[1]
                                                          : temp_words[0];
  } else if (temp_words.size() == 1 && stat_penalty.compare(get_env_string(
                                           "SOCCER_STAT_CHECK_PENALTY")) == 0) {
    std::vector<std::string> temp_result =
        split_string(temp_words[0].text, ' ');
    frame_data.home_penalty_result.text = temp_result[0];
    frame_data.away_penalty_result.text = temp_result[1];
  }

  return;
}

auto SOCCER::single_image_result_extraction(_In_ uint8_t *pRawImage,
                                            _In_ int height, _In_ int width,
                                            _In_ ocr_callback *callback)
    -> int {
  int desired_height = get_env_int("SOCCER_GLOBAL_FRAME_HEIGHT");
  int desired_width = get_env_int("SOCCER_GLOBAL_FRAME_WIDTH");
  if (!pRawImage || height != desired_height || width != desired_width) {
    return 1;
  }
  cv::Mat original_image = cv::Mat(height, width, CV_8UC3, pRawImage);

  frame_result_struct temp_frame_data;

  if (get_env_boolean("SOCCER_GLOBAL_PLATFORM_FREE")) {
    extract_result_based_on_clusters_symmetricity(original_image,
                                                  temp_frame_data);
  } else {
    extract_result_from_frame_boxes(original_image, temp_frame_data);
  }

  if (temp_frame_data.stat.compare("") != 0 &&
      temp_frame_data.home_result.text.compare("") != 0 &&
      temp_frame_data.away_result.text.compare("") != 0 &&
      temp_frame_data.home_name.text.compare("") != 0 &&
      temp_frame_data.away_name.text.compare("") != 0) {
    temp_frame_data.frame_number = frame_number;
    cv::Mat temp_image = original_image.clone();
    update_match_data(temp_frame_data, temp_image);
    temp_image.release();
  }

  // update the frame number
  frame_number += 1;
  for (int i = 0; i < matches_data.size(); i++) {
    if (i == 0 && frame_number < matches_data[i].frame_number + 10) {
      continue;
    }
    matches_data[i].ready = true;
  }

  extract_game_results();

  if (callback) {
    for (int i = 0; i < matches_data.size(); i++) {
      if (matches_data[i].extracted && !matches_data[i].applied) {
        std::string str_result = matches_data[i].stat + "," +
                                 matches_data[i].home_name.text + "," +
                                 matches_data[i].home_result.text + "," +
                                 matches_data[i].away_name.text + "," +
                                 matches_data[i].away_result.text;
        if (matches_data[i].home_penalty_result.text.compare("") != 0 &&
            matches_data[i].away_penalty_result.text.compare("") != 0) {
          str_result = str_result + "," +
                       matches_data[i].home_penalty_result.text + "," +
                       matches_data[i].away_penalty_result.text;
        }
        char *char_result = new char[256];
        strcpy(char_result, str_result.c_str());

        callback(char_result, 256, matches_data[i].result_image.data,
                 matches_data[i].result_image.cols,
                 matches_data[i].result_image.rows);
        the_last_result_message = str_result;

        matches_data[i].applied = true;
      }
    }
  }

  return 0;
}

auto SOCCER::extract_all_image_char_clusters(
    cv::Mat &image,
    std::vector<std::vector<OCR::characters_struct>> &digits_candidates,
    std::vector<std::vector<OCR::characters_struct>> &words_candidates,
    std::vector<std::vector<OCR::characters_struct>> &time_candidates) -> void {
  cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
  int global_threshold = get_env_int("SOCCER_GLOBAL_THRESHOLD");
  cv::threshold(image, image, global_threshold, 255,
                cv::THRESH_BINARY); // cv::THRESH_OTSU);

  std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i> hierarchy;

  cv::findContours(image, contours, hierarchy, cv::RETR_TREE,
                   cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
  std::vector<OCR::characters_struct> modified_bounding_rects =
      ocr_object.contours_to_char_structs(contours);

  modified_bounding_rects = ocr_object.filter_chars_by_contour_size(
      modified_bounding_rects, platform_free);
  contours.clear();
  hierarchy.clear();

  ocr_object.merge_overlapped_contours(modified_bounding_rects, platform_free);

  std::vector<std::vector<OCR::characters_struct>> clustered_characters =
      ocr_object.cluster_char_structs(modified_bounding_rects, platform_free);

  int ref_index = clustered_characters.size();
  for (int i = 0; i < ref_index; i++) {
    int index = ref_index - (i + 1);
    if (clustered_characters[index].size() < 3 &&
        ocr_object.same_level(clustered_characters[index])) {
      digits_candidates.push_back(clustered_characters[index]);
    }
    if (clustered_characters[index].size() >= 3 &&
        ocr_object.same_level(clustered_characters[index])) {
      words_candidates.push_back(clustered_characters[index]);
    }
    if (clustered_characters[index][0].bound_rect.x < image.cols / 2 &&
        clustered_characters[index][clustered_characters[index].size() - 1]
                .bound_rect.x > image.cols / 2 &&
        clustered_characters[index].size() < 16 &&
        clustered_characters[index].size() > 1) {
      time_candidates.push_back(clustered_characters[index]);
    }
  }
  // cv::Mat temp_image = cv::Mat(image.rows, image.cols, CV_8UC1,
  // cv::Scalar(0)); for (int i = 0; i < clustered_characters.size(); i++)
  // {
  // 	ocr_object.show_contours(temp_image, clustered_characters[i], "Hello",
  // true);
  // }

  clustered_characters.clear();

  ocr_object.keep_twins(words_candidates, image.cols, image.rows, true);
  ocr_object.keep_twins(digits_candidates, image.cols, image.rows, false);
  ocr_object.keep_time(time_candidates);

  for (int i = 0; i < digits_candidates.size(); i++) {
    for (size_t j = 0; j < digits_candidates[i].size(); j++) {
      ocr_object.margin_bounding_rect(digits_candidates[i][j].bound_rect,
                                      platform_free.margin, image);
    }
  }

  for (int i = 0; i < words_candidates.size(); i++) {
    for (size_t j = 0; j < words_candidates[i].size(); j++) {
      ocr_object.margin_bounding_rect(words_candidates[i][j].bound_rect,
                                      platform_free.margin, image);
    }
  }

  for (int i = 0; i < time_candidates.size(); i++) {
    for (size_t j = 0; j < time_candidates[i].size(); j++) {
      ocr_object.margin_bounding_rect(time_candidates[i][j].bound_rect,
                                      platform_free.margin, image);
    }
  }

  return;
}

auto SOCCER::replace_team_names_with_most_similar_string(
    _Inout_ std::vector<ocr_referee_class::match_result_struct> &result)
    -> void {
  for (int i = 0; i < int(result.size()); i++) {
    // LOG_P(w_log_type::W_LOG_INFO, "recognized home name : %s",
    // result[i].home_name.text); LOG_P(w_log_type::W_LOG_INFO, "recognized away
    // name : %s", result[i].away_name.text);
    std::string path = get_env_string("SIMILAR_STRINGS_FILE_PATH");
    result[i].home_name.text =
        get_nearest_string(result[i].home_name.text, path);
    result[i].away_name.text =
        get_nearest_string(result[i].away_name.text, path);
    // LOG_P(w_log_type::W_LOG_INFO, "recognized home name after replace with
    // most similar: %s", result[i].home_name.text);
    // LOG_P(w_log_type::W_LOG_INFO, "recognized away name after replace with
    // most similar: %s", result[i].away_name.text);
  }
}

auto SOCCER::initial_match_result_struct(
    ocr_referee_class::frame_result_struct frame_data, cv::Mat &image)
    -> ocr_referee_class::match_result_struct {
  ocr_referee_class::match_result_struct temp_match_data;
  temp_match_data.all_frames_results.push_back(frame_data);
  temp_match_data.stat = frame_data.stat;
  temp_match_data.result_image = image;
  temp_match_data.frame_number = frame_data.frame_number;

  return temp_match_data;
}

auto SOCCER::update_match_data(
    _In_ ocr_referee_class::frame_result_struct frame_data, _In_ cv::Mat &image)
    -> void {
  if (matches_data.size() == 0 ||
      frame_data.frame_number >
          matches_data[matches_data.size() - 1].frame_number + 10) {
    matches_data.push_back(initial_match_result_struct(frame_data, image));
  } else {
    matches_data[matches_data.size() - 1].frame_number =
        frame_data.frame_number;
    matches_data[matches_data.size() - 1].all_frames_results.push_back(
        frame_data);
  }
}

auto SOCCER::extract_game_results() -> void {
  ocr_referee_class ocr_object;

  for (int i = 0; i < matches_data.size(); i++) {
    int min_frames = get_env_int("SOCCER_GLOBAL_MIN_FRAMES");
    if (!matches_data[i].ready || matches_data[i].extracted ||
        matches_data[i].all_frames_results.size() < min_frames) {
      continue;
    }
    ocr_referee_class::frame_result_struct temp_frame_result;
    ocr_object.voting_over_results_and_names(
        temp_frame_result, matches_data[i].all_frames_results);

    if (get_env_boolean("SIMILARITY_USE_FOR_TEAM_NAMES")) {
      std::string path = get_env_string("SIMILAR_STRINGS_FILE_PATH");
      temp_frame_result.away_name.text =
          get_nearest_string(temp_frame_result.away_name.text, path);
      temp_frame_result.home_name.text =
          get_nearest_string(temp_frame_result.home_name.text, path);
    }

    matches_data[i].extracted = true;
    matches_data[i].away_name = temp_frame_result.away_name;
    matches_data[i].away_result = temp_frame_result.away_result;
    matches_data[i].home_name = temp_frame_result.home_name;
    matches_data[i].home_result = temp_frame_result.home_result;
    matches_data[i].home_penalty_result = temp_frame_result.home_penalty_result;
    matches_data[i].away_penalty_result = temp_frame_result.away_penalty_result;
  }
}

auto SOCCER::get_matches_data()
    -> std::vector<ocr_referee_class::match_result_struct> {
  return matches_data;
}

auto SOCCER::get_stat_map() -> std::map<std::string, std::string> {
  return stat_map;
}
