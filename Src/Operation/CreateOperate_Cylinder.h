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
#include "MOperate.h"

namespace acamcad
{

class CreateOperate_Cylinder : public MOperation
{
public:
	CreateOperate_Cylinder();
	CreateOperate_Cylinder(const AMCAX::Coord3& bottomCenter, double radius, double height, const AMCAX::Coord3& xaxis, double angle);
	CreateOperate_Cylinder(const AMCAX::Coord3& bottomCenter, double radius, double height, const AMCAX::Coord3& axis, const AMCAX::Coord3& xaxis, double angle);

	virtual ~CreateOperate_Cylinder();

public:

	virtual void operateWithBRep(BRepObject *brep_object);

private:

	AMCAX::Coord3 bottomCenter_;
	double radius_;
	double height_;

	AMCAX::Coord3 axis_;
	AMCAX::Coord3 xaxis_;

	double angle_;
};

}