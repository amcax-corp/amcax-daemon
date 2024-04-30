#include "SGLOperate_Crease.h"

#include <tmeshSplineAPI/TMSplineCreaseTool.hpp>

namespace acamcad
{
	SGLOperate_CreaseEdge::SGLOperate_CreaseEdge(const SelectModel& smodle)
		:MOperate_SingleObject(smodle)
	{
	}

	bool SGLOperate_CreaseEdge::DoOperate(AdapterObject* object)
	{
		if (selmodel_ != SelectModel::EDGE_MODEL ||
			object->dataType() != DataType::TSPLINEU_TYPE)
			return false;

		std::vector<int> s_id_list = getSelectSubList(s_info_list_);

		AMCAX::TMS::TMSplineCreaseTool::AddCreaseEdge(object->tSpline->getShape(), s_id_list);

		object->updateDraw();

		return true;
	}

}
