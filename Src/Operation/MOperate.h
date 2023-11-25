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
#include "../Object/ObjectDefine.h"
#include "../Core/CoreDefine.h"
#include <vector>

namespace acamcad
{

class MOperation
{
public:
	MOperation();
	MOperation(const SelectModel& smodel);
	virtual ~MOperation() {};

public:

	virtual void operateWithBRep(BRepObject* brep_object);

public:
	virtual MeshOperationType OperationType() { return MeshOperationType::NO_operotion; }

public:
	virtual void setSelectModel(const SelectModel& smodle);

protected:
	SelectModel selmodel_;
};


}// acamcad