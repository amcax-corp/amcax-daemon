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
#include "CreateOperate_Wedge.h"

using namespace acamcad;

#include <modeling/MakeWedge.hpp>

CreateOperate_Wedge::CreateOperate_Wedge(): type_(TYPE_NONE)
{

}

CreateOperate_Wedge::CreateOperate_Wedge(const double dx, const double dy, const double dz, const double ltx):
    type_(TYPE_ARG_DX_DY_DZ_LTX), axis_(.0, .0, 1.0), dx_(dx), dy_(dy), dz_(dz), ltx_(ltx)
{
    
}

CreateOperate_Wedge::CreateOperate_Wedge(const AMCAX::Coord3 & center,
                                         const double dx, const double dy, const double dz, const double ltx):
    type_(TYPE_ARG_C_DX_DY_DZ_LTX), center_(center), axis_(.0, .0, 1.0), dx_(dx), dy_(dy), dz_(dz), ltx_(ltx)
{

}

CreateOperate_Wedge::CreateOperate_Wedge(const AMCAX::Coord3 & center, const AMCAX::Coord3 & axis,
                                         const double dx, const double dy, const double dz, const double ltx):
    type_(TYPE_ARG_C_A_DX_DY_DZ_LTX), center_(center), axis_(axis), dx_(dx), dy_(dy), dz_(dz), ltx_(ltx)
{

}

CreateOperate_Wedge::CreateOperate_Wedge(const double dx, const double dy, const double dz,
                                         const double xmin, const double zmin, const double xmax, const double zmax):
    type_(TYPE_ARG_DX_DY_DZ_XI_ZI_XA_ZA), axis_(.0, .0, 1.0),
    dx_(dx), dy_(dy), dz_(dz), xmin_(xmin), zmin_(zmin), xmax_(xmax), zmax_(zmax)
{

}

CreateOperate_Wedge::CreateOperate_Wedge(const AMCAX::Coord3 & center,
                                         const double dx, const double dy, const double dz,
                                         const double xmin, const double zmin, const double xmax, const double zmax):
    type_(TYPE_ARG_C_DX_DY_DZ_XI_ZI_XA_ZA), center_(center), axis_(.0, .0, 1.0),
    dx_(dx), dy_(dy), dz_(dz), xmin_(xmin), zmin_(zmin), xmax_(xmax), zmax_(zmax)
{

}

CreateOperate_Wedge::CreateOperate_Wedge(const AMCAX::Coord3 & center, const AMCAX::Coord3 & axis,
                                         const double dx, const double dy,
 const double dz,
                                         const double xmin, const double zmin, const double xmax, const double zmax):
    type_(TYPE_ARG_C_A_DX_DY_DZ_XI_ZI_XA_ZA), center_(center), axis_(axis),
    dx_(dx), dy_(dy), dz_(dz), xmin_(xmin), zmin_(zmin), xmax_(xmax), zmax_(zmax)
{
    
}

CreateOperate_Wedge::~CreateOperate_Wedge()
{

}

void CreateOperate_Wedge::operateWithBRep(BRepObject *brep_object)
{
    AMCAX::Frame3 frame;
    AMCAX::Point3 center(center_.X(), center_.Y(), center_.Z());
    AMCAX::Direction3 dir(axis_.X(), axis_.Y(), axis_.Z());

    if (type_ == TYPE_ARG_C_DX_DY_DZ_LTX || TYPE_ARG_C_DX_DY_DZ_XI_ZI_XA_ZA)
    {
        frame.SetLocation(center);
    } else if (type_ == TYPE_ARG_C_A_DX_DY_DZ_LTX && type_ == TYPE_ARG_C_A_DX_DY_DZ_XI_ZI_XA_ZA)
    {
        frame.SetLocation(center);
        frame.SetDirection(dir);
    }

    AMCAX::TopoShape shape;

    switch (type_)
    {
        case TYPE_ARG_DX_DY_DZ_LTX:
        {
            shape = AMCAX::MakeWedge(dx_, dy_, dz_, ltx_);
        } 
        break;
        case TYPE_ARG_C_DX_DY_DZ_LTX: [[fallthrough]];
        case TYPE_ARG_C_A_DX_DY_DZ_LTX:
        {
            shape = AMCAX::MakeWedge(frame, dx_, dy_, dz_, ltx_);
        }
        break;
        case TYPE_ARG_DX_DY_DZ_XI_ZI_XA_ZA:
        {
            shape = AMCAX::MakeWedge(dx_, dy_, dz_, xmin_, zmin_, xmax_, zmax_);
        }
        break;
        case TYPE_ARG_C_DX_DY_DZ_XI_ZI_XA_ZA: [[fallthrough]];
        case TYPE_ARG_C_A_DX_DY_DZ_XI_ZI_XA_ZA:
        {
            shape = AMCAX::MakeWedge(frame, dx_, dy_, dz_, xmin_, zmin_, xmax_, zmax_);
        }
        break;
        default:
        return;
    }
    brep_object->getShape() = shape;
}
