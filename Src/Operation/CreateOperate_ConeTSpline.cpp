#include "CreateOperate_ConeTSpline.h"

//#include <tmeshSplineAPI/TMSplineMakeCylinder.hpp>
#include <tmeshSplineAPI/TMSplineMakeCone.hpp>

namespace acamcad
{
	CreateOperate_ConeTSpline::CreateOperate_ConeTSpline(const MPoint3& b_center, const AMCAX::Vector3& axis,
		double radius, double height, size_t rf_num, size_t vf_num, bool bottom) :
		b_center_(b_center), axis_(axis), radius_(radius), height_(height), rf_num_(rf_num), vf_num_(vf_num), bottom_(bottom)
	{
	}

	bool CreateOperate_ConeTSpline::DoOperate(AdapterObject* object)
	{
		AMCAX::Direction3 dir(axis_);
		//AMCAX::Frame
		AMCAX::Frame3 frame(b_center_, dir);

		AMCAX::TMS::TMSplineMakeCone cone(frame, radius_, height_, bottom_, rf_num_, vf_num_);

		object->tSpline->setTSpline(cone.BuildSpline());

		object->tSpline->updateDraw();

		return true;
	}
}