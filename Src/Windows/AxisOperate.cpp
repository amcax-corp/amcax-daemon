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
#include "AxisOperate.h"
#include "../Utils/MathUtils.h"

#include "AxisOperateDraw.h"

using namespace acamcad;

static const double ring[11] = {-0.5, -0.4, -0.3, -0.2, -0.1, 0.0, 0.1, 0.2, 0.3, 0.4, 0.5};

acamcad::AxisOperation::AxisOperation() 
	: len_x_(1.0), len_y_(1.0), len_z_(1.0)
{
	center_ = AMCAX::Coord3(0.0, 0.0, 0.0);
	dir_x_ = AMCAX::Coord3(1.0, 0.0, 0.0);
	dir_y_ = AMCAX::Coord3(0.0, 1.0, 0.0);
	dir_z_ = AMCAX::Coord3(0.0, 0.0, 1.0);

	draw_tool = new AxisOperateDraw();
}

acamcad::AxisOperation::AxisOperation(const AMCAX::Coord3& center) 
	: len_x_(1.0), len_y_(1.0), len_z_(1.0)
{
	center_ = center;
	dir_x_ = AMCAX::Coord3(1.0, 0.0, 0.0);
	dir_y_ = AMCAX::Coord3(0.0, 1.0, 0.0);
	dir_z_ = AMCAX::Coord3(0.0, 0.0, 1.0);

	draw_tool = new AxisOperateDraw();
}

acamcad::AxisOperation::AxisOperation(const AMCAX::Coord3& center, const AMCAX::Coord3& dir_z) 
	: len_x_(1.0), len_y_(1.0), len_z_(1.0)
{
	center_ = center;

	dir_z_ = dir_z; 
	dir_z_.Normalize();

	AMCAX::Coord3 au, av;
	MathUtils::GramSchmidtAlign(dir_z_, au, av);
	dir_x_ = au; dir_x_.Normalize();
	dir_y_ = av; dir_y_.Normalize();

	draw_tool = new AxisOperateDraw();
}

acamcad::AxisOperation::AxisOperation(const AMCAX::Coord3& center, const AMCAX::Coord3& dir_x, const AMCAX::Coord3& dir_y) 
	: len_x_(1.0), len_y_(1.0), len_z_(1.0)
{
	center_ = center;
	dir_x_ = dir_x; dir_x_.Normalize();
	dir_y_ = dir_y; dir_y_.Normalize();
	dir_z_ = dir_x_.Cross(dir_y_); dir_z_.Normalize();

	draw_tool = new AxisOperateDraw();
}

acamcad::AxisOperation::AxisOperation(const AMCAX::Coord3& center, const AMCAX::Coord3& dir_x, const AMCAX::Coord3& dir_y, const AMCAX::Coord3& dir_z) 
	: len_x_(1.0), len_y_(1.0), len_z_(1.0)
{
	center_ = center;
	dir_x_ = dir_x; dir_x_.Normalize();
	dir_y_ = dir_y; dir_y_.Normalize();
	dir_z_ = dir_z; dir_z_.Normalize();

	draw_tool = new AxisOperateDraw();
}

AxisOperation::~AxisOperation()
{
	if (draw_tool != nullptr)
		delete draw_tool;
}


AMCAX::Coord3 acamcad::AxisOperation::operateMovePart(const AMCAX::Coord3& mouse_vector, double length)
{
	if (select_part_ == 0)
	{
		return (mouse_vector * length);
	}
	else if (select_part_ == 1)
	{
		length *= MathUtils::signTwoVector(dir_x_, mouse_vector);
		return (dir_x_ * length);
	}
	else if (select_part_ == 2)
	{
		length *= MathUtils::signTwoVector(dir_y_, mouse_vector);
		return (dir_y_ * length);
	}
	else if (select_part_ == 3)
	{
		length *= MathUtils::signTwoVector(dir_z_, mouse_vector);
		return (dir_z_ * length);
	}
	else if (select_part_ == 4)	//xy
	{
		double dis = mouse_vector.Dot(dir_z_);
		AMCAX::Coord3 vp_new = mouse_vector - (dis * dir_z_);

		return length * vp_new;
	}
	else if (select_part_ == 5)	//xz
	{
		double dis = mouse_vector.Dot(dir_y_);
		AMCAX::Coord3 vp_new = mouse_vector - (dis * dir_y_);

		return length * vp_new;
	}
	else if (select_part_ == 6)	//yz
	{
		double dis = mouse_vector.Dot(dir_x_);
		AMCAX::Coord3 vp_new = mouse_vector - (dis * dir_x_);

		return length * vp_new;
	}
	else
	{
		return (mouse_vector * 0.0);
	}
}

