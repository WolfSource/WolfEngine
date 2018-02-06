/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website          : http://WolfSource.io
    Name             : py_render_pass.h
    Description      : The python exporter for w_render_pass class
    Comment          :
*/

#ifdef __PYTHON__

#ifndef __PY_RENDER_PASS_H__
#define __PY_RENDER_PASS_H__

#include <python_exporter/w_boost_python_helper.h>

namespace pywolf
{
	using namespace wolf::graphics;

	class py_render_pass : public w_render_pass
	{
	public:
		py_render_pass()
		{
		}

		bool py_load(
			_In_ boost::shared_ptr<w_graphics_device>& pGDevice,
			_In_ const w_viewport& pViewPort,
			_In_ const w_viewport_scissor& pViewPortScissor)
		{
			//create render pass attchaments
			if (!pGDevice.get()) return false;
			
			auto _gDevice = boost_shared_ptr_to_std_shared_ptr<w_graphics_device>(pGDevice);

			//TODO: export w_image_view to python and make the following code, editable from python
			std::vector<std::vector<w_image_view>> _render_pass_attachments;
			for (size_t i = 0; i < _gDevice->output_presentation_window.vk_swap_chain_image_views.size(); ++i)
			{
				_render_pass_attachments.push_back
				(
					//COLOR										  , DEPTH
					{ _gDevice->output_presentation_window.vk_swap_chain_image_views[i], _gDevice->output_presentation_window.vk_depth_buffer_image_view }
				);
			}
			if (!_render_pass_attachments.size()) return false;

			auto _hr = _super::load(
				_gDevice,
				pViewPort,
				pViewPortScissor,
				_render_pass_attachments);

			//reset local shared_ptr
			_gDevice.reset();

			return _hr == S_OK;
		}

		void py_begin(
			_In_ const uint32_t& pFrameBufferIndex,
			_In_ const w_command_buffer& pCommandBuffer,
			_In_ const w_color& pClearColor,
			_In_ const float& pClearDepth,
			_In_ const uint32_t& pClearStencil)
		{
			_super::begin(
				pFrameBufferIndex,
				&pCommandBuffer,
				pClearColor,
				pClearDepth,
				pClearStencil);
		}

		void py_end(_In_ const w_command_buffer& pCommandBuffer)
		{
			_super::end(&pCommandBuffer);
		}

	private:
		typedef  w_render_pass			_super;
	};

	static void py_render_pass_export()
	{
		using namespace boost::python;

		//export w_render_pass class
		class_<py_render_pass, boost::noncopyable>("w_render_pass")
			.def("load", &py_render_pass::py_load, "load render pass which contains frame buffers")
			.def("begin", &py_render_pass::py_begin, "begin render pass")
			.def("end", &py_render_pass::py_end, "end render pass")
			.def("release", &py_render_pass::release, "release")
			.def("viewport", &py_render_pass::get_viewport, "get viewport")
			.def("viewport_scissor", &py_render_pass::get_viewport_scissor, "get viewport scissor")
			.def("get_number_of_frame_buffers", &py_render_pass::get_number_of_frame_buffers, "get number of frame buffers")
			.def("set_viewport", &py_render_pass::set_viewport, "set viewport")
			.def("set_viewport_scissor", &py_render_pass::set_viewport_scissor, "set viewport scissor")
			;
	}
}

#endif//__PY_RENDER_PASS_H__

#endif//__PYTHON__

