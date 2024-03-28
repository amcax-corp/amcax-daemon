#include "SGLOperate_SplitFaceVertex.h"

#include <polymeshAPI/MeshSplit.hpp>
#include <polymesh/PolyMesh.hpp>
#include <tmeshSplineAPI/TMSplineSplit.hpp>

namespace acamcad
{

	SGLOperate_SplitFaceVertex::SGLOperate_SplitFaceVertex(const SelectModel& smodle)
		:MOperate_SingleObject(smodle)
	{
	}

	bool SGLOperate_SplitFaceVertex::DoOperate(AdapterObject* adapter)
	{
		if (selmodel_ != SelectModel::VERTEX_MODEL)
			return false;

		if (s_info_list_.size() < 2)
			return false;

		if (s_info_list_[0].object_id_ != s_info_list_[1].object_id_)
			return false;

		if (s_info_list_[0].object_subselect_id_ == s_info_list_[1].object_subselect_id_)
			return false;

		bool result = false;

		switch (adapter->dataType())
		{
		case DataType::MESH_TYPE:
		{
			std::vector<int> edges = getSelectSubList(s_info_list_);

			result = AMCAX::SubD::MeshSplit::SplitFaceWithVertex(adapter->mesh->mesh(), edges);
		}
		break;
		case DataType::TSPLINEU_TYPE:
		{
			std::vector<int> edges = getSelectSubList(s_info_list_);

			AMCAX::TMS::TMSplineSplit tool;

			result = tool.CanSplitFaceWithVertex(adapter->tSpline->getTSpline(), edges);

			if (result) {
				tool.SplitFaceWithVertex(adapter->tSpline->getTSpline(), edges);
				adapter->updateDraw();
			}
		}
		break;
		default:
			break;
		}

		return result;
	}

}