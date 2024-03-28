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

#include <polymesh/PolyMesh.hpp>
#include <polymeshAPI/MeshTool.hpp>

acamcad::MOperation_getCenter::MOperation_getCenter(const SelectModel& smodle)
	:MOperation(smodle)
{
}



bool acamcad::MOperation_getCenter::DoOperate(AdapterObject* adapter)
{

	switch (selmodel_)
	{
	case acamcad::SelectModel::VERTEX_MODEL:
	{
		AMCAX::Coord3  position;
		adapter->GetBoundingBoxByVetex(object_element_id_, position);

		center_ = position;
	}
	break;
	case acamcad::SelectModel::EDGE_MODEL:
	{
		AMCAX::Coord3  min, max;
		adapter->GetBoundingBoxByEdge(object_element_id_, min, max);
		center_ = (min + max) / 2;
	}
	break;
	case acamcad::SelectModel::FACE_MODEL:
	{
		AMCAX::Coord3  min, max;

		adapter->GetBoundingBoxByFace(object_element_id_, min, max);
		center_ = (min + max) / 2;
	}
	break;
	case acamcad::SelectModel::OBJECT_MODEL:
	{
		AMCAX::Coord3  min, max;
		adapter->GetBoundingBox(min, max);
		center_ = (min + max) / 2;
	}
	break;
	default:
		break;
	}

	return false;


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
