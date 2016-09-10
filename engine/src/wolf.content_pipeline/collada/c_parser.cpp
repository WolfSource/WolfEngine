#include "w_cpipeline_pch.h"
#include "c_parser.h"
#include "w_model.h"
#include "c_skin.h"

using namespace std;
using namespace wolf::system;
using namespace wolf::content_pipeline;
using namespace wolf::content_pipeline::collada;

//static variables which are necessary for parsing collada file 
static std::vector<c_bone*>			sBones;
static std::vector<c_node*>			sNodes;
static std::string					sSceneID;
static std::vector<std::string>		sSkeletonNames;
static std::vector<c_material*>		sMaterials;
static std::string					sSkipChildrenOfThisNode;
static c_xsi_extra					sXSI_Extra;
static std::vector<unsigned short>	sXSI_Indices;
static rapidxml::xml_node<>*		SGeometryLibraryNode;

const char* c_parser::_trace_class_name = "w_collada_parser";

HRESULT c_parser::parse_collada_from_file(const std::wstring& pFilePath, _Inout_ w_scene* pScene, bool pOptimizePoints, bool pInvertNormals)
{
	auto _hr = S_OK;

	//open stream of xml 
	std::ifstream _file(pFilePath);
	std::stringstream _string_stream;
	_string_stream << _file.rdbuf();
	_file.close();
	std::string content(_string_stream.str());

	sSkipChildrenOfThisNode = "NULL";

	using namespace rapidxml;
	xml_document<> _doc;
	try
	{
		_doc.parse<0>(&content[0]);
	}
	catch (...)
	{
		_hr = S_FALSE;
		V(_hr, L"Could not parse collada file on following path : " + pFilePath, _trace_class_name, 3);
	}

	_hr = _process_xml_node(_doc.first_node());
	V(_hr, L"processing xml node : " + pFilePath, _trace_class_name, 3);
	
	//create scene
	_create_scene(pScene, pOptimizePoints, pInvertNormals);

	//clear all
	_doc.clear();
	_clear_all_resources();

	return _hr;
}

HRESULT c_parser::_process_xml_node(_In_ rapidxml::xml_node<>* pXNode)
{
	//get the name of node
	auto _node_name = _get_node_name(pXNode);

#ifdef DEBUG
	logger.write(_node_name);
#endif

	auto _parent = pXNode->parent();
	auto _parent_node_name = _get_node_name(_parent);
	if (_parent != nullptr && _parent_node_name == sSkipChildrenOfThisNode) return S_OK;

	if (_node_name == "collada")
	{
#pragma region collada headers
		//check collada version 1.4.1
		auto _attr = pXNode->first_attribute("xmlns", 0Ui64, false);
		if (_attr && 0 != std::strcmp(_attr->value(), "http://www.collada.org/2005/11/COLLADASchema"))
		{
			logger.error("Collada file does not have standard COLLADA header");
			return S_FALSE;
		}

		_attr = pXNode->first_attribute("version", 0Ui64, false);
		if (_attr && 0 != std::strcmp(_attr->value(), "1.4.1"))
		{
			logger.error("Collada file does not have standard COLLADA header");
			return S_FALSE;
		}
#pragma endregion
	}
	else if (_node_name == "asset")
	{
		//ToDo: Asset
	}
	else if (_node_name == "library_cameras")
	{
		//we don't need basic information of camera, such as near plan, far plan and etc
		sSkipChildrenOfThisNode = _node_name;
		return S_OK;
	}
	else if (_node_name == "library_lights")
	{
		//ToDo: read lights
	}
	else if (_node_name == "library_effects")
	{
		//ToDo: read effects
	}
	else if (_node_name == "library_materials")
	{
		//ToDo: read material
	}
	else if (_node_name == "library_geometries")
	{
		SGeometryLibraryNode = pXNode;
	}
	else if (_node_name == "library_visual_scenes")
	{
#pragma region parse visual scenes
		//process all childs
		for (auto _child = pXNode->first_node(); _child != nullptr; _child = _child->next_sibling())
		{
			auto _node_name = _get_node_name(_child);
			if (_node_name == "visual_scene")
			{
				//read scene id
				_get_node_attribute_value(_child, "id", sSceneID);

				//read visual scene nodes
				_read_visual_scene_nodes(_child, sNodes);
			}
		}
		sSkipChildrenOfThisNode = _node_name;
#pragma endregion
	}
	else if (_node_name == "scene")
	{

	}
	else if (_node_name == "extra" && _parent_node_name == "collada")
	{
#pragma region parse extra
		//process all childs of extra
		for (auto _child = pXNode->first_node(); _child != nullptr; _child = _child->next_sibling())
		{
			auto _node_name = _get_node_name(_child);
#ifdef DEBUG
			logger.write(_node_name);
#endif

			if (_node_name == "technique")
			{
				for (auto __child = _child->first_node(); __child != nullptr; __child = __child->next_sibling())
				{
					auto __node_name = _get_node_name(__child);
#ifdef DEBUG
					logger.write(__node_name);
#endif

					if (__node_name == "si_scene")
					{
						std::vector<c_value_obj*> _si_scene;
						_get_si_scene_data(__child, _si_scene);

						for (auto _si : _si_scene)
						{
							if (_si == nullptr) continue;

							if (_si->c_sid == "timing")
							{
								sXSI_Extra.timing = _si->value;
							}
							else if (_si->c_sid == "timing")
							{
								sXSI_Extra.start = std::atoi(_si->value.c_str());
							}
							else if (_si->c_sid == "timing")
							{
								sXSI_Extra.end = std::atoi(_si->value.c_str());
							}
							else if (_si->c_sid == "timing")
							{
								sXSI_Extra.frame_rate = std::atoi(_si->value.c_str());
							}
						}
					}
					else if (__node_name == "xsi_trianglelist")
					{
						for (auto ___child = __child->first_node(); ___child != nullptr; ___child = ___child->next_sibling())
						{
							string _sid;
							_get_node_attribute_value(___child, "sid", _sid);
							if (_sid == "vertexIndices")
							{
								std::string _value = ___child->value();
								wolf::system::convert::split_string_then_convert_to<USHORT>(_value, " ", sXSI_Indices);
							}
						}
					}
				}
			}
		}
#pragma endregion
	}

	//process all childs
	for (auto _child = pXNode->first_node(); _child != nullptr; _child = _child->next_sibling())
	{
		_process_xml_node(_child);
	}

	return S_OK;
}

