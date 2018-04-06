//++++++++++++++++++++++++++++++++++++++++++++++++++++
//The following codes have been added for this project
//++++++++++++++++++++++++++++++++++++++++++++++++++++

/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : compute_stage.h
	Description		 : The main class for cmpute shader of model
	Comment          : 
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_COMPUTE_STAGE_H__
#define __W_COMPUTE_STAGE_H__

#include "w_graphics_device_manager.h"
#include <w_graphics/w_command_buffers.h>
#include <w_graphics/w_pipeline.h>
#include <w_graphics/w_uniform.h>

#pragma pack(push,1)
struct compute_instance_data
{
	glm::vec4   pos;
};
#pragma pack(pop)

#pragma region compute uniforms

#pragma pack(push,1)
struct compute_unifrom_x1
{
	glm::vec4           camera_pos;
	glm::vec4	        is_visible;
};
#pragma pack(pop)

#pragma pack(push,1)
struct compute_unifrom_x2
{
	glm::vec4           camera_pos;
	glm::vec4	        is_visible;
};
#pragma pack(pop)

#pragma pack(push,1)
struct compute_unifrom_x4
{
	glm::vec4           camera_pos;
	glm::vec4	        is_visible;
};
#pragma pack(pop)

#pragma pack(push,1)
struct compute_unifrom_x8
{
	glm::vec4           camera_pos;
	glm::vec4	        is_visible[2];
};
#pragma pack(pop)

#pragma pack(push,1)
struct compute_unifrom_x16
{
	glm::vec4           camera_pos;
	glm::vec4	        is_visible[4];
};
#pragma pack(pop)

#pragma pack(push,1)
struct compute_unifrom_x32
{
	glm::vec4           camera_pos;
	glm::vec4	        is_visible[8];
};
#pragma pack(pop)

#pragma pack(push,1)
struct compute_unifrom_x64
{
	glm::vec4           camera_pos;
	glm::vec4	        is_visible[16];
};
#pragma pack(pop)

#pragma pack(push,1)
struct compute_unifrom_x128
{
	glm::vec4           camera_pos;
	glm::vec4	        is_visible[32];
};
#pragma pack(pop)

#pragma pack(push,1)
struct compute_unifrom_x256
{
	glm::vec4           camera_pos;
	glm::vec4	        is_visible[64];
};
#pragma pack(pop)

#pragma pack(push,1)
struct compute_unifrom_x512
{
	glm::vec4           camera_pos;
	glm::vec4	        is_visible[128];
};
#pragma pack(pop)

#pragma pack(push,1)
struct compute_unifrom_x1024
{
	glm::vec4           camera_pos;
	glm::vec4	        is_visible[256];
};
#pragma pack(pop)

struct compute_stage
{
	uint32_t                                                batch_local_size = 1;

	wolf::graphics::w_uniform<compute_unifrom_x1>*          unifrom_x1;
	wolf::graphics::w_uniform<compute_unifrom_x2>*          unifrom_x2;
	wolf::graphics::w_uniform<compute_unifrom_x4>*          unifrom_x4;
	wolf::graphics::w_uniform<compute_unifrom_x8>*          unifrom_x8;
	wolf::graphics::w_uniform<compute_unifrom_x16>*         unifrom_x16;
	wolf::graphics::w_uniform<compute_unifrom_x32>*         unifrom_x32;
	wolf::graphics::w_uniform<compute_unifrom_x64>*         unifrom_x64;
	wolf::graphics::w_uniform<compute_unifrom_x128>*        unifrom_x128;
	wolf::graphics::w_uniform<compute_unifrom_x256>*        unifrom_x256;
	wolf::graphics::w_uniform<compute_unifrom_x512>*        unifrom_x512;
	wolf::graphics::w_uniform<compute_unifrom_x1024>*       unifrom_x1024;

	wolf::graphics::w_buffer                                instance_buffer;
	wolf::graphics::w_buffer                                lod_levels_buffers;

	wolf::graphics::w_pipeline                              pipeline;
	wolf::graphics::w_command_buffers                       command_buffers;
	wolf::graphics::w_semaphore                             semaphore;

	void release()
	{
		SAFE_RELEASE(this->unifrom_x1);
		SAFE_RELEASE(this->unifrom_x2);
		SAFE_RELEASE(this->unifrom_x4);
		SAFE_RELEASE(this->unifrom_x8);
		SAFE_RELEASE(this->unifrom_x16);
		SAFE_RELEASE(this->unifrom_x32);
		SAFE_RELEASE(this->unifrom_x64);
		SAFE_RELEASE(this->unifrom_x128);
		SAFE_RELEASE(this->unifrom_x256);
		SAFE_RELEASE(this->unifrom_x512);
		SAFE_RELEASE(this->unifrom_x1024);

		this->instance_buffer.release();
		this->lod_levels_buffers.release();
		this->pipeline.release();
		this->command_buffers.release();
		this->semaphore.release();
	}

};

#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++