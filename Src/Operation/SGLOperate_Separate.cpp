#include "SGLOperate_Separate.h"

#include <polymesh/PolyMesh.hpp>
#include <polymeshAPI/MeshSeparate.hpp>
#include <tmeshSplineAPI/TMSplineSeparate.hpp>


using namespace acamcad;

SGLOperate_Separate::SGLOperate_Separate(const SelectModel& smodle)
	:MOperate_SingleObject(smodle)
{
}

bool SGLOperate_Separate::DoOperate(AdapterObject* adapter)
{
	if (selmodel_ != SelectModel::EDGE_MODEL)
		return false;

	bool result = false;

	switch (adapter->dataType())
	{
	case DataType::MESH_TYPE:
	{
		std::vector<int> s_id_list = getSelectSubList(s_info_list_);
		//polymesh::meshSeparateEdges(mesh_object->mesh(), s_id_list);

		result = AMCAX::SubD::MeshSeparate::SeparateEdges(adapter->mesh->mesh(), s_id_list);
	}
	break;
	case DataType::TSPLINEU_TYPE:
	{
		std::vector<int> s_id_list = getSelectSubList(s_info_list_);

		//result = true;
		result = AMCAX::TMS::TMSplineSeparate().SeparateEdges(adapter->tSpline->getTSpline(), s_id_list);

	}
	break;
	default:
		break;
	}

	return result;
}

