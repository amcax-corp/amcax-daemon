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
#include "CreateOperate_Prism.h"

#include <modeling/MakeFace.hpp>
#include <modeling/MakeWire.hpp>
#include <modeling/MakeEdge.hpp>
#include <modeling/MakePrism.hpp>

#include <topology/TopoFace.hpp>


acamcad::CreateOperate_Prism::CreateOperate_Prism(const std::vector<AMCAX::Coord3>& point_list,
											      const AMCAX::Coord3&prism_vec):
	point_list_(point_list), prism_vec_(prism_vec)
{
}

acamcad::CreateOperate_Prism::~CreateOperate_Prism()
{
}

void acamcad::CreateOperate_Prism::operateWithBRep(BRepObject *brep_object)
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

	AMCAX::TopoFace base = AMCAX::MakeFace(mkw.Wire());
	brep_object->getShape() = AMCAX::MakePrism(base, VTrans(prism_vec_));
	brep_object->updateDraw();
}