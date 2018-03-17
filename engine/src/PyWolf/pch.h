/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : pch.hpp
	Description		 : Pre-Compiled header of pyWolf
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __PCH_H__
#define __PCH_H__

#include <wolf.h>

#ifdef __PYTHON__

#include <boost/python.hpp>

//from wolf::system namespaces
#include <w_bounding.h>
#include <w_color.h>
#include <w_timer.h>
#include <w_timer_callback.h>
#include <w_inputs_manager.h>
#include <w_time_span.h>
#include <w_point.h>
#include <w_plane.h>
#include <w_rectangle.h>
#include <w_window.h>

//from wolf::framework namespaces
#include <w_framework/w_game.h>
#include <w_framework/w_first_person_camera.h>

//from wolf::graphics namespaces
#include <w_graphics_device_manager.h>
#include <w_graphics/w_queue.h>
#include <w_graphics/w_semaphore.h>
#include <w_graphics/w_fences.h>
#include <w_graphics/w_command_buffers.h>
#include <w_graphics/w_render_pass.h>
#include <w_graphics/w_shader.h>
#include <w_graphics/w_texture.h>
#include <w_graphics/w_mesh.h>
#include <w_graphics/w_pipeline.h>
#include <w_graphics/w_uniform.h>
#include <w_graphics/w_render_target.h>

#include <python_exporter/py_glm.h>

#endif //__PYTHON__

#endif //__PCH_H__