void c_parser::_read_visual_scene_nodes(_In_ rapidxml::xml_node<>* pXNode, _Inout_ std::vector<c_node*>& pNodes)
{

#ifdef DEBUG
	std::string _id;
	_get_node_attribute_value(pXNode, "id", _id);
	logger.write(_id);
#endif

	//iterate over children of this node
	for (auto _child = pXNode->first_node(); _child != nullptr; _child = _child->next_sibling())
	{
		//get the name of node
		auto _child_name = _get_node_name(_child);

#ifdef DEBUG
		std::string __id;
		_get_node_attribute_value(_child, "id", __id);
		logger.write(__id);
#endif

		if (_child_name == "node")
		{
			//iterate over children of this node
			for (auto __child = _child->first_node(); __child != nullptr; __child = __child->next_sibling())
			{
				auto __child_name = _get_node_name(__child);
#ifdef DEBUG
				std::string ___id;
				_get_node_attribute_value(__child, "id", ___id);
				logger.write(___id);
#endif

				if (__child_name == "instance_controller")
				{
					//iterate over children of this node
					for (auto ___child = __child->first_node(); ___child != nullptr; ___child = ___child->next_sibling())
					{
						auto ___child_name = _get_node_name(___child);
						if (___child_name == "skeleton")
						{
							std::string _value = ___child->value();
							sSkeletonNames.push_back(_value);
						}
					}
				}
				else if (__child_name == "node")
				{
					rapidxml::xml_node<>* _joint_node = nullptr;
					_find_node(__child, "type", "JOINT", _joint_node);
					if (_joint_node != nullptr)
					{
						c_bone _temp_bone;
						std::memset(&_temp_bone, 0, sizeof(_temp_bone));

						auto _base_class = (c_obj*) (&_temp_bone);
						_get_collada_obj_attribute(_joint_node, _base_class);
						_get_bones(_joint_node, &_temp_bone, _temp_bone.flat_bones);

						sBones.push_back(&_temp_bone);
					}
					_get_nodes(__child, pNodes);

#ifdef DEBUG
					logger.write(std::to_string(pNodes.size()));
#endif
				}
			}
		}
	}
}

