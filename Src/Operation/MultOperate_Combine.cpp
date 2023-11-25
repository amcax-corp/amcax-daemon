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
#include "MultOperate_Combine.h"

#include <topology/TopoBuilder.hpp>
#include <topology/TopoCompound.hpp>

acamcad::MultOperate_Combine::MultOperate_Combine(const SelectModel& smodel)
	:MultOperate(smodel)
{
}


acamcad::BaseObject* acamcad::MultOperate_Combine::operateWithBRep()
{
	BRepObject* newObject = new BRepObject();
	newObject->getShape() = AMCAX::TopoCompound();

	AMCAX::TopoBuilder TB;
	TB.MakeCompound(static_cast<AMCAX::TopoCompound&>(newObject->getShape()));
	for (int i = 0; i < objectList_.size(); ++i)
	{
		BRepObject* bobj = dynamic_cast<BRepObject*>(objectList_[i]);
		TB.Add(newObject->getShape(), bobj->getShape());
	}

	newObject->updateDraw();
	return newObject;
}
