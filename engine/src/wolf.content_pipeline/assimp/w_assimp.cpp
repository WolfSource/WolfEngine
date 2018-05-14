#include "w_cpipeline_pch.h"
#include "w_assimp.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

using namespace assimp;
using namespace wolf::content_pipeline;

w_cpipeline_scene* w_assimp::load(_In_z_ const std::wstring& pAssetPath)
{
	Assimp::Importer _assimp_importer;

	// Flags for loading the mesh
	static const int _assimp_flags =
		aiProcess_ValidateDataStructure			// perform a full validation of the loader's output
		| aiProcess_Triangulate					// triangulate polygons with more than 3 edges
		| aiProcess_ImproveCacheLocality		// improve the cache locality of the output vertices
		| aiProcess_RemoveRedundantMaterials	// remove redundant materials
		| aiProcess_FindDegenerates				// remove degenerated polygons from the import
		| aiProcess_FindInvalidData				// detect invalid model data, such as invalid normal vectors
		| aiProcess_OptimizeMeshes				// join small meshes, if possible
		| aiProcess_GenSmoothNormals			// generate smooth normals if needed
		| aiProcess_CalcTangentSpace			// calculate tangant and bitangent when normal presented
		| aiProcess_FindInstances				// duplicate meshes 
		| aiProcess_SplitLargeMeshes			// split large, unrenderable meshes into submeshes
		| aiProcess_MakeLeftHanded				// make left hand coordinate system
		;
	auto _path = wolf::system::convert::wstring_to_string(pAssetPath);
	auto _scene = _assimp_importer.ReadFile(_path.c_str(), _assimp_flags);
	if (_scene)
	{
		if (_scene->HasCameras())
		{

		}
	}

	return nullptr;
}