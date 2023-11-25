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

#include <vector>
#include <common/CoordT.hpp>

namespace acamcad
{

class AxisOperation
{
	friend class AxisOperateDraw;

public:
	AxisOperation();
	AxisOperation(const AMCAX::Coord3& center);
	AxisOperation(const AMCAX::Coord3& center, const AMCAX::Coord3& dir_z);
	AxisOperation(const AMCAX::Coord3& center, const AMCAX::Coord3& dir_x, const AMCAX::Coord3& dir_y);
	AxisOperation(const AMCAX::Coord3& center, const AMCAX::Coord3& dir_x, const AMCAX::Coord3& dir_y, const AMCAX::Coord3& dir_z);

	~AxisOperation();

public:
	AMCAX::Coord3 operateMovePart(const AMCAX::Coord3& mouse_vector, double length);

	AMCAX::Coord3 operateScalePart(const AMCAX::Coord3& mouse_vector, double length);
	double operateScalePart_scale();

	AMCAX::Coord3 operateRotation(double& u);

public:
	const AMCAX::Coord3& center() const { return center_; }
	void setCenter(const AMCAX::Coord3& c) { center_ = c; }

	const AMCAX::Coord3& dirX() const { return dir_x_; }
	const AMCAX::Coord3& dirY() const { return dir_y_; }
	const AMCAX::Coord3& dirZ() const { return dir_z_; }

public:
	void setLenX(double len) { len_x_ = len; }
	void setLenY(double len) { len_y_ = len; }
	void setLenZ(double len) { len_z_ = len; }

	double lenX() const { return len_x_; }
	double lenY() const { return len_y_; }
	double lenZ() const { return len_z_; }

	void reSetLen();

	void setSelectPart(int spart) { select_part_ = spart; }
	int selectPart() const { return select_part_; }

private:
	AMCAX::Coord3 center_;
	AMCAX::Coord3 dir_x_;
	AMCAX::Coord3 dir_y_;
	AMCAX::Coord3 dir_z_;

	double len_x_;
	double len_y_;
	double len_z_;

	/*On move model
	* 0:center, 1:x, 2:y, 3:z, 4:xy, 5:xz, 6:yz
	* On Scale model
	* 1:x, 2:y, 3:z, other:NULL
	*/
	int select_part_;

public:
	void draw(const std::string& draw_type);
	void drawForSelect(const std::string& draw_type);

private:
	AxisOperateDraw* draw_tool;

public:
	AMCAX::Coord3 centerOffset(int d) const;
	AMCAX::Coord3 centerFinal(int d) const;
	void planeOffset(int p, std::vector<AMCAX::Coord3>& plane) const;
	void planeScaleOffset(int p, std::vector<AMCAX::Coord3>& plane) const;
	void planeCyCleOffset(int p, std::vector<AMCAX::Coord3>& c_plane) const;

};

} //namespace acamcad