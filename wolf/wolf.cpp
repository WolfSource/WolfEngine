#include "wolf.hpp"

#ifdef WOLF_SYSTEM_MIMALLOC

#include <DISABLE_ANALYSIS_BEGIN>
#include <mimalloc-new-delete.h>
#include <DISABLE_ANALYSIS_END>

#endif

#if defined(WOLF_SYSTEM_STACKTRACE) && !defined(WOLF_TESTS)

#include <csignal>
#include <filesystem>
#include <fstream>
#include <sstream>

static void w_signal_handler(int p_signum)
{
    auto _path = wolf::system::w_current_process_dir();
    _path = _path.append("wolf.dump");

    std::ignore = signal(p_signum, nullptr);

    std::stringstream _traces;

#ifdef __clang__
#pragma unroll
#endif // __clang__
    for (const auto& trace : boost::stacktrace::stacktrace())
    {
        _traces << "name: " << trace.name() << 
            " source_file: " << trace.source_file() << 
            "(" << std::to_string(trace.source_line()) << ")\r\n";
    }

    std::fstream _file;
    _file.open(_path.string(), std::ios_base::out, std::ios_base::_Default_open_prot);
    if (_file.is_open())
    {
        _file << _traces.str();
        _file.close();
    }

    std::ignore = raise(SIGABRT);
}

#endif

std::string wolf::w_init()
{
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

#ifdef __clang__
    int _size = std::snprintf( nullptr, 0, "v{%d}.{%d}.{%d}.{%d}",
        WOLF_MAJOR_VERSION, WOLF_MINOR_VERSION, WOLF_PATCH_VERSION, WOLF_DEBUG_VERSION);
		
    std::string _version(_size + 1, '\0');
	
    sprintf(&_version[0],
        "v{%d}.{%d}.{%d}.{%d}",
        WOLF_MAJOR_VERSION, WOLF_MINOR_VERSION, WOLF_PATCH_VERSION, WOLF_DEBUG_VERSION);
#else
    auto _version = std::format(
        "v{}.{}.{}.{}",
        WOLF_MAJOR_VERSION, WOLF_MINOR_VERSION, WOLF_PATCH_VERSION, WOLF_DEBUG_VERSION);
#endif
    return _version;
}
