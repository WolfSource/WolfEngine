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
		auto _c_str = wolf::system::convert::wstring_to_string(_file_name).c_str();

		auto _dir = wolf::system::io::get_parent_directoryW(_file_name) + L"\\";
		auto _name = wolf::system::io::get_file_nameW(_file_name);
		auto _base_name = wolf::system::io::get_base_file_nameW(_name);
		auto _ext = wolf::system::io::get_file_extentionW(_name);

		if (_ext.empty() || _ext == L"." || _ext == L".wscene") continue;

		auto _scene = w_content_manager::load<w_cpipeline_scene>(_file_name);
		if (_scene)
		{
			printf("start converting: %s\r\n", _c_str);
			std::vector<w_cpipeline_scene> _scene_packs = { *_scene };
			if (w_content_manager::save_wolf_scenes_to_file(_scene_packs, _dir + _base_name + L".wscene") == W_PASSED)
			{
				printf("scene %s converted\r\n", _c_str);
			}
			else
			{
				printf("error on converting %s\r\n", _c_str);
			}
			_scene->release();
		}
		else
		{
			printf("file %s not exists\r\n", _c_str);
		}
	}
	w_content_manager::release();	
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

    return 0;
}
