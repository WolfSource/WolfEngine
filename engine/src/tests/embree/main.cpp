#include "pch.h"
#include "scene.h"

using namespace std;
using namespace wolf;

int main()
{
    //initialize and content path and log file
    auto _running_dir = wolf::system::io::get_current_directoryW();
    std::wstring _content_path = _running_dir + L"/content/";
    
    //init logger
    wolf::system::w_logger_config _log_config;
    _log_config.app_name = L"wolf.engine.embree.test";
    _log_config.log_path = _running_dir;
    _log_config.flush_log_only_on_error = false;
    _log_config.log_to_std_out = true;
    
    logger.initialize(_log_config);
    
    //query for graphics devices
    int _number_of_graphics_devices = -1;
    auto _graphics_devices_infos = wolf::render::embree::w_graphic_device_manager::query_for_available_graphic_devices(_number_of_graphics_devices);
    if (_number_of_graphics_devices != -1)
    {
        for (auto i = 0; i < _number_of_graphics_devices; ++i)
        {
            logger.write("graphics device #{}:{}", _graphics_devices_infos[i].index, _graphics_devices_infos[i].description);
        }
    }
    
    w_present_info _present_info;
    _present_info.width = 800;
    _present_info.height = 600;
    _present_info.preferred_graphics_device_index = 0;
    
    //create scene
    auto _scene = new scene(_log_config.app_name, _content_path);
    _scene->run(_present_info);
    
    //release all
    _scene->release();
    wolf::release_heap_data();

	return EXIT_SUCCESS;
}
