#include "SGLOperate_SplitFaceByEdge.h"

#include <polymesh/PolyMesh.hpp>
#include <polymeshAPI/MeshSplit.hpp>
#include <polymeshAPI/MeshTool.hpp>
#include <tmeshSplineAPI/TMSplineSplit.hpp>

acamcad::SGLOperate_SplitFaceByEdge::SGLOperate_SplitFaceByEdge(const SelectModel& smodle)
	:MOperate_SingleObject(smodle)
{
}

bool acamcad::SGLOperate_SplitFaceByEdge::DoOperate(AdapterObject* adapter)
{
	if (s_info_list_.size() != re_points_.size())
	{
		std::cout << "The edge num is not equal point num" << std::endl;
		return false;
	}
	/*
	std::vector<int> edge_id = getSelectSubList(s_info_list_);
	std::vector<polymesh::MEdge*> edge_list; edge_list.resize(edge_id.size());
	std::vector<double> edge_t; edge_t.resize(re_points_.size());
	for (int i = 0; i < edge_id.size(); i++)
	{
		polymesh::MEdge* edge = mesh_object->mesh()->edge(edge_id[i]);
		polymesh::MHalfedge* he0 = edge->halfEdge();
		MVector3 vp = re_points_[i] - he0->fromVertex()->position();
		MVector3 vv = he0->toVertex()->position() - he0->fromVertex()->position();
		double t = vp.dot(vv) / vv.normSq(); t = 1 - t;
		if (t < 0) t = 0;
		else if (t > 1) t = 1;

		edge_t[i] = t;
		edge_list[i] = edge;
	}
	*/
	//polymesh::splitFaceWithEdge(mesh_object->mesh(), edge_list, edge_t);
	bool result = false;

	switch (adapter->dataType())
	{
	case DataType::MESH_TYPE:
	{
		std::vector<int> edge_id = getSelectSubList(s_info_list_);

		std::vector<AMCAX::Point3> edge_t;

		for (int i = 0; i < re_points_.size(); i++) {
			edge_t.push_back(AMCAX::Point3(re_points_[i]));
		}

		result = AMCAX::SubD::MeshSplit::SplitFaceWithEdge(adapter->mesh->getShape(), edge_id, edge_t);
	}
	break;
	case DataType::TSPLINEU_TYPE:
	{
		std::vector<int> edge_id = getSelectSubList(s_info_list_);

		std::vector<AMCAX::Point3> edge_t;
		for (int i = 0; i < re_points_.size(); i++) {
			edge_t.push_back(AMCAX::Point3(re_points_[i]));
		}
		AMCAX::TMS::TMSplineSplit tool;

		result = tool.CanSplitFaceWithEdge(adapter->tSpline->getShape(), edge_id, edge_t);

		if (result) {
			tool.SplitFaceWithEdge(adapter->tSpline->getShape(), edge_id, edge_t);
			adapter->updateDraw();
		}

	}
	break;
	default:
		break;
	}

	return result;

}

void acamcad::SGLOperate_SplitFaceByEdge::setParameter(const std::vector<AMCAX::Coord3>& related_point)
{
	re_points_ = related_point;
}
