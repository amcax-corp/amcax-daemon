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

#include <vector>
#include "../Object/ObjectDefine.h"
#include "UndoManage.h"

namespace acamcad
{
	class AdapterObject;
	typedef std::vector<acamcad::AdapterObject*>::iterator ObjectIter;
	typedef std::vector<acamcad::AdapterObject*>::const_iterator CObjectIter;
	typedef std::vector<acamcad::AdapterObject*>::reverse_iterator ObjectRIter;
	typedef std::vector<acamcad::AdapterObject*>::const_reverse_iterator CObjectRIter;

	class DataManager :public UndoManager
	{
	public:
		DataManager();
		~DataManager();

	public:
		ObjectIter objects_begin();
		ObjectRIter objects_rbegin();
		ObjectIter objects_end();
		ObjectRIter objects_rend();

		CObjectIter objects_begin() const;
		CObjectRIter objects_rbegin() const;
		CObjectIter objects_end() const;
		CObjectRIter objects_rend() const;

		AdapterObject* getObjectByPersistentID(int o_id);	//use object persistent to get the object
		int  getLastPersistentId();

		void addObject(AdapterObject* object);		//正常的添加单元
		void deleteObject(ObjectIter& it);

		AdapterObject* copyObject(AdapterObject* object);
		void deleteObject(AdapterObject* object);

		void deleteAllObject();
		void restoreObject(AdapterObject* object);	//恢复一个被删除的单元，在备份的时候使用

		//UndoManager
		virtual	void addObject_UndoManage(AdapterObject* object)  override;
		virtual void deleteObject_UndoManage(int persistentId) override;
		virtual void sortObject_UndoManage() override;


	public:
		void clearSelected();
		void getSelectedList(std::vector<AdapterObject*>& selected);

	private:
		std::vector<AdapterObject*> objects_;
	private:
		int data_persistent_;
	};


} //namespace acamcad