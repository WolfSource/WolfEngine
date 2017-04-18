/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_io.h
	Description		 : The manager class for IO functions
	Comment          : get_file_extention and get_file_name functions were declared based on std::experimental::filesystem::path class
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_IO_H__
#define __W_IO_H__

#if defined(__WIN32)

#include <Windows.h>
#include <fileapi.h>
#include <io.h>
#include "w_convert.h"

#elif defined(__UWP)

#include <wrl.h>
#include <wrl/client.h>
#include <ppltasks.h>

#elif defined(__ANDROID) || defined(__linux) || defined(__APPLE__)

#include  "w_std.h"

#if defined(__linux) || defined(__APPLE__)
#include <unistd.h>//for getcwd
#endif

#endif //__WIN32

#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <sys/stat.h>

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
            
			//Get the content directory path 
			inline const std::wstring get_content_directoryW()
			{
				return get_current_directoryW() + L"Content\\";
			}

			//check whether a file does exist or not
			inline HRESULT get_is_file(_In_z_ const char* pPath)
			{
				//On Windows
				FILE* _file = nullptr;
				fopen_s(&_file, pPath, "r");
				if (_file)
				{
					fclose(_file);
					return S_OK;
				}
				return S_FALSE;
			}

			//check whether a file does exist or not
			inline HRESULT get_is_file(_In_z_ const wchar_t* pPath)
			{
				FILE* _file = nullptr;
				_wfopen_s(&_file, pPath, L"r");
				if (_file)
				{
					fclose(_file);
					return S_OK;
				}
				return S_FALSE;
			}

			//get size of file
			inline unsigned long get_file_sizeW(_In_z_ const wchar_t* pPath)
			{
				std::ifstream _file(pPath, std::ifstream::ate | std::ifstream::binary);
				return static_cast<unsigned long>(_file.tellg());
			}

			//get size of file
			inline unsigned long get_file_size(_In_z_ const char* pPath)
			{
				std::ifstream _file(pPath, std::ifstream::ate | std::ifstream::binary);
				return static_cast<unsigned long>(_file.tellg());
			}

#ifdef __WIN32
			/*
				Check whether a directory does exist or not
				returns:
						S_OK	: Directory exists
						S_FALSE : Directory does not exist
						-1		: INVALID_FILE_ATTRIBUTES. Something is wrong with the path
			*/
			inline HRESULT get_is_directory(_In_z_ const char* pPath)
			{
				DWORD _file_attr = GetFileAttributesA(pPath);

				if (_file_attr == INVALID_FILE_ATTRIBUTES) return INVALID_FILE_ATTRIBUTES;
				if (_file_attr == FILE_ATTRIBUTE_DIRECTORY) return S_OK;
				return S_FALSE;
			}

			/*
				Check whether a directory does exist or not
				returns:
						S_OK	: Directory exists
						S_FALSE : Directory not exists
						-1		: INVALID_FILE_ATTRIBUTES. Something is wrong with the path
			*/
			inline HRESULT get_is_directoryW(_In_z_ const wchar_t* pPath)
			{
				DWORD _file_attr = GetFileAttributesW(pPath);
				if (_file_attr == INVALID_FILE_ATTRIBUTES) return INVALID_FILE_ATTRIBUTES;
				if (_file_attr == FILE_ATTRIBUTE_DIRECTORY) return S_OK;
				return S_FALSE;
			}

			//create directory
			inline HRESULT create_directoryW(_In_ const wchar_t* pPath)
			{
				if (CreateDirectoryW(pPath, NULL) || ERROR_ALREADY_EXISTS == GetLastError())
				{
					return S_OK;
				}
				return S_FALSE;
			}

			//create directory
			inline HRESULT create_directory(_In_z_ const char* pPath)
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
			*/
			inline HRESULT read_binary_file(_In_z_ const wchar_t* pPath, _Inout_ std::unique_ptr<unsigned char[]>& pData,
				_Out_ size_t* pDataSize, _Out_ int& pFileState)
			{
				if (pFileState = get_is_file(pPath) && !pFileState)
				{
					return S_FALSE;
				}

				// Open the file.
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
				ScopedHandle _hFile(safe_handle(CreateFile2(pPath, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr)));
#else
				ScopedHandle _hFile(safe_handle(CreateFileW(_path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)));
#endif //(_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)

				if (!_hFile) return HRESULT_FROM_WIN32(GetLastError());

				// Get the file size.
				LARGE_INTEGER _file_size = { 0 };

#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA)
				FILE_STANDARD_INFO fileInfo;

				if (!GetFileInformationByHandleEx(_hFile.get(), FileStandardInfo, &fileInfo, sizeof(fileInfo)))
				{
					return HRESULT_FROM_WIN32(GetLastError());
				}

				_file_size = fileInfo.EndOfFile;
#else
				GetFileSizeEx(hFile.get(), &_fileSize);
#endif //(_WIN32_WINNT >= _WIN32_WINNT_VISTA)

				// File is too big for 32-bit allocation, so reject read.
				if (_file_size.HighPart > 0)
				{
					return E_FAIL;
				}

				// Create enough space for the file data.
				pData.reset(new unsigned char[_file_size.LowPart]);

				if (!pData)
				{
					return E_OUTOFMEMORY;
				}
				// Read the data in.
				DWORD _bytesRead = 0;

				if (!ReadFile(_hFile.get(), pData.get(), _file_size.LowPart, &_bytesRead, nullptr))
				{
					return HRESULT_FROM_WIN32(GetLastError());
				}

				if (_bytesRead < _file_size.LowPart)
				{
					return E_FAIL;
				}

				*pDataSize = _bytesRead;

				return S_OK;
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
                    return std::string(_current_working_dir);
                }
                return "";
			}
                        
                        //Get the content directory path 
			inline const std::string get_content_directory()
			{
				return get_current_directory() + "Content\\";
			}
