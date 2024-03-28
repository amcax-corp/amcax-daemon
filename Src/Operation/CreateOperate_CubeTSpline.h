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
#include "CreateOperate_Cube.h"

namespace acamcad
{

	class CreateOperate_CubeTSpline : public CreateOperate_Cube
	{
	public:
		//CreateOperate_Cube();
		CreateOperate_CubeTSpline(const AMCAX::Coord3& bb_min, const AMCAX::Coord3& bb_max, size_t segX = 2, size_t segY = 2, size_t segZ = 2);
		//virtual ~CreateOperate_CubeMesh();

	public:

		virtual bool DoOperate(AdapterObject* brep_object) override;


	};

}