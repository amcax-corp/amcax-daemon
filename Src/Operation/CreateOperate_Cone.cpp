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
#include "CreateOperate_Cone.h"
#include <modeling/MakeCone.hpp>

namespace acamcad
{

CreateOperate_Cone::CreateOperate_Cone():
	is_bottom_(true), segR_(8), segV_(4)
{
}

CreateOperate_Cone::CreateOperate_Cone(const AMCAX::Coord3 & b_center, const AMCAX::Coord3 & axis,
							    	   double radiusR, double radiusr, double height, double angle):
    segR_(8), segV_(4),
    is_bottom_(true), center_(b_center), axis_(axis),
    xaxis_(NAN, NAN, NAN), radiusR_(radiusR), radiusr_(radiusr), height_(height), angle_(angle)
{
}

CreateOperate_Cone::CreateOperate_Cone(const AMCAX::Coord3 & b_center, const AMCAX::Coord3 & axis, const AMCAX::Coord3 & xaxis, 
							    	   double radiusR, double radiusr, double height, double angle):
    segR_(8), segV_(4),
    is_bottom_(true), center_(b_center), axis_(axis),
    xaxis_(xaxis), radiusR_(radiusR), radiusr_(radiusr), height_(height), angle_(angle)
{
}

CreateOperate_Cone::~CreateOperate_Cone()
{
}


void CreateOperate_Cone::operateWithBRep(BRepObject* brep_object) {
	AMCAX::Frame3 frameZ(AMCAX::Point3{center_[0], center_[1], center_[2]},
						 AMCAX::Direction3{axis_[0], axis_[1], axis_[2]});

	AMCAX::Frame3 frameXZ(AMCAX::Point3{center_[0], center_[1], center_[2]},
						  AMCAX::Direction3{axis_[0], axis_[1], axis_[2]},
						  AMCAX::Direction3{xaxis_[0], xaxis_[1], xaxis_[2]});

	AMCAX::MakeCone mc(std::isnan(xaxis_[0]) ? frameZ : frameXZ, 
				       radiusR_, radiusr_, height_, angle_);

    brep_object->getShape() = mc.Shape();
    brep_object->updateDraw();
}


}

