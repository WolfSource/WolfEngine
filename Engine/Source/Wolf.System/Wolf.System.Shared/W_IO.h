/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @Wolf_Engine on twitter
	Name			 : W_IO.h
	Description		 : The manager class for IO functions
	Comment          :
*/

#pragma once

#include <windows.h>
#include <string>
#include <memory>
#include <fstream>
#include <fileapi.h>
#include <io.h>
#include <vector>
#include "W_Convert.h"

#if defined(__WIN32)
#include <sstream>
#endif

namespace Wolf
{
	namespace System
	{
		namespace IO
		{
			struct handle_closer { void operator()(HANDLE h) { if (h) CloseHandle(h); } };
			typedef public std::unique_ptr<void, handle_closer> ScopedHandle;
			inline HANDLE safe_handle(HANDLE h) { return (h == INVALID_HANDLE_VALUE) ? 0 : h; }

			/*
				Return value :
				-1 : File is not exists
				1  : File is exists
				2  : File is read only
			*/
			inline int IsFileExists(const std::wstring& filePath)
			{
				auto path = filePath.c_str();
				auto hr = _waccess(path, 0);
				if (hr != -1)
				{
					//File exists, now check for write permission
					hr = _waccess(path, 2);
					if (hr == -1)
					{
						std::wcout << "File" << path << "does not have write permission." << std::endl;
						return 2;
					}
					return 1;
				}
				return -1;
			}

			//Get the length of file
			inline unsigned long GetFileLength(std::ifstream& file)
			{
				using namespace std;

				if (!file.good()) return 0;

				unsigned long pos = file.tellg();
				file.seekg(0, ios::end);
				unsigned long len = file.tellg();
				file.seekg(ios::beg);

				return len;
			}

#if defined(__WIN32)

			//Get the current running directory path
			inline std::wstring GetCurrentDirectory()
			{
				const int Lenght = 2048;
				TCHAR exePath[Lenght];
				GetModuleFileName(NULL, exePath, Lenght);
				auto path = Convert::TCharsToWString(exePath, Lenght);
				int i = 0;
				for (i = path.length() - 1; i >= 0; i--)
				{
					if (path[i] == '\\')
					{
						break;
					}
				}
				return path.replace(path.begin() + i + 1, path.end(), L"");
			}

			//Get the content directory path 
			inline std::wstring GetContentDirectory()
			{
				return GetCurrentDirectory() + L"Content\\";
			}

			//Get the wolf content directory
			inline std::wstring GetWolfContentDirectory()
			{
				return GetCurrentDirectory() + L"WolfContent\\";
			}

			//Create directory
			inline void CreateWDirectory(LPCWSTR pPath)
			{
				WIN32_FIND_DATAW data;
				HANDLE handle = ::FindFirstFileW(pPath, &data);
				if (handle == INVALID_HANDLE_VALUE)
				{
					CreateDirectoryW(pPath, NULL);
				}
				else
				{
					FindClose(handle);
				}
			}

			//Get a unique name - format: mm-dd-year-hour-min-sec
			inline std::wstring GetUniqueName()
			{
				using namespace std;

				//get time now
				time_t t = time(0);
				struct tm* now = localtime(&t);
				auto s = stringstream();
				s << 1 + now->tm_mon << "-" << now->tm_mday << "-" << 1900 + now->tm_year << "-" << now->tm_hour << "-" << now->tm_min << "-" << now->tm_sec;

				auto str = s.str();
				wstring name(str.begin(), str.end());

				//Clear sstream
				s.clear();
				s.str(std::string());

				return name;
			}

			//check whether a directory is exist or not
			inline bool IsDirectoryExists(const std::wstring directoryPath)
			{
				HRESULT hr = GetFileAttributesW(directoryPath.c_str());
				if (hr == INVALID_FILE_ATTRIBUTES)
				{
					std::wcout << directoryPath + L"not found" << std::endl;
					return nullptr;
				}
				std::wcout << "State of IsDirectoryExists : " << directoryPath << " " << hr << std::endl;
				if (hr & FILE_ATTRIBUTE_DIRECTORY) return true;// this is a directory
				return false;
			}

