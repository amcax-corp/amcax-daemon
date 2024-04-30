#include "CreateOperate_TorusTSpline.h"

#include <tmeshSplineAPI/TMSplineMakeTorus.hpp>
#include <common/FrameT.hpp>

namespace acamcad
{
	CreateOperate_TorusTSpline::CreateOperate_TorusTSpline(const AMCAX::Coord3& center,
		const AMCAX::Coord3& axis, double radiusMajor, double radiusMinor, size_t segMajor, size_t segMinor) :
		center_(center), axis_(axis), radiusMajor_(radiusMajor), radiusMinor_(radiusMinor),
		segMajor_(segMajor), segMinor_(segMinor)
	{
	}

	bool CreateOperate_TorusTSpline::DoOperate(AdapterObject* object)
	{
		AMCAX::Direction3 dir(axis_);
		AMCAX::Point3 p(center_);
		AMCAX::Frame3 frame(p, dir);
		AMCAX::TMS::TMSplineMakeTorus torus(frame, radiusMajor_, radiusMinor_, segMajor_, segMinor_);

		object->tSpline->setShape(torus.BuildSpline());

		object->tSpline->updateDraw();

		return object->tSpline->getShape();
	}
}