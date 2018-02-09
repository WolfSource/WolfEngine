#include "w_system_pch.h"
#include "w_logger.h"
#include <mutex>

#if defined(__WIN32)

#include <codecvt>

#elif defined(__UWP)

#include <codecvt>
#include <ppltasks.h>

#elif defined(__linux) || defined(__APPLE__)

#include <iostream>

#elif defined(__ANDROID)

#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "Wolf.Engine", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "Wolf.Engine", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "Wolf.Engine", __VA_ARGS__))

#endif

using namespace wolf::system;

namespace wolf
{
    namespace system
    {
        class w_logger_pimp
        {
        public:
            w_logger_pimp() : _is_released(false)
            {
            }

#ifdef __UWP
            bool initialize(_In_z_ const std::wstring pAppName)
            {
                //if directory of log is not existed
                auto _local_folder = Windows::Storage::ApplicationData::Current->LocalFolder;
                auto _log_file_path = _local_folder->Path + ref new Platform::String(L"\\log\\") + ref new Platform::String((io::get_unique_nameW() + L".wLog").c_str());

                auto _function = [this, _log_file_path, pAppName]() ->bool
                {
                    this->_log_file.open(_log_file_path->Data());

                    std::wstring _created_time = L"\t\"Time\"             : \"" + get_date_timeW() + L"\",\r\n";
                    std::wstring _app_name = L"\t\"Application Name\" : \"" + pAppName + L"\",\r\n";

                    std::wstring _version = L"\t\"Version\"          : \"" + std::to_wstring(WOLF_MAJOR_VERSION) + L":" + std::to_wstring(WOLF_MINOR_VERSION) +
                        L":" + std::to_wstring(WOLF_PATCH_VERSION) + L":" + std::to_wstring(WOLF_DEBUG_VERSION) + L"\",\r\n";

                    this->_log_file << L"{\r\n";
                    this->_log_file << L"\t\"Project\"          : \"Wolf Engine(http://WolfSource.io). Copyright(c) Pooya Eimandar(http://PooyaEimandar.com). All rights reserved.\",\r\n";
                    this->_log_file << L"\t\"Info\"             : \"contact@WolfSource.io\r\n";
                    this->_log_file << _version.c_str();
                    this->_log_file << _created_time.c_str();
                    this->_log_file << _app_name.c_str();
                    this->_log_file << L"\t\"Logs\":{\r\n";

                    //store all buffered logs
                    auto _buffer = get_buffer();
                    for (size_t i = 0; i < _buffer.size(); ++i)
                    {
                        this->_log_file << _buffer[i].c_str();
                    }
                    clear_buffer();

                    this->_log_file.flush();

                    return this->_log_file.is_open();
                };

                wolf::system::io::get_directory_exists_async(_local_folder, "log")
                    .then([=](_In_ BOOL pExists)
                {
                    if (!pExists)
                    {
                        wolf::system::io::create_directory_async(_local_folder, "log")
                            .then([=](Windows::Storage::StorageFolder^ pFolder)
                        {
                            _function();
                        });
                    }
                    else
                    {
                        _function();
                    }
                });

                this->_is_released = false;


                return true;
            }
#else
            bool initialize(_In_z_ const std::wstring pAppName, _In_z_ const std::wstring pLogPath)
            {

#ifdef __WIN32
                auto _log_directory = pLogPath + L"\\Log\\";
                auto _log_directory_cstr = _log_directory.c_str();
                //if directory of log is not existed
                if (io::get_is_directoryW(_log_directory_cstr) != W_OK)
                {
                    //Create the directory of log inside the root directory
                    io::create_directoryW(_log_directory_cstr);
                }
                auto _log_file_path = _log_directory + io::get_unique_nameW() + L".wLog";
#else

                auto _log_directory = wolf::system::convert::wstring_to_string(pLogPath) + "/Log/";
                auto _log_directory_cstr = _log_directory.c_str();
                //if directory of log is not existed
                if (io::get_is_directory(_log_directory_cstr) != W_OK)
                {
                    //Create the directory of log inside the root directory
                    io::create_directory(_log_directory_cstr);
                }
                auto _log_file_path = _log_directory + io::get_unique_name() + ".wLog";
#endif
                std::wstring _created_time = L"\t\"Time\"             : \"" + get_date_timeW() + L"\",\r\n";
                std::wstring _app_name = L"\t\"Application Name\" : \"" + pAppName + L"\",\r\n";
                
#ifdef __WIN32
                this->_log_file.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::generate_header>));
#endif

