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
#include "CreateOperate_Cube.h"
#include <modeling/MakeBox.hpp>

namespace acamcad
{

	CreateOperate_Cube::CreateOperate_Cube() :
		bb_min_(0.0, 0.0, 0.0), bb_max_(1.0, 1.0, 1.0), segX_(2), segY_(2), segZ_(2)
	{
	}

	CreateOperate_Cube::CreateOperate_Cube(const AMCAX::Coord3& bb_min, const AMCAX::Coord3& bb_max, size_t segX, size_t segY, size_t segZ) :
		bb_min_(bb_min), bb_max_(bb_max), segX_(segX), segY_(segY), segZ_(segZ)
	{

	}

	CreateOperate_Cube::~CreateOperate_Cube()
	{
	}


	bool CreateOperate_Cube::DoOperate(AdapterObject* brep_object)
	{
		AMCAX::Point3 bmin(bb_min_.X(), bb_min_.Y(), bb_min_.Z());
		AMCAX::Point3 bmax(bb_max_.X(), bb_max_.Y(), bb_max_.Z());
		AMCAX::MakeBox mkbox(bmin, bmax);

		brep_object->bRep->SetShape(mkbox.Shape());
		brep_object->bRep->updateDraw();

		return true;
	}

}


