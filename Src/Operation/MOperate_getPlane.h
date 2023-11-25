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
#include "MOperate_SelectInfo.h"

namespace acamcad
{

/// <summary>
/// 根据选择信息获取一个工作平面
/// </summary>
class MOperation_getPlane : public MOperate_SingleObject
{
public:
	MOperation_getPlane(const SelectModel& smodle);
	virtual ~MOperation_getPlane() {};

public:

public:
	AMCAX::Coord3 getCenter() { return center_; }
	AMCAX::Coord3 getDirection() { return direction_; }

private:
	AMCAX::Coord3 center_;
	AMCAX::Coord3 direction_;

};

} //namespace acamcad