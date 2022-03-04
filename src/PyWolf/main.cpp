#include "pch.h"

enum DLL_EXIT_STATE { RUNNING, EXITING, EXITED};
static std::atomic<DLL_EXIT_STATE> sDLLExitState;

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
      
        pyWolf::py_bounding_export();
        pyWolf::py_color_export();
        pyWolf::py_game_time_export();
        pyWolf::py_timer_export();
        pyWolf::py_time_span_export();
        pyWolf::py_inputs_manager_export();
        pyWolf::py_point_export();
		pyWolf::py_plane_export();
        pyWolf::py_rectangle_export();
        pyWolf::py_window_export();
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

		pyWolf::py_graphics_device_manager_export();
		pyWolf::py_command_buffer_export();
		pyWolf::py_queue_export();
		pyWolf::py_fences_export();
		pyWolf::py_semaphore_export();
		pyWolf::py_render_pass_export();
		pyWolf::py_shader_export();
		pyWolf::py_texture_export();
		pyWolf::py_mesh_export();
		pyWolf::py_pipeline_export();
		pyWolf::py_uniform_export();
		pyWolf::py_render_target_export();
		pyWolf::py_shapes_export();

		{
			struct w_graphics_device {};
			scope _graphics_device = class_<w_graphics_device>("w_graphics_device");

			struct defaults_states {};
			scope _defaults_states = class_<defaults_states>("defaults_states");

			pyWolf::py_defaults_states_export();
		}
	}

    //export wolf::framework classes to pyWolf.framework scope
    {
        struct framework {};
        scope _framework = class_<framework>("framework");

        pyWolf::py_game_export();

    }

    {
        struct glm {};
        scope _glm = class_<glm>("glm");
        
		pyWolf::py_glm_export();
    }
    
    //export logger in global scope
	pyWolf::py_std_export();
    pyWolf::py_logger_export();

	def("rotate", &glm::py_rotate, "perform rotation");
	def("scale", &glm::py_scale, "perform scale");
	def("lookAtRH", &glm::py_lookAtRH, "perform lookAt Right Handed");
	def("lookAtLH", &glm::py_lookAtLH, "perform lookAt Leftt Handed");
	def("perspectiveRH", &glm::py_perspectiveRH, "perform perspective Right Handed");
	def("perspectiveLH", &glm::py_perspectiveLH, "perform perspective Left Handed");
	def("multiply_vectors", &glm::py_multiply_vec2, "perform multiply two 2D vectors");
	def("multiply_vectors", &glm::py_multiply_vec3, "perform multiply two 3D vectors");
	def("multiply_vectors", &glm::py_multiply_vec4, "perform multiply two 4D vectors");
	def("multiply_matrices", &glm::py_multiply_mat3x3, "perform multiply two 3x3 matrices");
	def("multiply_matrices", &glm::py_multiply_mat4x4, "perform multiply two 4x4 matrices");
    def("release_shared_data_over_all_instances", wolf::release_shared_data_over_all_instances);

}

#endif
