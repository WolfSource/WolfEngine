/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : main.cpp
	Description		 : This sample shows how to convert collada file to Wolf Scene Pack
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/
*/

#include "pch.h"
#include <w_io.h>
#include <w_content_manager.h>
#include <stdio.h>

using namespace std;
using namespace wolf;
using namespace wolf::system;
using namespace wolf::content_pipeline;

int main()
{
	//set content path directory
	auto _content_path_dir = wolf::system::io::get_current_directoryW();
#ifdef WIN32
	_content_path_dir += L"/../../../../content/";
#elif defined(__APPLE__)
	_content_path_dir += L"/../../../../../content/";
#endif // WIN32

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//load all collada models
	auto _models_dir = _content_path_dir + L"models/sponza/";
	std::vector<std::wstring> _file_names;
	wolf::system::io::get_files_folders_in_directoryW(_models_dir, _file_names);
	for (auto& _file_name : _file_names)
	{
		auto _ext = wolf::system::io::get_file_extentionW(_file_name);
		if (_ext != L".DAE") continue;

		auto _scene = w_content_manager::load<w_cpipeline_scene>(_file_name);
		if (_scene)
		{
			//convert collada model to wscene model
			auto _dir = wolf::system::io::get_parent_directoryW(_file_name) + L"\\";
			auto _name = wolf::system::io::get_base_file_nameW(_file_name);

			printf("start converting: %s\r\n", _file_name);
			std::vector<w_cpipeline_scene> _scene_packs = { *_scene };
			if (w_content_manager::save_wolf_scenes_to_file(_scene_packs, _dir + _name + L".wscene") == W_PASSED)
			{
				printf("scene %s converted\r\n", _name);
			}
			else
			{
				printf("error on converting %s\r\n", _name);
			}
			_scene->release();
		}
		else
		{
			std::cout << _file_name.c_str() << L"not exists" << std::endl;
		}
	}
	w_content_manager::release();	
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

    return 0;
}
