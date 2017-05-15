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
#include <w_object.h>
#include <string>
#include <w_io.h>
#include "collada/c_parser.h"
#include <msgpack.hpp>

namespace wolf
{
	namespace content_pipeline
	{
		class w_content_manager : public wolf::system::w_object
		{
		public:

			template<class T>
            static T* load(std::wstring pAssetPath)
            {
#if defined(__WIN32) || defined(__UWP)
                auto _file_exists = wolf::system::io::get_is_file(pAssetPath.c_str());
#else
                auto _path = wolf::system::convert::wstring_to_string(pAssetPath);
                auto _file_exists = wolf::system::io::get_is_file(_path.c_str());
#endif
                if (_file_exists == S_FALSE)
                {
                    logger.error(L"File asset not available on following path : " + pAssetPath);
                    return nullptr;
                }

                auto _extension = wolf::system::io::get_file_extentionW(pAssetPath);
                auto _name = wolf::system::io::get_base_file_name(wolf::system::convert::wstring_to_string(pAssetPath));

                //to lower
                std::transform(_extension.begin(), _extension.end(), _extension.begin(), ::tolower);
                std::string _type(typeid(T).name());

                if (std::is_same<T, wolf::content_pipeline::w_cpipeline_scene>::value)
                {
                    if (_extension == L".dae")
                    {
                        //load scene from collada file
                        auto _scene = new w_cpipeline_scene();
                        _scene->set_name(_name);
                        auto _hr = collada::c_parser::parse_collada_from_file(pAssetPath, _scene);

                        _extension.clear();
                        _name.clear();
                        _type.clear();

                        if (_hr == S_OK)
                        {
                            return _scene;
                        }

                        SAFE_RELEASE(_scene);
                    }
                    else if (_extension == L".wscene")
                    {
                        std::vector<w_cpipeline_scene> _scenes;
                        auto _hr = load_wolf_scenes_from_file(_scenes, pAssetPath);
                        if (_hr == S_OK && _scenes.size())
                        {
                            //make a deep copy from first element
                            auto _scene = new w_cpipeline_scene(_scenes[0]);
                            _scenes.clear();
                            return _scene;
                        }
                        _scenes.clear();
                    }
                }
                logger.error(L"Asset not supported : " + pAssetPath);
                return nullptr;
            }

            static HRESULT save_wolf_scenes_to_file(_In_ std::vector<w_cpipeline_scene>& pScenePacks, _In_z_ std::wstring pWolfSceneFilePath)
            {
#if defined(__WIN32) || defined(__UWP)
                auto _path = pWolfSceneFilePath;
#else
                auto _path = wolf::system::convert::wstring_to_string(pWolfSceneFilePath);
#endif
                
                std::stringstream _sbuffer;
                msgpack::pack(_sbuffer, pScenePacks);
 
                std::ofstream _file(_path, std::ios::out | std::ios::binary);
                if (!_file || _file.bad())
                {
#if defined(__WIN32) || defined(__UWP)
                    logger.error(L"Error on creating saving wolf scene file on following path: " + _path);
#else
                    logger.error("Error on creating saving wolf scene file on following path: " + _path);
#endif
                    _file.close();
                    return S_FALSE;
                }
                _file << _sbuffer.rdbuf();
                _file.flush();
                _file.close();

                _sbuffer.clear();

                return S_OK;
            }

            static HRESULT load_wolf_scenes_from_file(_In_ std::vector<w_cpipeline_scene>& pScenePacks, _In_z_ std::wstring pWolfSceneFilePath)
                                                      
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
                    logger.error(L"Error on opening wolf scene file from following path: " + _path);
#else
                    logger.error("Error on opening wolf scene file from following path: " + _path);
#endif
                    _file.close();
                    return S_FALSE;
                }
                std::string _str((std::istreambuf_iterator<char>(_file)),
                    std::istreambuf_iterator<char>());

                _file.close();

                auto _msg = msgpack::unpack(_str.data(), _str.size());
                _str.clear();

                auto _obj = _msg.get();
                _obj.convert(pScenePacks);

                return S_OK;
            }

		private:
			typedef w_object _super;

			
		};
	}
}

#endif
