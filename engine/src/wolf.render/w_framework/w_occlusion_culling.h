/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfSource/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_occlusion_culling.h
	Description		 : The w_occlusion_culling
	Comment          :
*/

#ifndef __W_OCCLUSION_CULLING_H__
#define __W_OCCLUSION_CULLING_H__

#include "w_render_export.h"
#include <w_object.h>

namespace wolf
{
	namespace framework
	{
		class w_occlusion_culling_pimp;
		class w_occlusion_culling : public system::w_object
		{
		public:
			W_EXP W_RESULT initialize(_In_ uint32_t& pNumberOfWorkerThreads);

		private:
			typedef system::w_object						_super;
			w_occlusion_culling_pimp*                       _pimp;

		};
	}
}

#endif
