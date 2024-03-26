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
#pragma once
#include "ObjectDraw.h"
#include "../Object/BRepObject.h"
#include <unordered_map>

#include <math/TriangularMesh.hpp>
#include <math/PolygonOnTriangularMesh.hpp>

namespace acamcad
{
    class BRepObjectDraw : public ObjectDraw
    {
    public:
        BRepObjectDraw(BaseObject* object);
        virtual ~BRepObjectDraw();

    public:
        virtual void draw(const DrawModel& dmodel, const SelectModel& smodel) const override;

        virtual void drawSelected(const SelectModel& smodel, int d_id) const override;

    public:
        virtual void drawForSelect() const override;
        virtual void drawForSelectPart() const override;

    public:
        virtual void updatedrawState();

    private:
        void draw(const DrawModel& dmodel, bool is_show_point) const;

    private:
        void draw_mesh_wireframe(bool is_selected, bool showPoint) const;

        void draw_mesh_solidflat(bool is_selected) const;

        void draw_mesh_smooth(bool is_selected) const;

        void draw_curve() const;

    private:
        BRepObject* brep_object_;

        std::vector<double> vert_position_;
        std::vector<double> vert_normal_;
        std::vector<std::vector<int>> face_vertices_;
        std::vector<std::vector<int>> edge_vertices_;

        struct FaceInfo
        {
            AMCAX::OrientationType ori;
            std::shared_ptr<AMCAX::TriangularMesh> triMesh;
            AMCAX::TopoLocation loc;
        };
        struct EdgeInfo
        {
            std::shared_ptr<AMCAX::TriangularMesh> triMesh;
            std::shared_ptr<AMCAX::PolygonOnTriangularMesh> polyMesh;
            AMCAX::TopoLocation loc;
        };
        std::vector<FaceInfo> faceInfos_;
        std::vector<EdgeInfo> edgeInfos_;
        std::vector<AMCAX::Point3> curvePointsInfo_;

    };
} //namespace acamcad