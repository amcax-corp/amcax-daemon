#include "CreateOperate_QuadballTSpline.h"

#include <tmeshSplineAPI/TMSplineMakeQuadball.hpp>

namespace acamcad
{
	CreateOperate_QuadballTSpline::CreateOperate_QuadballTSpline(AMCAX::Point3 center, double radius, size_t segment) :
		center_(center), radius_(radius), seg_(segment)
	{
	}

	bool CreateOperate_QuadballTSpline::DoOperate(AdapterObject* object)
	{


		AMCAX::TMS::TMSplineMakeQuadball qb(center_, radius_, seg_);

		object->tSpline->setShape(qb.BuildSpline());

		object->tSpline->updateDraw();

		return object->tSpline->getShape();
	}
}