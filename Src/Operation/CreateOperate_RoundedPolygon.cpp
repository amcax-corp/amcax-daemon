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
#include "CreateOperate_RoundedPolygon.h"

#include <memory>
#include <vector>
#include <list>

#include <modeling/MakeFace.hpp>
#include <modeling/MakeWire.hpp>
#include <modeling/MakeEdge.hpp>

#include <geometry/Geom3TrimmedCurve.hpp>
#include <geometry/MakeSegment.hpp>
#include <geometry/MakeArcOfCircle.hpp>

#include <common/VectorT.hpp>

#include <topology/TopoWire.hpp>
#include <topology/TopoFace.hpp>

using namespace acamcad;

CreateOperate_RoundedPolygon::CreateOperate_RoundedPolygon(const std::vector<AMCAX::Coord3>& point_list):
    MOperation(), point_list_(point_list)
{
    if (point_list_.size() % 2)
    { // make it even
        point_list_.pop_back();
    }
}

CreateOperate_RoundedPolygon::~CreateOperate_RoundedPolygon()
{
}

void CreateOperate_RoundedPolygon::operateWithBRep(BRepObject *brep_object)
{
    const auto MTpo = [](const AMCAX::Coord3 &mp) -> AMCAX::Point3 {
        return AMCAX::Point3(mp[0], mp[1], mp[2]);
    };
    std::vector<AMCAX::Point3> points(point_list_.size());
    std::transform(point_list_.begin(), point_list_.end(), points.begin(), MTpo);
    if (points.size() >= 4)
    {
        std::vector<std::shared_ptr<AMCAX::Geom3TrimmedCurve>> segments;
        std::vector<std::shared_ptr<AMCAX::Geom3TrimmedCurve>> arcs;
        for (int i = 1; i < points.size(); i+=2)
        {
            segments.push_back(AMCAX::MakeSegment(points[i-1], 
                                                  points[i]));
            arcs.push_back(AMCAX::MakeArcOfCircle(points[i], 
                                                  AMCAX::Vector3(points[i-1], points[i]), 
                                                  points[(i+1)%points.size()]));
        }
        std::list<AMCAX::TopoShape> edges;
        for (int i = 0; i < segments.size() && i < arcs.size(); ++i)
        {
            edges.push_back(AMCAX::MakeEdge(segments[i]));
            edges.push_back(AMCAX::MakeEdge(arcs[i]));
        }
        AMCAX::MakeWire makeWire;
        makeWire.Add(edges);
        AMCAX::TopoWire wire = makeWire.Wire();

        brep_object->getShape() = AMCAX::MakeFace(wire);
        brep_object->updateDraw();
    }
}
