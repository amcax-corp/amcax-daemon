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
#include "SGLOperate_FilletEdge.h"

#include <topology/TopoEdge.hpp>
#include <fillet/MakeFillet.hpp>

acamcad::SGLOperate_FilletEdge::SGLOperate_FilletEdge(const SelectModel& smodle)
	:MOperate_SingleObject(smodle)
{
}

void acamcad::SGLOperate_FilletEdge::operateWithBRep(BRepObject* brep_object)
{
	if (selmodel_ != SelectModel::EDGE_MODEL)
		return;

	if (s_info_list_.size() < 1) return;

	std::vector<int> s_id_list = getSelectSubList(s_info_list_);

	AMCAX::MakeFillet mkFillet(brep_object->getShape());
	for (int i = 0; i < s_id_list.size(); ++i)
	{
		const AMCAX::TopoEdge& edge = static_cast<const AMCAX::TopoEdge&>(brep_object->getShapeEdges()[s_id_list[i]]);
		mkFillet.Add(0.1, 0.1, edge);
	}

	brep_object->getShape() = mkFillet.Shape();
}
