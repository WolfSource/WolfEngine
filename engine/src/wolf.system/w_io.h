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
#include <turbojpeg.h>
#include "libpng/png.h"

#if defined(_WIN32) && defined(_MSC_VER)
#include <filesystem>
#endif

#ifndef _MSC_VER
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
			inline void read_binary_fileW(_In_z_ const wchar_t* pPath, _Inout_ std::vector<uint8_t>& pData,
				_Out_ int& pFileState)
			{
				if (get_is_fileW(pPath) == W_FAILED)
				{
					pFileState = -1;
					return;
				}

				// Open the file.
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
				ScopedHandle _hFile(safe_handle(CreateFile2(pPath, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr)));
#else
				ScopedHandle _hFile(safe_handle(CreateFileW(pPath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)));
#endif //(_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)

				if (!_hFile)
				{
					pFileState = -2;
					return;// HRESULT_FROM_WIN32(GetLastError());
				}
				// Get the file size.
				LARGE_INTEGER _file_size = { 0 };

#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA)
				FILE_STANDARD_INFO fileInfo;

				if (!GetFileInformationByHandleEx(_hFile.get(), FileStandardInfo, &fileInfo, sizeof(fileInfo)))
				{
					pFileState = -2;
					return;// HRESULT_FROM_WIN32(GetLastError());
				}

				_file_size = fileInfo.EndOfFile;
#else
				GetFileSizeEx(hFile.get(), &_fileSize);
#endif //(_WIN32_WINNT >= _WIN32_WINNT_VISTA)

				// File is too big for 32-bit allocation, so reject read.
				if (_file_size.HighPart > 0)
				{
					pFileState = -3;
					return;// E_FAIL;
				}

				// Read the data in.
				DWORD _bytesRead = 0;

				auto _file_data = static_cast<uint8_t*>(malloc(_file_size.LowPart * sizeof(uint8_t)));
				if (!ReadFile(_hFile.get(), _file_data, _file_size.LowPart, &_bytesRead, nullptr))
				{
					pFileState = -2;
					return;// HRESULT_FROM_WIN32(GetLastError());
				}

				if (_bytesRead < _file_size.LowPart)
				{
					pFileState = -2;
					return;// E_FAIL;
				}

				// Create enough space for the file data.
				pData.reserve(_file_size.LowPart);
				std::copy(&_file_data[0], &_file_data[_file_size.LowPart], std::back_inserter(pData));
				free(_file_data);
			}

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

