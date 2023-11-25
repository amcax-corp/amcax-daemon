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
#include "CreateOperate_Polygon.h"

#include <modeling/MakeFace.hpp>
#include <modeling/MakeWire.hpp>
#include <modeling/MakeEdge.hpp>

#include <topology/TopoFace.hpp>

acamcad::CreateOperate_Polygon::CreateOperate_Polygon(const std::vector<AMCAX::Coord3>& point_list):
	point_list_(point_list)
{
}



acamcad::CreateOperate_Polygon::~CreateOperate_Polygon()
{
}

void acamcad::CreateOperate_Polygon::operateWithBRep(BRepObject *brep_object)
{
	const auto PTrans = [](const AMCAX::Coord3& P) { return AMCAX::Point3(P.X(), P.Y(), P.Z()); };
	const auto VTrans = [](const AMCAX::Coord3& V) { return AMCAX::Vector3(V.X(), V.Y(), V.Z()); };

	std::list<AMCAX::TopoShape> edges;
	for (int i = 1; i < point_list_.size(); ++i)
	{
		edges.push_back(AMCAX::MakeEdge(PTrans(point_list_[i - 1]), PTrans(point_list_[i])));
	}
	edges.push_back(AMCAX::MakeEdge(PTrans(point_list_.back()), PTrans(point_list_.front())));

	AMCAX::MakeWire mkw;
	mkw.Add(edges);

	brep_object->getShape() = AMCAX::MakeFace(mkw.Wire());
	brep_object->updateDraw();
}