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
#include "CreateOperate_PlaneTSpline.h"

#include <tmeshSplineAPI/TMSplineMakeRectangle.hpp>
#include "../Object/TSplineObject.h"


namespace acamcad
{

	CreateOperate_PlaneTSpline::CreateOperate_PlaneTSpline(const AMCAX::Coord2& p0, const AMCAX::Coord2& p1, size_t u_seg, size_t v_seg) :
		CreateOperate_Plane(p0, p1, u_seg, v_seg)
	{
	}

	bool CreateOperate_PlaneTSpline::DoOperate(AdapterObject* adapter)
	{
		//double xmin = p0_.X(), xmax = p1_.Y();
		//double ymin = p0_.Y(), ymax = p1_.Y();
		AMCAX::Point2 p0(p0_.X(), p0_.Y()), p1(p1_.X(), p1_.Y());

		AMCAX::TMS::TMSplineMakeRectangle mesh(p0, p1, segU_, segV_);

		//AMCAX::SubD::MeshMakeRectangle mesh(p0, p1, segU_, segV_);


		adapter->tSpline->setShape(mesh.BuildSpline());
		//brep_object->bRep->getShape() = AMCAX::MakeFace(AMCAX::Plane(), xmin, xmax, ymin, ymax);
		//brep_object->bRep->updateDraw();

		adapter->updateDraw();

		return adapter->tSpline->getShape();
	}

}