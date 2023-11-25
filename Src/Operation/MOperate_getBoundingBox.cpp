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
#include "MOperate_getBoundingBox.h"
#include <common/BoundingBox3.hpp>
#include <topology/BRepBoundingBox.hpp>

namespace acamcad {

	MOperation_getBoundingBox::MOperation_getBoundingBox(const SelectModel& smodle)
		:MOperate_SingleObject(smodle)
	{
	}

	void MOperation_getBoundingBox::operateWithBRep(BRepObject* brep_object)
	{
		AMCAX::BoundingBox3 box;
		if (selmodel_ == SelectModel::OBJECT_MODEL)
		{
			AMCAX::BRepBoundingBox::AddToBox(brep_object->getShape(), box);
		}
		else if (selmodel_ == SelectModel::FACE_MODEL)
		{
			std::vector<int> s_id_list = getSelectSubList(s_info_list_);
			for (int i = 0; i < s_id_list.size(); i++)
			{
				AMCAX::BRepBoundingBox::AddToBox(brep_object->getShapeFaces()[s_id_list[i]], box);
			}
		}
		else if (selmodel_ == SelectModel::EDGE_MODEL)
		{
			std::vector<int> s_id_list = getSelectSubList(s_info_list_);
			for (int i = 0; i < s_id_list.size(); i++)
			{
				AMCAX::BRepBoundingBox::AddToBox(brep_object->getShapeEdges()[s_id_list[i]], box);
			}
		}
		else if (selmodel_ == SelectModel::VERTEX_MODEL)
		{
			std::vector<int> s_id_list = getSelectSubList(s_info_list_);
			for (int i = 0; i < s_id_list.size(); i++)
			{
				AMCAX::BRepBoundingBox::AddToBox(brep_object->getShapeVertices()[s_id_list[i]], box);
			}
		}

		bbox_ = AMCAX::BoundingBox3(box.CornerMin(), box.CornerMax()); 

	}

	void MOperation_getBoundingBox::getBoundingBox(AMCAX::Coord3& bb_min, AMCAX::Coord3& bb_max) const
	{
		bb_min = bbox_.CornerMin().Coord(); 
		bb_max = bbox_.CornerMax().Coord();
	}


}

