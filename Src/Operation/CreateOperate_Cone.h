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

class CreateOperate_Cone : public MOperation
{
public:
	CreateOperate_Cone();
	CreateOperate_Cone(const AMCAX::Coord3 & b_center, const AMCAX::Coord3 & axis,
					   double radiusR, double radiusr, double height, double angle);
	CreateOperate_Cone(const AMCAX::Coord3 & b_center, const AMCAX::Coord3 & axis, const AMCAX::Coord3 & xaxis, 
					   double radiusR, double radiusr, double height, double angle);
	virtual ~CreateOperate_Cone();

public:
	
	virtual void operateWithBRep(BRepObject* brep_object);

private:

	size_t segR_, segV_;
	bool is_bottom_;

    // TODO: Replace with MTrapezoidCone when available
	AMCAX::Coord3 center_;
	AMCAX::Coord3 axis_;
	AMCAX::Coord3 xaxis_;
	double radiusR_, radiusr_;
	double height_;
	double angle_;
};

}