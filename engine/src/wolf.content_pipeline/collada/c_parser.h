/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : c_parser.h
	Description		 : This class responsible to bind python and c++ functions callbacks 
	Comment          :
*/

#ifndef __C_PARSER_H__
#define __C_PARSER_H__

#include <rapidxml.hpp>
#include "w_scene.h"
#include "w_cpipeline_export.h"
#include "c_node.h"
#include "c_bone.h"
#include "c_extra.h"
#include "c_animation.h"

namespace wolf
{
	namespace content_pipeline
	{
		namespace collada
		{
			class c_parser
			{
			public:
				c_parser() {}
				virtual ~c_parser() {};

				WCP_EXP static HRESULT parse_collada_from_file(const std::wstring& pFilePath, _Inout_ wolf::content_pipeline::w_scene* pScene,
					bool pOptimizePoints = true, bool pInvertNormals = false);

			private:
				static HRESULT		_process_xml_node(_In_ rapidxml::xml_node<>* pXNode);
				static void			_read_visual_scene_nodes(_In_ rapidxml::xml_node<>* pXNode, _Inout_ std::vector<c_node*>& pNodes);
				static void			_get_si_scene_data(_In_ rapidxml::xml_node<>* pXNode, _Inout_ std::vector<c_value_obj*>& pNodes);
				static void			_find_node(_In_ rapidxml::xml_node<>* pXNode, const std::string& pAttributeName, const std::string& pAttributeValue, _Inout_ rapidxml::xml_node<>* pFoundNode);
				static std::string	_get_node_name(_In_ rapidxml::xml_node<>* pXNode);
				static bool			_get_node_attribute_value(_In_ rapidxml::xml_node<>* pXNode, _In_ const std::string& pAttributeName, _Inout_ std::string& pAttributeValue);
				static void			_get_collada_obj_attribute(_In_ rapidxml::xml_node<>* pXNode, _Inout_ c_obj* pCObj);
				static void			_get_bones(_In_ rapidxml::xml_node<>* pXNode, _Inout_ c_bone* pBone, _Inout_ std::vector<c_bone*>& pFlatBones);
				static void			_get_node_data(_In_ rapidxml::xml_node<>* pXNode, _Inout_ c_node** pNode);
				static void			_get_sources(_In_ rapidxml::xml_node<>* pXNode, std::string pID, std::string pName, _Inout_ c_geometry& pGeometry);
				static void			_get_vertices(_In_ rapidxml::xml_node<>*, _Inout_ c_geometry& pGeometry);
				static void			_get_triangles(_In_ rapidxml::xml_node<>* pXNode, _Inout_ c_geometry& pGeometry);
				static HRESULT		_create_scene(_Inout_ w_scene* pScene, bool pOptimizePoints, bool pInvertNormals);
				static void			_clear_all_resources();

				static const char*	_trace_class_name;
			};
		}
	}
}

#endif
