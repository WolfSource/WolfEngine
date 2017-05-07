#include "w_cpipeline_pch.h"
#include "c_node.h"
#include <algorithm>

using namespace wolf::content_pipeline::collada;

c_node* c_node::find_instanced_geomaetry_node()
{
	if (!this->instanced_geometry_name.empty()) return this;

    auto _iter =  std::find_if(this->child_nodes.begin(), this->child_nodes.end(), [](_In_ c_node* pNode)
                                {
                                    return pNode && !pNode->instanced_geometry_name.empty();
                                });

    if (_iter != this->child_nodes.end()) return (*_iter);
    return nullptr;
}

c_node* c_node::find_instanced_geomaetry_node(std::string pGeometryName)
{
	if (this->instanced_geometry_name == pGeometryName) return this;
    
    auto _iter =  std::find_if(this->child_nodes.begin(), this->child_nodes.end(), [&pGeometryName](_In_ c_node* pNode)
                                {
                                    return pNode && pNode->instanced_geometry_name == pGeometryName;
                                });
    
    if (_iter != this->child_nodes.end()) return (*_iter);
    return nullptr;
}

//c_node* c_node::find_instanced_node_name()
//{
//	if (!this->instanced_node_name.empty()) return this;
//    
//    auto _iter =  std::find_if(this->child_nodes.begin(), this->child_nodes.end(), [](_In_ c_node* pNode)
//                               {
//                                   return pNode && !pNode->instanced_node_name.empty();
//                               });
//    
//    if (_iter != this->child_nodes.end()) return (*_iter);
//    return nullptr;
//}

ULONG c_node::release()
{
	if (is_released) return 0;
	
	for (size_t i = 0; i < this->child_nodes.size(); ++i)
	{
		SAFE_RELEASE(this->child_nodes.at(i));
	}
	this->child_nodes.clear();

	return 1;
}
