#include "w_utilities.hpp"

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <opencv2/opencv.hpp>
#include <sstream>
#include <string>
#include <vector>

// #include "spdlog/sinks/basic_file_sink.h"
// #include "spdlog/sinks/stdout_color_sinks.h"
// #include "spdlog/spdlog.h"
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

namespace wolf::ml::ocr {

auto get_nearest_string(_In_ std::string pInput, std::string pFilePath)
    -> std::string {
  // LOG_P(w_log_type::W_LOG_INFO, "path to similar strings file: %s",
  // pFilePath);
  std::ifstream similar_strings(pFilePath);
  float threshold = get_env_float("SIMILARITY_THRESHOLD");
  // LOG_P(w_log_type::W_LOG_INFO, "similarity threshold: %f", threshold);
  std::string candidate_string;
  float best_similarity = 0;
  std::string most_similar;

  if (pInput.length() == 0) {
    return pInput;
  }

  while (std::getline(similar_strings, candidate_string)) {
    float similarity =
        normalized_levenshtein_similarity(pInput, candidate_string);
    if (similarity > best_similarity) {
      most_similar = candidate_string;
      best_similarity = similarity;
    }
  }

  if (best_similarity > threshold) {
    return most_similar;
  } else {
    return pInput;
  }
}

auto get_nearest_string(_In_ std::string pInput,
                        _In_ std::map<std::string, std::string> pMap)
    -> std::string

{
  float threshold = get_env_float("SIMILARITY_THRESHOLD_STAT");
  // LOG_P(w_log_type::W_LOG_INFO, "similarity threshold: %f", threshold);
  std::string candidate_string;
  float best_similarity = 0;
  std::string most_similar;

  if (pInput.length() == 0) {
    return pInput;
  }

  for (auto it = pMap.begin(); it != pMap.end(); it++) {
    candidate_string = it->first;
    float similarity =
        normalized_levenshtein_similarity(pInput, candidate_string);
    if (similarity > best_similarity) {
      most_similar = candidate_string;
      best_similarity = similarity;
    }
  }

  if (best_similarity > threshold) {
    return most_similar;
  } else {
    return "";
  }
}

std::string get_value_from_json_file_by_key(std::string pJsonFilePath,
                                            std::string pKey) {
#ifdef __TELEMETRY
  auto span = get_tracer()->StartSpan("get_value_from_json_file_by_key");
#endif
  using namespace rapidjson;

  std::ifstream ifs{pJsonFilePath};
  if (!ifs.is_open()) {
    fs::path cwd = fs::current_path();
    // spdlog::error("current path {}", cwd.string());
    // spdlog::error("Could not open {} file for reading!", pJsonFilePath);
    std::exit(ENOENT);
  }

  IStreamWrapper isw{ifs};

  Document doc{};
  doc.ParseStream(isw);
  std::string out = doc[pKey.c_str()].GetString();

  return out;
}

std::vector<int>
line_of_numbers_in_string_to_vector_of_integers(std::string pVariable) {
#ifdef __TELEMETRY
  auto span = get_tracer()->StartSpan(
      "line_of_numbers_in_string_to_vector_of_integers");
#endif
  std::vector<int> result;

  std::vector<std::string> temp = split_string(pVariable, ',');
  for (int i = 0; i < temp.size(); i++) {
    result.push_back(std::stoi(temp[i]));
  }

  return result;
}

auto normalized_levenshtein_similarity(_In_ const std::string &s1,
                                       _In_ const std::string &s2) -> float {
  const size_t m = s1.size();
  const size_t n = s2.size();
  int distance;
  if (m == 0 && n == 0)
    return 0;
  else if (m == 0)
    distance = n;
  else if (n == 0)
    distance = m;
  else {
    std::vector<size_t> costs(n + 1);
    std::iota(costs.begin(), costs.end(), 0);
    size_t i = 0;
    for (auto c1 : s1) {
      costs[0] = i + 1;
      size_t corner = i;
      size_t j = 0;
      for (auto c2 : s2) {
        size_t upper = costs[j + 1];
        costs[j + 1] = (c1 == c2)
                           ? corner
                           : 1 + std::min(std::min(upper, corner), costs[j]);
        corner = upper;
        ++j;
      }
      ++i;
    }
    distance = costs[n];
  }
  float max = std::max(s1.length(), s2.length());
  float normalized_distance = distance / max;
  return 1 - normalized_distance;
}

auto read_text_file_line_by_line(_In_ std::string pFilePath)
    -> std::vector<std::string> {
#ifdef __TELEMETRY
  auto span =
      trace::Scope(get_tracer()->StartSpan("read_text_file_line_by_line"));
#endif
  std::vector<std::string> lines;
  std::string line;

  std::ifstream input_file(pFilePath);
  if (!input_file.is_open()) {
    std::cerr << "Could not open the file - '" << pFilePath << "'" << std::endl;
    return lines;
  }

  while (std::getline(input_file, line)) {
    lines.push_back(line);
  }

  input_file.close();
  return lines;
}

bool replace_string(std::string &str, const std::string &from,
                    const std::string &to) {
  size_t start_pos = str.find(from);
  if (start_pos == std::string::npos)
    return false;
  str.replace(start_pos, from.length(), to);
  return true;
}

std::vector<std::string> split_string(std::string input_string,
                                      char reference) {
#ifdef __TELEMETRY
  auto scoped_span = trace::Scope(get_tracer()->StartSpan("split_string"));
  // auto scoped_span = get_tracer()->StartSpan("split_string");
#endif
  std::stringstream test(input_string);
  std::string segment;
  std::vector<std::string> seglist;

  while (std::getline(test, segment, reference)) {
    seglist.push_back(segment);
  }

  return seglist;
}

bool string_2_boolean(std::string pVariable) {
#ifdef __TELEMETRY
  auto span = get_tracer()->StartSpan("string_2_boolean");
#endif
  bool result;
  std::transform(pVariable.begin(), pVariable.end(), pVariable.begin(),
                 ::tolower);

  if (pVariable.compare("true") == 0 || pVariable.compare("false") == 0) {
    std::istringstream is(pVariable);
    is >> std::boolalpha >> result;
  } else {
    throw std::runtime_error(
        "Invalid input, the input must be 'true' or 'false' not " + pVariable);
  }

  return result;
}

auto store_image_in_folder(
    _In_ std::vector<w_referee::match_result_struct> &pVideoResult,
    _In_ std::string pOutputImageFolderPath, _In_ std::string pVideoPath)
    -> void {
#ifdef __TELEMETRY
  auto span = get_tracer()->StartSpan("store_image_in_folder");
#endif
  fs::path temp_video_path = pVideoPath;
  std::string temp_name = temp_video_path.filename().string();
  std::string video_name = split_string(temp_name, '.')[0];

  for (size_t i = 0; i < pVideoResult.size(); i++) {
    fs::path out_path = pOutputImageFolderPath + "/" + video_name + "_" +
                        std::to_string(i) + ".png";
    cv::imwrite(out_path.string().c_str(), pVideoResult[i].result_image);
    cv::waitKey(300);
    pVideoResult[i].release();
  }

  return;
}

void write_in_file_append(std::string file_path, std::string content) {
#ifdef __TELEMETRY
  auto scoped_span =
      trace::Scope(get_tracer()->StartSpan("write_in_file_append"));
  // auto scoped_span = get_tracer()->StartSpan("write_in_file_append");
#endif

  std::ofstream file;

  file.open(file_path, std::ios_base::app); // append instead of overwrite
  file << content << std::endl;

  file.close();
  return;
}

void write_in_file(std::string file_path, std::string content) {
#ifdef __TELEMETRY
  auto scoped_span =
      trace::Scope(get_tracer()->StartSpan("write_in_file_append"));
  // auto scoped_span = get_tracer()->StartSpan("write_in_file_append");
#endif

  std::ofstream file;

  file.open(file_path); // overwrite
  file << content << std::endl;

  file.close();
  return;
}

auto write_results_in_file(
    _In_ std::vector<w_referee::match_result_struct> &pVideoResult,
    _In_ std::string pOutputTextPath) -> void {
#ifdef __TELEMETRY
  auto span = trace::Scope(get_tracer()->StartSpan("write_results_in_file"));
#endif
  for (size_t i = 0; i < pVideoResult.size(); i++) {
    if (pVideoResult[i].home_penalty_result.text.compare("") != 0 &&
        pVideoResult[i].away_penalty_result.text.compare("") != 0) {
      write_in_file_append(pOutputTextPath,
                           pVideoResult[i].stat + "," +
                               pVideoResult[i].home_name.text + "," +
                               pVideoResult[i].home_result.text + "," +
                               pVideoResult[i].away_name.text + "," +
                               pVideoResult[i].away_result.text + "," +
                               pVideoResult[i].home_penalty_result.text + "," +
                               pVideoResult[i].away_penalty_result.text + "," +
                               std::to_string(pVideoResult[i].frame_number));
    } else {
      write_in_file_append(pOutputTextPath,
                           pVideoResult[i].stat + "," +
                               pVideoResult[i].home_name.text + "," +
                               pVideoResult[i].home_result.text + "," +
                               pVideoResult[i].away_name.text + "," +
                               pVideoResult[i].away_result.text + "," +
                               std::to_string(pVideoResult[i].frame_number));
    }
  }

  return;
}

auto is_line_contains_variable(const std::string pStr) -> bool {
#ifdef __TELEMETRY
  auto span =
      trace::Scope(get_tracer()->StartSpan("is_line_contains_variable"));
#endif
  bool decision = false;
  if (pStr.size() > 0) {
    if (pStr.at(0) != '#' && pStr.size() > 2) {
      decision = true;
    }
  }
  return decision;
}

auto set_env(_In_ const char *pDotEnvFilePath) -> void {
#ifdef __TELEMETRY
  auto span = trace::Scope(get_tracer()->StartSpan("set_env"));
#endif
  std::string env_file_path(pDotEnvFilePath);
  auto lines = read_text_file_line_by_line(env_file_path);

  std::vector<std::vector<std::string>> env_vector;
  for (int i = 0; i < lines.size(); i++) {
    if (is_line_contains_variable(lines[i])) {
      env_vector.push_back(split_string(lines[i], '='));
    }
  }

  for (int i = 0; i < env_vector.size(); i++) {
#ifdef _WIN32
    _putenv_s(env_vector[i][0].c_str(), env_vector[i][1].c_str());
#else
    setenv(env_vector[i][0].c_str(), env_vector[i][1].c_str(), 1);
#endif
  }
}

auto get_env_int(_In_ const char *pKey) -> int {
  int value = -1;
  if (const char *env_p = getenv(pKey)) {
    std::string temp(env_p);
    value = std::stoi(temp);
  } else {
    // TODO add log
  }

  return value;
}

auto get_env_float(_In_ const char *pKey) -> float {
  float value = -1;
  if (const char *env_p = getenv(pKey)) {
    std::string temp(env_p);
    value = std::stof(temp);
  } else {
    // TODO add log
  }

  return value;
}

auto get_env_boolean(_In_ const char *pKey) -> bool {
  bool value = false;
  if (const char *env_p = getenv(pKey)) {
    std::string temp(env_p);
    value = string_2_boolean(temp);
  } else {
    // TODO add log
  }

  return value;
}

auto get_env_string(_In_ const char *pKey) -> std::string {
  std::string value;
  if (const char *env_p = getenv(pKey)) {
    value = std::string(env_p);
  } else {
    // TODO add log
  }

  return value;
}

auto get_env_cv_rect(_In_ const char *pKey) -> cv::Rect {
  cv::Rect value = cv::Rect(0, 0, 0, 0);
  if (const char *env_p = getenv(pKey)) {
    std::string temp(env_p);
    std::vector<int> int_vect =
        line_of_numbers_in_string_to_vector_of_integers(temp);
    value = cv::Rect(int_vect[0], int_vect[1], int_vect[2], int_vect[3]);
  } else {
    // TODO add log
  }

  return value;
}

auto get_relative_path_to_root() -> std::string {
  fs::path cwd = fs::current_path();
  fs::path dot_env_file_path;
  if (cwd.parent_path().filename().compare("build") == 0) {
    dot_env_file_path = "../../";
  } else if (cwd.filename().compare("build") == 0 ||
             cwd.filename().compare("ocr") == 0) {
    dot_env_file_path = "../";
  } else if (cwd.parent_path().parent_path().filename().compare("build") == 0) {
    dot_env_file_path = "../../../";
  } else {
    dot_env_file_path = "";
  }

  std::string temp = dot_env_file_path.string();

  return temp;
}

auto get_first_character_of_string(_In_ std::string pStr, _In_ bool pEscape)
    -> std::string {
  if (pEscape || pStr.length() == 0) {
    return pStr;
  }

  char first_char = pStr[0];
  std::string result(1, first_char);

  return result;
}
} // namespace wolf::ml::ocr