#endif          
			//check whether a file does exist or not
			inline HRESULT get_is_file(_In_z_ const char* pPath)
			{
				struct stat _stat;
                auto _result = stat(pPath, &_stat);
				return _result == -1 ? S_FALSE : S_OK;
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
						S_OK	: Directory exists
						S_FALSE : Directory does not exist
			*/
			inline HRESULT get_is_directory(_In_z_ const char* pPath)
			{
				struct stat _stat;
				if (stat(pPath, &_stat) == 0 && S_ISDIR(_stat.st_mode))
				{
					return S_OK;
				}
				return S_FALSE;
			}

			//create a directory
			inline HRESULT create_directory(_In_z_ std::string pPath, mode_t pMode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)
			{
				struct stat _stat;
				HRESULT  _hr = S_OK;

				auto _path = pPath.c_str();
				if (stat(_path, &_stat) != 0)
				{
					//Directory does not exist, so make it. EEXIST for race condition
					if (mkdir(_path, pMode) != 0 && errno != EEXIST)
					{
						_hr = S_FALSE;
					}
				}
				else if (!S_ISDIR(_stat.st_mode))
				{
					errno = ENOTDIR;
					_hr = S_FALSE;
				}

				return _hr;
			}
                        
#endif
			 
			//Get a unique name - format: mm-dd-year-hour-min-sec
			inline std::string get_unique_name()
			{
				time_t _t = time(0);
				//get time now
#if defined(__WIN32) || defined(__UWP)
				struct tm _now;
				localtime_s(&_now, &_t);

#else
				struct tm _now = *localtime(&_t);
#endif
				std::stringstream _ss;
				_ss << 1 + _now.tm_mon << "-" << _now.tm_mday << "-" << 1900 + _now.tm_year << "-"
					<< _now.tm_hour << "-" << _now.tm_min << "-" << _now.tm_sec;

				auto _name = _ss.str();
				
				//Clear sstream
				_ss.clear();
				_ss.str(std::string());

				return _name;
			}
			
			//Get a unique name - format: mm-dd-year-hour-min-sec
			inline std::wstring get_unique_nameW()
			{
				auto _str = wolf::system::io::get_unique_name();
				return std::wstring(_str.begin(), _str.end());
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

			//Get file name
			inline std::string get_file_name(_In_z_ std::string pPath)
			{
				auto _str0 = pPath;
				auto _str1 = get_file_extention(pPath);
				_str0.resize(_str0.size() - _str1.size());

				return _str0;
			}

			//Get file name
			inline std::wstring get_file_nameW(_In_z_ std::wstring pPath)
			{
				auto _str0 = pPath;
				auto _str1 = get_file_extentionW(pPath.c_str());
				_str0.resize(_str0.size() - _str1.size());

				return _str0;
			}

			// pick off stem (base name) in filename before dot
			inline std::string  get_base_file_name(_In_z_ std::string pPath)
			{
				auto _str = get_file_name(pPath);
				std::string _base_name;
				for (int i = (int)_str.size() - 1; i >= 0; i--)
				{
					if (_str[i] == '\\' || _str[i] == '/')
					{
						break;
					}
					_base_name += _str[i];
				}
				std::reverse(_base_name.begin(), _base_name.end());
				return _base_name;
			}

			// pick off stem (base name) in filename before dot
			inline std::wstring  get_base_file_nameW(_In_z_ std::wstring pPath)
			{
				auto _str = get_file_nameW(pPath);
				std::wstring _base_name;
				for (int i = (int)_str.size() - 1; i >= 0; i--)
				{
					if (_str[i] == '\\' || _str[i] == '/')
					{
						break;
					}
					_base_name += _str[i];
				}
				std::reverse(_base_name.begin(), _base_name.end());
				return _base_name;
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
				if (!get_is_file(pPath))
				{
					pState = -1;
					return nullptr;
				}

				std::ifstream _file(pPath, std::ios::in);
				if (!_file)
				{
					//file is exist but it might be corrupted
					pState = -2;
					return nullptr;
				}
				auto _filelength = get_file_size(pPath);
				if (_filelength == 0)
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

				auto _source = new char[_filelength + 1];
				_source[_filelength] = 0;

				unsigned int _i = 0;
				while (_file.good())
				{
					_source[_i] = _file.get();
					if (!_file.eof())
					{
						_i++;
					}
				}
				_source[_i] = 0;

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
				std::copy(std::istream_iterator<unsigned char>(_file),
					std::istream_iterator<unsigned char>(),
					std::back_inserter(pData));

                        }
		}
	}
}

#endif //__W_IO_H__
