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
	auto _current_path_dir = wolf::system::io::get_current_directoryW();
#ifdef WIN32
	auto _content_path_dir = _current_path_dir + L"/../../../../content/";
#elif defined(__APPLE__)
	auto _content_path_dir = _current_path_dir + L"/../../../../../content/";
#endif // WIN32

 
	wolf::system::w_logger_config _log_config;
	_log_config.app_name = L"05_convert_to_wscene.Win32";
	_log_config.log_path = _current_path_dir;
	_log_config.log_to_std_out = true;

    logger.initialize(_log_config);
	//log to output file
	logger.write(L"wolf initialized");

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//load all models from following folder "content/models/sponza/"
	auto _parent_dir = _content_path_dir + L"models/sponza/";
	std::vector<std::wstring> _file_names;
	wolf::system::io::get_files_folders_in_directoryW(_parent_dir, _file_names);
	for (auto& _file_name : _file_names)
	{
		auto _ext = wolf::system::io::get_file_extentionW(_file_name);
        auto _base_name = wolf::system::io::get_base_file_nameW(_file_name);

		if (_ext.empty() || _ext == L"." || _ext == L".wscene") continue;
        
        auto _scene = w_content_manager::load<w_cpipeline_scene>(_parent_dir + _file_name);
		if (_scene)
		{
			logger.write(L"start converting: {}", _file_name);
			std::vector<w_cpipeline_scene> _scene_packs = { *_scene };
            
            auto _out_path = _parent_dir + _base_name + L".wscene";
			if (w_content_manager::save_wolf_scenes_to_file(_scene_packs, _out_path) == W_PASSED)
			{
				logger.write(L"scene {} converted", _file_name);
			}
			else
			{
				logger.write(L"error on converting {}", _file_name);
			}
			_scene->release();
		}
		else
		{
			logger.write(L"file {} not exists", _file_name);
		}
	}
	w_content_manager::release();	
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

	release_heap_data();

    return EXIT_SUCCESS;
}
