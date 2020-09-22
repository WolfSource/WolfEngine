#ifndef __W_SIMPLYGON_H__
#define __W_SIMPLYGON_H__

#ifdef __WIN32

#if _MSC_VER > 1000
#pragma once
#endif

#include "SimplygonSDKLoader.h"
#include <windows.h>
#include <process.h>
#include <w_logger.h>
#include "w_vertex_struct.h"
#include "w_cpipeline_export.h"

namespace simplygon
{
	//a pointer to the simplygon interface
	extern SimplygonSDK::ISimplygonSDK* iSimplygonSDK;
	extern bool is_initialized;

	W_RESULT initialize(_In_z_ const std::wstring& pSimplygonSDKPath);

	W_RESULT generate_lod(
		_Inout_ SimplygonSDK::CountedPointer<SimplygonSDK::IScene>& pOutScene,
		_In_ SimplygonSDK::CountedPointer<SimplygonSDK::IScene>& pInScene,
		_In_z_ const SimplygonSDK::TangentSpaceMethod& pTangentSpaceMethod = SimplygonSDK::TangentSpaceMethod::SG_TANGENTSPACEMETHOD_ORTHONORMAL);
	
	W_RESULT obj_writer(
		_In_z_ const std::wstring& pObjPath,
		_In_ SimplygonSDK::CountedPointer<SimplygonSDK::IScene>& pScene);
	
	W_RESULT obj_reader(
		_In_z_ const std::wstring& pObjPath,
		_Inout_ SimplygonSDK::CountedPointer<SimplygonSDK::IScene>& pScene);

	//W_RESULT create_obj_scene_from_data(
	//	_In_ std::vector<wolf::content_pipeline::w_vertex_struct>& pVerticesData,
	//	_In_ std::vector<uint32_t>& pIndicesData,
	//	_Inout_ SimplygonSDK::CountedPointer<SimplygonSDK::IScene>& pScene);

	//W_RESULT retrieve_data_from_obj_scene(
	//	_Inout_ std::vector<wolf::content_pipeline::w_vertex_struct>& pVerticesData,
	//	_Inout_ std::vector<uint32_t>& pIndicesData,
	//	_In_ SimplygonSDK::CountedPointer<SimplygonSDK::IScene>& pScene);


	WCP_EXP void release();
}

#endif //WIN32

#endif //__SIMPLYGONSDKLOADER_H__
