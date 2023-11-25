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

void acamcad::MOperation_Select::operateWithBRep(BRepObject* brep_object)
{

	s_info_.object_subselect_id_ = -1;
	if (selmodel_ == SelectModel::VERTEX_MODEL)
	{
		AMCAX::TopoVertex vMin;
		double min_dis = std::numeric_limits<double>::max();
		AMCAX::Coord3 ve_min;

		AMCAX::TopoShape f = brep_object->getShapeFaces()[s_info_.object_subpart_id_];
		for (AMCAX::TopoExplorer expV(f, AMCAX::ShapeType::Vertex); expV.More(); expV.Next())
		{
			AMCAX::TopoVertex v = static_cast<const AMCAX::TopoVertex&>(expV.Current());
			AMCAX::Point3 p = AMCAX::TopoTool::Point(v);
			double len = MathUtils::distPointLineSquared(p.Coord(), s_info_.c_begin_, s_info_.c_end_, ve_min);
			if (min_dis > len)
			{
				min_dis = len;
				vMin = v;
			}
		}

		if (min_dis < 0.01) 
			s_info_.object_subselect_id_ = brep_object->getShapeVertices().index(vMin);

		return;
	}
	else if (selmodel_ == SelectModel::EDGE_MODEL)
	{
		AMCAX::TopoEdge eMin;
		double min_dis = std::numeric_limits<double>::max();
		AMCAX::Coord3 ve_min(0.0, 0.0, 0.0), vL_min(0.0, 0.0, 0.0);

		AMCAX::TopoShape f = brep_object->getShapeFaces()[s_info_.object_subpart_id_];
		for (AMCAX::TopoExplorer expE(f, AMCAX::ShapeType::Edge); expE.More(); expE.Next())
		{
			const AMCAX::TopoEdge& e = static_cast<const AMCAX::TopoEdge&>(expE.Current());
			AMCAX::TopoLocation loc;
			std::shared_ptr<AMCAX::TriangularMesh> triMesh;
			std::shared_ptr<AMCAX::PolygonOnTriangularMesh> polyMesh;
			AMCAX::TopoTool::PolygonOnTriangulation(e, polyMesh, triMesh, loc);
			const AMCAX::Transformation3& tr = loc.Transformation();
			for (int pid = 1; pid < polyMesh->NVertices(); ++pid)
			{
				const AMCAX::Point3& p0 = triMesh->Vertex(polyMesh->Vertex(pid - 1)).Transformed(tr);
				const AMCAX::Point3& p1 = triMesh->Vertex(polyMesh->Vertex(pid)).Transformed(tr);

				double len = MathUtils::distLine_SLineSquared(p0.Coord(), p1.Coord(), s_info_.c_begin_, s_info_.c_end_, ve_min, vL_min);
				if (len < min_dis)
				{
					min_dis = len;
					eMin = e;
				}
			}
		}

		if (min_dis < 0.01)
			s_info_.object_subselect_id_ = brep_object->getShapeEdges().index(eMin);

		return;
	}
	else if (selmodel_ == SelectModel::FACE_MODEL)
	{
		s_info_.object_subselect_id_ = s_info_.object_subpart_id_;
	}
}
