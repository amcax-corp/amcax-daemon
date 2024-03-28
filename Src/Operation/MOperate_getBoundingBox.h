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
#include <common/BoundingBox3.hpp>

namespace acamcad
{
	/// <summary>
	/// 根据选择信息获取其当前的BoundingBox
	/// </summary>
	class MOperation_getBoundingBox : public MOperate_SingleObject
	{
	public:
		MOperation_getBoundingBox(const SelectModel& smodle);
		virtual ~MOperation_getBoundingBox() {};

	public:

		virtual bool DoOperate(AdapterObject* brep_object) override;

	public:
		AMCAX::BoundingBox3 getBoundingBox() const { return bbox_; }
		void getBoundingBox(AMCAX::Coord3& bb_min, AMCAX::Coord3& bb_max) const;

	private:
		AMCAX::BoundingBox3 bbox_;
	};

} //namespace acamcad