                this->_log_file.open(_log_file_path.c_str());

                this->_is_released = false;

                std::wstring _version = L"\t\"Version\"          : \"" + std::to_wstring(WOLF_MAJOR_VERSION) + L":" + std::to_wstring(WOLF_MINOR_VERSION) +
                    L":" + std::to_wstring(WOLF_PATCH_VERSION) + L":" + std::to_wstring(WOLF_DEBUG_VERSION) + L"\",\r\n";

                this->_log_file << L"{\r\n";
                this->_log_file << L"\t\"Project\"          : \"Wolf Engine(http://WolfSource.io). Copyright(c) Pooya Eimandar(http://PooyaEimandar.com). All rights reserved.\",\r\n";
                this->_log_file << L"\t\"Info\"             : \"contact@WolfSource.io\r\n";
                this->_log_file << _version.c_str();
                this->_log_file << _created_time.c_str();
                this->_log_file << _app_name.c_str();
                this->_log_file << L"\t\"Logs\":{\r\n";

                //store all buffered logs
                auto _buffer = get_buffer();
                for (size_t i = 0; i < _buffer.size(); ++i)
                {
                    this->_log_file << _buffer[i].c_str();
                }
                clear_buffer();

                this->_log_file.flush();

                return this->_log_file.is_open();
            }

#endif //__UWP

            void print_buffer(std::wstring pMsg)
            {
                std::unique_lock<std::mutex> _lock(this->_mutex);
                this->_msgs.push_back(pMsg + L"\r\n");
            }

            void clear_buffer()
            {
                std::unique_lock<std::mutex> _lock(this->_mutex);
                this->_msgs.clear();
            }

            std::vector<std::wstring> get_buffer()
            {
                return this->_msgs;
            }

            void flush()
            {
                this->_log_file.flush();
            }

            void write(_In_z_ std::string pMsg, _In_z_ const std::string pState)
            {
                auto _msg = std::wstring(pMsg.begin(), pMsg.end());
                auto _state = std::wstring(pState.begin(), pState.end());
                write(_msg, _state);
                _msg.clear();
                _state.clear();
            }

            void write(_In_z_ std::wstring pMsg, _In_z_ const std::wstring pState)
            {
                pMsg = L"\t\t\"" + get_date_timeW() + L"\"" + L": {\"msg\":\"" + (pMsg.empty() ? L"NULL" : pMsg) + L"\",\"state\":\"" + pState + L"\"},\r\n";

#if defined(__WIN32) || defined(__UWP)
                OutputDebugString(pMsg.c_str());
#elif defined(__linux) || defined(__APPLE__)
                std::cout << std::string(pMsg.begin(), pMsg.end()).c_str();
#elif defined(__ANDROID)
                if (pState == L"Warning")
                {
                    LOGW(std::string(pMsg.begin(), pMsg.end()).c_str());
                }
                else if (pState == L"Error")
                {
                    LOGE(std::string(pMsg.begin(), pMsg.end()).c_str());
                }
                else
                {
                    LOGI(std::string(pMsg.begin(), pMsg.end()).c_str());
                }
#elif defined(MAYA)
                switch (pState)
                {
                default:
                    "Info" :
                        MGlobal::displayInfo(Msg.c_str());
                    break;
                    "Warning":
                    MGlobal::displayWarning(Msg.c_str());
                    break;
                    "Error":
                    MGlobal::displayError(Msg.c_str());
                    break;
                }
#endif

                if (this->_log_file.is_open())
                {
                    this->_log_file << pMsg.c_str();
                    this->_log_file.flush();
                }
                else
                {
                    //log file is not available yet, so store it in to the buffer
                    std::unique_lock<std::mutex> _lock(this->_mutex);
                    this->_msgs.push_back(pMsg);
                }
            }

            void user(_In_z_ std::wstring pMsg)
            {
                write(pMsg, L"User");
            }

            void user(_In_z_ std::string pMsg)
            {
                write(pMsg, "User");
            }

