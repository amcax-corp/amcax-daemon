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
#include "MOperate_getCenter.h"
#include <topology/BRepBoundingBox.hpp>

acamcad::MOperation_getCenter::MOperation_getCenter(const SelectModel& smodle)
	:MOperation(smodle)
{
}



void acamcad::MOperation_getCenter::operateWithBRep(BRepObject* brep_object)
{
	AMCAX::BoundingBox3 box;
	if (selmodel_ == SelectModel::OBJECT_MODEL)
		AMCAX::BRepBoundingBox::AddToBox(brep_object->getShape(), box);
	else if (selmodel_ == SelectModel::FACE_MODEL)
		AMCAX::BRepBoundingBox::AddToBox(brep_object->getShapeFaces()[object_element_id_], box);
	else if (selmodel_ == SelectModel::EDGE_MODEL)
		AMCAX::BRepBoundingBox::AddToBox(brep_object->getShapeEdges()[object_element_id_], box);
	else if (selmodel_ == SelectModel::VERTEX_MODEL)
		AMCAX::BRepBoundingBox::AddToBox(brep_object->getShapeVertices()[object_element_id_], box);

	AMCAX::Coord3 c(box.CornerMin().Coord() * 0.5 + box.CornerMax().Coord() * 0.5);
	center_ = c;
}

void acamcad::MOperation_getCenter::setOperateId(const SelectInfo& s_info)
{
	object_id_ = s_info.object_id_;
	object_element_id_ = s_info.object_subselect_id_;
}

void acamcad::MOperation_getCenter::setSelectInfoCamera(const SelectInfoWithCamera& s_info)
{
	object_id_ = s_info.object_id_;
	object_element_id_ = s_info.object_subselect_id_;
}
