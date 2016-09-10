/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_io.h
	Description		 : The manager class for IO functions
	Comment          :
*/

#ifndef __W_IO_H__
#define __W_IO_H__

#include <windows.h>
#include <string>
#include <memory>
#include <fstream>
#include <fileapi.h>
#include <io.h>
#include <vector>
#include <boost/filesystem.hpp>
#include "w_convert.h"

#if defined(WIN32)
#include <sstream>
#endif

namespace wolf
{
	namespace system
	{
		namespace io
		{
			//Return the permission status (boost::filesystem::perms). File not found is no_perms rather than perms_not_known
			inline boost::filesystem::perms get_is_file(const std::wstring& pPath)
			{
				auto _exists = boost::filesystem::exists(pPath);
				if (!_exists) return boost::filesystem::perms::no_perms;

				auto _file_status = boost::filesystem::status(pPath);
				auto _permission = _file_status.permissions();

				return _permission;
			}

			//Check whether a directory is exist or not
			inline bool get_is_directory(const std::wstring pPath)
			{
				return boost::filesystem::is_directory(pPath);
			}


			//Get the length of file
			inline unsigned long get_file_length(std::ifstream& pFile)
			{
				using namespace std;

				if (!pFile.good()) return 0;

				unsigned long _pos = pFile.tellg();
				pFile.seekg(0, ios::end);
				unsigned long _len = pFile.tellg();
				pFile.seekg(ios::beg);

				return _len;
			}

			//Get a unique name - format: mm-dd-year-hour-min-sec
			inline std::wstring get_unique_name()
			{
				using namespace std;

				struct tm _now;
				time_t _t = time(NULL);

				//get time now
				localtime_s(&_now, &_t);
				auto s = stringstream();
				s << 1 + _now.tm_mon << "-" << _now.tm_mday << "-" << 1900 + _now.tm_year << "-" << _now.tm_hour << "-" << _now.tm_min << "-" << _now.tm_sec;

				auto str = s.str();
				wstring name(str.begin(), str.end());

				//Clear sstream
				s.clear();
				s.str(std::string());

				return name;
			}

			//Create directory
			inline bool create_directory_w(std::wstring pPath)
			{
				return boost::filesystem::create_directory(pPath);
			}

			//Get file extension
			inline std::wstring get_file_extention(std::wstring pPath)
			{
				auto _path = boost::filesystem::path(pPath);
				return _path.has_extension() ? _path.extension().wstring() : L"";
			}

			//Get file name
			inline std::wstring get_file_name(std::wstring pPath)
			{
				auto _path = boost::filesystem::path(pPath);
				return _path.has_filename() ? _path.filename().wstring() : L"";
			}

			//Get file name without extenstion
			inline std::wstring get_file_name_without_extension(std::wstring pPath)
			{
				auto _path = boost::filesystem::path(pPath);
				return _path.has_stem() ? _path.stem().wstring() : L"";
			}

			//Get parent directory 
			inline std::wstring get_parent_directory(std::wstring pPath)
			{
				auto _path = boost::filesystem::path(pPath);
				return _path.has_parent_path() ? _path.parent_path().wstring() : L"";
			}

#ifdef WIN32

