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

class PlaneObject : BaseObject
{
public:
	PlaneObject();
	PlaneObject(const AMCAX::Coord3& normal);
	PlaneObject(const AMCAX::Coord3& center, const AMCAX::Coord3& normal);

	PlaneObject(const PlaneObject& object);

	virtual ~PlaneObject() {};

public:
	bool pointInPlane(const AMCAX::Coord3& point);

	bool lineInsertPlane(const AMCAX::Coord3& v0, const AMCAX::Coord3& d0, AMCAX::Coord3& v_insert);

	void projectPointToPlane(const AMCAX::Coord3& point, AMCAX::Coord3& project_point);

public:
	AMCAX::Coord3& center() { return center_; }
	const AMCAX::Coord3& center() const { return center_; }	
	void setCenter(const AMCAX::Coord3& c);

	AMCAX::Coord3& normal() { return normal_; }
	const AMCAX::Coord3& normal() const { return normal_; }
	void setNormal(const AMCAX::Coord3& n);
private:
	AMCAX::Coord3 center_;
	AMCAX::Coord3 normal_;
};

} // namespace acamcad