void c_parser::_get_si_scene_data(_In_ rapidxml::xml_node<>* pXNode, _Inout_ std::vector<c_value_obj*>& pSIs)
{
	for (auto _child = pXNode->first_node(); _child != nullptr; _child = _child->next_sibling())
	{
		auto _node_name = _get_node_name(_child);

#ifdef DEBUG
		logger.write(_node_name);
#endif

		if (_node_name == "xsi_param")
		{
			string _sid;
			_get_node_attribute_value(_child, "sid", _sid);

			auto _value_obj = new c_value_obj();
			_value_obj->c_sid = _sid;
			_value_obj->value = _child->value();

			pSIs.push_back(_value_obj);
		}
	}
}

void c_parser::_find_node(_In_ rapidxml::xml_node<>* pXNode, const string& pAttributeName, const string& pAttributeValue, _Inout_ rapidxml::xml_node<>* pFoundNode)
{
	pFoundNode = nullptr;

	std::string _attr_value = "";
	auto found = _get_node_attribute_value(pXNode, pAttributeName, _attr_value);
	if (found && _attr_value == pAttributeValue)
	{
		pFoundNode = pXNode;
		return;
	}

	//iterate over children of this node
	for (auto _child = pXNode->first_node(); _child != nullptr; _child = _child->next_sibling())
	{
		_find_node(_child, pAttributeName, pAttributeValue, pFoundNode);
		if (pFoundNode != nullptr)
		{
			break;
		}
	}
}

std::string c_parser::_get_node_name(_In_ rapidxml::xml_node<>* pXNode)
{
	//get the name of node
	std::string _name(pXNode->name());
	//to lower it
	std::transform(_name.begin(), _name.end(), _name.begin(), ::tolower);

	return _name;
}

bool c_parser::_get_node_attribute_value(_In_ rapidxml::xml_node<>* pXNode, _In_ const std::string& pAttributeName, _Inout_ std::string& pAttributeValue )
{
	auto _attr = pXNode->first_attribute(pAttributeName.c_str());
	if (_attr)
	{
		pAttributeValue = _attr->value();
		return true;
	}
	return false;
}

void c_parser::_get_collada_obj_attribute(_In_ rapidxml::xml_node<>* pXNode, _Inout_ c_obj* pCObj)
{
	if (pCObj == nullptr) return;

	std::string _id = "NULL", _name = "NULL", _sid = "NULL";

	_get_node_attribute_value(pXNode, "id", _id);
	_get_node_attribute_value(pXNode, "name", _name);
	_get_node_attribute_value(pXNode, "sid", _sid);

	pCObj->c_id = _id;
	pCObj->c_name = _name;
	pCObj->c_sid = _sid;
}

void c_parser::_get_bones(_In_ rapidxml::xml_node<>* pXNode, _Inout_ c_bone* pBone, _Inout_ std::vector<c_bone*>& pFlatBones)
{
	if (pBone == nullptr) throw std::runtime_error("pbone must not null");

	pBone->index = pFlatBones.size();
	pFlatBones.push_back(pBone);

	auto _no_matrix = false;
	auto _transform = glm::vec3(0);
	float _rotate[4];
	auto _rotate_index = 0;
	glm::vec3 _scale;

	for (auto _child = pXNode->first_node(); _child != nullptr; _child = _child->next_sibling())
	{
		auto _bone = new c_bone();
		_bone->parent = pBone;

		auto _name = _get_node_name(_child);
		if (_name == "matrix")
		{
#ifdef DEBUG
			logger.write(_child->value());
#endif
			//pBone->initial_matrix = e.Value.Split(' ').ToMatrix().Transpose();

			pBone->position = glm::vec4_from_mat4x4_row(pBone->initial_matrix, 3);
		}
		else if (_name == "translate")
		{
			_no_matrix = true;
			_transform = glm::to_vec3(_child->value());
		}
		else if (_name == "scale")
		{
			auto _parent_rotation = pBone->parent != nullptr ? pBone->parent->rotation : glm::vec3(0);
			pBone->scale = _scale = glm::to_vec3(_child->value());
			pBone->rotation_matrix = glm::rotate(_rotate[2], glm::vec3(1, 0, 0)) *
				glm::rotate(_rotate[1], glm::vec3(0, 1, 0)) *
				glm::rotate(_rotate[0], glm::vec3(0, 0, 1));

			pBone->initial_matrix = pBone->world_transform = (glm::scale(_scale) *
				pBone->rotation_matrix *
				glm::translate(_transform) *
				(pBone->parent != nullptr ? pBone->parent->initial_matrix : glm::mat4x4())); // if glm::mat4x4() is identity

			pBone->initial_matrix = pBone->world_transform = (glm::scale(_scale) *
				pBone->rotation_matrix *
				glm::translate(_transform));

			pBone->position = glm::vec4_from_mat4x4_row(pBone->initial_matrix, 3);
			pBone->rotation = glm::vec3(glm::degrees(_rotate[2]), glm::degrees(_rotate[1]), glm::degrees(_rotate[0]));
		}
		else if (_name == "rotate")
		{
			if (_rotate_index < W_ARRAY_SIZE(_rotate))
			{
				auto _angle = glm::to_vec3(_child->value());
				_rotate[_rotate_index] = glm::radians(_angle[3]);
				
				_rotate_index++;
			}
		}
		else if (_name == "node")
		{
			std::string _id = "NULL", _name = "NULL", _sid = "NULL";

			_get_node_attribute_value(_child, "id", _id);
			_get_node_attribute_value(_child, "name", _name);
			_get_node_attribute_value(_child, "sid", _sid);

			_bone->c_id = _id;
			_bone->c_name = _name;
			_bone->c_sid = _id;

			if (_no_matrix)
			{
				_rotate_index = 0;
			}

			_get_bones(_child, _bone, pFlatBones);
			pBone->children.push_back(_bone);
		}
	}
}

