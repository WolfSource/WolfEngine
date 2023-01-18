#include "ocr_engine.h"

#include "DISABLE_ANALYSIS_BEGIN"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include <cctype>
#include <cstring>
#include <filesystem>

#include "DISABLE_ANALYSIS_END"

#include "utilities.h"
namespace fs = std::filesystem;

#ifdef __TELEMETRY
#include "opentelemetry/sdk/version/version.h"
#include "opentelemetry/trace/provider.h"

namespace trace = opentelemetry::trace;
namespace nostd = opentelemetry::nostd;

namespace {
nostd::shared_ptr<trace::Tracer> get_tracer() {
  auto provider = trace::Provider::GetTracerProvider();
  return provider->GetTracer("pes_21", OPENTELEMETRY_SDK_VERSION);
}
} // namespace
#endif

OCR::OCR() {
  std::string key = "TESSERACT_LOG";
  std::string tesseract_log = get_env_string(key.c_str());

  digit_api->Init(nullptr, "eng", tesseract::OEM_LSTM_ONLY);
  digit_api->SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
  digit_api->SetVariable("tessedit_char_whitelist", "0123456789");
  digit_api->SetVariable("user_defined_dpi", "70");
  digit_api->SetVariable("debug_file", tesseract_log.c_str());

  word_api->Init(nullptr, "eng", tesseract::OEM_LSTM_ONLY);
  word_api->SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
  word_api->SetVariable(
      "tessedit_char_whitelist",
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"); //	,"ABCDEFGHIJKLMNOPQRSTUVWXYZ");
                                                               //// ,
  word_api->SetVariable("user_defined_dpi", "70");
  word_api->SetVariable("debug_file", tesseract_log.c_str());
}

OCR::~OCR() {
  digit_api->End();
  word_api->End();
}

bool OCR::check_if_overlapped(_In_ cv::Rect box_1, _In_ cv::Rect box_2,
                              _In_ config_for_ocr_struct &ocr_config) {
  bool if_overlapped = false;
  int area_1, area_2, overlapped_area;

  int dx = std::min(box_1.x + box_1.width, box_2.x + box_2.width) -
           std::max(box_1.x, box_2.x);
  int dy = std::min(box_1.y + box_1.height, box_2.y + box_2.height) -
           std::max(box_1.y, box_2.y);

  if (dx > 0 && dy > 0) {
    area_1 = box_1.width * box_1.height;
    area_2 = box_2.width * box_2.height;

    overlapped_area = dx * dy;

    if (double(overlapped_area) / double(area_1) >
            ocr_config.overlapped_threshold ||
        double(overlapped_area) / double(area_2) >
            ocr_config.overlapped_threshold) {
      if_overlapped = true;
    }
  }

  return if_overlapped;
}

// this function is related to cluster_char_structs function
bool compare_char_by_x_position(
    const OCR::characters_struct &first_charactor,
    const OCR::characters_struct &second_charactor) {
  return first_charactor.center.x < second_charactor.center.x;
}

std::vector<OCR::characters_struct> OCR::contours_to_char_structs(
    _In_ std::vector<std::vector<cv::Point>> contours) {
  std::vector<characters_struct> modified_contours;
  size_t number_of_contours = contours.size();

  for (size_t i = 0; i < number_of_contours; i++) {
    std::vector<cv::Point> contour_poly;
    characters_struct temp_modified_contour;
    temp_modified_contour.contour = contours[i];

    double epsilon = 3;
    bool closed = true;
    cv::approxPolyDP(cv::Mat(contours[i]), contour_poly, epsilon, closed);

    temp_modified_contour.bound_rect = cv::boundingRect(cv::Mat(contour_poly));

    contour_poly.clear();
    temp_modified_contour.center.x = temp_modified_contour.bound_rect.x +
                                     temp_modified_contour.bound_rect.width / 2;

    temp_modified_contour.center.y =
        temp_modified_contour.bound_rect.y +
        temp_modified_contour.bound_rect.height / 2;

    temp_modified_contour.height = temp_modified_contour.bound_rect.height;

    modified_contours.push_back(temp_modified_contour);
  }
  return modified_contours;
}

void OCR::enhance_contour_image_for_model(
    _Inout_ cv::Mat &contour_image, _In_ config_for_ocr_struct &ocr_config) {
  if (!(ocr_config.make_white_background || ocr_config.do_resize_contour)) {
    return;
  }

  if (ocr_config.make_white_background) {
    make_contour_white_background(contour_image, ocr_config);
  }

  if (ocr_config.do_resize_contour) {
    // float resize_fraction =
    // float(ocr_config.desired_contour_height)/float(height);
    int dist_height = ocr_config.desired_contour_height;
    int dist_width = 24; // int(resize_fraction*width);

    cv::resize(contour_image, contour_image, cv::Size(dist_width, dist_height),
               0.0, 0.0, cv::InterpolationFlags::INTER_AREA);
  }

  return;
}

double OCR::euclidean_distance(characters_struct &first_character,
                               characters_struct &second_character) {
  double dist_x = std::pow(
      float(first_character.center.x - second_character.center.x), 2.0);
  double dist_y = std::pow(
      float(first_character.center.y - second_character.center.y), 2.0);
  double dist = std::pow(dist_x + dist_y, 0.5);

  return dist;
}

auto OCR::euclidean_distance(int x1, int x2, int y1, int y2) -> double {
  double dist_x = std::pow(float(x1 - x2), 2.0);
  double dist_y = std::pow(float(y1 - y2), 2.0);
  double dist = std::pow(dist_x + dist_y, 0.5);

  return dist;
}

auto OCR::spaces_between_two_chars(characters_struct left_char,
                                   characters_struct right_char,
                                   float height_to_dist_ratio) -> std::string {
  std::string temp_spaces = "";

  int left_char_right_corner =
      left_char.bound_rect.x + left_char.bound_rect.width;
  int right_char_left_corner = right_char.bound_rect.x;

  if (right_char_left_corner - left_char_right_corner > 0) {
    if (float(right_char_left_corner - left_char_right_corner) >
        float(left_char.bound_rect.height) * height_to_dist_ratio) {
      temp_spaces = "  ";
    } else {
      temp_spaces = " ";
    }
  }

  return temp_spaces;
}

std::vector<OCR::character_and_center> OCR::char_clusters_to_text(
    std::vector<std::vector<characters_struct>> clustered_characters) {
  std::vector<OCR::character_and_center> words;

  for (size_t i = 0; i < clustered_characters.size(); i++) {
    std::sort(clustered_characters[i].begin(), clustered_characters[i].end());
    character_and_center temp;
    temp.center = clustered_characters[i][0].center;

    std::string spaces = "";
    float height_to_dist_ratio =
        get_env_float("SOCCER_GLOBAL_HEIGHT_TO_DIST_RATIO");

    for (size_t j = 0; j < clustered_characters[i].size(); j++) {
      std::string temp_string =
          split_string(clustered_characters[i][j].text, '\n')[0];
      if (j < clustered_characters[i].size() - 1) {
        spaces = spaces_between_two_chars(clustered_characters[i][j],
                                          clustered_characters[i][j + 1],
                                          height_to_dist_ratio);
      }
      temp_string += spaces;
      temp.text.append(temp_string);
    }

    std::transform(temp.text.begin(), temp.text.end(), temp.text.begin(),
                   ::toupper);

    words.push_back(temp);
  }

  clustered_characters.clear();

  std::sort(words.begin(), words.end());
  return words;
}

std::vector<OCR::characters_struct> OCR::filter_chars_by_contour_size(
    _Inout_ std::vector<characters_struct> &character,
    _In_ config_for_ocr_struct &ocr_config) {
  std::vector<characters_struct> filtered_characters;
  for (int i = 0; i < character.size(); i++) {
    double area = cv::contourArea(character[i].contour);
    if (area < ocr_config.restrictions.min_area ||
        area > ocr_config.restrictions.max_area) {
      continue;
    }
    if (character[i].bound_rect.height < ocr_config.restrictions.min_height ||
        character[i].bound_rect.height > ocr_config.restrictions.max_height ||
        character[i].bound_rect.width < ocr_config.restrictions.min_width ||
        character[i].bound_rect.width > ocr_config.restrictions.max_width) {
      continue;
    }
    filtered_characters.push_back(character[i]);
  }
  return filtered_characters;
}

std::vector<OCR::characters_struct>
OCR::image_to_char_structs(_In_ cv::Mat &image_box,
                           _In_ config_for_ocr_struct &ocr_config) {
  cv::Mat filtered_image =
      prepare_image_for_contour_detection(image_box, ocr_config);

  std::vector<std::vector<cv::Point>> contours =
      find_all_countors(filtered_image);
  std::vector<characters_struct> characters =
      contours_to_char_structs(contours);

  std::vector<characters_struct> filtered_characters =
      filter_chars_by_contour_size(characters, ocr_config);

  merge_overlapped_contours(filtered_characters, ocr_config);

  for (size_t i = 0; i < filtered_characters.size(); i++) {
    margin_bounding_rect(filtered_characters[i].bound_rect, ocr_config.margin,
                         filtered_image);
  }

  // TODO add this log "This code has not been optimized for color image, yet"
  // << std::endl;
  return filtered_characters;
}

auto OCR::char_vec_to_string(
    _In_ std::vector<OCR::characters_struct> char_vector, _In_ cv::Mat &frame,
    _In_ config_for_ocr_struct &ocr_config)
    -> std::vector<character_and_center> {
  std::vector<OCR::characters_struct> labeled_characters =
      label_chars_in_char_structs(char_vector, frame, ocr_config);
  std::vector<std::vector<OCR::characters_struct>> clustered_characters =
      cluster_char_structs(labeled_characters, ocr_config);
  std::vector<OCR::character_and_center> string =
      char_clusters_to_text(clustered_characters);

  return string;
}

std::vector<OCR::character_and_center>
OCR::image_to_string(_In_ cv::Mat &image,
                     _In_ config_for_ocr_struct &ocr_config) {
  std::vector<characters_struct> characters =
      image_to_char_structs(image, ocr_config);
  std::vector<characters_struct> labeled_characters =
      label_chars_in_char_structs(characters, image, ocr_config);
  std::vector<std::vector<characters_struct>> clustered_characters =
      cluster_char_structs(labeled_characters, ocr_config);
  std::vector<character_and_center> string =
      char_clusters_to_text(clustered_characters);

  return string;
}

std::vector<OCR::characters_struct> OCR::label_chars_in_char_structs(
    _In_ std::vector<OCR::characters_struct> &characters,
    _In_ cv::Mat &image_box, _In_ config_for_ocr_struct &ocr_config) {
  std::vector<characters_struct> labeled_chars;
  tesseract::TessBaseAPI *tess_api;
  if (ocr_config.is_digit) {
    tess_api = digit_api;
  } else {
    tess_api = word_api;
  }

  for (size_t i = 0; i < characters.size(); i++) {
    cv::Mat temp_contour_image;
    cv::Mat contour_image;
    // Sometimes it is better to use the original image for OCR
    if (ocr_config.binary) {
      cv::Mat filtered_image =
          prepare_image_for_contour_detection(image_box, ocr_config);
      filtered_image(characters[i].bound_rect).copyTo(contour_image);
    } else {
      // original_image(modified_bounding_rects[i].bound_rect).copyTo(contour_image);
      contour_image = mask_contour(image_box, characters[i]);
    }

    if (ocr_config.is_white) {
      negative_image(contour_image);
    }

    if (ocr_config.verbose) {
      temp_contour_image = contour_image.clone();
    }

    enhance_contour_image_for_model(contour_image, ocr_config);
    tess_api->SetImage(contour_image.data, contour_image.cols,
                       contour_image.rows, 3, int(contour_image.step));

    std::string text_data = tess_api->GetUTF8Text();

    if (std::strcmp(text_data.c_str(), "") != 0) {
      characters_struct temp_character;
      temp_character = characters[i];
      temp_character.text = split_string(text_data, '\n')[0];

      if (!temp_character.text.empty()) {
        temp_character.text = temp_character.text.substr(0, 1);
      }

      labeled_chars.push_back(temp_character);
    }
    contour_image.release();
    temp_contour_image.release();
  }
  return labeled_chars;
}

void OCR::margin_bounding_rect(_Inout_ cv::Rect &bounding_rect, _In_ int margin,
                               _In_ cv::Mat &filtered_image) {
  int height = filtered_image.rows;
  int width = filtered_image.cols;
  int temp;
  int temp_margin_width = int(std::ceil(float(margin) / 2));
  int temp_margin_height = int(std::ceil(float(margin) / 2));

  if (bounding_rect.width < 1 && margin > 0) {
    temp_margin_width = 2;
  }

  if (bounding_rect.width < bounding_rect.height / 5) {
    // temp = (bounding_rect.x - 1 * bounding_rect.width);
    // bounding_rect.x = temp > 0 ? temp : 0;
    // temp = (bounding_rect.x + 4 * bounding_rect.width);
    // bounding_rect.width = temp < width ? 4 * bounding_rect.width : (width -
    // bounding_rect.x - 1);
    temp = (bounding_rect.x - 1 * bounding_rect.height / 6);
    bounding_rect.x = temp > 0 ? temp : 0;
    temp = (bounding_rect.x + bounding_rect.height);
    bounding_rect.width =
        temp < width ? bounding_rect.height / 2 : (width - bounding_rect.x - 1);
  } else {
    temp = (bounding_rect.x - temp_margin_width);
    bounding_rect.x = temp > 0 ? temp : 0;
    temp = (bounding_rect.x + bounding_rect.width + 3 * temp_margin_width);
    bounding_rect.width = temp < width
                              ? bounding_rect.width + 3 * temp_margin_width
                              : (width - bounding_rect.x - 1);
  }

  temp = (bounding_rect.y - temp_margin_height);
  bounding_rect.y = temp > 0 ? temp : 0;
  temp = (bounding_rect.y + bounding_rect.height + 2 * temp_margin_height);
  bounding_rect.height = temp < height
                             ? bounding_rect.height + 2 * temp_margin_height
                             : (height - bounding_rect.y - 1);
}

cv::Mat OCR::mask_contour(_In_ cv::Mat &image,
                          _In_ characters_struct &contour_info) {
  cv::Mat temp_plane_image =
      cv::Mat(cv::Size(image.cols, image.rows), CV_8UC1, cv::Scalar(0));
  cv::Mat mask_image;
  cv::Mat contour_image;

  std::vector<std::vector<cv::Point>> temp_contours;
  temp_contours.push_back(contour_info.contour);
  std::vector<std::vector<cv::Point>> hull(temp_contours.size());
  for (unsigned int i = 0, n = temp_contours.size(); i < n; ++i) {
    cv::convexHull(cv::Mat(temp_contours[i]), hull[i], false);
  }
  cv::drawContours(temp_plane_image, temp_contours, 0, cv::Scalar(255), 3);
  cv::fillPoly(temp_plane_image, temp_contours, cv::Scalar(255));

  temp_plane_image(contour_info.bound_rect).copyTo(mask_image);
  image(contour_info.bound_rect).copyTo(contour_image, mask_image);

  temp_plane_image.release();
  mask_image.release();
  return contour_image;
}

void OCR::merge_overlapped_contours(
    _Inout_ std::vector<characters_struct> &character,
    _In_ config_for_ocr_struct &ocr_config) {
  cv::Rect ref_box;
  int index;
  std::vector<int> overlapped_boxes_index;
  // int width, height;

  bool flag;
  if (character.size() > 0) {
    flag = true;
  } else {
    flag = false;
  }

  index = 0;
  while (flag) {
    ref_box = character[index].bound_rect;

    overlapped_boxes_index.clear();
    for (int i = 0; i < character.size(); i++) {
      if (i == index) {
        continue;
      }
      if (check_if_overlapped(ref_box, character[i].bound_rect, ocr_config)) {
        overlapped_boxes_index.push_back(i);
      }
    }

    for (int j = overlapped_boxes_index.size() - 1; j >= 0; j--) {
      character[index].bound_rect.x =
          std::min(character[index].bound_rect.x,
                   character[overlapped_boxes_index[j]].bound_rect.x);
      character[index].bound_rect.y =
          std::min(character[index].bound_rect.y,
                   character[overlapped_boxes_index[j]].bound_rect.y);
      character[index].bound_rect.width =
          std::max(character[index].bound_rect.x +
                       character[index].bound_rect.width,
                   character[overlapped_boxes_index[j]].bound_rect.x +
                       character[overlapped_boxes_index[j]].bound_rect.width) -
          character[index].bound_rect.x;
      character[index].bound_rect.height =
          std::max(character[index].bound_rect.y +
                       character[index].bound_rect.height,
                   character[overlapped_boxes_index[j]].bound_rect.y +
                       character[overlapped_boxes_index[j]].bound_rect.height) -
          character[index].bound_rect.y;
      character.erase(character.begin() + int(overlapped_boxes_index[j]));
    }

    overlapped_boxes_index.clear();
    if (index >= character.size() - 1) {
      flag = false;
    }

    index++;
  }
}

std::vector<std::vector<OCR::characters_struct>>
OCR::cluster_char_structs(std::vector<OCR::characters_struct> characters,
                          config_for_ocr_struct &ocr_config) {
  std::vector<std::vector<characters_struct>> clustered_characters;

  if (characters.size() == 0) {
    return clustered_characters;
  }

  std::vector<characters_struct> temp_char_cluster;
  std::vector<size_t> temp_index_list;
  bool is_clustering;

  temp_char_cluster.push_back(characters.back());
  characters.pop_back();

  if (characters.size() > 0) {
    is_clustering = true;

    while (is_clustering) {
      for (size_t index = 0; index < characters.size(); index++) {
        for (size_t i = 0; i < temp_char_cluster.size(); i++) {
          if (~temp_char_cluster[i].processed) {
            // double temp_dist = euclidean_distance(temp_char_cluster[i],
            // characters[index]);

            double temp_dist_1 =
                euclidean_distance(temp_char_cluster[i].bound_rect.x,
                                   characters[index].bound_rect.x +
                                       characters[index].bound_rect.width,
                                   temp_char_cluster[i].bound_rect.y +
                                       temp_char_cluster[i].bound_rect.height,
                                   characters[index].bound_rect.y +
                                       characters[index].bound_rect.height);
            double temp_dist_2 =
                euclidean_distance(temp_char_cluster[i].bound_rect.x +
                                       temp_char_cluster[i].bound_rect.width,
                                   characters[index].bound_rect.x,
                                   temp_char_cluster[i].bound_rect.y +
                                       temp_char_cluster[i].bound_rect.height,
                                   characters[index].bound_rect.y +
                                       characters[index].bound_rect.height);

            int temp_y_dist = std::abs(temp_char_cluster[i].bound_rect.y -
                                       characters[index].bound_rect.y);

            if ((temp_dist_1 < 0.8 * double(temp_char_cluster[i].height) ||
                 temp_dist_2 < 0.8 * double(temp_char_cluster[i].height)) &&
                temp_y_dist < temp_char_cluster[i].height) {
              temp_index_list.push_back(index);
              break;
            }
          }
        }
      }

      for (size_t i = 0; i < temp_char_cluster.size(); i++) {
        temp_char_cluster[i].processed = true;
      }

      if (temp_index_list.size() > 0) {
        std::reverse(temp_index_list.begin(), temp_index_list.end());

        for (size_t i = 0; i < temp_index_list.size(); i++) {
          temp_char_cluster.push_back(characters[temp_index_list[i]]);
          characters.erase(characters.begin() + int(temp_index_list[i]));
        }

        if (characters.size() == 0) {
          clustered_characters.push_back(temp_char_cluster);
          temp_char_cluster.clear();
        }
      } else {
        clustered_characters.push_back(temp_char_cluster);
        temp_char_cluster.clear();

        temp_char_cluster.push_back(characters.back());
        characters.pop_back();
        if (characters.size() == 0) {
          clustered_characters.push_back(temp_char_cluster);
          temp_char_cluster.clear();
        }
      }

      if (characters.size() == 0) {
        is_clustering = false;
      }

      temp_index_list.clear();
    }
  } else {
    clustered_characters.push_back(temp_char_cluster);
    temp_char_cluster.clear();
    // return;
  }
  return clustered_characters;
}

cv::Mat OCR::show_in_better_way(cv::Mat &input_image, int out_put_image_height,
                                float resize_factor) {
  int height = out_put_image_height;
  int width = height * 4 / 3;
  cv::Mat temp_image;
  if (resize_factor * input_image.rows > height) {
    resize_factor = float(height / input_image.rows);
  }
  if (resize_factor * input_image.cols > width) {
    resize_factor = float(width / input_image.cols);
  }
  if (input_image.channels() == 3) {
    temp_image = cv::Mat(cv::Size(width, height), CV_8UC3, cv::Scalar(0, 0, 0));
  } else {
    temp_image = cv::Mat(cv::Size(width, height), CV_8UC1, cv::Scalar(0));
  }

  cv::Mat temp_contour = input_image.clone();
  cv::resize(temp_contour, temp_contour,
             cv::Size(input_image.cols * resize_factor,
                      input_image.rows * resize_factor));
  temp_contour.copyTo(
      temp_image(cv::Rect(0, 0, input_image.cols * resize_factor,
                          input_image.rows * resize_factor)));

  temp_contour.release();
  return temp_image;
}

std::vector<std::string> OCR::split_string(std::string input_string,
                                           char reference) {
  std::stringstream test(input_string);
  std::string segment;
  std::vector<std::string> seglist;

  while (std::getline(test, segment, reference)) {
    seglist.push_back(segment);
  }

  return seglist;
}

auto OCR::same_height(_In_ std::vector<characters_struct> pClusteredChars)
    -> bool {
  bool result = true;
  int average_height = 0;

  for (int i = 0; i < pClusteredChars.size(); i++) {
    average_height += pClusteredChars[i].height;
  }
  average_height /= pClusteredChars.size();
  int min_height = average_height - average_height / 5;
  int max_height = average_height + average_height / 5;

  for (int i = 0; i < pClusteredChars.size(); i++) {
    if (pClusteredChars[i].height > max_height ||
        pClusteredChars[i].height < min_height) {
      result = false;
    }
  }

  return result;
}

auto OCR::same_level(_In_ std::vector<characters_struct> pClusteredChars)
    -> bool {
  bool result = true;
  int average_height = 0;
  int average_level = 0;

  for (int i = 0; i < pClusteredChars.size(); i++) {
    average_height += pClusteredChars[i].height;
    average_level +=
        pClusteredChars[i].bound_rect.y + pClusteredChars[i].bound_rect.height;
  }
  average_height /= pClusteredChars.size();
  average_level /= pClusteredChars.size();
  int min_level = average_level - average_height / 10;
  int max_level = average_level + average_height / 10;

  for (int i = 0; i < pClusteredChars.size(); i++) {
    if (pClusteredChars[i].bound_rect.y + pClusteredChars[i].bound_rect.height >
            max_level ||
        pClusteredChars[i].bound_rect.y + pClusteredChars[i].bound_rect.height <
            min_level) {
      result = false;
    }
  }

  return result;
}

auto OCR::show_contours(_Inout_ cv::Mat &pImage,
                        _In_ std::vector<characters_struct> pClusteredChars,
                        _In_ std::string pWindowName, _In_ bool pShow) -> void {
  cv::Mat mask_image;

  for (int i = 0; i < pClusteredChars.size(); i++) {
    std::vector<std::vector<cv::Point>> temp_contours;
    temp_contours.push_back(pClusteredChars[i].contour);
    std::vector<std::vector<cv::Point>> hull(temp_contours.size());
    for (unsigned int i = 0, n = temp_contours.size(); i < n; ++i) {
      cv::convexHull(cv::Mat(temp_contours[i]), hull[i], false);
    }
    cv::drawContours(pImage, temp_contours, 0, cv::Scalar(255), 3);
    cv::fillPoly(pImage, temp_contours, cv::Scalar(255));
  }

  if (pShow) {
    cv::imshow(pWindowName, pImage);
    cv::waitKey();
  }

  return;
}

auto OCR::fill_cluster_features(
    _Inout_ std::vector<characters_struct> &pClusteredChar,
    _In_ int pImageWidth, _In_ int pIndex) -> cluster_features {
  cluster_features features;

  features.index_in_parent_vector = pIndex;

  features.min_x = pClusteredChar[0].bound_rect.x;
  features.max_x =
      pClusteredChar[0].bound_rect.x + pClusteredChar[0].bound_rect.width;
  features.min_y = pClusteredChar[0].bound_rect.y;
  features.average_y = pClusteredChar[0].bound_rect.y;
  features.average_height = pClusteredChar[0].bound_rect.height;

  for (int i = 1; i < pClusteredChar.size(); i++) {
    features.min_x = (features.min_x > pClusteredChar[i].bound_rect.x)
                         ? pClusteredChar[i].bound_rect.x
                         : features.min_x;
    features.max_x = (features.max_x > pClusteredChar[i].bound_rect.x +
                                           pClusteredChar[i].bound_rect.width)
                         ? features.max_x
                         : pClusteredChar[i].bound_rect.x +
                               pClusteredChar[i].bound_rect.width;
    features.min_y = (features.min_y > pClusteredChar[i].bound_rect.y)
                         ? pClusteredChar[i].bound_rect.y
                         : features.min_y;
    features.average_y += pClusteredChar[0].bound_rect.y;
    features.average_height += pClusteredChar[0].bound_rect.height;
  }

  features.average_y /= pClusteredChar.size();
  features.average_height /= pClusteredChar.size();

  int mid = pImageWidth / 2;
  int temp1 = mid - features.min_x;
  int temp2 = mid - features.max_x;

  if (temp1 >= 0) {
    if (temp2 > 0) {
      features.position = cluster_position::left;
      features.symmetric_x1 = temp2;
      features.symmetric_x2 = temp1;
    } else {
      features.position = cluster_position::middle;
      features.symmetric_x1 = 0;
      features.symmetric_x2 = 0;
    }
  } else {
    if (temp2 >= 0) {
      features.position = cluster_position::middle;
      features.symmetric_x1 = 0;
      features.symmetric_x2 = 0;
    } else {
      features.position = cluster_position::right;
      features.symmetric_x1 = abs(temp1);
      features.symmetric_x2 = abs(temp2);
    }
  }

  return features;
}

auto OCR::check_twin_clusters(_In_ cluster_features &pFirstInput,
                              _In_ cluster_features &pSecondInput,
                              _In_ float pThreshold) -> bool {
  bool result = false;

  float Y_diff_ration =
      float(std::abs(pFirstInput.average_y - pSecondInput.average_y)) /
      float((pFirstInput.average_height + pSecondInput.average_height) / 2);
  float a, b, overlapped_ratio;

  float height_diff_ratio =
      (pFirstInput.average_height > pSecondInput.average_height)
          ? float(pFirstInput.average_height - pSecondInput.average_height) /
                float(pFirstInput.average_height)
          : float(pSecondInput.average_height - pFirstInput.average_height) /
                float(pSecondInput.average_height);

  if (Y_diff_ration < 0.05 && height_diff_ratio < 0.05) {
    a = (pFirstInput.symmetric_x1 < pSecondInput.symmetric_x1)
            ? float(pSecondInput.symmetric_x1)
            : float(pFirstInput.symmetric_x1);
    b = (pFirstInput.symmetric_x2 < pSecondInput.symmetric_x2)
            ? float(pFirstInput.symmetric_x2)
            : float(pSecondInput.symmetric_x2);

    overlapped_ratio =
        (b - a) / float(pFirstInput.symmetric_x2 - pFirstInput.symmetric_x1);
    float temp =
        (b - a) / float(pSecondInput.symmetric_x2 - pSecondInput.symmetric_x1);

    overlapped_ratio = (overlapped_ratio > temp) ? overlapped_ratio : temp;

    if (overlapped_ratio > pThreshold) {
      result = true;
    }
  }

  return result;
}

auto OCR::keep_twins(
    _Inout_ std::vector<std::vector<characters_struct>> &pClusteredChar,
    _In_ int pImageWidth, _In_ int pImageHeight, _In_ bool pWord) -> void {
  std::vector<cluster_features> cluster_features_vector;
  int n_cluster = pClusteredChar.size();

  for (int i = 0; i < n_cluster; i++) {
    cluster_features_vector.push_back(
        fill_cluster_features(pClusteredChar[i], pImageWidth, i));
  }

  for (int i = 0; i < n_cluster - 1; i++) {
    if (cluster_features_vector[i].matched) {
      continue;
    }
    for (int j = i + 1; j < n_cluster; j++) {
      if (cluster_features_vector[j].matched) {
        continue;
      }
      if (check_twin_clusters(cluster_features_vector[i],
                              cluster_features_vector[j], 0.6)) {
        cluster_features_vector[i].matched = true;
        cluster_features_vector[j].matched = true;

        cluster_features_vector[i].twin_index_in_parent_vector =
            cluster_features_vector[j].index_in_parent_vector;
        cluster_features_vector[j].twin_index_in_parent_vector =
            cluster_features_vector[i].index_in_parent_vector;
      }
    }
  }

  if (pWord) {
    for (int i = 1; i < n_cluster; i++) {
      if (!cluster_features_vector[i].matched) {
        continue;
      }

      if (cluster_features_vector[i].average_y > pImageHeight * 4 / 5) {
        cluster_features_vector[i].matched = false;
        cluster_features_vector[cluster_features_vector[i]
                                    .twin_index_in_parent_vector]
            .matched = false;
      }
    }

    int more = -1;

    for (int i = 1; i < n_cluster; i++) {
      if (!cluster_features_vector[i].matched) {
        continue;
      }
      if (more == -1) {
        more = pClusteredChar[i].size();
      }

      if (more < pClusteredChar[i].size()) {
        more = pClusteredChar[i].size();
      }
    }

    for (int i = 1; i < n_cluster; i++) {
      if (!cluster_features_vector[i].matched) {
        continue;
      }

      if (more > pClusteredChar[i].size() &&
          more > pClusteredChar[cluster_features_vector[i]
                                    .twin_index_in_parent_vector]
                     .size()) {
        cluster_features_vector[i].matched = false;
        cluster_features_vector[cluster_features_vector[i]
                                    .twin_index_in_parent_vector]
            .matched = false;
      }
    }
  } else {
    int largest = -1;

    for (int i = 1; i < n_cluster; i++) {
      if (!cluster_features_vector[i].matched) {
        continue;
      }
      if (largest == -1) {
        largest = cluster_features_vector[i].average_height;
      }

      if (largest < cluster_features_vector[i].average_height) {
        largest = cluster_features_vector[i].average_height;
      }
    }

    for (int i = 1; i < n_cluster; i++) {
      if (!cluster_features_vector[i].matched) {
        continue;
      }
      if (largest == -1) {
        largest = cluster_features_vector[i].average_height;
      }

      if (largest > cluster_features_vector[i].average_height &&
          largest > cluster_features_vector[cluster_features_vector[i]
                                                .twin_index_in_parent_vector]
                        .average_height) {
        cluster_features_vector[i].matched = false;
        cluster_features_vector[cluster_features_vector[i]
                                    .twin_index_in_parent_vector]
            .matched = false;
      } else {
        largest = cluster_features_vector[i].average_height;
      }
    }
  }

  for (int i = 0; i < n_cluster; i++) {
    int index = n_cluster - (i + 1);
    if (!cluster_features_vector[index].matched) {
      pClusteredChar.erase(pClusteredChar.begin() + index);
    }
  }

  return;
}

auto OCR::keep_time(
    _Inout_ std::vector<std::vector<characters_struct>> &pClusteredChar)
    -> void {
  int n_cluster = pClusteredChar.size();
  if (n_cluster == 0) {
    return;
  }
  int height = pClusteredChar[n_cluster - 1][0].bound_rect.y;

  for (int i = 1; i < n_cluster; i++) {
    int index = n_cluster - (i + 1);
    if (height < pClusteredChar[index][0].bound_rect.y) {
      pClusteredChar.erase(pClusteredChar.begin() + index);
    } else {
      height = pClusteredChar[index][0].bound_rect.y;
      pClusteredChar.pop_back();
    }
  }
}

auto OCR::add_text_to_original_image(
    _Inout_ cv::Mat &pImage,
    _In_ std::vector<characters_struct> &pClusteredChar) -> void {
  for (int i = 0; i < pClusteredChar.size(); i++) {
    cv::putText(pImage, pClusteredChar[i].text,
                cv::Point(pClusteredChar[i].bound_rect.x,
                          pClusteredChar[i].bound_rect.y + 100),
                cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 255, 0), false);
  }

  return;
}
