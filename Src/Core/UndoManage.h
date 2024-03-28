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

namespace acamcad
{

	//class AdapterObject

	class UndoManager
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

	protected:

		virtual	void addObject_UndoManage(AdapterObject* object) = 0;
		virtual void deleteObject_UndoManage(AdapterObject* object) = 0;
		virtual void sortObject_UndoManage() = 0;

	private:
		struct Data
		{
			///DataType type;
			std::list<std::shared_ptr<AdapterObject>> curObjects;
			std::list<std::shared_ptr<AdapterObject>> oldObjects;
		};
		void nextStep(std::vector<AdapterObject*> curData, std::vector<AdapterObject*> oldData);

		void nextStep(const Data& data);


		std::shared_ptr<AdapterObject> getObjectInData(AdapterObject* object);

	protected:

		std::list<Data> data_;
		int data_index_;

		std::list<std::shared_ptr<AdapterObject>> oldObjects_;

		//std::vector<std::shared_ptr<AdapterObject>> auto_olds;
		//std::vector<std::shared_ptr<AdapterObject>> auto_news;
	};

}