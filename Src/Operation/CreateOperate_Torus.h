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

	class CreateOperate_Torus : public MOperation
	{
	public:
		CreateOperate_Torus();

		CreateOperate_Torus(const AMCAX::Coord3& center, double radiusMajor, double radiusMinor, const AMCAX::Coord3& axis, double angle);
		CreateOperate_Torus(const AMCAX::Coord3& center, double radiusMajor, double radiusMinor, const AMCAX::Coord3& axis, const AMCAX::Coord3& xaxis, double angle);

		virtual ~CreateOperate_Torus();

	public:
		virtual bool DoOperate(AdapterObject* brep_object) override;

	private:
		AMCAX::Coord3 center_; double radiusMajor_; double radiusMinor_;

		AMCAX::Coord3 axis_;
		AMCAX::Coord3 xaxis_;
		size_t segR_, segV_;

		double angle_;
	};

}
