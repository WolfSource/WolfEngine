#include "w_embree_pch.h"
#include "w_graphic_device_manager.h"
#include <mmintrin.h>//FTZ
#include <pmmintrin.h>//DAZ

using namespace wolf::system;
using namespace wolf::render::embree;

namespace wolf
{
    namespace render
    {
        namespace embree
        {
            class w_graphic_device_pimp
            {
            public:
                w_graphic_device_pimp() :
                _name("w_graphic_device_pimp"),
                _device(nullptr)
                {
                }
                
                W_RESULT initialize(_In_ w_present_info& pPresentInfo)
                {
                    //set platform
                    pPresentInfo.platform = "embree";
                    
                    //create embree device
                    this->_device = rtcNewDevice("start_threads=1,set_affinity=1");
                    
                    //setup callbacks
                    rtcSetDeviceErrorFunction(this->_device, _embree_device_error_callback, nullptr);
#ifdef DEBUG
                    rtcSetDeviceMemoryMonitorFunction(this->_device, _embree_memory_monitor_callback, nullptr);
#endif
                    
                    return W_PASSED;
                }
                
                ULONG release()
                {
                    if (this->_device)
                    {
                        rtcReleaseDevice(this->_device);
                        this->_device = nullptr;
                    }
                    return 1;
                }
                
#pragma region Getters
                
                RTCDevice get_device() const { return this->_device; }
                
#pragma endregion
                
            private:
                static void _embree_device_error_callback(_In_ void* pUserPtr, _In_ RTCError pCode, _In_z_ const char* pStr)
                {
                    logger.error("embree error code: {}, error message: {}", pCode, pStr);
                }
                
#ifdef DEBUG
                static bool _embree_memory_monitor_callback(_In_ void* pUserPtr, _In_ ssize_t pBytes, _In_ bool pPost)
                {
                    logger.error("embree {} bytes allocated", pBytes);
                    return true;
                }
#endif
                
                RTCDevice                                           _device;
                w_present_info                                      _present_info;
                bool                                                _is_released;
                std::string                                         _name;
            };
        }
    }
}

using namespace wolf::render::embree;

w_graphic_device::w_graphic_device() : _pimp(new w_graphic_device_pimp())
{
}

w_graphic_device::~w_graphic_device()
{
    release();
}

W_RESULT w_graphic_device::initialize(_In_ w_present_info& pPresentInfo)
{
    return this->_pimp ? this->_pimp->initialize(pPresentInfo) : W_FAILED;
}

ULONG w_graphic_device::release()
{
    //release all resources
    return this->_pimp ? this->_pimp->release() : 1;
}

#pragma region Getters

RTCDevice w_graphic_device::get_device() const
{
   return this->_pimp ? this->_pimp->get_device() : nullptr;
}

#pragma endregion

static std::once_flag s_FTZ_DAZ;
std::vector<w_graphic_device_info> w_graphic_device_manager::_graphic_devices_info;
std::vector<std::shared_ptr<w_graphic_device>> w_graphic_device_manager::_graphic_devices_instances;

static void init_once()
{
    std::call_once(s_FTZ_DAZ, []()
                   {
                       //for best performance set FTZ and DAZ flags in MXCSR control and status register
                       _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
                       _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
                       
#if defined(__WIN32) && !defined(__PYTHON__)
                       auto _hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
                       if (FAILED(_hr))
                       {
                           logger.error("error on CoInitializeEx");
                       }
#endif
                   });
}

w_graphic_device_info* w_graphic_device_manager::query_for_available_graphic_devices(_In_ int& pNumberOfGraphicsDevices)
{
    //embree returns only one graphics device
    pNumberOfGraphicsDevices = 1;
    _graphic_devices_info.resize(pNumberOfGraphicsDevices);
    _graphic_devices_info[0].index = 0;
    _graphic_devices_info[0].description = "embree";
    
    return _graphic_devices_info.data();
}

W_RESULT w_graphic_device_manager::initialize(_Out_ std::shared_ptr<w_graphic_device>& pGraphicDevice,
                                               _In_ w_present_info& pPresentInfo,
                                               _In_ const w_graphic_device_info* pGraphicDeviceInfo)
{
    const char* _trace_info = "w_graphics_device_manager::initialize";
    W_RESULT _hr = W_FAILED;
    
    //get the first default graphics device
    if (!pGraphicDeviceInfo)
    {
        auto _gDevice = std::make_shared<w_graphic_device>();
        if (!_gDevice)
        {
            logger.error("could not allocate memory for graphics device: #{}:default, trace info:{}", 0, _trace_info);
            return W_FAILED;
        }
        if(_gDevice->initialize(pPresentInfo) == W_PASSED)
        {
            _graphic_devices_instances.push_back(_gDevice);
            pGraphicDevice = _gDevice;
            _hr = W_PASSED;
        }
        else
        {
            logger.error("could not initialize graphics device: #{}:default, trace info:{}", 0, _trace_info);
            SHARED_RELEASE(_gDevice);
        }
    }
    else
    {
        //iterate over all graphics devices
        for (auto& _info : _graphic_devices_info)
        {
            if (_info.description == pGraphicDeviceInfo->description &&
                _info.index == pGraphicDeviceInfo->index)
            {
                auto _gDevice = std::make_shared<w_graphic_device>();
                if (!_gDevice)
                {
                    logger.error("could not allocate memory for graphics device: #{}:{}, trace info:{}", _info.index, _info.description, _trace_info);
                    continue;
                }
                if(_gDevice->initialize(pPresentInfo) == W_PASSED)
                {
                    _graphic_devices_instances.push_back(_gDevice);
                    pGraphicDevice = _gDevice;
                    _hr = W_PASSED;
                }
                else
                {
                    logger.error("could not initialize graphics device: #{}:{}, trace info:{}", _info.index, _info.description, _trace_info);
                    SHARED_RELEASE(_gDevice);
                }
            }
        }
    }
    
    return _hr;
}

ULONG w_graphic_device_manager::release()
{
#if defined(__WIN32) && !defined(__PYTHON__)
	CoUninitialize();
#endif
    
    _graphic_devices_info.clear();
    
    //release all graphics devices
    for (auto _gDevice : _graphic_devices_instances)
    {
        SHARED_RELEASE(_gDevice);
    }
    _graphic_devices_instances.clear();
    
    return 0;
}
