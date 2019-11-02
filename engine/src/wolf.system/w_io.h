/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_io.h
	Description		 : The manager class for IO functions
	Comment                  : get_file_extention and get_file_name functions were declared based on std::experimental::filesystem::path class
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_IO_H__
#define __W_IO_H__

#if defined(__WIN32)

//#include <fileapi.h>
#include <io.h>

#elif defined(__UWP)

#include <wrl.h>
#include <wrl/client.h>
#include <ppltasks.h>

#elif defined(__ANDROID) || defined(__linux) || defined(__APPLE__)

#include  "w_std.h"

#if defined(__linux) || defined(__APPLE__)
#include <unistd.h>//for getcwd
#include <sys/time.h>
#endif

#endif //__WIN32

#include "w_std.h"
#include "w_convert.h"
#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <sys/stat.h>
#include "w_image.h"

#ifdef _MSC_VER
    #include <filesystem>
        namespace fs = std::filesystem;
#else
        #ifdef __cpp_lib_filesystem
            #include <filesystem>
            namespace fs = std::filesystem;
        #elif __cpp_lib_experimental_filesystem
            #include <experimental/filesystem>
            namespace fs = std::experimental::filesystem;
        #else
            #error "no filesystem support"
        #endif
        
        #include <dirent.h>
#endif

namespace wolf
{
	namespace system
	{
		namespace io
		{
#if defined(__WIN32) || defined(__UWP)

			//Get the current running directory directory path
#ifdef __WIN32
			inline const std::wstring get_current_directoryW()
			{
				const int _lenght = 4096;
				TCHAR _exe_path[_lenght];
				GetModuleFileName(NULL, _exe_path, _lenght);
				auto _path = wolf::system::convert::tchars_to_wstring(_exe_path, _lenght);
				size_t _i = 0;
				for (_i = _path.length() - 1; _i >= 0; _i--)
				{
					if (_path[_i] == '\\')
					{
						break;
					}
				}
				return _path.replace(_path.begin() + _i + 1, _path.end(), L"");
			}

			inline const std::string get_current_directory()
			{
				return wolf::system::convert::wstring_to_string(get_current_directoryW());
			}
#else
			inline const std::wstring get_current_directoryW()
			{
				auto _installed_location_folder = Windows::ApplicationModel::Package::Current->InstalledLocation;
				return std::wstring(_installed_location_folder->Path->ToString()->Data());
			}

			inline Concurrency::task<BOOL> get_directory_exists_async(_In_ Windows::Storage::StorageFolder^ pSourceFolder, _In_z_ Platform::String^ pFolderNameShoudBeFound)
			{
				return Concurrency::create_task(pSourceFolder->GetFoldersAsync()).then([=](Windows::Foundation::Collections::IVectorView<Windows::Storage::StorageFolder^>^ pFolders) -> BOOL
				{
					BOOL _result = FALSE;
					for (auto it = pFolders->First(); it->HasCurrent; it->MoveNext())
					{
						auto _file = it->Current;
						if (_file->Name == pFolderNameShoudBeFound)
						{
							_result = TRUE;
							break;
						}
					}
					return _result;
				});
			}

			inline Concurrency::task<Windows::Storage::StorageFolder^> create_directory_async(_In_ Windows::Storage::StorageFolder^ pSourceFolder,
				_In_z_ Platform::String^ pFolderNameShoudBeCreated,
				_In_ Windows::Storage::CreationCollisionOption pCreationCollisionOption = Windows::Storage::CreationCollisionOption::FailIfExists)
			{
				return Concurrency::create_task(pSourceFolder->CreateFolderAsync(pFolderNameShoudBeCreated, pCreationCollisionOption));
			}

			inline Concurrency::task<BOOL> get_file_exists_async(_In_ Windows::Storage::StorageFolder^ pSourceFolder, _In_z_ Platform::String^ pFileNameShoudBeFound)
			{
				return Concurrency::create_task(pSourceFolder->GetFilesAsync()).then([=](Windows::Foundation::Collections::IVectorView<Windows::Storage::StorageFile^>^ pFiles) -> BOOL
				{
					BOOL _result = FALSE;
					for (auto it = pFiles->First(); it->HasCurrent; it->MoveNext())
					{
						auto _file = it->Current;
						if (_file->Name == pFileNameShoudBeFound)
						{
							_result = TRUE;
							break;
						}
					}
					return _result;
				});
			}

