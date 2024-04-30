#include "CreateOperate_CircularTSpline.h"

#include <tmeshSplineAPI/TMSplineMakeCircular.hpp>
#include <common/FrameT.hpp>

namespace acamcad
{
	CreateOperate_CircularTSpline::CreateOperate_CircularTSpline(AMCAX::Coord3 center, AMCAX::Coord3 axis,
		double radius, double angle, size_t seg) : center_(center), axis_(axis), radius_(radius),
		angle_(angle), seg_(seg)
	{

	}

	bool CreateOperate_CircularTSpline::DoOperate(AdapterObject* object)
	{
		AMCAX::Direction3 dir(axis_);
		AMCAX::Point3 p(center_);
		AMCAX::Frame3 frame(p, dir);

		AMCAX::TMS::TMSplineMakeCircular shape(frame, radius_, angle_, seg_);


		object->tSpline->setShape(shape.BuildSpline());
		object->tSpline->updateDraw();
		return object->tSpline->getShape();
	}


}