void c_parser::_get_nodes(_In_ rapidxml::xml_node<>* pXNode, _Inout_ std::vector<c_node*>& pNodes)
{
	auto _node = new c_node();
	std::memset(_node, 0, sizeof(_node));

	_get_collada_obj_attribute(pXNode, _node);

#ifdef DEBUG
	auto _name = _get_node_name(pXNode);
	std::string _id;
	_get_node_attribute_value(pXNode, "id", _id);

	logger.write(_name + " with id: " + _id);
#endif

	for (auto _child = pXNode->first_node(); _child != nullptr; _child = _child->next_sibling())
	{
		auto _node_name = _get_node_name(_child);
#ifdef DEBUG
		logger.write(_node_name);
#endif
		if (_node_name == "translate")
		{
			_node->translate = glm::to_vec3(_child->value());
		}
		else if (_node_name == "rotate")
		{
			std::string _rotation_type;
			_get_node_attribute_value(_child, "sid", _rotation_type);
			auto _vec4 = glm::to_vec4(_child->value());

			if (_rotation_type == "rotation_z")
			{
				_node->rotation.z = _vec4[3];
			}
			else if (_rotation_type == "rotation_y")
			{
				_node->rotation.y = _vec4[3];
			}
			else if (_rotation_type == "rotation_x")
			{
				_node->rotation.x = _vec4[3];
			}
		}
		else if (_node_name == "scale")
		{
			_node->scale = glm::to_vec3(_child->value());
		}
		else if (_node_name == "node")
		{
			_get_nodes(_child, _node->nodes);
		}
		else if (_node_name == "instance_geometry")
		{
			_get_node_attribute_value(_child, "url", _node->instanced_geometry_name);
			if (_node->instanced_geometry_name[0] == '#')
			{
				_node->instanced_geometry_name = _node->instanced_geometry_name.erase(0, 1);
			}

		}
		else if (_node_name == "instance_node")
		{
			_get_node_attribute_value(_child, "url", _node->instanced_node_name);
			if (_node->instanced_geometry_name[0] == '#')
			{
				_node->instanced_geometry_name = _node->instanced_geometry_name.erase(0, 1);
			}
		}
		else if (_node_name == "instance_camera")
		{
			_get_node_attribute_value(_child, "url", _node->instanced_camera_name);
			if (_node->instanced_camera_name[0] == '#')
			{
				_node->instanced_camera_name = _node->instanced_camera_name.erase(0, 1);
			}
		}
		else if (_node_name == "instance_light")
		{
			//ToDo
		}
	}

	_node->transform = glm::make_wpv_mat(_node->scale, _node->rotation, _node->translate);
	pNodes.push_back(_node);

#ifdef DEBUG
	logger.write(std::to_string(pNodes.size()));
#endif // DEBUG
}

