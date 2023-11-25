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

/// <summary>
/// 根据选择信息获取其center
/// </summary>
class MOperation_getCenter : public MOperation
{
public:
	MOperation_getCenter(const SelectModel& smodle);
	virtual ~MOperation_getCenter() {};

public:

	virtual void operateWithBRep(BRepObject* brep_object) override;

public:
	void setOperateId(const SelectInfo& s_info);
	void setSelectInfoCamera(const SelectInfoWithCamera& s_info);

	AMCAX::Coord3 getCenter() { return center_; }

private:
	int object_id_;
	int object_element_id_;

	AMCAX::Coord3 center_;

};

} //namespace acamcad