#if defined(_WIN32) && defined(_MSC_VER)// || (__cplusplus >= 201703L && !defined(__APPLE__))
            inline void get_files_folders_in_directoryW(_In_z_ const std::wstring& pDirectoryPath, _Inout_ std::vector<std::wstring>& pPaths)
            {
                pPaths.clear();

                for (auto& _file_name : std::experimental::filesystem::directory_iterator(pDirectoryPath))
                {
                    pPaths.push_back(get_file_nameW(_file_name.path()));
                }
            }
            inline void get_files_folders_in_directory(_In_z_ const std::string& pDirectoryPath, _Inout_ std::vector<std::string>& pPaths)
            {
                pPaths.clear();

				std::string _name;
                for (auto& _file_name : std::experimental::filesystem::directory_iterator(pDirectoryPath))
                {
					_name = wolf::system::convert::wstring_to_string(_file_name.path());
                    pPaths.push_back(get_file_name(_name));
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
				if (get_is_file(pPath) == W_FAILED)
				{
					pState = -1;
					return nullptr;
				}

				std::ifstream _file(pPath, std::ios::ate | std::ios::in);
				if (!_file)
				{
					//file is exist but it might be corrupted
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
				_source[_file_length] = 0;

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
				if (get_is_fileW(pPath) == W_FAILED)
				{
					pState = -1;
					return nullptr;
				}

				std::ifstream _file(pPath, std::ios::ate | std::ios::in);
				if (!_file)
				{
					//file is exist but it might be corrupted
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
				_source[_file_length] = 0;

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

			//Check is png
			inline W_RESULT is_png_file(_Inout_ std::istream& pStream)
			{
				const int _png_paging_size = 8;
				png_byte _header[_png_paging_size];

				pStream.read((char*)_header, _png_paging_size);

				if (!pStream.good()) return W_INVALID_FILE_ATTRIBUTES;

				//check for png
				auto _hr = png_sig_cmp(_header, 0, _png_paging_size);
				//seek to first of stream
				pStream.seekg(0, std::ios::beg);

				return _hr == 0 ? W_PASSED : W_FAILED;
			}

			//Check is png
			inline W_RESULT is_png_file(_In_z_ const char* pFilePath)
			{
				std::ifstream _file(pFilePath, std::ios::binary);
				if (!_file)
				{
					//file is exist but it might be corrupted
					return W_INVALID_FILE_ATTRIBUTES;
				}			
				auto _hr = is_png_file(_file);
				_file.close();

				return _hr == 0 ? W_PASSED : W_FAILED;
			}

			inline void png_user_read_data(
				png_structp pPngPtr, 
				png_bytep pData, 
				png_size_t pLength) 
			{
				//cast istream
				png_voidp _io = png_get_io_ptr(pPngPtr);
				((std::istream*)_io)->read((char*)pData, pLength);
			}

			inline void png_user_write_data(
				png_structp pPngPtr,
				png_bytep pData,
				png_size_t pLength)
			{
				//cast istream
				png_voidp _io = png_get_io_ptr(pPngPtr);
				((std::ostream*)_io)->write((char*)pData, pLength);
			}

			//inline void png_user_flush_data(png_structp png_ptr)
			//{

			//}

			/*
				read png from stream
				pState indicates the state
				 0 means the succeded
				 1 means file is not png
				-1 means the file could not be opened for reading
				-2 means internal function error
			*/
			inline uint8_t* read_png_from_stream(_Inout_ std::istream& pStream,
				_Out_ uint32_t& pWidth,
				_Out_ uint32_t& pHeight,
				_Out_ uint8_t& pColorType,
				_Out_ uint8_t& pBitDepth,
				_Out_ int& pNumberOfPasses,
				_Out_ int& pState)
			{
				pState = 0;

				const int _png_paging_size = 8;
				png_byte _header[_png_paging_size];
				pStream.read((char*)_header, _png_paging_size);
				if (!pStream.good())
				{
					pState = -2;
					return nullptr;
				}

				//check for png
				if (png_sig_cmp(_header, 0, _png_paging_size))
				{
					pState = 1;
					return nullptr;
				}

				//initialize stuff
				auto _png_ptr = png_create_read_struct(
					PNG_LIBPNG_VER_STRING,
					NULL,
					NULL,
					NULL);
				if (!_png_ptr)
				{
					pState = -2;
					return nullptr;
				}

				auto _info_ptr = png_create_info_struct(_png_ptr);
				if (!_info_ptr)
				{
					pState = -2;
					return nullptr;
				}

				if (setjmp(png_jmpbuf(_png_ptr)))
				{
					pState = -2;
					png_destroy_read_struct(&_png_ptr, &_info_ptr, (png_infopp)0);
					return nullptr;
				}

				png_set_read_fn(_png_ptr, (void*)&pStream, png_user_read_data);//png_init_io(_png_ptr, _file);
				png_set_sig_bytes(_png_ptr, _png_paging_size);
				png_read_info(_png_ptr, _info_ptr);

				pWidth = png_get_image_width(_png_ptr, _info_ptr);
				pHeight = png_get_image_height(_png_ptr, _info_ptr);
				pColorType = png_get_color_type(_png_ptr, _info_ptr);
				pBitDepth = png_get_bit_depth(_png_ptr, _info_ptr);
				pNumberOfPasses = png_set_interlace_handling(_png_ptr);
				png_read_update_info(_png_ptr, _info_ptr);

				//read file
				if (setjmp(png_jmpbuf(_png_ptr)))
				{
					pState = -2;
					return nullptr;
				}

				auto _row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * pHeight);
				for (uint32_t j = 0; j < pHeight; ++j)
				{
					_row_pointers[j] = (png_byte*)malloc(png_get_rowbytes(_png_ptr, _info_ptr));
				}
				png_read_image(_png_ptr, _row_pointers);
				png_destroy_read_struct(&_png_ptr, &_info_ptr, (png_infopp)0);

				return (uint8_t*)_row_pointers;
			}

			/*
				read png from file
				pState indicates the state
				 0 means the succeded
				 1 means file is not png
				-1 means the file could not be opened for reading
				-2 means internal function error
			*/
			inline uint8_t* read_png_from_file(_In_z_ const char* pFilePath,
				_Out_ uint32_t& pWidth,
				_Out_ uint32_t& pHeight,
				_Out_ uint8_t& pColorType,
				_Out_ uint8_t& pBitDepth,
				_Out_ int& pNumberOfPasses,
				_Out_ int& pState)
			{
				pState = 0;

				std::ifstream _file(pFilePath, std::ios::binary);
				if (!_file)
				{
					pState = -1;
					return nullptr;
				}
				auto _pixels = read_png_from_stream(
					_file,
					pWidth,
					pHeight,
					pColorType,
					pBitDepth,
					pNumberOfPasses,
					pState);
				_file.close();

				return _pixels;
			}

			/*
				write png to file
				pState indicates the state
				 0 means succeeded
				-1 means file could not be opened for writing
				-2 means internal function error
			*/
			inline void write_png_to_file(
				_In_z_ const char* pFilePath,
				_In_ const uint8_t* pPixels, 
				_In_ const uint32_t& pWidth,
				_In_ const uint32_t& pHeight,
				_In_ const uint8_t& pColorType,
				_In_ const uint8_t& pBitDepth,
				_Out_ int& pState)
			{
				pState = 0;

				//create file
#if defined(__WIN32) || defined(__UWP)
				FILE* _file;
				fopen_s(&_file, pFilePath, "wb");

#else
				FILE* _file = fopen(pFilePath, "wb");
#endif
				if (!_file)
				{
					pState = -1;
					return;
				}

				//initialize stuff
				auto _png_ptr = png_create_write_struct(
					PNG_LIBPNG_VER_STRING, 
					NULL, 
					NULL, 
					NULL);
				if (!_png_ptr)
				{
					pState = -2;
					fclose(_file);
					return;
				}

				auto _info_ptr = png_create_info_struct(_png_ptr);
				if (!_info_ptr)
				{
					pState = -2;
					png_destroy_write_struct(&_png_ptr, &_info_ptr);
					fclose(_file);
					return;
				}

				if (setjmp(png_jmpbuf(_png_ptr)))
				{
					pState = -2;
					png_destroy_write_struct(&_png_ptr, &_info_ptr);
					fclose(_file);
					return;
				}
				png_init_io(_png_ptr, _file);

				//write header
				if (setjmp(png_jmpbuf(_png_ptr)))
				{
					pState = -2;
					png_destroy_write_struct(&_png_ptr, &_info_ptr);
					fclose(_file);
					return;
				}

				png_set_IHDR(
					_png_ptr, 
					_info_ptr, 
					pWidth, 
					pHeight,
					pBitDepth, 
					pColorType, 
					PNG_INTERLACE_NONE,
					PNG_COMPRESSION_TYPE_BASE, 
					PNG_FILTER_TYPE_BASE);

				png_write_info(_png_ptr, _info_ptr);
				
				//write bytes
				if (setjmp(png_jmpbuf(_png_ptr)))
				{
					pState = -2;
					png_destroy_write_struct(&_png_ptr, &_info_ptr);
					fclose(_file);
					return;
				}
				png_write_image(_png_ptr, (png_bytep*)pPixels);

				//end write
				if (setjmp(png_jmpbuf(_png_ptr)))
				{
					pState = -2;
					png_destroy_write_struct(&_png_ptr, &_info_ptr);
					fclose(_file);
					return;
				}
				png_write_end(_png_ptr, NULL);
				png_destroy_write_struct(&_png_ptr, &_info_ptr);
				fclose(_file);
			}


			/*
				write png to stream
				pState indicates the state
				 0 means succeeded
				-1 means file could not be opened for writing
				-2 means internal function error
			*/
			inline void write_png_to_stream(
				_Inout_ std::ostream& pStream,
				_In_ const uint8_t* pPixels,
				_In_ const uint32_t& pWidth,
				_In_ const uint32_t& pHeight,
				_In_ const uint8_t& pColorType,
				_In_ const uint8_t& pBitDepth,
				_Out_ int& pState)
			{
				pState = 0;

				//initialize stuff
				auto _png_ptr = png_create_write_struct(
					PNG_LIBPNG_VER_STRING,
					NULL,
					NULL,
					NULL);
				if (!_png_ptr)
				{
					pState = -2;
					return;
				}

				auto _info_ptr = png_create_info_struct(_png_ptr);
				if (!_info_ptr)
				{
					pState = -2;
					png_destroy_write_struct(&_png_ptr, &_info_ptr);
					return;
				}

				if (setjmp(png_jmpbuf(_png_ptr)))
				{
					pState = -2;
					png_destroy_write_struct(&_png_ptr, &_info_ptr);
					return;
				}

				png_set_write_fn(_png_ptr, (void*)&pStream, png_user_write_data, NULL);//png_init_io(_png_ptr, _file);

				//write header
				if (setjmp(png_jmpbuf(_png_ptr)))
				{
					pState = -2;
					png_destroy_write_struct(&_png_ptr, &_info_ptr);
					return;
				}

				png_set_IHDR(
					_png_ptr,
					_info_ptr,
					pWidth,
					pHeight,
					pBitDepth,
					pColorType,
					PNG_INTERLACE_NONE,
					PNG_COMPRESSION_TYPE_BASE,
					PNG_FILTER_TYPE_BASE);

				png_write_info(_png_ptr, _info_ptr);

				//write bytes
				if (setjmp(png_jmpbuf(_png_ptr)))
				{
					pState = -2;
					png_destroy_write_struct(&_png_ptr, &_info_ptr);
					return;
				}
				png_write_image(_png_ptr, (png_bytep*)pPixels);

				//end write
				if (setjmp(png_jmpbuf(_png_ptr)))
				{
					pState = -2;
					png_destroy_write_struct(&_png_ptr, &_info_ptr);
					return;
				}
				png_write_end(_png_ptr, NULL);
				png_destroy_write_struct(&_png_ptr, &_info_ptr);
			}

#pragma endregion

#pragma region JPG Methods

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
				_Out_ int& pState)
			{
				pState = 0;

#if defined(__WIN32) || defined(__UWP)
				FILE* _file;
				fopen_s(&_file, pFilePath, "rb");

#else
				FILE* _file = fopen(pFilePath, "rb");
#endif
				if (!_file)
				{
					pState = -1;
					return nullptr;
				}

				unsigned long _size = 0;
				if (fseek(_file, 0, SEEK_END) < 0 || ((_size = ftell(_file)) < 0) || fseek(_file, 0, SEEK_SET) < 0)
				{
					pState = -1;
					fclose(_file);
					return nullptr;
				}

				if (_size == 0)
				{
					pState = -1;
					fclose(_file);
					return nullptr;
				}

				auto _pixels = (uint8_t*)tjAlloc(_size);
				if (!_pixels)
				{
					pState = -2;
					fclose(_file);
					return nullptr;
				}

				if (fread(_pixels, _size, 1, _file) < 1)
				{
					pState = -2;
					fclose(_file);
					return nullptr;
				}

				fclose(_file);  
				_file = NULL;

				auto _tj_instance = tjInitDecompress();
				if (!_tj_instance)
				{
					pState = -2;
					return nullptr;
				}

				if (tjDecompressHeader3(
					_tj_instance,
					_pixels,
					_size,
					&pWidth,
					&pHeight,
					&pSubSample,
					&pColorSpace) < 0)
				{
					pState = 1;
					tjFree(_pixels);
					return nullptr;
				}

				return _pixels;
			}

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
				_Out_ uint32_t& pWidth,
				_Out_ uint32_t& pHeight,
				_Out_ int& pSubSample,
				_Out_ int& pColorSpace,
				_Out_ int& pNumberOfPasses,
				_Out_ int& pState,
				_In_ const TJPF& pComponents = TJPF_RGB)
			{
				pState = 0;

				if (pComponents == TJPF_UNKNOWN)
				{
					pState = -2;
					return nullptr;
				}

				//find size of file
				pStream.seekg(0, std::ios::end);
				unsigned long _jpeg_buffer_len = static_cast<unsigned long>(pStream.tellg());
				if (_jpeg_buffer_len <= 0)
				{
					pState = -1;
					return nullptr;
				}

				//go to begin 
				pStream.seekg(0, std::ios::beg);
				auto _jpeg_buffer = (uint8_t*)tjAlloc(_jpeg_buffer_len);
				if (!_jpeg_buffer)
				{
					pState = -2;
					return nullptr;
				}

				pStream.read((char*)_jpeg_buffer, _jpeg_buffer_len);
				if (!pStream.good())
				{
					pState = -1;
					return nullptr;
				}

				auto _tj_instance = tjInitDecompress();
				if (!_tj_instance)
				{
					pState = -2;
					return nullptr;
				}

				int _w = 0, _h = 0;
				if (tjDecompressHeader3(
					_tj_instance,
					_jpeg_buffer,
					_jpeg_buffer_len,
					&_w,
					&_h,
					&pSubSample,
					&pColorSpace))
				{
					pState = 1;
					free(_jpeg_buffer);
					return nullptr;
				}

				pWidth = static_cast<int>(_w);
				pHeight = static_cast<int>(_h);

				auto _comp = 4;
				switch (pComponents)
				{
				case TJPF_RGB:
				case TJPF_BGR:
					_comp = 3;
					break;
				}

				//TODO: should be alligned_malloc
				auto _pixels = (uint8_t*)malloc(pComponents * pWidth * pHeight * sizeof(uint8_t));
				auto _hr = tjDecompress2(
					_tj_instance,
					_jpeg_buffer,
					_jpeg_buffer_len,
					_pixels,
					pWidth,
					pComponents * pWidth,
					pHeight,
					pComponents,
					0);

				tjFree(_jpeg_buffer);
				tjDestroy(_tj_instance);

				if (_hr)
				{
					pState = -2;
					return nullptr;
				}
				
				return _pixels;
			}

#pragma endregion

		}
	}
}

#endif //__W_IO_H__
