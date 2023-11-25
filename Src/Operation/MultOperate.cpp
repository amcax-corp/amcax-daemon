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
#include "MultOperate.h"

namespace acamcad
{
	MultOperate::MultOperate()
		:selmodel_(SelectModel::OBJECT_MODEL)
	{
	}

	MultOperate::MultOperate(const SelectModel& smodel)
		:selmodel_(smodel)
	{
	}

	BaseObject* MultOperate::operateWithBRep()
	{
	//	Massage::Error("MultOperate::operateWithSpline -- this type can not do this operation");
		return nullptr;
	}

	bool MultOperate::checkOjbectTypeUnanimous()
	{
		if (objectList_.size() < 2)
			return false;

		for (int i = 1; i < objectList_.size(); i++)
		{
			if (objectList_[i] == nullptr)
				return false;
		}

		objectType_ = objectList_.front()->dataType();
		for (int i = 1; i < objectList_.size(); i++)
		{
			if (objectList_[i]->dataType() != objectType_)
				return false;
		}

		return true;
	}

	void MultOperate::reInline()
	{
		objectList_.clear();
		selmodel_ = SelectModel::OBJECT_MODEL;
		isSaveOri_ = false;
		objectType_ = DataType::UNKNOW_TYPE;
	}

	void MultOperate::setObjectList(std::vector<BaseObject*> object_list)
	{
		objectList_ = object_list;
	}

	void MultOperate::setSelectModel(const SelectModel& smodle)
	{
		selmodel_ = smodle;
	}

	void MultOperate::setIsSaveOriObject(bool isSave)
	{
		isSaveOri_ = isSave;
	}
}