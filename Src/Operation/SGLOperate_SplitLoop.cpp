#include "SGLOperate_SplitLoop.h"

#include <polymesh/PolyMesh.hpp>
#include <polymeshAPI/MeshSplit.hpp>
#include <polymeshAPI/MeshTool.hpp>
#include <tmeshSplineAPI/TMSplineSplit.hpp>
#include <tmeshSplineAPI/TMSplineTool.hpp>

acamcad::SGLOperate_SplitLoop::SGLOperate_SplitLoop(const SelectModel& smodle)
	:MOperate_SingleObject(smodle)
{
}

bool acamcad::SGLOperate_SplitLoop::DoOperate(AdapterObject* adapter)
{
	if (selmodel_ != SelectModel::EDGE_MODEL)
		return false;

	if (s_info_list_.size() != 1 || re_points_.size() != s_info_list_.size())
		return false;

	bool result = false;

	switch (adapter->dataType())
	{
	case DataType::MESH_TYPE:
	{
		int p0, p1;
		AMCAX::SubD::MeshTool::EdgeVertexIndexs(adapter->mesh->mesh(), s_info_list_[0].object_subselect_id_, p0, p1);

		AMCAX::Point3 point0 = AMCAX::SubD::MeshTool::Position(adapter->mesh->mesh(), p0),
			point1 = AMCAX::SubD::MeshTool::Position(adapter->mesh->mesh(), p1),
			pointT(re_points_[0]);

		double t = point0.Distance(pointT) / point0.Distance(point1);

		result = AMCAX::SubD::MeshSplit::SplitLoop(adapter->mesh->mesh(), s_info_list_[0].object_subselect_id_, 1 - t);
	}
	break;
	case DataType::TSPLINEU_TYPE:
	{
		int p0, p1;
		AMCAX::TMS::TMSplineTool::EdgeVertexIndexs(adapter->tSpline->getTSpline(), s_info_list_[0].object_subselect_id_,
			p0, p1);
		AMCAX::Point3 point0 = AMCAX::TMS::TMSplineTool::ControlPosition(adapter->tSpline->getTSpline(), p0),
			point1 = AMCAX::TMS::TMSplineTool::ControlPosition(adapter->tSpline->getTSpline(), p1),
			pointT(re_points_[0]);

		double t = point0.Distance(pointT) / point0.Distance(point1);
		AMCAX::TMS::TMSplineSplit tool;
		result = tool.CanSplitFaceLoop(adapter->tSpline->getTSpline(), s_info_list_[0].object_subselect_id_, t);
		if (result) {
			tool.SplitFaceLoop(adapter->tSpline->getTSpline(), s_info_list_[0].object_subselect_id_, t);
			adapter->updateDraw();
		}
		//result = AMCAX::TMS::TMSplineSplit().SplitFaceLoop(adapter->tSpline->getTSpline(), 1 - t);
	}
	break;
	default:
		break;
	}

	return result;

	//polymesh::splitLoop(mesh_object->mesh(), edge, t);
}

/*
void acamcad::SGLOperate_SplitLoop::operateWithTSpline(TSplineObject* tspline_object)
{
	if (selmodel_ != SelectModel::EDGE_MODEL)
		return;

	if (s_info_list_.size() != 1 || re_points_.size() != s_info_list_.size())
		return;

	tspline::TEdge* edge = tspline_object->getTSpline()->edge(s_info_list_[0].object_subselect_id_);
	tspline::TLink* he0 = edge->halfEdge();
	MVector3 vp = re_points_[0].point() - he0->fromVertex()->controlPosition();
	MVector3 vv = he0->toVertex()->controlPosition() - he0->fromVertex()->controlPosition();
	double t = vp.dot(vv) / vv.normSq();
	if (t < 0) t = 0;
	else if (t > 1) t = 1;

	tspline::TO_SplitControl split;
	split.splitFaceLoop(tspline_object->getTSpline(), edge->index(), t);
	split.getTSpline(tspline_object->getTSpline());
	tspline_object->updateDraw();
}
*/

void acamcad::SGLOperate_SplitLoop::setParameter(const std::vector<AMCAX::Coord3>& related_point)
{
	re_points_ = related_point;
}
