/*
    Project:    Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/Wolf.Engine
*/

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef WIN32
#define WOLF_EXPORT_API __declspec(dllexport)
#else
#define WOLF_EXPORT_API
#endif

    /**
     * initialize wolf cc
     * @return 0 on success
     */
    WOLF_EXPORT_API
    int wolf_init(void);

    /**
     * fill version of wolf cc
     */
    WOLF_EXPORT_API
    void wolf_version(char *p_version);

    /**
     * release all resources of wolf cc
     */
    WOLF_EXPORT_API
    int wolf_fini(void);

#ifdef __cplusplus
}
#endif