void c_parser::_get_sources(_In_ rapidxml::xml_node<>* pXNode, std::string pID, std::string pName, _Inout_ c_geometry& pGeometry)
{
	string _float_array_str = "";

	for (auto _child = pXNode->first_node(); _child != nullptr; _child = _child->next_sibling())
	{
		auto _node_name =  _get_node_name(_child);

#ifdef DEBUG
		logger.write(_node_name);
#endif

		if (_node_name == "float_array")
		{
			_float_array_str = _child->value();
		}
		else if (_node_name == "technique_common")
		{
			auto _stride = 1;
			for (auto __child = _child->first_node(); __child != nullptr; __child = __child->next_sibling())
			{
				std::string _stride_str;
				_get_node_attribute_value(__child, "stride", _stride_str);
				_stride = std::atoi(_stride_str.c_str());
			}

			auto _source = new c_source();
			_source->c_id = pID;
			_source->c_name = pName;
			_source->stride = _stride;

			wolf::system::convert::split_string_then_convert_to<float>(_float_array_str, " ", _source->float_array);
			pGeometry.sources.push_back(_source);
		}
	}
}

void c_parser::_get_vertices(_In_ rapidxml::xml_node<>* pXNode, _Inout_ c_geometry& pGeometry)
{
	for (auto _child = pXNode->first_node(); _child != nullptr; _child = _child->next_sibling())
	{
		std::string _semantic, _source;
		_get_node_attribute_value(_child, "semantic", _semantic);
		_get_node_attribute_value(_child, "source", _source);

		if (_semantic != "" && _source != "")
		{
			if (pGeometry.vertices != nullptr)
			{
				auto _c_semantic = new c_semantic();
				if (_semantic[0] == '#') _semantic = _semantic.erase(0, 1);
				_c_semantic->semantic = _semantic;

				if (_source[0] == '#') _source = _source.erase(0, 1);
				_c_semantic->source = _source;

				pGeometry.vertices->semantics.push_back(_c_semantic);
			}
		}
	}
}

void c_parser::_get_triangles(_In_ rapidxml::xml_node<>* pXNode, _Inout_ c_geometry& pGeometry)
{
	std::string _material_name;
	_get_node_attribute_value(pXNode, "material", _material_name);
	auto _triangles = new c_triangles();
	_triangles->material_name = _material_name;

	for (auto _child = pXNode->first_node(); _child != nullptr; _child = _child->next_sibling())
	{
		auto _node_name = _get_node_name(_child);

#ifdef DEBUG
		logger.write(_node_name);
#endif

		if (_node_name == "input")
		{
			std::string _source, _offset, _semantic;

			_get_node_attribute_value(_child, "source", _source);
			_get_node_attribute_value(_child, "offset", _offset);
			_get_node_attribute_value(_child, "semantic", _semantic);

			if (_source != "" && _offset != "" && _semantic != "")
			{
				if (_source[0] == '#') _source = _source.erase(0, 1);
				
				if (pGeometry.vertices->id != _source)
				{
					auto _c_semantic = new c_semantic();
					if (_semantic[0] == '#') _semantic = _semantic.erase(0, 1);
					_c_semantic->offset = _triangles->semantics.size();
					_c_semantic->source = _source;
					_c_semantic->semantic = _semantic;

					_triangles->semantics.push_back(_c_semantic);
				}
				else
				{
					_triangles->semantics = pGeometry.vertices->semantics;
				}
			}
		}
		else if (_node_name == "p")
		{
			wolf::system::convert::split_string_then_convert_to<unsigned short>(_child->value(), " ", _triangles->indices);
		}
	}

	pGeometry.triangles.push_back(_triangles);
}

