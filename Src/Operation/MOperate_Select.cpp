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
#include "MOperate_Select.h"

#include <topology/TopoVertex.hpp>
#include <topology/TopoEdge.hpp>
#include <topology/TopoFace.hpp>
#include <topology/TopoExplorer.hpp>
#include <topology/TopoTool.hpp>
#include <math/PolygonOnTriangularMesh.hpp>
#include <math/TriangularMesh.hpp>
#include "../Utils/MathUtils.h"

using namespace acamcad;

MOperation_Select::MOperation_Select(const SelectModel& smodle)
	:MOperate_SelectInfoCamera(smodle)
{
}

bool acamcad::MOperation_Select::DoOperate(AdapterObject* adapter)
{

	switch (selmodel_)
	{
	case acamcad::SelectModel::VERTEX_MODEL:
	case acamcad::SelectModel::EDGE_MODEL:
		//case acamcad::SelectModel::FACE_MODEL:
	{
		void (AdapterObject:: * ptr[3])(const MVector3 & begin, const MVector3 & end, int f_id, int& se_id) {
			&AdapterObject::selectVertWithFaceId, & AdapterObject::selectEdgeWithFaceId
		};

		AMCAX::Vector3 begin(s_info_.c_begin_),
			end(s_info_.c_end_);

		(adapter->*ptr[(size_t)selmodel_])(begin, end, s_info_.object_subpart_id_, s_info_.object_subselect_id_);

	}
	break;
	case acamcad::SelectModel::FACE_MODEL:
		s_info_.object_subselect_id_ = s_info_.object_subpart_id_;
		break;
	default:
		break;
	}

	return true;

}
