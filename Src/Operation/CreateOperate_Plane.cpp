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
#include "CreateOperate_Plane.h"
#include <modeling/MakeFace.hpp>
#include <math/Plane.hpp>

namespace acamcad
{

	CreateOperate_Plane::CreateOperate_Plane() :
		p0_(0.0), p1_(1.0, 1.0), segU_(2), segV_(2)
	{

	}

	CreateOperate_Plane::CreateOperate_Plane(const AMCAX::Coord2& p0, const AMCAX::Coord2& p1, size_t u_seg, size_t v_seg) :
		p0_(p0), p1_(p1), segU_(u_seg), segV_(v_seg)
	{
	}

	CreateOperate_Plane::~CreateOperate_Plane()
	{
	}

	bool CreateOperate_Plane::DoOperate(AdapterObject* brep_object)
	{

		double xmin = p0_.X(), xmax = p1_.Y();
		double ymin = p0_.Y(), ymax = p1_.Y();

		brep_object->bRep->getShape() = AMCAX::MakeFace(AMCAX::Plane(), xmin, xmax, ymin, ymax);
		brep_object->bRep->updateDraw();

		return true;
	}

}