AMCAX::Coord3 acamcad::AxisOperation::operateScalePart(const AMCAX::Coord3& mouse_vector, double length)
{
	if (select_part_ == 1)
	{
		length *= MathUtils::signTwoVector(dir_x_, mouse_vector);
		len_x_ = len_x_ + length;
		return dir_x_;
	}
	else if (select_part_ == 2)
	{
		length *= MathUtils::signTwoVector(dir_y_, mouse_vector);
		len_y_ = len_y_ + length;
		return dir_y_;
	}
	else if (select_part_ == 3)
	{
		length *= MathUtils::signTwoVector(dir_z_, mouse_vector);
		len_z_ = len_z_ + length;
		return dir_z_;
	}
	else if (select_part_ == 4)
	{
		length *= MathUtils::signTwoVector(dir_x_, mouse_vector);
		len_x_ = len_x_ + length;
		len_y_ = len_y_ + length;
		return dir_z_;
	}
	else if (select_part_ == 5)
	{
		length *= MathUtils::signTwoVector(dir_x_, mouse_vector);
		len_x_ = len_x_ + length;
		len_z_ = len_z_ + length;
		return dir_y_;
	}
	else if (select_part_ == 6)
	{
		length *= MathUtils::signTwoVector(dir_y_, mouse_vector);
		len_y_ = len_y_ + length;
		len_z_ = len_z_ + length;
		return dir_x_;
	}
	else
	{
		return AMCAX::Coord3(0.0, 0.0, 0.0);
	}
}

double acamcad::AxisOperation::operateScalePart_scale()
{
	if (select_part_ == 1)
	{
		return len_x_;
	}
	else if (select_part_ == 2)
	{
		return len_y_;
	}
	else if (select_part_ == 3)
	{
		return len_z_;
	}
	else if (select_part_ == 4)
	{
		return len_x_;
	}
	else if (select_part_ == 5)
	{
		return len_x_;
	}
	else if (select_part_ == 6)
	{
		return len_y_;
	}
	else
	{
		return 1.0;
	}
}

AMCAX::Coord3 acamcad::AxisOperation::operateRotation(double& u)
{
	if (select_part_ == 1)
	{
		u = -u;
		return dir_z_;
	}
	else if (select_part_ == 2)
	{
		u = -u;
		return dir_y_;
	}
	else if (select_part_ == 3)
	{
		return dir_x_;
	}
	else
	{
		return AMCAX::Coord3(0.0, 0.0, 0.0);
	}
}


void acamcad::AxisOperation::reSetLen()
{
	len_x_ = 1;
	len_y_ = 1;
	len_z_ = 1;
}


void acamcad::AxisOperation::draw(const std::string& draw_type)
{
	draw_tool->drawOperateAxis(this, draw_type);
}

void acamcad::AxisOperation::drawForSelect(const std::string& draw_type)
{
	draw_tool->drawOperateAxisInSelect(this, draw_type);
}

AMCAX::Coord3 acamcad::AxisOperation::centerOffset(int d) const
{
	switch (d)
	{
	case 0:
		return center_ + dir_x_ * 0.1;
	case 1:
		return center_ + dir_y_ * 0.1;
	case 2:
		return center_ + dir_z_ * 0.1;
	default:
		return center_;
		break;
	}
}

AMCAX::Coord3 acamcad::AxisOperation::centerFinal(int d) const
{
	switch (d)
	{
	case 0:
		return center_ + dir_x_;
	case 1:
		return center_ + dir_y_;
	case 2:
		return center_ + dir_z_;
	default:
		return center_;
		break;
	}
}

