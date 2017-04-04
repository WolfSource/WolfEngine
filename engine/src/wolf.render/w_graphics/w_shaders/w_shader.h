/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_shader.h
	Description		 : A class which is responsible to manage shaders for materials
	Comment          :
*/

#ifndef __W_SHADER_H__
#define __W_SHADER_H__

#include "w_graphics_device_manager.h"

enum w_shader_stage
{
	VERTEX_SHADER,
#if defined(__DX12__) || defined(__DX11__)
	PIXEL_SHADER,
#elif defined(__VULKAN__)
        FRAGMENT_SHADER,
#endif
	GEOMETRY_SHADER,
	DOMAIN_SHADER,
	HULL_SHADER,
	COMPUTE_SHADER
};

namespace wolf
{
	namespace graphics
	{
		class w_shader : public system::w_object
		{
		public:
			W_EXP w_shader();
			W_EXP virtual ~w_shader();

			//Create shader from binary file
			virtual HRESULT load(_In_ std::shared_ptr<w_graphics_device>& pGDevice,
                            const char* pShaderBinaryPath);

			//Apply shader, this must be called before drawing primitives
			virtual void apply();
                        
			virtual ULONG release() override;

#pragma region Getters

#pragma endregion

#pragma region Setters


#pragma endregion

		private:
                    typedef	system::w_object                                _super;
                    std::shared_ptr<w_graphics_device>                          _gDevice;
                    
#if defined(__DX11__) || defined(__DX12__)
#elif defined(__VULKAN__)
                    VkShaderModule                                              _shader_module;                   
#endif
		};
	}
}

#endif
