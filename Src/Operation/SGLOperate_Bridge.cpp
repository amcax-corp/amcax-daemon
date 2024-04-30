#include "SGLOperate_Bridge.h"

#include <polymesh/PolyMesh.hpp>
#include <polymeshAPI/MeshInsertFace.hpp>
#include <tmeshSplineAPI/TMSplineBridgeEdge.hpp>

using namespace acamcad;

SGLOperate_Bridge::SGLOperate_Bridge(const SelectModel& smodle)
	:MOperate_SingleObject(smodle)
{
}

bool SGLOperate_Bridge::DoOperate(AdapterObject* adapter)
{
	if (selmodel_ != SelectModel::EDGE_MODEL)
		return false;

	if (s_info_list_.size() < 2) return false;

	if (s_info_list_[0].object_id_ != s_info_list_[1].object_id_)
		return false;

	bool result = true;

	switch (adapter->dataType())
	{
	case DataType::MESH_TYPE:
	{
		std::vector<int> s_id_list = getSelectSubList(s_info_list_);

		result = AMCAX::SubD::MeshInsertFace::AddFaceByEdge(adapter->mesh->getShape(), s_id_list[0], s_id_list[1]);
	}
	break;
	case DataType::TSPLINEU_TYPE:
	{
		std::vector<int> s_id_list = getSelectSubList(s_info_list_);

		AMCAX::TMS::TMSplineBridgeEdge tool;

		result = tool.CanBridgeEdge(adapter->tSpline->getShape(), s_id_list[0], s_id_list[1]);

		if (result) {
			tool.BridgeEdge(adapter->tSpline->getShape(), s_id_list[0], s_id_list[1]);
			adapter->updateDraw();
		}
	}
	break;
	default:
		break;
	}

	return result;
	//polymesh::MEdge* e0 = mesh_object->mesh()->edge(s_id_list[0]);
	//polymesh::MEdge* e1 = mesh_object->mesh()->edge(s_id_list[1]);
	//polymesh::bridgeEdge(mesh_object->mesh(), e0, e1);



}