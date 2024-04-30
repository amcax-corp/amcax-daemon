#include "SGLOperate_Repair.h"

#include <tmeshSplineAPI/TMSplineRepair.hpp>

namespace acamcad
{
	SGLOperate_Repair::SGLOperate_Repair(const SelectModel& smodle)
		:MOperate_SingleObject(smodle)
	{
	}

	bool SGLOperate_Repair::DoOperate(AdapterObject* adapter)
	{
		if (selmodel_ != SelectModel::OBJECT_MODEL ||
			adapter->dataType() != DataType::TSPLINEU_TYPE)
			return false;

		return AMCAX::TMS::TMSplineRepair().RepairTSpline(adapter->tSpline->getShape());
	}

}

