/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_bounding_frustum.h
	Description		 : The bounding box helper
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_BOUNDING_FRUSTUM_H__
#define __W_BOUNDING_FRUSTUM_H__

#include "w_graphics_device_manager.h"
#include "w_cpipeline_model.h"

#ifdef __GNUC__
#pragma GCC visibility push(default)
#endif

namespace wolf
{
	namespace framework
	{
		//The bounding frustum class
		class w_bounding_frustum : public wolf::system::w_object
		{
		public:
            W_EXP w_bounding_frustum();
            W_EXP w_bounding_frustum(_In_ const wolf::content_pipeline::w_bounding_box& pBoundingBox);
            W_EXP virtual ~w_bounding_frustum();
            
			//Release all resources
			W_EXP ULONG release() override;

#pragma region Setters

			
#pragma endregion

#pragma region Getters


#pragma endregion

		protected:
			
		private:
			typedef	 wolf::system::w_object                       _super;


		};
	}
}

#ifdef __GNUC__
#pragma GCC visibility pop
#endif

#endif
