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

namespace acamcad
{
	class CreateOperate
	{
	public:
		CreateOperate();
		virtual ~CreateOperate() {};

	public:

		virtual BaseObject* operateWithBRep();

	public:
		virtual MeshOperationType OperationType() { return MeshOperationType::NO_operotion; }

		void setObjectType(DataType type) { objectType_ = type; }

	protected:
		SelectModel selmodel_;
		DataType objectType_;
	};


}// acamcad