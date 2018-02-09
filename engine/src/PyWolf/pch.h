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


#include <boost/python.hpp>

#include <tbb/atomic.h>

//from wolf::system namespaces
#include <wolf.h>
#include <w_bounding.h>
#include <w_color.h>
#include <w_timer.h>
#include <w_timer_callback.h>
#include <w_inputs_manager.h>
#include <w_time_span.h>
#include <w_point.h>
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
#include <w_graphics/w_command_buffer.h>
#include <w_graphics/w_render_pass.h>
#include <w_graphics/w_texture.h>

#endif //__PCH_H__
