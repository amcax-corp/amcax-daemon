#include "SGLOperate_SplitFace.h"

#include <tmeshSplineAPI/TMSplineSplit.hpp>

namespace acamcad
{
	SGLOperate_SplitFace::SGLOperate_SplitFace(const SelectModel& smodle) :MOperate_SingleObject(smodle)
	{}

	bool SGLOperate_SplitFace::DoOperate(AdapterObject* adapter)
	{
		if (selmodel_ != SelectModel::FACE_MODEL ||
			adapter->dataType() != DataType::TSPLINEU_TYPE)
			return false;

		std::vector<int> ids = getSelectSubList(s_info_list_);

		AMCAX::TMS::TMSplineSplit tool;

		bool result = tool.CanSplitFacesCross(adapter->tSpline->getTSpline(), ids);

		if (result) {
			tool.SplitFacesCross(adapter->tSpline->getTSpline(), ids);
			adapter->updateDraw();
		}

		return true;
	}
}