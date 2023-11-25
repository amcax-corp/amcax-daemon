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
#include "MOperate_SelectInfo.h"

acamcad::MOperate_SingleObject::MOperate_SingleObject(const SelectModel& smodle) : MOperation(smodle)
{
}

void acamcad::MOperate_SingleObject::setOperateId(int object_id, const std::vector<int>& s_info_list)
{
	s_info_list_.resize(s_info_list.size());
	for (int i = 0; i < s_info_list_.size(); i++)
	{
		s_info_list_[i].object_id_ = object_id;
		s_info_list_[i].object_subselect_id_ = s_info_list[i];
	}
}

void acamcad::MOperate_SingleObject::setselectInfo(const std::vector<SelectInfo>& s_info_list)
{
	s_info_list_ = s_info_list;
}

std::vector<int> acamcad::MOperate_SingleObject::getOperateIdNew() const
{
	std::vector<int> operate_id = getSelectSubList(s_info_list_);
	return operate_id;
}

std::vector<acamcad::SelectInfo> acamcad::MOperate_SingleObject::getSelectInfoNew() const
{
	return s_info_list_;
}
