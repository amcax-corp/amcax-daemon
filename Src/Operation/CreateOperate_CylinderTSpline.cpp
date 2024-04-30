#include "CreateOperate_CylinderTSpline.h"

#include <tmeshSplineAPI/TMSplineMakeCylinder.hpp>

namespace acamcad
{

	CreateOperate_CylinderTSpline::CreateOperate_CylinderTSpline(const MPoint3& b_center, const AMCAX::Vector3& axis,
		double radius, double height, size_t rf_num, size_t vf_num, bool top, bool bottom) :
		center_(b_center), axis_(axis), radius_(radius), height_(height), rf_num_(rf_num), vf_num_(vf_num), top_(top), bottom_(bottom)
	{}

	bool CreateOperate_CylinderTSpline::DoOperate(AdapterObject* object)
	{
		AMCAX::Direction3 dir(axis_);
		AMCAX::Frame3 frame(center_, dir);

		AMCAX::TMS::TMSplineMakeCylinder shape(frame, radius_, height_, top_, bottom_, rf_num_, vf_num_);

		object->tSpline->setShape(shape.BuildSpline());

		object->updateDraw();

		return object->tSpline->getShape();


	}
}