			inline Concurrency::task<Windows::Storage::StorageFile^> create_file_async(_In_ Windows::Storage::StorageFolder^ pSourceFolder, _In_z_ Platform::String^ pFileNameShoudBeCreated)
			{
				return Concurrency::create_task(pSourceFolder->CreateFileAsync(pFileNameShoudBeCreated));
			}

#endif //__WIN32

			//check whether a file does exist or not
			inline W_RESULT get_is_file(_In_z_ const char* pPath)
			{
				//On Windows
				FILE* _file = nullptr;
				fopen_s(&_file, pPath, "r");
				if (_file)
				{
					fclose(_file);
					return W_PASSED;
				}
				return W_FAILED;
			}

			//check whether a file does exist or not
			inline W_RESULT get_is_fileW(_In_z_ const wchar_t* pPath)
			{
				FILE* _file = nullptr;
				_wfopen_s(&_file, pPath, L"r");
				if (_file)
				{
					fclose(_file);
					return W_PASSED;
				}
				return W_FAILED;
			}

			//get size of file
			inline unsigned long get_file_sizeW(_In_z_ const wchar_t* pPath)
			{
				std::ifstream _file(pPath, std::ifstream::ate | std::ifstream::binary);
				auto _size = static_cast<unsigned long>(_file.tellg());
				_file.close();
				return _size;
			}

			//get size of file
			inline unsigned long get_file_size(_In_z_ const char* pPath)
			{
				std::ifstream _file(pPath, std::ifstream::ate | std::ifstream::binary);
				auto _size = static_cast<unsigned long>(_file.tellg());
				_file.close();
				return _size;
			}

#ifdef __WIN32
			/*
				Check whether a directory does exist or not
				returns:
						W_PASSED	: Directory exists
						W_FAILED : Directory does not exist
						-1		: INVALID_FILE_ATTRIBUTES. Something is wrong with the path
			*/
			inline W_RESULT get_is_directory(_In_z_ const char* pPath)
			{
				DWORD _file_attr = GetFileAttributesA(pPath);

				if (_file_attr == INVALID_FILE_ATTRIBUTES) return W_INVALID_FILE_ATTRIBUTES;
				if (_file_attr == FILE_ATTRIBUTE_DIRECTORY) return W_PASSED;
				return W_FAILED;
			}

			/*
				Check whether a directory does exist or not
				returns:
						W_PASSED	: Directory exists
						W_FAILED : Directory not exists
						-1		: INVALID_FILE_ATTRIBUTES. Something is wrong with the path
			*/
			inline W_RESULT get_is_directoryW(_In_z_ const wchar_t* pPath)
			{
				DWORD _file_attr = GetFileAttributesW(pPath);
				if (_file_attr == INVALID_FILE_ATTRIBUTES) return W_INVALID_FILE_ATTRIBUTES;
				if (_file_attr == FILE_ATTRIBUTE_DIRECTORY) return W_PASSED;
				return W_FAILED;
			}

			//create directory
			inline W_RESULT create_directoryW(_In_ const wchar_t* pPath)
			{
				if (CreateDirectoryW(pPath, NULL) || ERROR_ALREADY_EXISTS == GetLastError())
				{
					return W_PASSED;
				}
				return W_FAILED;
			}

			//create directory
			inline W_RESULT create_directory(_In_z_ const char* pPath)
			{
				auto _str = std::string(pPath);
				auto _wstr = create_directoryW(std::wstring(_str.begin(), _str.end()).c_str());
				_str.clear();
				return _wstr;
			}

			struct handle_closer { void operator()(HANDLE h) { if (h) CloseHandle(h); } };
			typedef public std::unique_ptr<void, handle_closer> ScopedHandle;
			inline HANDLE safe_handle(HANDLE h) { return (h == INVALID_HANDLE_VALUE) ? 0 : h; }

