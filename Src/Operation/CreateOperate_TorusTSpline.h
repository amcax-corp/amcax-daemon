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

	class CreateOperate_TorusTSpline : public MOperation
	{
	public:
		//CreateOperate_Torus();

		CreateOperate_TorusTSpline(const AMCAX::Coord3& center, const AMCAX::Coord3& axis, double radiusMajor, double radiusMinor, size_t segMajor, size_t segMinor);
		//CreateOperate_Torus(const AMCAX::Coord3& center, double radiusMajor, double radiusMinor, const AMCAX::Coord3& axis, const AMCAX::Coord3& xaxis, double angle);

		//virtual ~CreateOperate_Torus();

	public:
		virtual bool DoOperate(AdapterObject* brep_object) override;

	private:
		AMCAX::Coord3 center_, axis_;
		double radiusMajor_, radiusMinor_;
		size_t segMajor_, segMinor_;
	};

}
