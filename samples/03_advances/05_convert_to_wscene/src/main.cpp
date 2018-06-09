/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : main.cpp
	Description		 : This sample shows how to convert collada file to Wolf Scene Pack
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/
*/

#include <pch.h>
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
	auto _content_path_dir = wolf::system::io::get_current_directory();
#ifdef WIN32
	_content_path_dir += "/../../../../content/";
#elif defined(__APPLE__)
	_content_path_dir += "/../../../../../content/";
#endif // WIN32

	logger.initialize(L"05_convert_to_wscene.Win32", wolf::system::convert::string_to_wstring(_content_path_dir));
	//log to output file
	logger.write(L"Wolf initialized");

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//load all collada models
	auto _models_dir = _content_path_dir + "models/sponza/";
	std::vector<std::string> _file_names;
	wolf::system::io::get_files_folders_in_directory(_models_dir, _file_names);
	for (auto& _file_name : _file_names)
	{
		auto _c_str = _file_name.c_str();

		auto _dir = wolf::system::io::get_parent_directory(_file_name) + "\\";
		auto _name = wolf::system::io::get_file_name(_file_name);
		auto _base_name = wolf::system::io::get_base_file_name(_name);
		auto _ext = wolf::system::io::get_file_extention(_name);

		if (_ext.empty() || _ext == "." || _ext == ".wscene") continue;
        
        auto _file_name_str = wolf::system::convert::string_to_wstring(_file_name);
		auto _scene = w_content_manager::load<w_cpipeline_scene>(_file_name_str);
		if (_scene)
		{
			printf("start converting: %s\r\n", _c_str);
			logger.write("start converting: {}", _c_str);
			std::vector<w_cpipeline_scene> _scene_packs = { *_scene };
            
            auto _out_path = wolf::system::convert::string_to_wstring(_dir + _base_name + ".wscene");
			if (w_content_manager::save_wolf_scenes_to_file(_scene_packs, _out_path) == W_PASSED)
			{
				printf("scene %s converted\r\n", _c_str);
				logger.write("scene {} converted", _c_str);
			}
			else
			{
				printf("error on converting %s\r\n", _c_str);
				logger.write("error on converting {}", _c_str);
			}
			_scene->release();
		}
		else
		{
			printf("file %s not exists\r\n", _c_str);
			logger.write("file {} not exists", _c_str);
		}
	}
	w_content_manager::release();	
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

	release_heap_data();

    return EXIT_SUCCESS;
}
