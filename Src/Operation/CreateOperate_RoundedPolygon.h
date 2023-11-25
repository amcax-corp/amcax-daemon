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
#ifndef CREATEOPERATE_ROUNDEDPOLYGON_H
#define CREATEOPERATE_ROUNDEDPOLYGON_H

#include "MOperate.h"

namespace acamcad
{

class CreateOperate_RoundedPolygon: public MOperation
{
public:
    CreateOperate_RoundedPolygon(const std::vector<AMCAX::Coord3>& point_list);
    ~CreateOperate_RoundedPolygon();

public:
    virtual void operateWithBRep(BRepObject *brep_object) override;

private:
    std::vector<AMCAX::Coord3> point_list_;

};

    
    
} // namespace acamcad


#endif // CREATEOPERATE_ROUNDEDPOLYGON_H