			//Get the current running directory path
			inline std::wstring get_current_directory()
			{
				const int _lenght = 4096;
				TCHAR _exePath[_lenght];
				GetModuleFileName(NULL, _exePath, _lenght);
				auto _path = convert::tchars_to_wstring(_exePath, _lenght);
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

			struct handle_closer { void operator()(HANDLE h) { if (h) CloseHandle(h); } };
			typedef public std::unique_ptr<void, handle_closer> ScopedHandle;
			inline HANDLE safe_handle(HANDLE h) { return (h == INVALID_HANDLE_VALUE) ? 0 : h; }

			// Read binary file and return array of uint8_t
			// fileState indicates to state of file
			// 1 means everything is ok
			//-1 means the file could not be found, 
			//-2 means file is exist but could not open
			inline HRESULT read_binary_file(_In_z_ std::wstring pFileName, _Inout_ std::unique_ptr<uint8_t[]>& pData,
				_Out_ size_t* pDataSize, _Out_ boost::filesystem::perms& pFileState)
			{
				pFileState = get_is_file(pFileName);
				if (pFileState == boost::filesystem::perms::no_perms)
				{
					return S_FALSE;
				}

				// Open the file.
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
				ScopedHandle _hFile(safe_handle(CreateFile2(pFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr)));
#else
				ScopedHandle _hFile(safe_handle(CreateFileW(_path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)));
#endif

				if (!_hFile) return HRESULT_FROM_WIN32(GetLastError());

				// Get the file size.
				LARGE_INTEGER _fileSize = { 0 };

#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA)
				FILE_STANDARD_INFO fileInfo;

				if (!GetFileInformationByHandleEx(_hFile.get(), FileStandardInfo, &fileInfo, sizeof(fileInfo)))
				{
					return HRESULT_FROM_WIN32(GetLastError());
				}

				_fileSize = fileInfo.EndOfFile;
#else
				GetFileSizeEx(hFile.get(), &_fileSize);
#endif

				// File is too big for 32-bit allocation, so reject read.
				if (_fileSize.HighPart > 0)
				{
					return E_FAIL;
				}

				// Create enough space for the file data.
				pData.reset(new uint8_t[_fileSize.LowPart]);

				if (!pData)
				{
					return E_OUTOFMEMORY;
				}
				// Read the data in.
				DWORD _bytesRead = 0;

				if (!ReadFile(_hFile.get(), pData.get(), _fileSize.LowPart, &_bytesRead, nullptr))
				{
					return HRESULT_FROM_WIN32(GetLastError());
				}

				if (_bytesRead < _fileSize.LowPart)
				{
					return E_FAIL;
				}

				*pDataSize = _bytesRead;

				return S_OK;
			}

#endif

			//Get the content directory path 
			inline std::wstring get_content_directory()
			{
				return get_current_directory() + L"Content\\";
			}

			/*
				Read text file from root content directory of Wolf(WolfContent) and return wstring
				pFileState indicates to state of file and the permission status, the first integer value means : 
							1 means everything is ok
						   -1 means the file could not be found, 
						   -2 means file is exist but it might be corrupted
						   -3 means file is empty
						   -4 means could not open file
			*/
			inline const char* read_text_file(std::wstring pFilename, _Out_ std::tuple<int, boost::filesystem::perms>& pFileState)
			{
				std::vector<byte> _data;
				std::wstring _dir = L"";

#if defined(MAYA) || defined(WIN32)

				_dir = get_content_directory();

#else
				using namespace Windows::Storage;
				auto folder = Windows::ApplicationModel::Package::Current->InstalledLocation;
				dir = folder->Path->ToString()->Data();

#endif // MAYA
				auto _fullpath = _dir.append(pFilename.c_str());

				auto _file_permission_status = get_is_file(_fullpath);
				//set the permission
				std::get<1>(pFileState) = _file_permission_status;

				if (_file_permission_status == boost::filesystem::perms::no_perms)
				{
					std::get<0>(pFileState) = -1;

					return nullptr;
				}

				std::ifstream file(_fullpath, std::ios::in);
				if (!file)
				{
					//file is exist but it might be corrupted
					std::get<0>(pFileState) = -2;
					return nullptr;
				}
				auto _filelength = get_file_length(file);
				if (_filelength == 0)
				{
					//file is empty
					std::get<0>(pFileState) = -3;
					return "";
				}

				if (!file.is_open())
				{
					//could not open file
					std::get<0>(pFileState) = -4;
					return nullptr;
				}

				auto _source = new char[_filelength + 1];
				_source[_filelength] = 0;

				unsigned int _i = 0;
				while (file.good())
				{
					_source[_i] = file.get();
					if (!file.eof())
					{
						_i++;
					}
				}
				_source[_i] = 0;

				//close the file
				file.close();
				
				std::get<0>(pFileState) = 1;
				return _source;
			}
		}
	}
}

#endif