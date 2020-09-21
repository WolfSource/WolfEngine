/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_assimp.h
	Description		 : The class converts assimp assets to wolf::content_pipeline::w_cpipeline_scene
	Comment          :
*/

#ifndef __W_ASSIMP_H__
#define __W_ASSIMP_H__

#include "w_cpipeline_export.h"
#include <w_cpipeline_scene.h>

namespace assimp
{
	struct w_assimp
	{
	public:
		WCP_EXP static wolf::content_pipeline::w_cpipeline_scene* load(_In_z_ const std::wstring& pAssetPath,
                                                                       _In_ const bool& pOptimizeMeshUsingAMDTootle
#ifdef __WIN32
                                                                       ,_In_ const bool& pGenerateLODUsingSimplygon
#endif
                                                                       );
	};
}

#endif
