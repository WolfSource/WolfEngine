/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfSource/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_content_manager.h
	Description		 : This content manager class
	Comment          :
*/

#ifndef __W_CONTENT_MANAGER_H__
#define __W_CONTENT_MANAGER_H__

#include "w_cpipeline_export.h"
#include <string>
#include <w_io.h>
//#include "collada/c_parser.h"
//#include "w_cpipeline_scene.h"
#include <msgpack.hpp>
#include "simplygon/simplygon.h"
#include <assimp/w_assimp.h>
#include <w_compress.hpp>

namespace wolf
{
	namespace content_pipeline
	{
		class w_content_manager
		{
		public:

			template<class T>
			static T* load(_In_z_ const std::wstring& pAssetPath,
                           _In_ const bool& pOptimizeMeshUsingAMDTootle =
#ifdef __WIN32
                           true
#else
                           false
#endif

#ifdef __WIN32
                           , _In_ const bool& pGenerateLODUsingSimplygon = true
#endif
            )
			{
#if defined(__WIN32) || defined(__UWP)
				auto _file_exists = wolf::system::io::get_is_fileW(pAssetPath.c_str());
#else
				auto _path = wolf::system::convert::wstring_to_string(pAssetPath);
				auto _file_exists = wolf::system::io::get_is_file(_path.c_str());
#endif
				if (_file_exists == W_FAILED)
				{
					logger.error(L"File asset not available on following path : {}", pAssetPath.c_str());
					return nullptr;
				}

				auto _extension = wolf::system::io::get_file_extentionW(pAssetPath);
				auto _name = wolf::system::io::get_base_file_name(wolf::system::convert::wstring_to_string(pAssetPath));

				//to lower
				std::transform(_extension.begin(), _extension.end(), _extension.begin(), ::tolower);
				std::string _type(typeid(T).name());

				if (std::is_same<T, wolf::content_pipeline::w_cpipeline_scene>::value)
				{
					if (_extension == L".wscene")
					{
						std::vector<w_cpipeline_scene> _scenes;
						auto _hr = load_wolf_scenes_from_file(_scenes, pAssetPath);
						if (_hr == W_PASSED && _scenes.size())
						{
							//make a deep copy from first element
							auto _scene = new w_cpipeline_scene(_scenes[0]);
							_scenes.clear();
							return _scene;
						}
						_scenes.clear();
					}
					else
					{
						return assimp::w_assimp::load(
							pAssetPath,
							pOptimizeMeshUsingAMDTootle
#ifdef __WIN32
							, pGenerateLODUsingSimplygon
#endif
						);
					}
				}
				logger.error(L"Type not supported for {}", pAssetPath);
				return nullptr;
			}

			static W_RESULT save_wolf_scenes_to_memory(_In_ std::vector<w_cpipeline_scene>& pScenePacks, 
				_Inout_ w_compress_result& pWolfScenePacked)
			{
				using namespace wolf::system;

				//pack it, then compress it
				std::stringstream _sbuffer;
				msgpack::pack(_sbuffer, pScenePacks);
				
				std::string _str = _sbuffer.str();
				pWolfScenePacked.size_in = _str.size();
				auto _hr = w_compress::compress_buffer(_str.c_str(), &pWolfScenePacked);
				if (_hr != W_PASSED)
				{
					logger.error("error happened while compressing scenes");
					pWolfScenePacked.size_in = 0;
					pWolfScenePacked.size_out = 0;
					if (pWolfScenePacked.data)
					{
						free(pWolfScenePacked.data);
						pWolfScenePacked.data = nullptr;
					}
				}

				_str.clear();
				_sbuffer.clear();

				return _hr;
			}

			static W_RESULT save_wolf_scenes_to_file(_In_ std::vector<w_cpipeline_scene>& pScenePacks, _In_z_ std::wstring pWolfSceneFilePath)
			{
#if defined(__WIN32) || defined(__UWP)
				auto _path = pWolfSceneFilePath;
#else
				auto _path = wolf::system::convert::wstring_to_string(pWolfSceneFilePath);
#endif

				w_compress_result _wolf_scene_packed;
				if (save_wolf_scenes_to_memory(pScenePacks, _wolf_scene_packed) == W_PASSED)
				{
					if (_wolf_scene_packed.data && _wolf_scene_packed.size_out)
					{
						std::ofstream _file(_path, std::ios::out | std::ios::binary);
						if (!_file || _file.bad())
						{
#if defined(__WIN32) || defined(__UWP)
							logger.error(L"Error on creating saving wolf scene file on following path: {}", _path);
#else
							logger.error("Error on creating saving wolf scene file on following path: {}", _path);
#endif
							_file.close();
							return W_FAILED;
						}

						for (size_t i = 0; i < _wolf_scene_packed.size_out; ++i)
						{
							_file << _wolf_scene_packed.data[i];
						}
						_file.flush();
						_file.close();

						free(_wolf_scene_packed.data);

						return W_PASSED;
					}
					else
					{
						logger.error(L"Could not create file with zero size on following path: {}", _path);
					}
				}

				return W_FAILED;
			}

			static W_RESULT load_wolf_scenes_from_memory(_In_z_ std::string pWolfScenePacked, _Inout_ std::vector<w_cpipeline_scene>& pScenePacks)
			{
				using namespace wolf::system;

				//decompress it, then unpack it
				w_compress_result _decompress_result;
				_decompress_result.size_in = pWolfScenePacked.size(); //1 for '\0'
				auto _hr = w_compress::decompress_buffer(pWolfScenePacked.c_str(), &_decompress_result);
				if (_hr == W_RESULT::W_PASSED)
				{
					auto _msg = msgpack::unpack(_decompress_result.data, _decompress_result.size_out + 1);
					_msg.get().convert(pScenePacks);
				}
				else
				{
					logger.error("error happened while decompressing scenes");
				}
				if (_decompress_result.data)
				{
					free(_decompress_result.data);
				}

				return _hr;
			}

			static W_RESULT load_wolf_scenes_from_file(_In_ std::vector<w_cpipeline_scene>& pScenePacks, _In_z_ std::wstring pWolfSceneFilePath)

			{
#if defined(__WIN32) || defined(__UWP)
				auto _path = pWolfSceneFilePath;
#else
				auto _path = wolf::system::convert::wstring_to_string(pWolfSceneFilePath);
#endif
				std::ifstream _file(_path, std::ios::in | std::ios::binary);
				if (!_file || _file.bad())
				{
#if defined(__WIN32) || defined(__UWP)
					logger.error(L"Error on opening wolf scene file from following path: {}", _path);
#else
					logger.error("Error on opening wolf scene file from following path: {}", _path);
#endif
					_file.close();
					return W_FAILED;
				}
				std::string _str((std::istreambuf_iterator<char>(_file)),
					std::istreambuf_iterator<char>());

				auto _hr = load_wolf_scenes_from_memory(_str, pScenePacks);

				_file.close();

				return _hr;
			}

			static void release()
			{
#ifdef __WIN32
				simplygon::release();
#endif
            }
		};
	}
}

#include "python_exporter/py_content_manager.h"

#endif
