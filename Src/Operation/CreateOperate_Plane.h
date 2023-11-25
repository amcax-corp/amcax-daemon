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
#include <common/CoordT.hpp>
namespace acamcad
{

class CreateOperate_Plane : public MOperation
{
public:
	CreateOperate_Plane();
	CreateOperate_Plane(const AMCAX::Coord2& p0, const AMCAX::Coord2& p1, size_t u_seg = 2, size_t v_seg = 2);
	virtual ~CreateOperate_Plane();

public:

	virtual void operateWithBRep(BRepObject* brep_object) override;

private:
	AMCAX::Coord2 p0_, p1_;
	size_t segU_, segV_;

};

}