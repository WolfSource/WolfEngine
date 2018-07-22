/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfSource/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_masked_occlusion_culling.h
	Description		 : The w_masked_occlusion_culling
	Comment          : //avx512 must build with "-march=skylake-avx512" flag and avx2 must build with "-mavx2 -mfma" flag
*/
#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_MASKED_OCCLUSION_CULLING_H__
#define __W_MASKED_OCCLUSION_CULLING_H__

#include "w_render_export.h"
#include <w_object.h>
#include <w_framework/masked_occlusion_culling/MaskedOcclusionCulling.h>

namespace wolf
{
	namespace framework
	{
		class w_masked_occlusion_culling_pimp;
		class w_masked_occlusion_culling : public system::w_object
		{
		public:
			W_VK_EXP w_masked_occlusion_culling();
			W_VK_EXP ~w_masked_occlusion_culling();

			W_VK_EXP W_RESULT initialize(_In_ const uint32_t& pNumberOfWorkerThreads, _In_ const bool& pEnableDebugging);
			W_VK_EXP W_RESULT suspend_threads();
			W_VK_EXP W_RESULT wake_threads();
			W_VK_EXP W_RESULT clear_buffer();
			
			W_VK_EXP W_RESULT render_triangles_async(
				_In_ const float* pVertices,
				_In_ const unsigned int* pTriangles,
				_In_ const int& pNumberOfTriangles,
				_In_ const MaskedOcclusionCulling::BackfaceWinding& pBackFaceWinding = MaskedOcclusionCulling::BackfaceWinding::BACKFACE_CCW,
				_In_ const MaskedOcclusionCulling::ClipPlanes& pClipPlaneMask = MaskedOcclusionCulling::CLIP_PLANE_ALL);

			W_VK_EXP MaskedOcclusionCulling::CullingResult test_triangles_async(
				_In_ const float* pVertices,
				_In_ const unsigned int* pTriangles,
				_In_ const int& pNumberOfTriangles,
				_In_ const MaskedOcclusionCulling::BackfaceWinding& pBackFaceWinding = MaskedOcclusionCulling::BackfaceWinding::BACKFACE_CCW,
				_In_ const MaskedOcclusionCulling::ClipPlanes& pClipPlaneMask = MaskedOcclusionCulling::CLIP_PLANE_ALL);

			W_VK_EXP W_RESULT render_triangles(
				_In_ const float* pVertices,
				_In_ const unsigned int* pTriangles,
				_In_ const int& pNumberOfTriangles,
				_In_ const float* pModelToClipMatrix,
				_In_ const MaskedOcclusionCulling::BackfaceWinding& pBackFaceWinding = MaskedOcclusionCulling::BackfaceWinding::BACKFACE_CCW,
				_In_ const MaskedOcclusionCulling::ClipPlanes& pClipPlaneMask = MaskedOcclusionCulling::CLIP_PLANE_ALL);
			
			W_VK_EXP MaskedOcclusionCulling::CullingResult test_triangles(
				_In_ const float* pVertices,
				_In_ const unsigned int* pTriangles,
				_In_ const int& pNumberOfTriangles,
				_In_ const float* pModelToClipMatrix,
				_In_ const MaskedOcclusionCulling::BackfaceWinding& pBackFaceWinding = MaskedOcclusionCulling::BackfaceWinding::BACKFACE_CCW,
				_In_ const MaskedOcclusionCulling::ClipPlanes& pClipPlaneMask = MaskedOcclusionCulling::CLIP_PLANE_ALL);

			W_VK_EXP W_RESULT flush();

			W_VK_EXP ULONG release() override;

#pragma region Getters

			W_VK_EXP uint8_t* get_debug_frame(_In_ bool pFlipY);

#pragma endregion

#pragma region Setters

			W_VK_EXP void		set_number_of_worker_threads(_In_ uint32_t& pNumberOfWorkerThreads);
			W_VK_EXP void		set_multi_threaded(_In_ const bool& pValue);
			W_VK_EXP W_RESULT	set_near_clip(_In_ const float& pValue);
			W_VK_EXP W_RESULT	set_resolution(_In_ const float& pWidth, _In_ const float& pHeight);
			W_VK_EXP void		set_matrix(_In_ const float* pModelToClipMatrix);

#pragma endregion

		private:
			typedef system::w_object						_super;
			w_masked_occlusion_culling_pimp*                _pimp;

		};
	}
}

#endif
