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
#include "CreateOperate_SingleFace.h"

#include <modeling/MakeFace.hpp>
#include <modeling/MakeWire.hpp>
#include <modeling/MakeEdge.hpp>


acamcad::CreateOperate_SingleFace::CreateOperate_SingleFace(const std::vector<AMCAX::Coord3>& point_list) :
	point_list_(point_list)
{
}

acamcad::CreateOperate_SingleFace::~CreateOperate_SingleFace()
{
	point_list_.clear();
}


bool acamcad::CreateOperate_SingleFace::DoOperate(acamcad::AdapterObject* new_adapter)
{
	auto PTrans = [](const AMCAX::Coord3& P) { return AMCAX::Point3(P.X(), P.Y(), P.Z()); };

	//AdapterObject* new_adapter = new AdapterObject;
	//new_adapter->setDataType(acamcad::DataType::BREP_TYPE);
	//new_adapter->bRep = std::make_unique<BRepObject>(new_adapter);

	///BRepObject* new_object = new BRepObject();
	new_adapter->bRep->assignBRep();

	std::list<AMCAX::TopoShape> edges;
	for (int i = 1; i < point_list_.size(); ++i)
	{
		edges.push_back(AMCAX::MakeEdge(PTrans(point_list_[i - 1]), PTrans(point_list_[i])));
	}
	edges.push_back(AMCAX::MakeEdge(PTrans(point_list_.back()), PTrans(point_list_.front())));

	AMCAX::MakeWire mkw;
	mkw.Add(edges);

	new_adapter->bRep->getShape() = AMCAX::MakeFace(mkw.Wire());
	new_adapter->bRep->updateDraw();

	return true;
	///return new_adapter;
}