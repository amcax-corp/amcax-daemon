#include "SGLOperate_FillHole.h"

#include <tmeshSplineAPI/TMSplineFillHole.hpp>

namespace acamcad
{
	SGLOperate_FillHole::SGLOperate_FillHole(const SelectModel& smodle)
		:MOperate_SingleObject(smodle)
	{
	}

	bool SGLOperate_FillHole::DoOperate(AdapterObject* adapter)
	{
		if (adapter->dataType() != DataType::TSPLINEU_TYPE)
			return false;

		bool result = false;

		switch (selmodel_)
		{
		case acamcad::SelectModel::EDGE_MODEL:
		{
			std::vector<int> ids = getSelectSubList(s_info_list_);
			if (ids.empty())
				break;

			AMCAX::TMS::TMSplineFillHole tool;
			result = tool.CanFillSingleHole(adapter->tSpline->getShape(), ids[0]);

			if (result)
			{
				tool.FillSingleHole(adapter->tSpline->getShape(), ids[0]);
				adapter->updateDraw();
			}

		}
		break;
		case acamcad::SelectModel::OBJECT_MODEL:
		{
			result = AMCAX::TMS::TMSplineFillHole().FillAllHoles(adapter->tSpline->getShape());

			adapter->updateDraw();
		}
		break;
		default:
			break;
		}

		return result;

	}


}