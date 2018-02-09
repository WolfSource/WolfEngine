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

#include <w_xml.h>
#include "w_cpipeline_scene.h"
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
                c_parser() : sZ_Up(true) {}
				virtual ~c_parser() {};

				WCP_EXP W_RESULT parse_collada_from_file(
                    _In_z_ const std::wstring& pFilePath, 
                    _Inout_ wolf::content_pipeline::w_cpipeline_scene* pScene,
					_In_ const bool& pAMDTootleOptimizing = true,
                    _In_ const bool& pInvertNormals = false,
                    _In_ const bool& pFind_LODs_BBs = true);

			private:
				W_RESULT	                                _process_xml_node(_In_ rapidxml::xml_node<>* pXNode);
                void                                        _get_library_cameras(_In_ rapidxml::xml_node<>* pXNode);
                void                                        _get_library_effects(_In_ rapidxml::xml_node<>* pXNode);
                void                                        _get_library_materials(_In_ rapidxml::xml_node<>* pXNode);
                void                                        _get_library_images(_In_ rapidxml::xml_node<>* pXNode);
				void			                            _read_visual_scene_nodes(_In_ rapidxml::xml_node<>* pXNode, _Inout_ std::vector<c_node*>& pNodes);
                std::tuple<std::string,std::string>         _get_instance_material_symbol_target_name(_In_ rapidxml::xml_node<>* pXNode);
                void			                            _get_si_scene_data(_In_ rapidxml::xml_node<>* pXNode, _Inout_ std::vector<c_value_obj*>& pNodes);
				void			                            _find_node(_In_ rapidxml::xml_node<>* pXNode, const std::string& pAttributeName, const std::string& pAttributeValue, _Inout_ rapidxml::xml_node<>* pFoundNode);
				std::string	                                _get_node_name(_In_ rapidxml::xml_node<>* pXNode);
				bool			                            _get_node_attribute_value(_In_ rapidxml::xml_node<>* pXNode, _In_ const std::string& pAttributeName, _Inout_ std::string& pAttributeValue);
				void			                            _get_collada_obj_attribute(_In_ rapidxml::xml_node<>* pXNode, _Inout_ c_obj* pCObj);
				void			                            _get_bones(_In_ rapidxml::xml_node<>* pXNode, _Inout_ c_bone* pBone, _Inout_ std::vector<c_bone*>& pFlatBones);
				void                                        _get_node_data(_In_ rapidxml::xml_node<>* pXNode, _Inout_ c_node** pNode);
				void			                            _get_sources(_In_ rapidxml::xml_node<>* pXNode, std::string pID, std::string pName, _Inout_ c_geometry& pGeometry);
				void			                            _get_vertices(_In_ rapidxml::xml_node<>*, _Inout_ c_geometry& pGeometry);
				void			                            _get_triangles(_In_ rapidxml::xml_node<>* pXNode, _In_ c_node* pNode, _Inout_ c_geometry& pGeometry);
                void                                        _iterate_over_nodes(
                                                                _In_ const bool& pAMDTootleOptimizing,
                                                                _In_ const bool& pInvertNormals,
                                                                _Inout_ std::vector<c_node*> pNodes,
                                                                _Inout_ std::vector<w_cpipeline_model*>& pModels,
                                                                _Inout_ std::vector<c_node*>& pNodeWithUnknownInstanceRef);
                
                void                                        _create_model(
                    _In_ const bool& pAMDTootleOptimizing,
                    _In_ const bool& pInvertNormals,
                    _Inout_ c_node** pNode,
                    _Inout_ w_cpipeline_model** pModel);
                
				W_RESULT	                                _create_scene(
                    _Inout_ w_cpipeline_scene* pScene,
                    _In_ const bool& pAMDTootleOptimizing,
                    _In_ const bool& pInvertNormals,
                    _In_ const bool& pFind_LODs_ConvexHulls_Boundaries);
				void			                            _clear_all_resources();

				static const char*	                        _trace_class_name;


                //static variables which are necessary for parsing collada file 
                std::vector<c_bone*>			            sBones;
                std::vector<c_node*>			            sNodes;
                std::string					                sSceneID;
                std::vector<std::string>		            sSkeletonNames;
                std::map<std::string, c_camera>             sLibraryCameras;
                std::map<std::string, std::string>          sLibraryMaterials;
                std::map<std::string, std::string>	        sLibraryEffects;
                std::map<std::string, std::string>	        sLibraryImages;
                std::string					                sSkipChildrenOfThisNode;
                c_xsi_extra					                sXSI_Extra;
                rapidxml::xml_node<>*		                SGeometryLibraryNode;
                bool                                        sZ_Up;
			};
		}
	}
}

#endif
