#include "SGLOperate_WeldEdge.h"

#include <polymesh/PolyMesh.hpp>
#include <polymeshAPI/MeshWeld.hpp>
#include <tmeshSplineAPI/TMSplineWeld.hpp>

using namespace acamcad;

SGLOperate_WeldEdge::SGLOperate_WeldEdge(const SelectModel& smodle)
	:MOperate_SingleObject(smodle)
{
}

bool SGLOperate_WeldEdge::DoOperate(AdapterObject* adapter)
{
	bool result = false;
	if (selmodel_ == SelectModel::EDGE_MODEL)
	{
		switch (adapter->dataType())
		{
		case DataType::MESH_TYPE:
		{
			std::vector<int> s_id_list = getSelectSubList(s_info_list_);

			result = AMCAX::SubD::MeshWeld::FilterVoidWeldEdges(adapter->mesh->getShape(), s_id_list);

			if (result) {
				AMCAX::SubD::MeshWeld::MeshWeldEdges(adapter->mesh->getShape(), s_id_list);
			}
		}
		break;
		case DataType::TSPLINEU_TYPE:
		{
			std::vector<int> s_id_list = getSelectSubList(s_info_list_);

			AMCAX::TMS::TMSplineWeld tool;
			result = tool.FilterVoidWeldEdges(adapter->tSpline->getShape(), s_id_list);

			if (result) {
				tool.WeldEdges(adapter->tSpline->getShape(), s_id_list);
				adapter->updateDraw();
			}

		}
		break;
		default:
			break;
		}
	}

	return result;


}

