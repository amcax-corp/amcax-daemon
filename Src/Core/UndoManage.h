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
#include <list>
#include <memory>

#include "../Object/ObjectDefine.h"
#include "LocalDataManager.h"

//class RibbonViewer;

namespace acamcad
{

	//class AdapterObject

	class UndoManager :public LocalDataManager
	{
	public:
		UndoManager();
		~UndoManager();



		void RecordAddObject(std::vector<AdapterObject*> data);
		void RecordDeleteObject(std::vector<AdapterObject*> data);
		void RecordModifyObject(std::vector<AdapterObject*> curData, std::vector<AdapterObject*> oldData);

		//bool isEmptyAuto();
		//void RecordAutoBegin(std::vector<AdapterObject*> datas);
		//void RecordAutoEnd(std::vector<AdapterObject*> datas);

		void clearModify();

		void RecordModifyBegin(std::vector<AdapterObject*> oldData);
		void RecordModifyEnd(std::vector<AdapterObject*> curData);


		bool canUndo();
		bool canRedo();

		void Undo();
		void Redo();

		void clearHistory();

		void setFunc(std::function<void(void)> func);

	protected:

		void addBegin(const std::vector<AdapterObject*>& oldData);
		void addEnd(const std::vector<AdapterObject*>& curData);

		virtual	void addObject_UndoManage(AdapterObject* object) = 0;
		virtual void deleteObject_UndoManage(int persistentId) = 0;
		virtual void sortObject_UndoManage() = 0;

	private:
		//struct Data
		//{
		//	///DataType type;
		//	std::list<std::shared_ptr<AdapterObject>> curObjects;
		//	std::list<std::shared_ptr<AdapterObject>> oldObjects;
		//};
		void nextStep(std::vector<AdapterObject*> curData, std::vector<AdapterObject*> oldData);

		void nextStep(const std::vector<DataModelHistory>& data);


		//std::shared_ptr<AdapterObject> getObjectInData(AdapterObject* object);

	protected:

		//std::list<Data> data_;
		int data_index_size_;
		int data_size_;

		std::vector<DataModelHistory> oldObjects_;
		std::function<void(void)> func_;

		//std::vector<std::shared_ptr<AdapterObject>> auto_olds;
		//std::vector<std::shared_ptr<AdapterObject>> auto_news;
	};

}