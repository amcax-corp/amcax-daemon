#pragma once
#include "MOperate.h"

namespace acamcad
{

	/// <summary>
	/// 根据选择信息获取其center
	/// </summary>
	class MOperation_SelectCloset : public MOperation
	{
	public:
		MOperation_SelectCloset(const SelectModel& smodle);
		virtual ~MOperation_SelectCloset() {};

	public:
		virtual bool DoOperate(AdapterObject* adapter) override;

		//virtual void operateWithTSpline(TSplineObject* tspline_object) override;

	public:
		void setSelectInfoCamera(const SelectInfoWithCamera& s_info);

		MPoint3 getCloset() { return closet_; }

	private:
		int object_id_;
		int object_element_id_;

		AMCAX::Coord3 begin_;
		AMCAX::Coord3 end_;
		MPoint3 closet_;

	};

} //namespace acamcad