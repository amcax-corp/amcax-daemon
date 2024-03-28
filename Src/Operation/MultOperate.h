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
	class MultOperate
	{
	public:
		MultOperate();
		MultOperate(const SelectModel& smodel);
		virtual ~MultOperate() {}

	public:

		virtual BaseObject* operate();

	public:
		bool checkOjbectTypeUnanimous();	//uset after setObjectList;

		bool IsSaveOri() { return isSaveOri_; }

		DataType ObjectType() { return objectType_; }

		virtual MeshOperationType OperationType() { return MeshOperationType::NO_operotion; }

	public:
		virtual void reInline();

		virtual void setObjectList(std::vector<AdapterObject*> object_list);

		virtual void setSelectModel(const SelectModel& smodle);

		virtual void setIsSaveOriObject(bool isSave);

	protected:
		bool isSaveOri_ = false;
		SelectModel selmodel_;
		std::vector<AdapterObject*> objectList_;

		DataType objectType_;
	};
}