			/*
                Read binary file and return array of uint8_t
                fileState indicates to state of file
                1 means everything is ok
                -1 means the file could not be found,
                -2 means file is exist but could not open
                -3 means file is to big to handle
			*/
//			inline void read_binary_fileW(_In_z_ const wchar_t* pPath, _Inout_ std::vector<uint8_t>& pData,
//				_Out_ int& pFileState)
//			{
//				if (get_is_fileW(pPath) == W_FAILED)
//				{
//					pFileState = -1;
//					return;
//				}
//
//				// Open the file.
//#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
//				ScopedHandle _hFile(safe_handle(CreateFile2(pPath, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr)));
//#else
//				ScopedHandle _hFile(safe_handle(CreateFileW(pPath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)));
//#endif //(_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
//
//				if (!_hFile)
//				{
//					pFileState = -2;
//					return;// HRESULT_FROM_WIN32(GetLastError());
//				}
//				// Get the file size.
//				LARGE_INTEGER _file_size = { 0 };
//
//#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA)
//				FILE_STANDARD_INFO fileInfo;
//
//				if (!GetFileInformationByHandleEx(_hFile.get(), FileStandardInfo, &fileInfo, sizeof(fileInfo)))
//				{
//					pFileState = -2;
//					return;// HRESULT_FROM_WIN32(GetLastError());
//				}
//
//				_file_size = fileInfo.EndOfFile;
//#else
//				GetFileSizeEx(hFile.get(), &_fileSize);
//#endif //(_WIN32_WINNT >= _WIN32_WINNT_VISTA)
//
//				// File is too big for 32-bit allocation, so reject read.
//				if (_file_size.HighPart > 0)
//				{
//					pFileState = -3;
//					return;// E_FAIL;
//				}
//
//				// Read the data in.
//				DWORD _bytesRead = 0;
//
//				auto _file_data = static_cast<uint8_t*>(malloc(_file_size.LowPart * sizeof(uint8_t)));
//				if (!ReadFile(_hFile.get(), _file_data, _file_size.LowPart, &_bytesRead, nullptr))
//				{
//					pFileState = -2;
//					return;// HRESULT_FROM_WIN32(GetLastError());
//				}
//
//				if (_bytesRead < _file_size.LowPart)
//				{
//					pFileState = -2;
//					return;// E_FAIL;
//				}
//
//				// Create enough space for the file data.
//				pData.reserve(_file_size.LowPart);
//				std::copy(&_file_data[0], &_file_data[_file_size.LowPart], std::back_inserter(pData));
//				free(_file_data);
//			}

            //Write text file
            inline void write_text_file(_In_z_ const wchar_t* pPath, _In_z_ const wchar_t* pTextContent)
            {
                std::wofstream _file;
                _file.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::generate_header>));
                _file.open(pPath);
                _file << pTextContent;
                _file.flush();
                _file.close();
            }

            //Write binary file
            inline void write_binary_file(_In_z_ const wchar_t* pPath, _In_z_ const wchar_t* pTextContent)
            {
                std::wofstream _file(pPath, std::ios::binary);
                _file << pTextContent;
                _file.flush();
                _file.close();
            }

#endif //__WIN32

#elif defined(__ANDROID) || defined(__linux) || defined(__APPLE__)

#if defined(__linux) || defined(__APPLE__)
            //get current working directory
            inline const std::string get_current_directory()
            {
                char _current_working_dir[4096];
                if (getcwd(_current_working_dir, sizeof(_current_working_dir)) != NULL)
                {
                    return _current_working_dir;
                }
                return "";
            }

            //get current working directory
            inline const std::wstring get_current_directoryW()
            {
                return wolf::system::convert::string_to_wstring(get_current_directory());
            }
			//Get the content directory path 
			inline const std::wstring get_content_directoryW()
			{
				return get_current_directoryW() + L"Content\\";
			}
            
            //Get the content directory path
            inline const std::string get_content_directory()
            {
                return get_current_directory() + "Content\\";
            }
#endif          
			//check whether a file does exist or not
			inline W_RESULT get_is_file(_In_z_ const char* pPath)
			{
				struct stat _stat;
				auto _result = stat(pPath, &_stat);
				return _result == -1 ? W_FAILED : W_PASSED;
			}

			//check size of file
			inline long long get_file_size(_In_z_ const char* pPath)
			{
				struct stat _stat;
				stat(pPath, &_stat);
				return _stat.st_size;
			}

