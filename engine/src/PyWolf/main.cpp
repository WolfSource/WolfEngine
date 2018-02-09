#include "pch.h"

enum DLL_EXIT_STATE { RUNNING, EXITING, EXITED};
static tbb::atomic<DLL_EXIT_STATE> sDLLExitState;

#if defined(__WIN32) || defined (__UWP)

BOOL APIENTRY DllMain(_In_ HMODULE pHModule, _In_ DWORD pULReasonForCall, _In_ LPVOID pLPReserved)
{
	switch (pULReasonForCall)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
		sDLLExitState = RUNNING;
		break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		sDLLExitState = EXITED;
		break;
	}
	return TRUE;
}

#endif

#ifdef __PYTHON__

using namespace boost::python;
using namespace wolf::system;

#include <python_exporter/py_logger.h>

BOOST_PYTHON_MODULE(pyWolf)
{
    //export wolf::system classes to pyWolf.system scope
    {
        struct system {};
        scope _system = class_<system>("system");
      
        pywolf::py_bounding_export();
        pywolf::py_color_export();
        pywolf::py_game_time_export();
        pywolf::py_timer_export();
        pywolf::py_time_span_export();
        pywolf::py_inputs_manager_export();
        pywolf::py_point_export();
        pywolf::py_rectangle_export();
        pywolf::py_window_export();
    }

    //export wolf::content_pipeline classes to pyWolf.content_pipeline scope
    {


    }

    //export wolf::media_core classes to pyWolf.media_core scope
    {


    }

    //export wolf::graphics classes to pyWolf.graphics scope
    {
        struct graphics {};
        scope _graphics = class_<graphics>("graphics");

        pywolf::py_graphics_device_manager_export();  
		pywolf::py_command_buffer_export();
		pywolf::py_queue_export();
		pywolf::py_fences_export();
		pywolf::py_semaphore_export();
		pywolf::py_render_pass_export();
		pywolf::py_shader_export();
		pywolf::py_texture_export();
    }

    //export wolf::framework classes to pyWolf.framework scope
    {
        struct framework {};
        scope _framework = class_<framework>("framework");

        pywolf::py_game_export();

    }

    //global scope
    def("bounding_box_from_bounding_sphere", w_bounding_box::create_from_bounding_sphere);
    def("bounding_sphere_from_bounding_box", w_bounding_sphere::create_from_bounding_box);
    //export logger in global scope
    pywolf::py_logger_export();

    def("release_shared_data_over_all_instances", wolf::release_shared_data_over_all_instances);
}

#endif