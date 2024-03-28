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

bool acamcad::SGLOperate_FilletEdge::DoOperate(AdapterObject* brep_object)
{
	if (selmodel_ != SelectModel::EDGE_MODEL)
		return false;

	if (s_info_list_.size() < 1) return false;



	std::vector<int> s_id_list = getSelectSubList(s_info_list_);
	bool result = false;
	switch (brep_object->dataType())
	{
	case DataType::BREP_TYPE:
	{
		result = true;
		AMCAX::MakeFillet mkFillet(brep_object->bRep->getShape());
		for (int i = 0; i < s_id_list.size(); ++i)
		{
			const AMCAX::TopoEdge& edge = static_cast<const AMCAX::TopoEdge&>(brep_object->bRep->getShapeEdges()[s_id_list[i]]);
			mkFillet.Add(0.1, 0.1, edge);
		}
		brep_object->bRep->getShape() = mkFillet.Shape();
		brep_object->updateDraw();
	}
	break;
	default:
		break;
	}

	return result;


}