			/*
				Check whether a directory does exist or not
				returns:
						W_PASSED	: Directory exists
						W_FAILED : Directory does not exist
			*/
			inline W_RESULT get_is_directory(_In_z_ const char* pPath)
			{
				struct stat _stat;
				if (stat(pPath, &_stat) == 0 && S_ISDIR(_stat.st_mode))
				{
					return W_PASSED;
				}
				return W_FAILED;
			}

			//create a directory
			inline W_RESULT create_directory(_In_z_ std::string pPath, mode_t pMode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)
			{
				struct stat _stat;
				W_RESULT _hr = W_PASSED;

				auto _path = pPath.c_str();
				if (stat(_path, &_stat) != 0)
				{
					//Directory does not exist, so make it. EEXIST for race condition
					if (mkdir(_path, pMode) != 0 && errno != EEXIST)
					{
						_hr = W_FAILED;
					}
				}
				else if (!S_ISDIR(_stat.st_mode))
				{
					errno = ENOTDIR;
					_hr = W_FAILED;
				}

				return _hr;
			}

#endif
			//Get time - format: Hour(xx)Min(xx)Sec(xx)MiliSec(xxx)
			inline std::string get_time_str()
			{
				std::string _time_str;

#if defined(__WIN32) || defined(__UWP)

				SYSTEMTIME _time;
				GetLocalTime(&_time);
				long _ms = (_time.wSecond * 1000) + _time.wMilliseconds;

				auto _sec_str = std::to_string(_time.wSecond);
				auto _min_str = std::to_string(_time.wMinute);
				auto _hr_str = std::to_string(_time.wHour);

#else

				time_t _t = time(0);
				struct tm _now = *localtime(&_t);

				struct timeval tp;
				gettimeofday(&tp, NULL);
				long _ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;

				auto _sec_str = std::to_string(_now.tm_sec);
				auto _min_str = std::to_string(_now.tm_min);
				auto _hr_str = std::to_string(_now.tm_hour);

#endif

				std::string _ms_str = std::to_string(_ms).substr(0, 3);

				//format of ms (xxx)
				if (_ms_str.size() < 1) _ms_str = "000";
				else if (_ms_str.size() < 2) _ms_str = "00" + _ms_str;
				if (_ms_str.size() < 3) _ms_str = "0" + _ms_str;

				//format of second (xx)
				if (_sec_str.size() < 1) _sec_str = "00";
				else if (_sec_str.size() < 2) _sec_str = "0" + _sec_str;

				//format of minute (xx)
				if (_min_str.size() < 1) _min_str = "00";
				else if (_min_str.size() < 2) _min_str = "0" + _min_str;

				//format of hour (xx)
				if (_hr_str.size() < 1) _hr_str = "00";
				else if (_hr_str.size() < 2) _hr_str = "0" + _hr_str;


				std::stringstream _str;
				_str << _hr_str << _min_str << _sec_str << _ms_str;

				_time_str = _str.str();

				//clear sstream
				_str.clear();
				_str.str("");

				return _time_str;
			}

			//Get time - format: Hour(xx)Min(xx)Sec(xx)MiliSec(xxx)
			inline std::wstring get_time_strW()
			{
				auto _str = wolf::system::io::get_time_str();
				return std::wstring(_str.begin(), _str.end());
			}

