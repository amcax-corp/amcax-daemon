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
#pragma once

#include "BaseObject.h"
#include <common/CoordT.hpp>

namespace acamcad
{
	class Arrow : BaseObject
	{
	public:
		Arrow();
		Arrow(const Arrow& object);
		Arrow(const AMCAX::Coord3& direction, double lenth, double radius = 1.0);
		Arrow(const AMCAX::Coord3& from, const AMCAX::Coord3& direction, double length, double radius = 1.0);
		Arrow(const AMCAX::Coord3& from, const AMCAX::Coord3& to, double radius = 1.0);
		virtual ~Arrow() {}

	public:
		AMCAX::Coord3 direction() const { return dir_; }
		AMCAX::Coord3& direction() { return dir_; }

		AMCAX::Coord3 from() const { return from_; }
		AMCAX::Coord3& from() { return from_; }

		void setDirection(const AMCAX::Coord3& dirction) {
			dir_ = dirction;
			dir_.Normalize();
		}

		void setFrom(const AMCAX::Coord3& point) { from_ = point; }

		double length() const { return length_; }
		double& length() { return length_; }

		void setLength(double length) { length_ = length; }

		double radius() const { return radius_; }
		double& radius() { return radius_; }
		void setRadius(double radius) { radius_ = radius; }
	public:
		void orthogonalFrame(Arrow& u, Arrow& v);

	private:
		double length_;
		double radius_;

		AMCAX::Coord3 dir_;
		AMCAX::Coord3 from_;
	};

	class Axis : BaseObject
	{
		friend class ObjectDraw;
		friend class AxisObjectDraw;

	public:
		Axis();
		Axis(double length);
		Axis(const AMCAX::Coord3& getCenter, const AMCAX::Coord3& z_axis);

		virtual ~Axis() {};

	public:
		const Arrow& axis_x() const { return axis_x_; }
		Arrow axis_x() { return axis_x_; }

		const Arrow& axis_y() const { return axis_y_; }
		Arrow axis_y() { return axis_y_; }

		const Arrow& axis_z() const { return axis_z_; }
		Arrow axis_z() { return axis_z_; }

		// x is 0, y is 1, z is 2;
		Arrow axis_vector(int i) const;
		Arrow axis_vector(int i);

		void setAxis(const AMCAX::Coord3& axis_z, double length = 1.0);

	public:
		double length() { return axis_z_.length(); }
		void setlentgh(double length = 1.0);

		AMCAX::Coord3 getCenter() { return axis_z_.from(); }
		void setCenter(const AMCAX::Coord3& getCenter);

		double radius() { return axis_z_.radius(); }
		void setRadius(double radius);

	private:
		AMCAX::Coord3 center_;

		Arrow axis_x_;
		Arrow axis_y_;
		Arrow axis_z_;

	public:
		const ObjectDraw* getDrawTool() const { return draw_tool_; }

	private:
		ObjectDraw* draw_tool_;
	};

} // namespace acamcad