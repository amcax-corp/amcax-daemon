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
#include "MultOperate_BoolCommon.h"
#include <boolean/BoolBRepCommon.hpp>

acamcad::MultOperate_BoolCommon::MultOperate_BoolCommon(const SelectModel& smodel)
	:MultOperate(smodel)
{
}

acamcad::BaseObject* acamcad::MultOperate_BoolCommon::operate()
{
	if (objectType_ != DataType::BREP_TYPE)
		return nullptr;


	std::vector<AMCAX::TopoShape> shapeVector(objectList_.size());
	for (int i = 0; i < objectList_.size(); ++i)
	{
		shapeVector[i] = dynamic_cast<AdapterObject*>(objectList_[i])->bRep->getShape();
	}

	AMCAX::TopoShape resultShape = AMCAX::BoolBRepCommon(shapeVector[0], shapeVector[1]);
	for (int i = 2; i < shapeVector.size(); ++i)
		resultShape = AMCAX::BoolBRepCommon(resultShape, shapeVector[i]);

	AdapterObject* adapter_object = new AdapterObject;
	adapter_object->setDataType(acamcad::DataType::BREP_TYPE);
	//adapter_object->bRep = std::make_unique<BRepObject>(adapter_object);

	//BRepObject* newObject = new BRepObject();
	adapter_object->bRep->getShape() = resultShape;

	adapter_object->setLabel(objectList_[0]->label());

	adapter_object->bRep->updateDraw();

	return adapter_object;
}
