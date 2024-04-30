#include "SGLOperate_Uniform.h"

#include <tmeshSplineAPI/TMSplineRepair.hpp>

namespace acamcad
{
	SGLOperate_Uniform::SGLOperate_Uniform(const SelectModel& smodle)
		:MOperate_SingleObject(smodle)
	{
	}

	bool SGLOperate_Uniform::DoOperate(AdapterObject* object)
	{
		if (selmodel_ != SelectModel::OBJECT_MODEL ||
			object->dataType() != DataType::TSPLINEU_TYPE)
			return false;

		AMCAX::TMS::TMSplineRepair repair;

		bool result = repair.UniformTSpline(object->tSpline->getShape());

		if (result)
		{
			object->updateDraw();
		}

		return result;
	}

}


