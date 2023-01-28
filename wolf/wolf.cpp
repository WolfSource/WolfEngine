#include "wolf.hpp"

#ifdef WOLF_SYSTEM_MIMALLOC

#include <DISABLE_ANALYSIS_BEGIN>
#include <DISABLE_ANALYSIS_END>
#include <mimalloc-new-delete.h>

#endif

#if defined(WOLF_SYSTEM_STACKTRACE) && !defined(WOLF_TESTS)

#include <csignal>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <system/w_process.hpp>

static void w_signal_handler(int p_signum) {
  auto _path = wolf::system::w_process::current_exe_path();
  if (_path.has_error()) {
    return;
  }

  auto _path_str = _path.value().append("wolf.dump");

  std::ignore = signal(p_signum, nullptr);

  std::stringstream _traces;

#ifdef __clang__
#pragma unroll
#endif // __clang__
  for (const auto &trace : boost::stacktrace::stacktrace()) {
    _traces << "name: " << trace.name()
            << " source_file: " << trace.source_file() << "("
            << std::to_string(trace.source_line()) << ")\r\n";
  }

  std::fstream _file;
  _file.open(_path_str.string(), std::ios_base::out);
  if (_file.is_open()) {
    _file << _traces.str();
    _file.close();
  }

  std::ignore = raise(SIGABRT);
}

#endif

std::string wolf::w_init() {
#if defined(WOLF_SYSTEM_STACKTRACE) && !defined(WOLF_TESTS)
  std::ignore = signal(SIGSEGV, &w_signal_handler);
  std::ignore = signal(SIGABRT, &w_signal_handler);
#endif

  // Making incompatible API changes
  constexpr auto WOLF_MAJOR_VERSION = 3;
  // Adding functionality in a backwards - compatible manner
  constexpr auto WOLF_MINOR_VERSION = 0;
  // bug fixes
  constexpr auto WOLF_PATCH_VERSION = 0;
  // for debugging
  constexpr auto WOLF_DEBUG_VERSION = 0;

  auto _version =
      wolf::format("v{}.{}.{}.{}", WOLF_MAJOR_VERSION, WOLF_MINOR_VERSION,
                   WOLF_PATCH_VERSION, WOLF_DEBUG_VERSION);
  return _version;
}
