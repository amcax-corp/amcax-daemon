#include "SGLOperate_DeleteElement.h"

#include <polymesh/PolyMesh.hpp>
#include <polymeshAPI/MeshReduce.hpp>

#include <tmeshSplineAPI/TMSplineReduce.hpp>

using namespace acamcad;

SGLOperate_Delete::SGLOperate_Delete(const SelectModel& smodle)
	:MOperate_SingleObject(smodle)
{
}

bool SGLOperate_Delete::DoOperate(AdapterObject* adapter)
{



	bool result = false;

	switch (adapter->dataType())
	{
	case DataType::MESH_TYPE:
	{
		if (selmodel_ != SelectModel::FACE_MODEL)
		{
			break;
		}
		std::vector<int> s_id_list = getSelectSubList(s_info_list_);
		result = true;
		AMCAX::SubD::MeshReduce::DeleteFaces(adapter->mesh->getShape(), s_id_list);
	}
	break;
	case DataType::TSPLINEU_TYPE:
	{
		switch (selmodel_)
		{
		case acamcad::SelectModel::VERTEX_MODEL:
		{

			std::vector<int> s_id_list = getSelectSubList(s_info_list_);
			std::sort(s_id_list.begin(), s_id_list.end(), std::greater<int>());
			s_id_list.erase(std::unique(s_id_list.begin(), s_id_list.end()), s_id_list.end());

			AMCAX::TMS::TMSplineReduce reduce;

			result = reduce.CanRemoveVertices(adapter->tSpline->getShape(), s_id_list);

			if (result) {
				reduce.RemoveEdges(adapter->tSpline->getShape(), s_id_list);
			}
		}
		break;
		case acamcad::SelectModel::EDGE_MODEL:
		{
			std::vector<int> s_id_list = getSelectSubList(s_info_list_);
			std::sort(s_id_list.begin(), s_id_list.end(), std::greater<int>());
			s_id_list.erase(std::unique(s_id_list.begin(), s_id_list.end()), s_id_list.end());

			AMCAX::TMS::TMSplineReduce reduce;

			result = reduce.CanRemoveEdges(adapter->tSpline->getShape(), s_id_list);

			if (result) {
				reduce.RemoveEdges(adapter->tSpline->getShape(), s_id_list);
			}
		}
		break;
		case acamcad::SelectModel::FACE_MODEL:
		{
			std::vector<int> s_id_list = getSelectSubList(s_info_list_);
			std::sort(s_id_list.begin(), s_id_list.end(), std::greater<int>());
			s_id_list.erase(std::unique(s_id_list.begin(), s_id_list.end()), s_id_list.end());

			AMCAX::TMS::TMSplineReduce reduce;

			result = reduce.CanDeleteFaces(adapter->tSpline->getShape(), s_id_list);

			if (result) {
				reduce.DeleteFaces(adapter->tSpline->getShape(), s_id_list);
			}
		}
		break;
		default:
			break;
		}
	}
	break;
	default:
		break;
	}


	return result;
}