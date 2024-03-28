/* ===================================================================
* Copyright (C) 2023 Hefei Jiushao Intelligent Technology Co., Ltd.
* All rights reserved.
*
* This software is licensed under the GNU Affero General Public License
* v3.0 (AGPLv3.0) or a commercial license. You may choose to use this
* software under the terms of either license.
*
* For more information about the AGPLv3.0 license, please visit:
* https://www.gnu.org/licenses/agpl-3.0.html
* For licensing inquiries or to obtain a commercial license, please
* contact Hefei Jiushao Intelligent Technology Co., Ltd.
* ===================================================================
* Author:
*/
#include "CreateOperate_Cylinder.h"
#include <modeling/MakeCylinder.hpp>

namespace acamcad
{

	CreateOperate_Cylinder::CreateOperate_Cylinder() :
		bottomCenter_(AMCAX::Coord3(0.0, 0.0, 0.0)), radius_(1.0), height_(1.0), axis_(0.0, 0.0, 1.0), xaxis_(NAN, NAN, NAN), angle_(0)
	{
	}


	CreateOperate_Cylinder::CreateOperate_Cylinder(const AMCAX::Coord3& bottomCenter, double radius, double height, const AMCAX::Coord3& axis, double angle) :
		bottomCenter_(bottomCenter), radius_(radius), height_(height), axis_(axis), xaxis_(NAN, NAN, NAN), angle_(angle)
	{
	}

	CreateOperate_Cylinder::CreateOperate_Cylinder(const AMCAX::Coord3& bottomCenter, double radius, double height, const AMCAX::Coord3& axis, const AMCAX::Coord3& xaxis, double angle) :
		bottomCenter_(bottomCenter), radius_(radius), height_(height), axis_(axis), xaxis_(xaxis), angle_(angle)
	{

	}


	CreateOperate_Cylinder::~CreateOperate_Cylinder()
	{
	}

	bool CreateOperate_Cylinder::DoOperate(AdapterObject* brep_object)
	{
		AMCAX::Frame3 frameZ(AMCAX::Point3{ bottomCenter_[0], bottomCenter_[1], bottomCenter_[2] },
			AMCAX::Direction3{ axis_[0], axis_[1], axis_[2] });

		AMCAX::Frame3 frameXZ(AMCAX::Point3{ bottomCenter_[0], bottomCenter_[1], bottomCenter_[2] },
			AMCAX::Direction3{ axis_[0], axis_[1], axis_[2] },
			AMCAX::Direction3{ xaxis_[0], xaxis_[1], xaxis_[2] });

		AMCAX::MakeCylinder mc(std::isnan(xaxis_[0]) ? frameZ : frameXZ, radius_, height_, angle_);

		brep_object->bRep->getShape() = mc.Shape();
		brep_object->updateDraw();

		return true;
	}

}

