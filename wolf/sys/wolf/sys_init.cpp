#include "sys_init.h"
#include <wolf.hpp>
#include <signal.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <boost/stacktrace.hpp>

static std::string s_dump_path;
static void w_signal_handler(int p_signum)
{
    auto _path = std::filesystem::path(s_dump_path);
    _path = _path.parent_path().append("wolf.dump");

    ::signal(p_signum, SIG_DFL);

    std::stringstream _traces;
    for (auto trace : boost::stacktrace::stacktrace())
    {
        _traces << "name: " << trace.name() << "\r\n" <<
            "source_file: " << trace.source_file() << "(" << std::to_string(trace.source_line()) << ")\r\n\r\n";
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

int w_sys_init(_In_z_ const char* p_dump_path, _Inout_z_ char* p_buf, _In_ size_t p_buf_len)
{
    if (p_dump_path != nullptr)
    {
        s_dump_path = p_dump_path;
    }

    ::signal(SIGSEGV, &w_signal_handler);
    ::signal(SIGABRT, &w_signal_handler);

    if (p_buf == nullptr || p_buf_len == 0)
    {
        return -1;
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
    (void)sprintf_s(p_buf, p_buf_len, "%d.%d.%d.%d", WOLF_MAJOR_VERSION,
        WOLF_MINOR_VERSION, WOLF_PATCH_VERSION, WOLF_DEBUG_VERSION);
#else
    (void)sprintf(p_buf, "%d.%d.%d.%d", WOLF_MAJOR_VERSION, WOLF_MINOR_VERSION,
        WOLF_PATCH_VERSION, WOLF_DEBUG_VERSION);
#endif

    return 0;
}