			//Get date time - format: Day(xx)Mon(xx)Year(xxxx)Hour(xx)Min(xx)Sec(xx)MiliSec(xxx)
			inline std::string get_date_time_str()
			{
				std::string _date_time_str;

#if defined(__WIN32) || defined(__UWP)

				SYSTEMTIME _time;
				GetLocalTime(&_time);
				long _ms = (_time.wSecond * 1000) + _time.wMilliseconds;

				auto _sec_str = std::to_string(_time.wSecond);
				auto _min_str = std::to_string(_time.wMinute);
				auto _hr_str = std::to_string(_time.wHour);

				auto _day_str = std::to_string(_time.wDay);
				auto _mon_str = std::to_string(_time.wMonth);
				auto _year_str = std::to_string(_time.wYear);

#else

				time_t _t = time(0);
				struct tm _now = *localtime(&_t);
				
				struct timeval tp;
				gettimeofday(&tp, NULL);
				long _ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
				
				auto _sec_str = std::to_string(_now.tm_sec);
				auto _min_str = std::to_string(_now.tm_min);
				auto _hr_str = std::to_string(_now.tm_hour);

				auto _day_str = std::to_string(_now.tm_mday);
				auto _mon_str = std::to_string(1 + _now.tm_mon);
				auto _year_str = std::to_string(1900 + _now.tm_year);
#endif
	
				std::string _ms_str = std::to_string(_ms).substr(0, 3);

				//format of ms (xxx)
				const auto _ms_str_size = _ms_str.size();
				if (_ms_str_size < 1) _ms_str = "000";
				else if (_ms_str_size < 2) _ms_str = "00" + _ms_str;
				else if (_ms_str_size < 3) _ms_str = "0" + _ms_str;

				//format of second (xx)
				const auto _sec_str_size = _sec_str.size();
				if (_sec_str_size < 1) _sec_str = "00";
				else if (_sec_str_size < 2) _sec_str = "0" + _sec_str;

				//format of minute (xx)
				const auto _min_str_size = _min_str.size();
				if (_min_str_size < 1) _min_str = "00";
				else if (_min_str_size < 2) _min_str = "0" + _min_str;

				//format of hour (xx)
				const auto _hr_str_size = _hr_str.size();
				if (_hr_str_size < 1) _hr_str = "00";
				else if (_hr_str_size < 2) _hr_str = "0" + _hr_str;

				//format of day (xx)
				const auto _day_str_size = _day_str.size();
				if (_day_str_size < 1) _day_str = "00";
				else if (_day_str_size < 2) _day_str = "0" + _day_str;

				//format of month (xx)
				const auto _mon_str_size = _mon_str.size();
				if (_mon_str_size < 1) _mon_str = "00";
				else if (_mon_str_size < 2) _mon_str = "0" + _mon_str;

				std::stringstream _str;
				_str << _day_str << _mon_str << _year_str <<
					_hr_str << _min_str << _sec_str << _ms_str;
			
				_date_time_str = _str.str();

				//clear sstream
				_str.clear();
				_str.str("");

				return _date_time_str;
			}

			//Get a date time - format: mm-dd-year-hour-min-sec
			inline std::wstring get_date_time_strW()
			{
				auto _str = wolf::system::io::get_date_time_str();
				return std::wstring(_str.begin(), _str.end());
			}

			//Get parent directory 
			inline std::string get_parent_directory(_In_z_ std::string pPath)
			{
				std::string _str;
				for (int i = (int)pPath.size() - 1; i >= 0; i--)
				{
					if (pPath[i] == '\\' || pPath[i] == '/')
					{
						_str = pPath.substr(0, i);
						break;
					}
				}
				return _str;
			}

			//Get parent directory 
			inline std::wstring get_parent_directoryW(_In_z_ std::wstring pPath)
			{
				std::wstring _str;
				for (int i = (int)pPath.size() - 1; i >= 0; i--)
				{
					if (pPath[i] == '\\' || pPath[i] == '/')
					{
						_str = pPath.substr(0, i);
						break;
					}
				}
				return _str;
			}

			// pick off .extension in filename, including dot
			inline std::string get_file_extention(_In_z_ std::string pPath)
			{
				typedef std::basic_string<char> _string_type;

				const auto __FS_PERIOD = '.';
				size_t _index = pPath.rfind(__FS_PERIOD);
				return (_index == _string_type::npos	// no .
					|| pPath.size() == 1	// only .
					|| (pPath.size() == 2 && pPath[0] == __FS_PERIOD && pPath[1] == __FS_PERIOD)	// only ..
					? "" : pPath.substr(_index));
			}

			// pick off .extension in filename, including dot
			inline std::wstring get_file_extentionW(_In_z_ std::wstring pPath)
			{
				typedef std::basic_string<wchar_t> _string_type;

				const auto __FS_PERIOD = L'.';
				size_t _index = pPath.rfind(__FS_PERIOD);
				return (_index == _string_type::npos	// no .
					|| pPath.size() == 1	// only .
					|| (pPath.size() == 2 && pPath[0] == __FS_PERIOD && pPath[1] == __FS_PERIOD)	// only ..
					? L"" : pPath.substr(_index));
			}