            void warning(_In_z_ std::wstring pMsg)
            {
                write(pMsg, L"Warning");
            }

            void warning(_In_z_ std::string pMsg)
            {
                write(pMsg, "Warning");
            }

            void error(_In_z_ std::wstring pMsg)
            {
                write(pMsg, L"Error");
            }

            void error(_In_z_ std::string pMsg)
            {
                write(pMsg, "Error");
            }

            ULONG release()
            {
                if (this->_is_released) return 1;

                this->_is_released = true;

                //store all buffered logs
                for (size_t i = 0; i < this->_msgs.size(); ++i)
                {
                    this->_log_file << this->_msgs[i].c_str();
                }
                //clear the buffered messages
                this->_msgs.clear();

                write("Wolf shutting down", "User");
                this->_log_file << L"\r\n\t}\r\n}";
                this->_log_file.flush();
                this->_log_file.close();

                return 0;
            }

            bool get_is_released()
            {
                return _is_released;
            }

            bool get_is_open()
            {
                return this->_log_file.is_open();
            }

        private:
            bool _is_released;
            std::wofstream _log_file;
            std::vector<std::wstring> _msgs;
            std::mutex _mutex;
        };
    }
}

w_logger::w_logger() : _pimp(new w_logger_pimp())
{

}

w_logger::~w_logger()
{
    release();
}

bool w_logger::initialize(
#ifdef __UWP
    _In_z_ const std::wstring pAppName)
{
    return this->_pimp->initialize(pAppName);
}
#else
    _In_z_ const std::wstring pAppName, _In_z_ const std::wstring pLogPath)
{
    return this->_pimp->initialize(pAppName, pLogPath);
}
#endif

void w_logger::print_buffer(std::wstring pMsg)
{
    this->_pimp->print_buffer(pMsg);
}

void w_logger::clear_buffer()
{
    this->_pimp->clear_buffer();
}

std::vector<std::wstring> w_logger::get_buffer()
{
    return this->_pimp->get_buffer();
}

void w_logger::flush()
{
    this->_pimp->flush();
}

void w_logger::write(_In_z_ std::string pMsg, _In_z_ const std::string pState)
{
    this->_pimp->write(pMsg, pState);
}

void w_logger::write(_In_z_ std::wstring pMsg, _In_z_ const std::wstring pState)
{
    this->_pimp->write(pMsg, pState);
}

void w_logger::user(_In_z_ std::wstring pMsg)
{
    this->_pimp->write(pMsg, L"User");
}

void w_logger::user(_In_z_ std::string pMsg)
{
    this->_pimp->write(pMsg, "User");
}

void w_logger::warning(_In_z_ std::wstring pMsg)
{
    this->_pimp->write(pMsg, L"Warning");
}

void w_logger::warning(_In_z_ std::string pMsg)
{
    this->_pimp->write(pMsg, "Warning");
}

void w_logger::error(_In_z_ std::wstring pMsg)
{
    this->_pimp->write(pMsg, L"Error");
}

void w_logger::error(_In_z_ std::string pMsg)
{
    this->_pimp->write(pMsg, "Error");
}

ULONG w_logger::release()
{
    if (!this->_pimp || (this->_pimp && this->_pimp->get_is_released())) return 1;

    ULONG _hr = 0;
    _hr = this->_pimp->release();
    delete this->_pimp;
    this->_pimp = nullptr;
    return _hr;
}

#pragma region Getters

bool w_logger::get_is_open() const
{
    if (!this->_pimp || (this->_pimp && this->_pimp->get_is_released())) return false;
    return this->_pimp->get_is_open();
}

#pragma endregion

//Declaration of extern objects as shared

#ifdef __WIN32
#pragma data_seg (".shared")
#endif

wolf::system::w_logger wolf::logger;
std::wstring wolf::content_path;
wolf::system::w_inputs_manager wolf::inputs_manager;
std::map<uint32_t, float> wolf::windows_frame_time_in_sec;
void wolf::release_shared_data_over_all_instances()
{
    logger.release();
    content_path.clear();
    inputs_manager.reset();
    windows_frame_time_in_sec.clear();
}

#ifdef __WIN32
#pragma data_seg ()
#pragma comment(linker,"/SECTION:.shared,RWS")
#endif
