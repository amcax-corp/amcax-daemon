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
#include "Record.h"

#include <string>
#include <vector>
#include <algorithm>
#include<cstdlib>
#include <sstream>

namespace acamcad
{
    Record::Step::Step(int stepNum, int object_id, std::string action, std::string lable) :stepNum(stepNum),object_id(object_id),action(action), lable(lable)
    {
    }
    Record::Record() {

    }
    Record::~Record() {

    }
   
    bool startsWith(const std::string& base, const std::string& sub)
    {
        return base.rfind(sub, 0) == 0;
    }

    std::vector<double> parsexyz(const std::string base) {
        std::vector<double> xyz;
        int nPos = 0;
        std::string str = base;
        nPos = str.find("(");
        str = str.substr(nPos + 1);
        nPos = str.find(")");
        str = str.substr(0,nPos);
        nPos = str.find(",");
        while (-1 != nPos)
        {
            std::string temp = str.substr(0,nPos);
            double d = 0;
            d = std::stold(temp);
            xyz.emplace_back(d);
            str = str.substr(nPos + 1);
            nPos = str.find(",");
        }
        xyz.emplace_back(std::stold(str));
        return xyz;
    }

    void Record::recordCylinder(const int object_id, const AMCAX::Coord3 & b_center, const AMCAX::Coord3 & axis, const AMCAX::Coord3 & xaxis,double radius, double height, double angle,const std::string& label) {
        if (object_id <= 0) return; 

        steps.push_back(Record::Step(steps.size() + 1, object_id, BREP_PRIM_API_MAKE_CYLINDER, label));
        object_num++;
    }
    void Record::recordCone(const int object_id, const AMCAX::Coord3 & b_center, const AMCAX::Coord3 & axis, const AMCAX::Coord3 & xaxis,double radiusR, double radiusr, double height,double angle, const std::string& label) {
        if (object_id <= 0) return;

        steps.push_back(Record::Step(steps.size() + 1, object_id, BREP_PRIM_API_MAKE_CONE, label));
        object_num++;
    }
    void Record::recordCube(const int object_id, const AMCAX::Coord3 & first, const AMCAX::Coord3 & second, const std::string& label) {
        if (object_id <= 0) return;

        steps.push_back(Record::Step(steps.size() + 1 , object_id, BREP_PRIM_API_MAKE_CUBE, label));
        object_num++;
    }
    void Record::recordTorus(const int object_id, const AMCAX::Coord3 & center, const AMCAX::Coord3 & axis, const AMCAX::Coord3 & xaxis, double radius0, double radius1, double angle, const std::string& label) {
        if (object_id <= 0) return;

        steps.push_back(Record::Step(steps.size() + 1, object_id, BREP_PRIM_API_MAKE_TORUS, label));
        object_num++;
    }
    void Record::recordSphere(const int object_id, const AMCAX::Coord3 & center, double radius,double angle1, double angle2, double angle3,const std::string& label) {
        if (object_id <= 0) return;

        steps.push_back(Record::Step(steps.size() + 1, object_id, BREP_PRIM_API_MAKE_SPHERE, label));
        object_num++;
    }
    void Record::recordPlane(const int object_id, const AMCAX::Coord3 & first, const AMCAX::Coord3 & second, const std::string& label) {
        if (object_id <= 0) return;

        steps.push_back(Record::Step(steps.size() + 1, object_id, BREP_PRIM_API_MAKE_PLANE, label));
        object_num++;
        
    }
    void Record::recordPolygon(const int object_id, const std::vector<AMCAX::Coord3d>& points, const std::string& label) {
        if (object_id <= 0) return;

        steps.push_back(Record::Step(steps.size() + 1, object_id, BREP_BUILDER_API_MAKE_POLYGON, label));
    }
    void Record::recordPrism(const int object_id, const std::vector<AMCAX::Coord3>& points, const AMCAX::Coord3& prism_axis, const std::string& label) {
        if (object_id <= 0) return;

        steps.push_back(Record::Step(steps.size() + 1, object_id, BREP_PRIM_API_MAKE_PRISM, label));
        object_num++;
    }
    int Record::getStepByObject(int id) {
        int object_id = id;
        int stepId = 0;
        for (int i = steps.size() - 1; i >= 0; i--)
        {
            if (steps[i].object_id == object_id)
            {
                stepId = steps[i].stepNum;
                break;
            }
        }
        return stepId;
    }
    void Record::recordMove(const int object_id, const AMCAX::Coord3d & vec){
        if (object_id <= 0) return;

        steps.push_back(Record::Step(steps.size() + 1, object_id, BREP_BUILDER_API_TRANSFORM, "move"));
    }

    void Record::recorRotation(const int object_id, const AMCAX::Coord3 & center, const AMCAX::Coord3 & axis,const double angle ) {
        if (object_id <= 0) return;

        steps.push_back(Record::Step(steps.size() + 1, object_id, BREP_BUILDER_API_ROTATION, "rotation"));
    }

    void Record::recordDelete(BaseObject* obj) {
        steps.push_back(Record::Step(steps.size() + 1, obj->id(), BREP_ALGO_API_DELETE, "delete"));
    }
    void Record::recordFuse(const int object_id, std::vector<int> persistent_id_list) {
        if (object_id <= 0) return;

        std::vector<int> stepList;
        for (int id : persistent_id_list)
        {
            stepList.emplace_back(getStepByObject(id));
        }
        steps.push_back(Record::Step(steps.size() + 1, object_id, BREP_ALGO_API_FUSE, "fuse"));
        object_num++;
    }
    void Record::recordCommon(const int object_id, std::vector<int> persistent_id_list) {
        if (object_id <= 0) return;

        std::vector<int> stepList;
        for (int id : persistent_id_list)
        {
            stepList.emplace_back(getStepByObject(id));
        }

        steps.push_back(Record::Step(steps.size() + 1, object_id, BREP_ALGO_API_COMMON, "common"));
        object_num++;
    }
    void Record::recordCut(const int object_id, std::vector<int> persistent_id_list) {
        if (object_id <= 0) return;

        std::vector<int> stepList;
        for (int id : persistent_id_list)
        {
            stepList.emplace_back(getStepByObject(id));
        }

        steps.push_back(Record::Step(steps.size() + 1, object_id, BREP_ALGO_API_CUT, "cut"));
        object_num++;
    }
}