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
#include "PlaneObject.h"
#include "../Utils/MathUtils.h"

using namespace acamcad;

PlaneObject::PlaneObject()
	:BaseObject(nullptr), center_(0.0, 0.0, 0.0), normal_(0.0, 0.0, 1.0)
{
	setDataType(DataType::CUSTOM_TYPE);
}

PlaneObject::PlaneObject(const AMCAX::Coord3& normal)
	: BaseObject(nullptr), center_(0.0, 0.0, 0.0), normal_(normal)
{
	setDataType(DataType::CUSTOM_TYPE);
}

PlaneObject::PlaneObject(const AMCAX::Coord3& center, const AMCAX::Coord3& normal)
	: BaseObject(nullptr), center_(center), normal_(normal)
{
	setDataType(DataType::CUSTOM_TYPE);
}

PlaneObject::PlaneObject(const PlaneObject& object)
	: BaseObject(object)
{
	center_ = object.center_;
	normal_ = object.normal_;
	setDataType(DataType::CUSTOM_TYPE);
}

void PlaneObject::setCenter(const AMCAX::Coord3& c)
{
	center_ = c;
}

void PlaneObject::setNormal(const AMCAX::Coord3& n)
{
	normal_ = n;
	normal_.Normalize();
}

bool PlaneObject::pointInPlane(const AMCAX::Coord3& point)
{
	AMCAX::Coord3 d = point - center_;
	double re = normal_.Dot(d);
	if (std::abs(re) < 1e-6)
		return true;
	else
		return false;
}

bool PlaneObject::lineInsertPlane(const AMCAX::Coord3& v0, const AMCAX::Coord3& d0, AMCAX::Coord3& v_insert)
{
	return MathUtils::intersection_Line_Plane(v0, d0, center_, normal_, v_insert);
}

void PlaneObject::projectPointToPlane(const AMCAX::Coord3& point, AMCAX::Coord3& project_point)
{
	project_point = MathUtils::projectPointPlane(center_, normal_, point);
}