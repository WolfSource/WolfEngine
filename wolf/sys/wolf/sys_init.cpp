#include "sys_init.h"
#include "wolf.hpp"
#include <filesystem>

#ifdef WOLF_SYSTEM_STACKTRACE

#include <signal.h>
#include <fstream>
#include <sstream>
#include <boost/stacktrace.hpp>

#endif

#ifdef WOLF_SYSTEM_STACKTRACE

static std::filesystem::path s_current_process_dir()
{
#ifdef WIN32
    std::array<wchar_t, W_MAX_PATH> _buffer;
    GetModuleFileNameW(NULL, _buffer.data(), sizeof(_buffer));
    return std::filesystem::path(_buffer.data()).parent_path();
#else
    std::string _str;
    std::ifstream("/proc/self/comm") >> _str;
    return std::filesystem::path(_str);
#endif
}

static void w_signal_handler(int p_signum)
{
    auto _path = s_current_process_dir();
    _path = _path.append("wolf.dump");

    ::signal(p_signum, SIG_DFL);

    std::stringstream _traces;
    for (auto trace : boost::stacktrace::stacktrace())
    {
        _traces << "name: " << trace.name() << 
            " source_file: " << trace.source_file() << 
            "(" << std::to_string(trace.source_line()) << ")\r\n";
    }

    std::fstream _file;
    _file.open(_path.string(), std::ios_base::out);
    if (_file.is_open())
    {
        _file << _traces.str();
        _file.close();
    }

    ::raise(SIGABRT);
}

#endif

int w_sys_init(_Inout_z_ char* p_buf, _In_ size_t p_buf_len)
{

#ifdef WOLF_SYSTEM_STACKTRACE
    ::signal(SIGSEGV, &w_signal_handler);
    ::signal(SIGABRT, &w_signal_handler);
#endif

    const auto _buf_nn = gsl::not_null<char*>(p_buf);
    if (p_buf_len == 0)
    {
        return W_FAILURE;
    }

    // Making incompatible API changes
    constexpr auto WOLF_MAJOR_VERSION = 3;
    // Adding functionality in a backwards - compatible manner
    constexpr auto WOLF_MINOR_VERSION = 1;
    // bug fixes
    constexpr auto WOLF_PATCH_VERSION = 0;
    // for debugging
    constexpr auto WOLF_DEBUG_VERSION = 0;

#if defined(_WIN32)
    (void)sprintf_s(_buf_nn, p_buf_len, "%d.%d.%d.%d", WOLF_MAJOR_VERSION,
        WOLF_MINOR_VERSION, WOLF_PATCH_VERSION, WOLF_DEBUG_VERSION);
#else
    (void)sprintf(_buf_nn, "%d.%d.%d.%d", WOLF_MAJOR_VERSION, WOLF_MINOR_VERSION,
        WOLF_PATCH_VERSION, WOLF_DEBUG_VERSION);
#endif

    return W_SUCCESS;
}
