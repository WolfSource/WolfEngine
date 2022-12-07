#include "w_process.hpp"
#include "w_trace.hpp"

#include <fstream>

using w_process = wolf::system::w_process;

boost::leaf::result<std::filesystem::path> w_process::current_path() {
  auto _path = std::filesystem::current_path();
  return W_SUCCESS(_path);
}

boost::leaf::result<std::filesystem::path> w_process::current_exe_path() {
  using path = std::filesystem::path;

#ifdef WIN32
  std::array<wchar_t, W_MAX_PATH> _buffer = {};
  std::ignore = GetModuleFileNameW(nullptr, _buffer.data(), sizeof(_buffer));
  const auto _ret = ::GetLastError();

  if (_ret != S_OK) {
    return W_FAILURE(_ret, "GetModuleFileNameW failed because:" +
                           get_last_win_error(_ret));
  }

  auto _path = path(_buffer.data(), path::auto_format).parent_path();
  return W_SUCCESS(_path);
#else
  std::stringstream buffer;
  buffer << std::ifstream("/proc/self/comm").rdbuf();

  std::string _str = buffer.str();
  return std::filesystem::path(_str);
#endif
}


