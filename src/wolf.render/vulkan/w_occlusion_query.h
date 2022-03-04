/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_occlusion_query.h
	Description		 : hardware occlusion query
	Comment          :
*/

#pragma once

#include <w_graphics_device_manager.h>

namespace wolf::render::vulkan
{
	class w_occlusion_query_pimp;
	class w_occlusion_query
	{
	public:
		W_VK_EXP w_occlusion_query();
		W_VK_EXP virtual ~w_occlusion_query();

		//initialize occlusion query
		W_VK_EXP W_RESULT initialize(
			_In_ const std::shared_ptr<wolf::render::vulkan::w_graphics_device>& pGDevice,
			_In_ const uint32_t pQuerySize);

		//wait for results of query
		W_VK_EXP uint64_t* wait_for_query_results(_Inout_ size_t& pNumberOfResults);
		//get results of query with availability bit
		W_VK_EXP uint64_t* get_available_query_results(_Inout_ size_t& pNumberOfResults);
		//get results of query with partial bit
		W_VK_EXP uint64_t* get_partial_query_results(_Inout_ size_t& pNumberOfResults);

		//reset query
		W_VK_EXP void reset(_In_ const w_command_buffer& pCommandBuffer, _In_ const uint32_t pFirstQuery = 0);
		//begin query
		W_VK_EXP void begin_query(_In_ const w_command_buffer& pCommandBuffer, _In_ const uint32_t pQueryIndex);
		//end query
		W_VK_EXP void end_query(_In_ const w_command_buffer& pCommandBuffer, _In_ const uint32_t pQueryIndex);

		//release occlusion query
		W_VK_EXP ULONG release();
	private:
		bool											_is_released;
		w_occlusion_query_pimp* _pimp;
	};
}
