#pragma once
#include "MOperate.h"

namespace acamcad
{

	class CreateOperate_QuadballTSpline : public MOperation
	{
	public:
		//CreateOperate_Quadball();
		CreateOperate_QuadballTSpline(AMCAX::Point3 center, double radius, size_t segment);
		//virtual ~CreateOperate_Quadball();

	public:
		virtual bool DoOperate(AdapterObject* object) override;

		//virtual void operateWithTSpline(TSplineUObject* tspline_object);

	private:
		AMCAX::Point3 center_;
		double radius_;

		size_t seg_;
	};

}