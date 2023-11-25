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

//TODO, remove MSphere; 

class CreateOperate_Sphere : public MOperation
{
public:
	CreateOperate_Sphere();
	CreateOperate_Sphere(const AMCAX::Coord3& center, double radius, size_t rotation_seg, size_t vertical_seg); 
	CreateOperate_Sphere(const AMCAX::Coord3& center, double radius, double angle1, double angle2, double angle3);

	virtual ~CreateOperate_Sphere();

public:

	virtual void operateWithBRep(BRepObject* brep_object);

private:

	AMCAX::Coord3 center_; 
	double radius_; 

	size_t segR_, segV_;
    double angle1_, angle2_, angle3_;
};

}
