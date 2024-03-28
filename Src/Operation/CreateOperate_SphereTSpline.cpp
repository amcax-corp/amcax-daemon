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
#include "CreateOperate_SphereTSpline.h"

#include <tmeshSplineAPI/TMSplineMakeSphere.hpp>

namespace acamcad
{

	CreateOperate_SphereTSpline::CreateOperate_SphereTSpline(const AMCAX::Coord3& center, double radius, size_t rotation_seg, size_t vertical_seg) :
		CreateOperate_Sphere(center, radius, rotation_seg, vertical_seg)
	{
	}

	bool CreateOperate_SphereTSpline::DoOperate(AdapterObject* adapter)
	{
		//brep_object->bRep->getShape() = AMCAX::MakeSphere(AMCAX::Point3(center_[0], center_[1], center_[2]), radius_, angle1_);
		//brep_object->updateDraw();
		AMCAX::TMS::TMSplineMakeSphere sphre(radius_, segR_, segV_);

		adapter->tSpline->setTSpline(sphre.BuildSpline());

		adapter->mesh_OPvert_Backup();
		adapter->meshMove(center_, AMCAX::Point3(center_).Distance(AMCAX::Point3(0.0, 0.0, 0.0)));
		adapter->mesh_Clear_OPvertBackup();

		adapter->updateDraw();

		return true;
	}


}



