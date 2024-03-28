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
#include "AxisObject.h"

#include "../ObjectDraw/AxisObjectDraw.h"
#include "../Utils/MathUtils.h"

using namespace acamcad;

Arrow::Arrow() :
	BaseObject(nullptr), from_(0.0, 0.0, 0.0), dir_(0.0, 0.0, 1.0), length_(1.0), radius_(1.0)
{
	setDataType(DataType::CUSTOM_TYPE);
}

Arrow::Arrow(const Arrow& a) :
	BaseObject(a)
{
	from_ = a.from_;
	dir_ = a.dir_;
	length_ = a.length_;
	radius_ = a.radius_;
	setDataType(DataType::CUSTOM_TYPE);
}

Arrow::Arrow(const AMCAX::Coord3& direction, double lenth, double radius) :
	BaseObject(), from_(0.0, 0.0, 0.0), dir_(direction), length_(lenth), radius_(radius)
{
	setDataType(DataType::CUSTOM_TYPE);
	dir_.Normalize();
}

Arrow::Arrow(const AMCAX::Coord3& from, const AMCAX::Coord3& direction, double length, double radius) :
	BaseObject(), from_(from), dir_(direction), length_(length), radius_(radius)
{
	setDataType(DataType::CUSTOM_TYPE);
	dir_.Normalize();
}

Arrow::Arrow(const AMCAX::Coord3& from, const AMCAX::Coord3& to, double radius) :
	BaseObject(), from_(from), radius_(radius)
{
	setDataType(DataType::CUSTOM_TYPE);
	dir_ = to - from;
	length_ = dir_.Norm();
	dir_.Normalize();
}


void Arrow::orthogonalFrame(Arrow& u, Arrow& v)
{
	AMCAX::Coord3 au, av;
	MathUtils::GramSchmidtAlign(dir_, au, av);

	u.setDirection(au);
	u.setFrom(from_);
	u.setLength(length_);

	v.setDirection(av);
	v.setFrom(from_);
	v.setLength(length_);
}

//========================================================================
//========================================================================

Axis::Axis() :
	BaseObject(), center_(0.0, 0.0, 0.0), axis_x_(), axis_y_(), axis_z_()
{
	setDataType(DataType::CUSTOM_TYPE);
	draw_tool_ = new AxisObjectDraw(this);

	axis_x_.setDirection(AMCAX::Coord3(1.0, 0.0, 0.0));
	axis_y_.setDirection(AMCAX::Coord3(0.0, 1.0, 0.0));
	axis_z_.setDirection(AMCAX::Coord3(0.0, 0.0, 1.0));
}

Axis::Axis(double length) :
	BaseObject(), center_(0.0, 0.0, 0.0), axis_x_(), axis_y_(), axis_z_()
{
	setDataType(DataType::CUSTOM_TYPE);
	draw_tool_ = new AxisObjectDraw(this);

	axis_x_.setDirection(AMCAX::Coord3(1.0, 0.0, 0.0)); axis_x_.setLength(length);
	axis_y_.setDirection(AMCAX::Coord3(0.0, 1.0, 0.0)); axis_y_.setLength(length);
	axis_z_.setDirection(AMCAX::Coord3(0.0, 0.0, 1.0)); axis_z_.setLength(length);
}

Axis::Axis(const AMCAX::Coord3& getCenter, const AMCAX::Coord3& z_axis) :
	BaseObject(), center_(getCenter), axis_z_(getCenter, z_axis), axis_x_(getCenter, z_axis), axis_y_(getCenter, z_axis)
{
	setDataType(DataType::CUSTOM_TYPE);
	draw_tool_ = new AxisObjectDraw(this);

	axis_z_.orthogonalFrame(axis_x_, axis_y_);
}

void Axis::setCenter(const AMCAX::Coord3& getCenter)
{
	center_ = getCenter;
	axis_x_.setFrom(center_);
	axis_y_.setFrom(center_);
	axis_z_.setFrom(center_);
}

void acamcad::Axis::setRadius(double radius)
{
	axis_x_.setRadius(radius);
	axis_y_.setRadius(radius);
	axis_z_.setRadius(radius);
}

Arrow acamcad::Axis::axis_vector(int i)
{
	if (i == 0) return axis_x_;
	else if (i == 1) return axis_y_;
	else if (i == 2) return axis_z_;
	else return axis_z_;
}
Arrow acamcad::Axis::axis_vector(int i) const
{
	if (i == 0) return axis_x_;
	else if (i == 1) return axis_y_;
	else if (i == 2) return axis_z_;
	else return axis_z_;
}

void Axis::setAxis(const AMCAX::Coord3& axis_z, double length)
{
	axis_z_.setDirection(axis_z);
	axis_z_.setLength(length);
	axis_z_.orthogonalFrame(axis_x_, axis_y_);
}

void Axis::setlentgh(double length)
{
	axis_x_.setLength(length);
	axis_y_.setLength(length);
	axis_z_.setLength(length);
}