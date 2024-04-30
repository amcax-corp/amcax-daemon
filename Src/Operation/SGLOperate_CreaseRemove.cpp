#include "SGLOperate_CreaseRemove.h"
#include <tmeshSplineAPI/TMSplineCreaseTool.hpp>

namespace acamcad
{
	SGLOperate_CreaseEdgeRemove::SGLOperate_CreaseEdgeRemove(const SelectModel& smodle)
		:MOperate_SingleObject(smodle)
	{
	}

	bool SGLOperate_CreaseEdgeRemove::DoOperate(AdapterObject* object)
	{
		if (selmodel_ != SelectModel::EDGE_MODEL ||
			object->dataType() != DataType::TSPLINEU_TYPE)
			return false;

		std::vector<int> s_id_list = getSelectSubList(s_info_list_);

		AMCAX::TMS::TMSplineCreaseTool::RemoveCreaseEdge(object->tSpline->getShape(), s_id_list);

		object->updateDraw();

		return true;
	}

}