			// pick off only filename.extension
			inline std::string get_file_name(_In_z_ std::string pPath)
			{
				auto _str1 = get_parent_directory(pPath.c_str());
				if (_str1.empty()) return pPath;
				return pPath.substr(_str1.size() + 1, pPath.size() - _str1.size());
			}

			// pick off only filename.extension
			inline std::wstring get_file_nameW(_In_z_ std::wstring pPath)
			{
				auto _str1 = get_parent_directoryW(pPath.c_str());
				if (_str1.empty()) return pPath;
				return pPath.substr(_str1.size() + 1, pPath.size() - _str1.size());
			}

			// pick off stem (base name) in filename before dot without extension
			inline std::string  get_base_file_name(_In_z_ std::string pPath)
			{
				auto _str = get_file_name(pPath);
				auto _ext = get_file_extention(pPath);
				return _str.substr(0, _str.size() - _ext.size());
			}

			//pick off stem (base name) in filename before dot without extension
			inline std::wstring  get_base_file_nameW(_In_z_ std::wstring pPath)
			{
				auto _str = get_file_nameW(pPath);
				auto _ext = get_file_extentionW(pPath);
				return _str.substr(0, _str.size() - _ext.size());
			}
#if defined(__cpp_lib_filesystem) || defined(__cpp_lib_experimental_filesystem)
			inline void get_files_folders_in_directoryW(_In_z_ const std::wstring& pDirectoryPath, _Inout_ std::vector<std::wstring>& pPaths)
			{
				pPaths.clear();

				for (auto& _file_name : fs::directory_iterator(pDirectoryPath))
				{
                    const auto _path = _file_name.path().wstring();
					pPaths.push_back(get_file_nameW(_path));
				}
			}
			inline void get_files_folders_in_directory(_In_z_ const std::string& pDirectoryPath, _Inout_ std::vector<std::string>& pPaths)
			{
				pPaths.clear();

				for (auto& _file_name : fs::directory_iterator(pDirectoryPath))
				{
                    const auto _path = _file_name.path().string();
					pPaths.push_back(get_file_name(_path));
				}
			}
#else
			inline void get_files_folders_in_directoryW(_In_z_ const std::wstring& pDirectoryPath, _Inout_ std::vector<std::wstring>& pPaths)
			{
				pPaths.clear();

				auto _path = wolf::system::convert::wstring_to_string(pDirectoryPath);
				DIR* _dir;
				struct dirent* _ent;
				if ((_dir = opendir(_path.c_str())) != NULL)
				{
					while ((_ent = readdir(_dir)) != NULL)
					{
						pPaths.push_back(wolf::system::convert::string_to_wstring(_ent->d_name));
					}
					closedir(_dir);
				}
			}