HRESULT c_parser::_create_scene(_Inout_ w_scene* pScene, bool pOptimizePoints, bool pInvertNormals)
{
	//The list of instanced geometries
	std::vector<string> _instanced_geometries;
	std::vector<c_geometry> _base_geometeries;

	//Finding the base geometries
	for (auto _node : sNodes)
	{
		auto _inst_geo = _node->find_instanced_geomaetry_node();
		if (_inst_geo == nullptr) continue;
		_instanced_geometries.push_back(_inst_geo->instanced_geometry_name);
	}

	//Loading geometries
	for (auto _child = SGeometryLibraryNode->first_node(); _child != nullptr; _child = _child->next_sibling())
	{
		string _id;
		_get_node_attribute_value(_child, "id", _id);
		if (_instanced_geometries.size() == 0 ||
			std::find(_instanced_geometries.begin(), _instanced_geometries.end(), _id) != _instanced_geometries.end())
		{
			c_geometry _g;

			_get_node_attribute_value(_child, "id", _g.id);
			_get_node_attribute_value(_child, "name", _g.name);

			for (auto __child = _child->first_node(); __child != nullptr; __child = __child->next_sibling())
			{
				auto _node_name = _get_node_name(__child);
#ifdef DEBUG
				logger.write(_node_name);
#endif

				if (_node_name == "mesh")
				{
#pragma region read mesh data

					for (auto ___child = __child->first_node(); ___child != nullptr; ___child = ___child->next_sibling())
					{
						std::string _name = ___child->name();

						std::string __id, __name;
						_get_node_attribute_value(___child, "id", __id);
						_get_node_attribute_value(___child, "name", __name);

#ifdef DEBUG
						logger.write(_name);
#endif

						if (_name == "source")
						{
							_get_sources(___child, __id, __name, _g);
						}
						else if (_name == "vertices")
						{
							_g.vertices = new c_vertices();
							_get_node_attribute_value(___child, "id", _g.vertices->id);

							_get_vertices(___child, _g);
						}
						else if (_name == "triangles")
						{
							_get_triangles(___child, _g);
						}
					}
#pragma endregion
				}
			}

			_base_geometeries.push_back(_g);
		}
	}

	//creating animation container
	auto _animation_container = new c_animation_container();
	_animation_container->xsi_extra = sXSI_Extra;

	//Check bones
	if (sBones.size() > 0)
	{

	}

	//add camera if avaiable
	std::for_each(sNodes.begin(), sNodes.end(), [pScene](c_node* pNode)
	{
		//we will find #instance_camera and the parent node contains camera information and the next child node is camera interest
		glm::vec3 _camera_interest;

		if (pNode)
		{
			bool _found = false;
			if (!pNode->instanced_camera_name.empty())
			{
				pScene->add_camera(pNode->c_name, pNode->translate, _camera_interest);
				pNode->proceeded = true;
				_found = true;
			}
			else
			{
				//Serach childs
				auto _size = pNode->nodes.size();
				for (size_t i = 0; i < _size; ++i)
				{
					auto pInnerNode = pNode->nodes[i];
					if (pInnerNode && !pInnerNode->instanced_camera_name.empty())
					{
						auto _next_node = i + 1;
						if (_next_node < _size)
						{
							_camera_interest = pNode->nodes[_next_node]->translate;
						}
						_found = true;
						break;
					}
				}
			}

			if (_found)
			{
				pNode->proceeded = true;
				pScene->add_camera(pNode->c_name, pNode->translate, pNode->translate + _camera_interest);
			}
		}
	});

	//add models
	for (auto _geometry : _base_geometeries)
	{
		c_skin* skin = nullptr;
		//if (s.Count != 0)
		//{
		//	skin = s[i];
		//}

		auto _model = w_model::create_model(_geometry, skin, sBones, sSkeletonNames.data(), sMaterials, sNodes, pOptimizePoints, sXSI_Indices);
		//_model->set_effects(effects);
		////_model.Textures = textureInfos;
		////_model.Initialize(dir);
		////_model.SceneName = sceneId;
		////_model.Name = geometry.Name;
		////_model.AnimationContainers.Add("Animation 1", animContainer);
		////_model.SetAnimation("Animation 1");
		
		pScene->add_model(_model);
	}

	return S_OK;
}

void c_parser::_clear_all_resources()
{
	sSceneID = "";
	sSkipChildrenOfThisNode = "NULL";
	if (sSkeletonNames.size() > 0)
	{
		sSkeletonNames.clear();
	}
	if (sXSI_Indices.size() > 0)
	{
		sXSI_Indices.clear();
	}

	if (sBones.size() > 0)
	{
		std::for_each(sBones.begin(), sBones.end(), [](c_bone* pBone)
		{
			pBone->release();
		});
	}
	if (sNodes.size() > 0)
	{
		std::for_each(sNodes.begin(), sNodes.end(), [](c_node* pNode)
		{
			pNode->release();
		});
	}
}