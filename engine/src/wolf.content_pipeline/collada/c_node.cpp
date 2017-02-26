#include "w_cpipeline_pch.h"
#include "c_node.h"
#include <algorithm>

using namespace wolf::content_pipeline::collada;

c_node* c_node::find_instanced_geomaetry_node()
{
	if (!this->instanced_geometry_name.empty()) return this;

	std::for_each(this->nodes.begin(), this->nodes.end(), [](_In_ c_node* pNode)
	{
		if (pNode && !pNode->instanced_geometry_name.empty())
		{
			return pNode;
		}
	});

	return nullptr;
}

c_node* c_node::find_instanced_geomaetry_node(std::string pGeometryName)
{
	if (this->instanced_geometry_name == pGeometryName) return this;
	std::for_each(this->nodes.begin(), this->nodes.end(), [&pGeometryName](_In_ c_node* pNode)
	{
		if (pNode && pNode->instanced_geometry_name == pGeometryName)
		{
			return pNode;
		}
	});
	return nullptr;
}

c_node* c_node::find_instanced_node_name()
{
	if (!this->instanced_node_name.empty()) return this;
	std::for_each(this->nodes.begin(), this->nodes.end(), [](_In_ c_node* pNode) -> c_node*
	{
		if (pNode && !pNode->instanced_node_name.empty())
		{
			return pNode;
		}
	});
	return nullptr;
}

ULONG c_node::release()
{
	if (is_released) return 0;
	
	for (size_t i = 0; i < this->nodes.size(); ++i)
	{
		SAFE_RELEASE(this->nodes.at(i));
	}
	this->nodes.clear();

	return 1;
}
