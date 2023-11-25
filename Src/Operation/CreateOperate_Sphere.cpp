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
#include "CreateOperate_Sphere.h"
#include <modeling/MakeSphere.hpp>

namespace acamcad
{

	CreateOperate_Sphere::CreateOperate_Sphere() :
		center_(AMCAX::Coord3(0.0, 0.0, 0.0)), radius_(1.0), segR_(8), segV_(2)
	{
	}

	CreateOperate_Sphere::CreateOperate_Sphere(const AMCAX::Coord3& center, double radius, size_t rotation_seg, size_t vertical_seg) :
		center_(center), radius_(radius), segR_(rotation_seg), segV_(vertical_seg)
	{
	}

	CreateOperate_Sphere::CreateOperate_Sphere(const AMCAX::Coord3& center, double radius, double angle1, double angle2, double angle3) :
		center_(center), radius_(radius), segR_(), segV_(), angle1_(angle1), angle2_(angle2), angle3_(angle3)
	{
	}

	CreateOperate_Sphere::~CreateOperate_Sphere()
	{
	}

	void CreateOperate_Sphere::operateWithBRep(BRepObject* brep_object)
	{
		brep_object->getShape() = AMCAX::MakeSphere(AMCAX::Point3(center_[0], center_[1], center_[2]), radius_, angle1_);
		brep_object->updateDraw();
	}


}