			inline void get_files_folders_in_directory(_In_z_ const std::string& pDirectoryPath, _Inout_ std::vector<std::string>& pPaths)
			{
				pPaths.clear();

				DIR* _dir;
				struct dirent* _ent;
				if ((_dir = opendir(pDirectoryPath.c_str())) != NULL)
				{
					while ((_ent = readdir(_dir)) != NULL)
					{
						pPaths.push_back(_ent->d_name);
					}
					closedir(_dir);
				}
			}
#endif
			/*
				Read text file from root path
				pState indicates to state of file and the permission status, the first integer value means :
					 1 means everything is ok
					-1 means the file could not be found,
					-2 means file is exist but it might be corrupted
					-3 means file is empty
					-4 means could not open file
			*/
			inline const char* read_text_file(_In_z_ const char* pPath, _Out_ int& pState)
			{
				std::ifstream _file(pPath, std::ios::ate | std::ios::in);
				if (!_file)
				{
					//file is not exist
					pState = -1;
					return nullptr;
				}
				if (_file.bad())
				{
					//file exists but it might be corrupted
					pState = -2;
					return nullptr;
				}

				auto _file_length = static_cast<unsigned long>(_file.tellg());
				if (_file_length == 0)
				{
					//file is empty
					pState = -3;
					return "";
				}

				if (!_file.is_open())
				{
					//could not open file
					pState = -4;
					return nullptr;
				}

				auto _source = new char[_file_length + 1];
				_source[_file_length] = '\0';
				_file.seekg(0, std::ios::beg);

				unsigned int _i = 0;
				while (_file.good())
				{
					_source[_i] = _file.get();
					if (!_file.eof())
					{
						_i++;
					}
				}
				_source[_i] = '\0';

				//close the file
				_file.close();

				pState = 1;
				return _source;
			}

#ifdef __WIN32
			/*
				Read text file from root path
				pState indicates to state of file and the permission status, the first integer value means :
					 1 means everything is ok
					-1 means the file could not be found,
					-2 means file is exist but it might be corrupted
					-3 means file is empty
					-4 means could not open file
			*/
			inline const wchar_t* read_text_fileW(_In_z_ const wchar_t* pPath, _Out_ int& pState)
			{
				std::ifstream _file(pPath, std::ios::ate | std::ios::in);
				if (!_file)
				{
					//file is not exist
					pState = -1;
					return nullptr;
				}

				if (_file.bad())
				{
					//file exists but it might be corrupted
					pState = -2;
					return nullptr;
				}

				auto _file_length = static_cast<unsigned long>(_file.tellg());
				if (_file_length == 0)
				{
					//file is empty
					pState = -3;
					return L"";
				}

				if (!_file.is_open())
				{
					//could not open file
					pState = -4;
					return nullptr;
				}

				auto _source = new wchar_t[_file_length + 1];
				_source[_file_length] = '\0';
				_file.seekg(0, std::ios::beg);

				unsigned int _i = 0;
				while (_file.good())
				{
					_source[_i] = _file.get();
					if (!_file.eof())
					{
						_i++;
					}
				}
				_source[_i] = '\0';

				//close the file
				_file.close();

				pState = 1;
				return _source;
			}

			/*
				Read binary file and return array of uint8_t
				fileState indicates to state of file
				1 means everything is ok
				-1 means the file could not be found,
				-2 means file is exist but could not open
			*/
			inline void read_binary_fileW(_In_z_ const wchar_t* pPath, _Inout_ std::vector<uint8_t>& pData,
				_Out_ int& pFileState)
			{
				pFileState = 1;
				std::ifstream _file(pPath, std::ios::binary);
				if (!_file)
				{
					pFileState = -1;
					return;
				}

				if (_file.fail())
				{
					pFileState = -2;
					return;
				}

				// Stop eating new lines in binary mode!!!
				_file.unsetf(std::ios::skipws);

				// get its size:
				//std::streampos _file_size;
				size_t _data_size = 0;

				_file.seekg(0, std::ios::end);
				_data_size = static_cast<size_t>(_file.tellg());
				_file.seekg(0, std::ios::beg);

				// reserve capacity
				pData.reserve(_data_size);

				// read the data:
				std::copy(std::istream_iterator<uint8_t>(_file),
					std::istream_iterator<uint8_t>(),
					std::back_inserter(pData));

			}

#endif
			/*
				Read binary file and return array of uint8_t
				fileState indicates to state of file
				1 means everything is ok
				-1 means the file could not be found,
				-2 means file is exist but could not open
			*/
			inline void read_binary_file(_In_z_ const char* pPath, _Inout_ std::vector<uint8_t>& pData,
				_Out_ int& pFileState)
			{
				pFileState = 1;
				std::ifstream _file(pPath, std::ios::binary);
				if (!_file)
				{
					pFileState = -1;
					return;
				}

				if (_file.fail())
				{
					pFileState = -2;
					return;
				}

				// Stop eating new lines in binary mode!!!
				_file.unsetf(std::ios::skipws);

				// get its size:
				//std::streampos _file_size;
				size_t _data_size = 0;

				_file.seekg(0, std::ios::end);
				_data_size = static_cast<size_t>(_file.tellg());
				_file.seekg(0, std::ios::beg);

				// reserve capacity
				pData.reserve(_data_size);

				// read the data:
				std::copy(std::istream_iterator<uint8_t>(_file),
					std::istream_iterator<uint8_t>(),
					std::back_inserter(pData));

			}

			
            inline void write_text_file(_In_z_ const char* pPath, _In_z_ const char* pTextContent)
            {
                std::wofstream _file;
                _file.open(pPath);
                _file << pTextContent;
                _file.flush();
                _file.close();
            }

