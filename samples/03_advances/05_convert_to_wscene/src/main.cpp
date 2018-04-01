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
	_content_path_dir += L"/../../../../samples/03_advances/06_scene/src/content/";
#elif defined(__APPLE__)
	_content_path_dir += L"/../../../../../samples/03_advances/06_scene/src/content/";
#endif // WIN32

	//load collada model
	auto _model_dir = _content_path_dir + L"models/sponza/";
	auto _scene = w_content_manager::load<w_cpipeline_scene>(_model_dir + L"sponza.DAE");
	if (_scene)
	{
		//convert collada model to wscene model
		printf("start converting\r\n");
		std::vector<w_cpipeline_scene> _scene_packs = { *_scene };
		if (w_content_manager::save_wolf_scenes_to_file(_scene_packs, _model_dir + L"/sponza.wscene") == W_PASSED)
		{
			printf("scene converted\r\n");
		}
		else
		{
			printf("error on converting\r\n");
		}
		_scene->release();
	}
	else
	{
		printf("scene not found\r\n");
	}
	
    return 0;
}
