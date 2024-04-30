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
#include "MOperate_getDirection.h"

#include <geometry/Geom3TrimmedCurve.hpp>
#include <geometry/Geom3TrimmedSurface.hpp>
#include <geometry/Geom3Line.hpp>
#include <geometry/Geom3Circle.hpp>
#include <geometry/Geom3Plane.hpp>
#include <geometry/Geom3CylindricalSurface.hpp>
#include <topology/TopoEdge.hpp>
#include <topology/TopoFace.hpp>
#include <topology/TopoTool.hpp>

#include <polymeshAPI/MeshTool.hpp>

namespace acamcad
{

	MOperation_getDirection::MOperation_getDirection(const SelectModel& smodle)
		:MOperate_SingleObject(smodle)
	{
	}


	bool MOperation_getDirection::DoOperate(AdapterObject* adapter)
	{
		direction_ = AMCAX::Coord3(0.0, 0.0, 1.0);
		if (s_info_list_.empty())
			return false;
		int s_id = s_info_list_[0].object_subselect_id_;

		if (selmodel_ == SelectModel::EDGE_MODEL)
		{
			//TODO: get More good info;
			switch (adapter->dataType())
			{
			case acamcad::DataType::BREP_TYPE:
			{
				const AMCAX::TopoEdge& e = static_cast<const AMCAX::TopoEdge&>(adapter->bRep->getShapeEdges()[s_id]);

				double cf, cl;
				std::shared_ptr<AMCAX::Geom3Curve> C = AMCAX::TopoTool::Curve(e, cf, cl);
				if (C->Type() == AMCAX::CurveType::TrimmedCurve)
					C = std::dynamic_pointer_cast<AMCAX::Geom3TrimmedCurve>(C)->BasisCurve();

				AMCAX::Direction3 dir;
				if (C->Type() == AMCAX::CurveType::Line)
					dir = std::dynamic_pointer_cast<AMCAX::Geom3Line>(C)->Direction();

				direction_ = AMCAX::Coord3(dir.X(), dir.Y(), dir.Z());
			}
			break;
			case acamcad::DataType::MESH_TYPE:
			{
				int id0, id1;
				AMCAX::SubD::MeshTool::EdgeVertexIndexs(adapter->mesh->getShape(), s_id, id0, id1);
				AMCAX::Point3 p0 = AMCAX::SubD::MeshTool::Position(adapter->mesh->getShape(), id0),
					p1 = AMCAX::SubD::MeshTool::Position(adapter->mesh->getShape(), id1);

				direction_ = (p1.Coord() - p0.Coord()).Normalized();
			}
			break;
			default:
				break;
			}
		}
		else if (selmodel_ == SelectModel::FACE_MODEL)
		{
			//TODO: get More good info;
			switch (adapter->dataType())
			{
			case acamcad::DataType::BREP_TYPE:
			{
				const AMCAX::TopoFace& f = static_cast<const AMCAX::TopoFace&>(adapter->bRep->getShapeFaces()[s_id]);

				std::shared_ptr<AMCAX::Geom3Surface> S = AMCAX::TopoTool::Surface(f);
				if (S->Type() == AMCAX::SurfaceType::TrimmedSurface)
					S = std::dynamic_pointer_cast<AMCAX::Geom3TrimmedSurface>(S)->BasisSurface();

				AMCAX::Frame3 frame;
				if (S->Type() == AMCAX::SurfaceType::Plane)
					frame = std::dynamic_pointer_cast<AMCAX::Geom3Plane>(S)->Position();

				AMCAX::Direction3 dir = frame.Direction();
				direction_ = frame.Direction().Coord();
			}
			break;
			case acamcad::DataType::MESH_TYPE:
			{
				direction_ = AMCAX::SubD::MeshTool::FaceNormal(adapter->mesh->getShape(), s_id).Coord().Normalized();
			}
			break;
			default:
				break;
			}
		}

		return false;
	}

}