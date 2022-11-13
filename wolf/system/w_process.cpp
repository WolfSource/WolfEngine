#include "w_process.hpp"

using w_process = wolf::system::w_process;

boost::leaf::result<std::filesystem::path> w_process::current_exe_path() {
  using path = std::filesystem::path;

#ifdef WIN32
  std::array<wchar_t, W_MAX_PATH> _buffer = {};
  std::ignore = GetModuleFileNameW(nullptr, _buffer.data(), sizeof(_buffer));
  const auto _ret = ::GetLastError();

  if (_ret != S_OK) {
    return W_ERR(_ret, "GetModuleFileNameW failed because:" +
                           get_last_win_error(_ret));
  }

  auto _path = path(_buffer.data(), path::auto_format).parent_path();
  return W_OK(_path);
#else
  std::string _str;
  std::ifstream("/proc/self/comm") >> _str;
  return std::filesystem::path(_str);
#endif
}


