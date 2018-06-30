/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfSource/Wolf.Engine/issues
	Website			 : https://WolfEngine.app
	Name			 : w_graphic_device_manager.h
	Description		 : The manager for graphics devices.
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_GRAPHIC_DEVICE_MANAGER_H__
#define __W_GRAPHIC_DEVICE_MANAGER_H__

#include "w_embree_export.h"
#include <w_object.h>
#include <w_window.h>
#include <embree3/rtcore.h>

namespace wolf
{
    namespace render
    {
        namespace embree
        {
            struct w_graphic_device_info
            {
                int             index = -1;
                std::string     description;
            };
            
            //forward declaration
            class w_graphic_device_pimp;
            //contains graphics device which performs primitive-based rendering
            class w_graphic_device
            {
                friend class w_graphic_device_manager;
            public:
                W_REN_EM w_graphic_device();
                W_REN_EM virtual ~w_graphic_device();
                W_REN_EM W_RESULT initialize(_In_ w_present_info& pPresentInfo);
                W_REN_EM ULONG release();
                
#pragma region Getters
                W_REN_EM RTCDevice get_device() const;
#pragma endregion
                
            private:
                //prevent copying
                w_graphic_device(w_graphic_device const&);
                w_graphic_device& operator= (w_graphic_device const&);
                
                w_graphic_device_pimp*                                         _pimp;
            };
            
            //handles the configuration and management of the graphics device.
            struct w_graphic_device_manager
            {
                //query for graphics devices
                W_REN_EM static w_graphic_device_info* query_for_available_graphic_devices(_In_ int& pNumberOfGraphicsDevices);
                //initialize graphics device
                W_REN_EM static W_RESULT initialize(_Out_ std::shared_ptr<w_graphic_device>& pGraphicsDevice,
                                                    _In_  w_present_info& pPresentInfo,
                                                    _In_  const w_graphic_device_info* pGraphicsDeviceInfo = nullptr);
                //release all resources
                W_REN_EM static ULONG release();
                
            private:
                static std::vector<w_graphic_device_info>                  _graphic_devices_info;
                static std::vector<std::shared_ptr<w_graphic_device>>      _graphic_devices_instances;
            };
        }
    }
}

#endif
