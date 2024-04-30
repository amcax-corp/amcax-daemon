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
#include "SGLOperate_ChamferEdge.h"

#include <topology/TopoEdge.hpp>
#include <fillet/MakeChamfer.hpp>
#include <tmeshSplineAPI/TMSplineSampleChamfer.hpp>

using namespace acamcad;

SGLOperate_ChamferEdge::SGLOperate_ChamferEdge(const SelectModel& smodle)
	:MOperate_SingleObject(smodle)
{
}

bool acamcad::SGLOperate_ChamferEdge::DoOperate(AdapterObject* brep_object)
{
	if (selmodel_ != SelectModel::EDGE_MODEL)
		return false;

	if (s_info_list_.size() < 1) return false;

	bool result = false;
	std::vector<int> s_id_list = getSelectSubList(s_info_list_);

	switch (brep_object->dataType())
	{
	case acamcad::DataType::BREP_TYPE:
	{
		AMCAX::MakeChamfer mkChamfer(brep_object->bRep->getShape());
		for (int i = 0; i < s_id_list.size(); ++i)
		{
			const AMCAX::TopoEdge& edge = static_cast<const AMCAX::TopoEdge&>(brep_object->bRep->getShapeEdges()[s_id_list[i]]);
			mkChamfer.Add(0.1, edge);
		}
		mkChamfer.SetMode(AMCAX::ChamferMode::ClassicChamfer);

		brep_object->bRep->getShape() = mkChamfer.Shape();

		brep_object->updateDraw();

		result = true;
	}
	break;
	case acamcad::DataType::TSPLINEU_TYPE:
	{
		AMCAX::TMS::TMSplineSampleChamfer tool(0.1);

		result = tool.CanEdgesSampleChamfer(brep_object->tSpline->getShape(), s_id_list);

		if (result) {
			tool.SampleChamfer(brep_object->tSpline->getShape(), s_id_list);
			brep_object->updateDraw();
		}
	}
	break;
	default:
		break;
	}

	return result;



}
