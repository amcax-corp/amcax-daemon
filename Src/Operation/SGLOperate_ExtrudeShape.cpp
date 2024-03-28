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
#include "SGLOperate_ExtrudeShape.h"

#include <modeling/MakePrism.hpp>
#include <modeling/CopyShape.hpp>

namespace acamcad
{
	SGLOperate_ExtrudeShape::SGLOperate_ExtrudeShape(const SelectModel& smodle)
		:MOperation(smodle)
	{
	}

	//diff with MeshExtrude
	bool acamcad::SGLOperate_ExtrudeShape::DoOperate(AdapterObject* brep_object)
	{
		if (selmodel_ != SelectModel::FACE_MODEL)
			return false;


		bool result = false;

		switch (brep_object->dataType())
		{
		case DataType::BREP_TYPE:
		{
			if (brep_object->bRep->getShapeFaces().size() == 1)
			{
				result = true;
				AMCAX::TopoShape f = AMCAX::CopyShape(brep_object->bRep->getShapeFaces()[0]);

				AMCAX::MakePrism prim(f, AMCAX::Vector3(dire_.X(), dire_.Y(), dire_.Z()));
				brep_object->bRep->getShape() = prim.Shape();
				brep_object->updateDraw();
			}
		}
		break;
		default:
			break;
		}

		return result;
	}

}