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
#include <common/CoordT.hpp>

namespace acamcad {

	MOperation_getBoundingBox::MOperation_getBoundingBox(const SelectModel& smodle)
		:MOperate_SingleObject(smodle)
	{
	}

	bool MOperation_getBoundingBox::DoOperate(AdapterObject* brep_object)
	{
		//AMCAX::BoundingBox3 box;
		AMCAX::Coord3 min, max;

		switch (selmodel_)
		{
		case SelectModel::OBJECT_MODEL:
		{

			brep_object->GetBoundingBox(min, max);
		}
		break;
		case SelectModel::FACE_MODEL:
		{
			std::vector<int> s_id_list = getSelectSubList(s_info_list_);
			for (int i = 0; i < s_id_list.size(); i++)
			{
				AMCAX::Coord3 t_min, t_max;
				brep_object->GetBoundingBoxByFace(i, t_min, t_max);
				if (i == 0) {
					min = t_min;
					max = t_max;
				}
				else
				{
					min.SetCoord(std::min(t_min.X(), min.X()), std::min(t_min.Y(), min.X()), std::min(t_min.Z(), min.Z()));
					max.SetCoord(std::max(t_max.X(), max.X()), std::max(t_max.Y(), max.Y()), std::max(t_max.Z(), max.Z()));
				}

				//AMCAX::BRepBoundingBox::AddToBox(brep_object->bRep->getShapeFaces()[s_id_list[i]], box);
			}
		}
		break;
		case SelectModel::EDGE_MODEL:
		{
			std::vector<int> s_id_list = getSelectSubList(s_info_list_);
			for (int i = 0; i < s_id_list.size(); i++)
			{
				AMCAX::Coord3 t_min, t_max;
				brep_object->GetBoundingBoxByEdge(i, t_min, t_max);

				//AMCAX::BRepBoundingBox::AddToBox(brep_object->bRep->getShapeEdges()[s_id_list[i]], box);
				if (i == 0) {
					min = t_min;
					max = t_max;
				}
				else
				{
					min.SetCoord(std::min(t_min.X(), min.X()),
						std::min(t_min.Y(), min.X()), std::min(t_min.Z(), min.Z()));
					max.SetCoord(std::max(t_max.X(),
						max.X()), std::max(t_max.Y(), max.Y()), std::max(t_max.Z(), max.Z()));
				}

			}
		}
		break;
		case SelectModel::VERTEX_MODEL:
		{
			std::vector<int> s_id_list = getSelectSubList(s_info_list_);
			for (int i = 0; i < s_id_list.size(); i++)
			{
				AMCAX::Coord3 t_position;
				brep_object->GetBoundingBoxByVetex(i, t_position);
				if (i == 0) {
					min = max = t_position;
				}
				else
				{
					min.SetCoord(std::min(t_position.X(), min.X()),
						std::min(t_position.Y(), min.X()), std::min(t_position.Z(), min.Z()));
					max.SetCoord(std::max(t_position.X(), max.X()),
						std::max(t_position.Y(), max.Y()), std::max(t_position.Z(), max.Z()));
				}
				//AMCAX::BRepBoundingBox::AddToBox(brep_object->bRep->getShapeVertices()[s_id_list[i]], box);
			}
		}
		break;
		default:
			break;
		}

		bbox_ = AMCAX::BoundingBox3(AMCAX::Point3(min.X(), min.Y(), min.Z()), AMCAX::Point3(max.X(), max.Y(), max.Z()));

		return false;
	}

	void MOperation_getBoundingBox::getBoundingBox(AMCAX::Coord3& bb_min, AMCAX::Coord3& bb_max) const
	{
		bb_min = bbox_.CornerMin().Coord();
		bb_max = bbox_.CornerMax().Coord();
	}


}

