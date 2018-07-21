/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_occlusion_query.h
	Description		 : hardware occlusion query
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_OCCLUSION_QUERY_H__
#define __W_OCCLUSION_QUERY_H__

#include <w_graphics_device_manager.h>

namespace wolf
{
	namespace render
	{
		namespace vulkan
		{
			class w_occlusion_query_pimp;
			class w_occlusion_query : public system::w_object
			{
			public:
				w_occlusion_query();
				virtual ~w_occlusion_query();

				//initialize occlusion query
				W_EXP W_RESULT initialize(
					_In_ const std::shared_ptr<wolf::render::vulkan::w_graphics_device>& pGDevice,
					_In_ const uint32_t& pQuerySize);

				//wait for results of query
				void wait_for_query_results();
				//get results of query with availability bit
				void get_available_query_results();
				//get results of query with partial bit
				void get_partial_query_results();
				//reset query
				void reset(_In_ const w_command_buffer& pCommandBuffer, _In_ const uint32_t& pFirstQuery = 0);
				//begin query
				void begin_query(_In_ const w_command_buffer& pCommandBuffer);
				//end query
				void end_query(_In_ const w_command_buffer& pCommandBuffer);

				//release occlusion query
				W_EXP ULONG release() override;
			private:
				typedef system::w_object                        _super;
				w_occlusion_query_pimp*                         _pimp;
			};
		}
	}
}

#endif
