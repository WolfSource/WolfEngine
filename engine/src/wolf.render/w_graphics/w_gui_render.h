/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_gui_render.h
	Description		 : render wolf::gui controls
	Comment          : 
*/

#ifndef __W_GUI_RENDER_H__
#define __W_GUI_RENDER_H__

#include "w_graphics_device_manager.h"
#include "w_render_pass.h"
#include <w_gui_vertex.h>

namespace wolf
{
	namespace graphics
	{
        class w_gui_render_pimp;
		class w_gui_render : public wolf::system::w_object
		{
		public:
			W_EXP w_gui_render();
			W_EXP virtual ~w_gui_render();
            
            W_EXP HRESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice);

            W_EXP HRESULT render(
                _In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                _In_ const w_render_pass* pRenderPass,
                _In_ const std::string& pPipelineCacheName,
                _In_ const VkCommandBuffer& pCommandBuffer, 
                _In_ std::map<std::string, std::vector<wolf::gui::w_gui_vertex_2d>>& pWidgetsVertices);

            W_EXP virtual ULONG release() override;

		private:
			typedef system::w_object						    _super;
            w_gui_render_pimp*                                  _pimp;
        };
    }
}

#endif