			// Read binary file and return array of uint8_t
			// fileState indicates to state of file
			// 1 means everything is ok
			//-1 means the file could not be found, 
			//-2 means file is exist but could not open
			inline HRESULT ReadBinaryFile(_In_z_ std::wstring fileName, _Inout_ std::unique_ptr<uint8_t[]>& data,
				_Out_ size_t* dataSize, _Out_ int* fileState)
			{
				//Based on DirectXTK

				auto path = fileName.c_str();
				*fileState = IsFileExists(path);
				if (*fileState != 1)
				{
					return S_FALSE;
				}

				// Open the file.
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
				ScopedHandle hFile(safe_handle(CreateFile2(path, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr)));
#else
				ScopedHandle hFile(safe_handle(CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)));
#endif

				if (!hFile)	return HRESULT_FROM_WIN32(GetLastError());

				// Get the file size.
				LARGE_INTEGER fileSize = { 0 };

#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA)
				FILE_STANDARD_INFO fileInfo;

				if (!GetFileInformationByHandleEx(hFile.get(), FileStandardInfo, &fileInfo, sizeof(fileInfo)))
				{
					return HRESULT_FROM_WIN32(GetLastError());
				}

				fileSize = fileInfo.EndOfFile;
#else
				GetFileSizeEx(hFile.get(), &fileSize);
#endif

				// File is too big for 32-bit allocation, so reject read.
				if (fileSize.HighPart > 0)
					return E_FAIL;

				// Create enough space for the file data.
				data.reset(new uint8_t[fileSize.LowPart]);

				if (!data)
				{
					return E_OUTOFMEMORY;
				}
				// Read the data in.
				DWORD bytesRead = 0;

				if (!ReadFile(hFile.get(), data.get(), fileSize.LowPart, &bytesRead, nullptr))
				{
					return HRESULT_FROM_WIN32(GetLastError());
				}

				if (bytesRead < fileSize.LowPart)
				{
					return E_FAIL;
				}

				*dataSize = bytesRead;

				return S_OK;
			}

#endif

			// Read text file from root content directory of Wolf(WolfContent) and return wstring
			// fileState indicates to state of file
			// 1 means everything is ok
			//-1 means the file could not be found, 
			//-2 means file is exist but it might be corrupted
			//-3 means file is empty
			//-4 means could not open file
			inline const char* ReadTextFile(std::wstring filename, _Out_ int& fileState)
			{
				std::vector<byte> data;
				std::wstring dir = L"";

#if defined(MAYA) || defined(__WIN32)

				dir = GetWolfContentDirectory();

#else
				using namespace Windows::Storage;
				auto folder = Windows::ApplicationModel::Package::Current->InstalledLocation;
				dir = folder->Path->ToString()->Data();

#endif // MAYA
				auto fullpath = dir.append(filename.c_str());

				fileState = IsFileExists(fullpath);
				if (fileState != 1)
				{
					return nullptr;
				}

				std::ifstream file(fullpath, std::ios::in);
				if (!file)
				{
					//file is exist but it might be corrupted
					fileState = -2;
					return nullptr;
				}
				auto filelength = GetFileLength(file);
				if (filelength == 0)
				{
					//file is empty
					fileState = -3;
					return "";
				}

				if (!file.is_open())
				{
					//could not open file
					fileState = -4;
					return nullptr;
				}

				auto source = new char[filelength + 1];
				source[filelength] = 0;

				unsigned int i = 0;
				while (file.good())
				{
					source[i] = file.get();
					if (!file.eof())
					{
						i++;
					}
				}
				source[i] = 0;

				//close the file
				file.close();
				
				fileState = 1;
				return source;
			}
		}
	}
}