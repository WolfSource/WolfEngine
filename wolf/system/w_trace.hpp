/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_SYSTEM_STACKTRACE

#pragma once

#include <deque>
#include <ostream>
#include <string>
#include <thread>

struct w_trace
{
    struct stack
    {
        friend std::ostream& operator << (std::ostream& p_os, stack const& p_trace)
        {
            return p_os <<
                "|tid:" << p_trace.thread_id <<
                "|code:" << p_trace.err_code <<
                "|msg:" << p_trace.err_msg <<
                "|src:" << p_trace.source_file << "(" << p_trace.source_file_line << ")" <<
                std::endl;
        }
        std::thread::id thread_id;
        int64_t err_code = 0;
        std::string err_msg;
        std::string source_file;
        uint32_t source_file_line = 0;
    } W_ALIGNMENT_128;

    w_trace(_In_ int64_t p_err_code,
        _In_ std::string_view p_err_msg,
        _In_ std::string_view p_source_file,
        _In_ uint32_t p_source_file_line)
    {
        this->_stacks.emplace_front(
            stack
            {
                std::this_thread::get_id(),
                p_err_code,
                p_err_msg.data(),
                p_source_file.data(),
                p_source_file_line
            });
    }

    friend std::ostream& operator << (std::ostream& p_os, w_trace const& p_trace)
    {
#ifdef __clang__
#pragma unroll
#endif
        for (const auto& index : p_trace._stacks)
        {
            p_os << index;
        }
        return p_os;
    }

private:
    std::deque<stack> _stacks = {};
} W_ALIGNMENT_128;

#define W_TRACE(p_code, p_msg) w_trace(p_code, p_msg, __FILE__, __LINE__)

#ifdef WIN32

inline std::string get_last_win_error(_In_ DWORD p_error_id)
{
    if (p_error_id == 0)
    {
        return {};
    }

    LPSTR _msg_buffer = nullptr;

    //Ask Win32 to give us the string version of that message ID.
    //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
    const auto _size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, p_error_id, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&_msg_buffer, 0, NULL);

    //Copy the error message into a std::string.
    std::string _message(_msg_buffer, _size);

    //Free the Win32's string's buffer.
    LocalFree(_msg_buffer);

    return _message;
}

#endif

#endif // WOLF_SYSTEM_STACKTRACE