void acamcad::AxisOperation::planeOffset(int p, std::vector<AMCAX::Coord3>& plane) const
{
	plane.resize(4);
	switch (p)
	{
	case 0:	//xy
		plane[0] = center_ + dir_x_ * 0.1 + dir_y_ * 0.1;
		plane[1] = center_ + dir_x_ * 0.2 + dir_y_ * 0.1;
		plane[2] = center_ + dir_x_ * 0.2 + dir_y_ * 0.2;
		plane[3] = center_ + dir_x_ * 0.1 + dir_y_ * 0.2;
		return;
	case 1:	//xz
		plane[0] = center_ + dir_x_ * 0.1 + dir_z_ * 0.1;
		plane[1] = center_ + dir_x_ * 0.2 + dir_z_ * 0.1;
		plane[2] = center_ + dir_x_ * 0.2 + dir_z_ * 0.2;
		plane[3] = center_ + dir_x_ * 0.1 + dir_z_ * 0.2;
		return;
	case 2: //yz
		plane[0] = center_ + dir_y_ * 0.1 + dir_z_ * 0.1;
		plane[1] = center_ + dir_y_ * 0.2 + dir_z_ * 0.1;
		plane[2] = center_ + dir_y_ * 0.2 + dir_z_ * 0.2;
		plane[3] = center_ + dir_y_ * 0.1 + dir_z_ * 0.2;
		return;
	default:
		plane.clear();
		break;
	}
}

void acamcad::AxisOperation::planeScaleOffset(int p, std::vector<AMCAX::Coord3>& plane) const
{
	plane.resize(4);
	switch (p)
	{
	case 0:	//xy
		plane[0] = center_ + dir_x_ * 0.95 * len_x_ + dir_y_ * 0.95 * len_y_;
		plane[1] = center_ + dir_x_ * 1.05 * len_x_ + dir_y_ * 0.95 * len_y_;
		plane[2] = center_ + dir_x_ * 1.05 * len_x_ + dir_y_ * 1.05 * len_y_;
		plane[3] = center_ + dir_x_ * 0.95 * len_x_ + dir_y_ * 1.05 * len_y_;
		return;
	case 1:	//xz
		plane[0] = center_ + dir_x_ * 0.95 * len_x_ + dir_z_ * 0.95 * len_z_;
		plane[1] = center_ + dir_x_ * 1.05 * len_x_ + dir_z_ * 0.95 * len_z_;
		plane[2] = center_ + dir_x_ * 1.05 * len_x_ + dir_z_ * 1.05 * len_z_;
		plane[3] = center_ + dir_x_ * 0.95 * len_x_ + dir_z_ * 1.05 * len_z_;
		return;
	case 2: //yz
		plane[0] = center_ + dir_y_ * 0.95 * len_y_ + dir_z_ * 0.95 * len_z_;
		plane[1] = center_ + dir_y_ * 1.05 * len_y_ + dir_z_ * 0.95 * len_z_;
		plane[2] = center_ + dir_y_ * 1.05 * len_y_ + dir_z_ * 1.05 * len_z_;
		plane[3] = center_ + dir_y_ * 0.95 * len_y_ + dir_z_ * 1.05 * len_z_;
		return;
	default:
		plane.clear();
		break;
	}
}

void acamcad::AxisOperation::planeCyCleOffset(int p, std::vector<AMCAX::Coord3>& c_plane) const
{
	c_plane.resize(22);
	switch (p)
	{
	case 0:	//xy
		for (size_t i = 0; i < 11; i++)
		{
			c_plane[i] = center_ + 0.9 * (cos(ring[i]) * dir_x_ + sin(ring[i]) * dir_y_);
			c_plane[i + 11] = center_ + cos(ring[i]) * dir_x_ + sin(ring[i]) * dir_y_;
		}
		return;
	case 1:	//xz
		for (int i = 0; i < 11; i++)
		{
			c_plane[i] = center_ + 0.9 * (cos(ring[i]) * dir_z_ + sin(ring[i]) * dir_x_);
			c_plane[i + 11] = center_ + cos(ring[i]) * dir_z_ + sin(ring[i]) * dir_x_;
		}
		return;
	case 2: //yz
		for (int i = 0; i < 11; i++)
		{
			c_plane[i] = center_ + 0.9 * (cos(ring[i]) * dir_y_ + sin(ring[i]) * dir_z_);
			c_plane[i + 11] = center_ + cos(ring[i]) * dir_y_ + sin(ring[i]) * dir_z_;
		}
		return;
	default:
		c_plane.clear();
		break;
	}
}

