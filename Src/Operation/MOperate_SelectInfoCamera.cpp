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
#include "MOperate_SelectInfoCamera.h"

acamcad::MOperate_SelectInfoCamera::MOperate_SelectInfoCamera(const SelectModel& smodle) : MOperation(smodle)
{
}

void acamcad::MOperate_SelectInfoCamera::setOperateId(const SelectInfoWithCamera& s_info)
{
	s_info_ = s_info;
}

acamcad::SelectInfoWithCamera acamcad::MOperate_SelectInfoCamera::getSelectInfoNew() const
{
	return s_info_;
}
