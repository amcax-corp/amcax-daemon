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
#include "CreateOperate_Torus.h"
#include <modeling/MakeTorus.hpp>

namespace acamcad
{

	CreateOperate_Torus::CreateOperate_Torus()
	{
	}

	CreateOperate_Torus::CreateOperate_Torus(const AMCAX::Coord3& center, double radiusMajor, double radiusMinor, const AMCAX::Coord3& axis, double angle) :
		center_(center), radiusMajor_(radiusMajor), radiusMinor_(radiusMinor), axis_(axis), xaxis_(NAN, NAN, NAN), angle_(angle)
	{

	}

	CreateOperate_Torus::CreateOperate_Torus(const AMCAX::Coord3& center, double radiusMajor, double radiusMinor, const AMCAX::Coord3& axis, const AMCAX::Coord3& xaxis, double angle) :
		center_(center), radiusMajor_(radiusMajor), radiusMinor_(radiusMinor), axis_(axis), xaxis_(xaxis), angle_(angle)
	{

	}

	CreateOperate_Torus::~CreateOperate_Torus()
	{
	}

	void CreateOperate_Torus::operateWithBRep(BRepObject* brep_object)
	{
		AMCAX::Frame3 frameZ(AMCAX::Point3{ center_[0], center_[1], center_[2] },
			AMCAX::Direction3{ axis_[0], axis_[1], axis_[2] });

		AMCAX::Frame3 frameXZ(AMCAX::Point3{ center_.X(), center_.Y(), center_.Z() },
			AMCAX::Direction3{ axis_[0], axis_[1], axis_[2] },
			AMCAX::Direction3{ xaxis_[0], xaxis_[1], xaxis_[2] });

		AMCAX::MakeTorus mkTorus(std::isnan(xaxis_[0]) ? frameZ : frameXZ, radiusMajor_, radiusMinor_, angle_);

		brep_object->getShape() = mkTorus.Shape();
		brep_object->updateDraw();
	}


}

