#include "w_common.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

namespace wolf::ml {

auto write_in_file_append(_In_ std::string pFilePath, _In_ std::string pContent) -> void {
  std::ofstream file;

  file.open(pFilePath, std::ios_base::app);  // append instead of overwrite
  file << pContent << std::endl;

  file.close();
}

auto read_text_file_line_by_line(_In_ std::string pFilePath) -> std::vector<std::string> {
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

auto split_string(_In_ std::string input_string, _In_ char reference) -> std::vector<std::string> {
  std::stringstream test(input_string);
  std::string segment;
  std::vector<std::string> seglist;

  while (std::getline(test, segment, reference)) {
    seglist.push_back(segment);
  }

  return seglist;
}

auto string_2_boolean(_In_ std::string pVariable) -> bool {
  bool result;
  std::transform(pVariable.begin(), pVariable.end(), pVariable.begin(), ::tolower);

  if (pVariable.compare("true") == 0 || pVariable.compare("false") == 0) {
    std::istringstream is(pVariable);
    is >> std::boolalpha >> result;
  } else {
    throw std::runtime_error("Invalid input, the input must be 'true' or 'false' not " + pVariable);
  }

  return result;
}

auto is_line_contains_variable(const std::string pStr) -> bool {
  bool decision = false;
  if (pStr.size() > 0) {
    if (pStr.at(0) != '#' && pStr.size() > 2) {
      decision = true;
    }
  }
  return decision;
}

auto set_env(_In_ const char* pDotEnvFilePath) -> void {
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

auto get_env_int(_In_ const char* pKey) -> int {
  int value = -1;
  if (const char* env_p = getenv(pKey)) {
    std::string temp(env_p);
    value = std::stoi(temp);
  } else {
    // TODO add log
  }

  return value;
}

auto get_env_float(_In_ const char* pKey) -> float {
  float value = -1;
  if (const char* env_p = getenv(pKey)) {
    std::string temp(env_p);
    value = std::stof(temp);
  } else {
    // TODO add log
  }

  return value;
}

auto get_env_boolean(_In_ const char* pKey) -> bool {
  bool value = false;
  if (const char* env_p = getenv(pKey)) {
    std::string temp(env_p);
    value = string_2_boolean(temp);
  } else {
    // TODO add log
  }

  return value;
}

auto get_env_string(_In_ const char* pKey) -> std::string {
  std::string value;
  if (const char* env_p = getenv(pKey)) {
    value = std::string(env_p);
  } else {
    // TODO add log
  }

  return value;
}

}  // namespace wolf::ml