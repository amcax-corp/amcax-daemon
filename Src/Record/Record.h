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
#include <vector>
#include <string>

#include <algorithm>
#include <common/CoordT.hpp>
#include "../Object/BaseObject.h"
#define BREP_PRIM_API_MAKE_PLANE "BRepPrimAPI_MakePlane"
#define BREP_PRIM_API_MAKE_CUBE "BRepPrimAPI_MakeBox"
#define BREP_PRIM_API_MAKE_SPHERE "BRepPrimAPI_MakeSphere"
#define BREP_PRIM_API_MAKE_CYLINDER "BRepPrimAPI_MakeCylinder"
#define BREP_PRIM_API_MAKE_CONE "BRepPrimAPI_MakeCone"
#define BREP_PRIM_API_MAKE_TORUS "BRepPrimAPI_MakeTorus"
#define BREP_PRIM_API_MAKE_PRISM "BRepPrimAPI_MakePrism"
#define BREP_PRIM_API_MAKE_ROUNDEDPRISM "BRepPrimAPI_MakeRoundedPrism"
#define BREP_PRIM_API_MAKE_WEDGE "BRepPrimAPI_MakeWedge"
#define BREP_BUILDER_API_MAKE_EDGE "BRepBuilderAPI_MakeEdge"
#define BREP_BUILDER_API_MAKE_WIRE "BRepBuilderAPI_MakeWire"
#define BREP_BUILDER_API_MAKE_FACE "BRepBuilderAPI_MakeFace"
#define BREP_BUILDER_API_MAKE_POLYGON "BRepBuilderAPI_MakePolygon"
#define BREP_BUILDER_API_TRANSFORM "BRepBuilderAPI_Transform"
#define BREP_BUILDER_API_ROTATION "BRepBuilderAPI_Rotation"
#define BREP_ALGO_API_DELETE "BRepAlgoAPI_Delete"
#define BREP_ALGO_API_FUSE "BRepAlgoAPI_Fuse"
#define BREP_ALGO_API_COMMON "BRepAlgoAPI_Common"
#define BREP_ALGO_API_CUT "BRepAlgoAPI_Cut"
#define JSON_KEYWORD_ACTION "action"
#define JSON_KEYWORD_STEP "step"
#define JSON_KEYWORD_ID "id"
#define JSON_KEYWORD_DESCRIPTION "description"
#define JSON_KEYWORD_ARGS "args"
#define JSON_KEYWORD_LABEL "lable"
#define BREP_BUILDER_API_TRANSFORM_SHAPE_PREFIX_L "shape"
#define BREP_BUILDER_API_TRANSFORM_SHAPE_PREFIX_U "Shape"
#define BREP_BUILDER_API_TRANSFORM_SHAPES_PREFIX_L "shapes"
#define BREP_BUILDER_API_TRANSFORM_SHAPES_PREFIX_U "Shapes"

#define JSON_KEYWORD_SHAPE "shape"
#define JSON_KEYWORD_VECTOR "vec"
#define JSON_KEYWORD_VECTOR_X "vector_x"
#define JSON_KEYWORD_VECTOR_Y "vector_y"
#define JSON_KEYWORD_VECTOR_Z "vector_z"

namespace acamcad
{

    class Record
    {
    public:
        struct Step
        {
            int stepNum;
            int object_id;
            std::string action;
            std::string lable;
            Step(int stepNum, int object_id,std::string action, std::string lable);
        };
    private:
        std::vector<Record::Step> steps;
        int object_num = 0;
    public:
        void recordCylinder(const int id, const AMCAX::Coord3 & b_center, const AMCAX::Coord3 & axis, const AMCAX::Coord3 & xaxis, double radius, double height, double angle, const std::string& label);
        void recordCone(const int id, const AMCAX::Coord3 & b_center, const AMCAX::Coord3 & axis, const AMCAX::Coord3 & xaxis, double radiusR, double radiusr, double height, double angle, const std::string& label);
        void recordCube(const int id, const AMCAX::Coord3 & first, const AMCAX::Coord3 & second, const std::string& label);
        void recordTorus(const int id, const AMCAX::Coord3 & center, const AMCAX::Coord3 & axis, const AMCAX::Coord3 & xaxis, double radius0, double radius1, double angle, const std::string& label);
        void recordSphere(const int id, const AMCAX::Coord3 & center, double radius,double angle1, double angle2, double angle3,const std::string& label);
        void recordPlane(const int id, const AMCAX::Coord3 & first, const AMCAX::Coord3 & second, const std::string& label);
        void recordPolygon(const int id, const std::vector<AMCAX::Coord3d>& points, const std::string& label);
        void recordPrism(const int id, const std::vector<AMCAX::Coord3>& points, const AMCAX::Coord3& prism_axis, const std::string& label);
        int getStepByObject(int id);
        void recordMove(const int id, const AMCAX::Coord3d & vec);
        void recorRotation(const int object_id, const AMCAX::Coord3 & center, const AMCAX::Coord3 & axis, const double angle);
        void recordDelete(BaseObject* obj);
        void recordFuse(const int object_id, std::vector<int> persistent_id_list);
        void recordCommon(const int object_id, std::vector<int> persistent_id_list);
        void recordCut(const int object_id, std::vector<int> persistent_id_list);

    public:
        Record();
        ~Record();

    };
}