/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_wavefront_obj.h
	Description		 : wavefront obj reader and writer for vertices and indices
	Comment          :
*/

#ifndef __W_WAVEFRONT_OBJ_H__
#define __W_WAVEFRONT_OBJ_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "w_cpipeline_export.h"
#include "w_std.h"
#include "w_vertex_struct.h"

namespace wolf
{
	namespace content_pipeline
	{
		namespace wavefront
		{
			struct obj
			{
				static void write(
					_In_ std::vector<w_vertex_struct>& pVerticesData,
					_In_ std::vector<uint32_t>& pIndicesData,
					_In_z_ const std::string& pOutputFilePath);

				static W_RESULT read(
					_Inout_ std::vector<w_vertex_struct>& pVerticesData,
					_Inout_ std::vector<uint32_t>& pIndicesData,
					//_Inout_ std::vector<float>& pJustVertexPosition,
					_In_z_ const std::string& pInputFilePath);
			};
		}
	}
}

#endif