            inline void write_binary_file(_In_z_ const char* pPath, _In_z_ const char* pTextContent)
            {
                std::wofstream _file(pPath, std::ios::binary);
                _file << pTextContent;
                _file.flush();
                _file.close();
            }

			//return zero upon success or non-zero value on error.
			inline int delete_file(_In_z_ const char* pPath)
			{
				return std::remove(pPath);
			}

#pragma region PNG Methods

			//is stream contains png data
			inline W_RESULT is_png_file(_Inout_ std::istream& pStream)
			{
				return w_image::is_png_file(pStream);
			}

			//is file contains png data
			inline W_RESULT is_png_file(_In_z_ const char* pFilePath)
			{
				return w_image::is_png_file(pFilePath);
			}

			/*
				read png from stream
				pState indicates the state
				 0 means the succeded
				 1 means file is not png
				-1 means the file could not be opened for reading
				-2 means internal function error
			*/
			inline uint8_t* read_png_from_stream(_Inout_ std::istream& pStream,
				_Out_ int& pWidth,
				_Out_ int& pHeight,
				_Out_ uint8_t& pColorType,
				_Out_ uint8_t& pBitDepth,
				_Out_ int& pNumberOfPasses,
				_Out_ int& pState,
				_In_ const w_png_pixel_format& pPixelFormat = w_png_pixel_format::RGBA_PNG)
			{
				return w_image::read_png_from_stream(
					pStream,
					pWidth,
					pHeight,
					pColorType,
					pBitDepth,
					pNumberOfPasses,
					pState,
					pPixelFormat);
			}

			/*
				read png from file
				pState indicates the state
				 0 means the succeded
				 1 means file is not png
				-1 means the file could not be opened for reading
				-2 means internal function error
			*/
			inline uint8_t* read_png_from_file(
				_In_z_ const char* pFilePath,
				_Out_ int& pWidth,
				_Out_ int& pHeight,
				_Out_ uint8_t& pColorType,
				_Out_ uint8_t& pBitDepth,
				_Out_ int& pNumberOfPasses,
				_Out_ int& pState,
				_In_ const w_png_pixel_format& pPixelFormat = w_png_pixel_format::RGBA_PNG)
			{
				return w_image::read_png_from_file(
					pFilePath,
					pWidth,
					pHeight,
					pColorType,
					pBitDepth,
					pNumberOfPasses,
					pState,
					pPixelFormat);
			}

#pragma endregion

#pragma region JPEG Methods
			
			/*
				read jpg from stream
				pState indicates the state
				 0 means the succeded
				 1 means file is not jpg
				-1 means the file could not be opened for reading
				-2 means internal function error
			*/
			inline uint8_t* read_jpeg_from_stream(
				_In_z_ std::istream& pStream,
				_Out_ int& pWidth,
				_Out_ int& pHeight,
				_Out_ int& pSubSample,
				_Out_ int& pColorSpace,
				_Out_ int& pNumberOfPasses,
				_Out_ int& pState,
				_In_ const w_jpeg_pixel_format& pPixelFormat = w_jpeg_pixel_format::RGB_JPEG)
			{
				return w_image::read_jpeg_from_stream(
					pStream,
					pWidth,
					pHeight,
					pSubSample,
					pColorSpace,
					pNumberOfPasses,
					pState,
					pPixelFormat);
			}

			/*
				read jpg from file
				pState indicates the state
				 0 means the succeded
				 1 means file is not jpg
				-1 means the file could not be opened for reading
				-2 means internal function error
			*/
			inline uint8_t* read_jpeg_from_file(
				_In_z_ const char* pFilePath,
				_Out_ int& pWidth,
				_Out_ int& pHeight,
				_Out_ int& pSubSample,
				_Out_ int& pColorSpace,
				_Out_ int& pNumberOfPasses,
				_Out_ int& pState,
				_In_ const w_jpeg_pixel_format& pPixelFormat = w_jpeg_pixel_format::RGB_JPEG)
			{
				return w_image::read_jpeg_from_file(
					pFilePath,
					pWidth,
					pHeight,
					pSubSample,
					pColorSpace,
					pNumberOfPasses,
					pState,
					pPixelFormat);
			}

#pragma endregion

		}
	}
}

#endif //__W_IO_H__
