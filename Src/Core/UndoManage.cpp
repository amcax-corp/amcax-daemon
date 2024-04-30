#include "UndoManage.h"
#include <iostream>
#include "../Windows/RibbonViewer.h"
#include <fstream>
#include <iostream>
#include <sstream>

#include <io/ShapeTool.hpp>
#include <polymesh/PolyMeshIO.hpp>
#include <tmeshSpline/TMSplineIO.hpp>

namespace acamcad
{
	UndoManager::UndoManager() :LocalDataManager(), data_index_size_(0), data_size_(0)
	{
	}

	UndoManager::~UndoManager()
	{
	}

	void UndoManager::RecordAddObject(std::vector<AdapterObject*> data)
	{
		nextStep(data, { });

	}

	void UndoManager::RecordDeleteObject(std::vector<AdapterObject*> data)
	{

		nextStep({}, data);
	}

	void UndoManager::RecordModifyObject(std::vector<AdapterObject*> curData, std::vector<AdapterObject*> oldData)
	{
		nextStep(curData, oldData);
	}


	void UndoManager::clearModify()
	{

		//auto_news.clear();
		oldObjects_.clear();
	}


	void UndoManager::RecordModifyBegin(std::vector<AdapterObject*> oldData)
	{
		if (oldObjects_.size() > 0)
		{
			return;
		}

		addBegin(oldData);

	}
	void UndoManager::RecordModifyEnd(std::vector<AdapterObject*> curData)
	{
		if (!curData.empty() && !oldObjects_.empty())
		{
			//Data data{ {} , oldObjects_ };

			addEnd(curData);
			nextStep(oldObjects_);

		}



		oldObjects_.clear();
	}


	void UndoManager::addBegin(const std::vector<AdapterObject*>& oldData)
	{
		for (auto& iter : oldData)
		{
			DataModelHistory data{ 2 * data_index_size_,iter->persistentId(),-1,"" };
			//std::shared_ptr<AdapterObject> object = getObjectInData(iter);
			oldObjects_.push_back(data);
		}
	}


	void UndoManager::addEnd(const std::vector<AdapterObject*>& curData)
	{
		for (auto& iter : curData) {
			//std::shared_ptr<AdapterObject> object = std::make_shared<AdapterObject>(*iter, false);
			//data.curObjects.push_back(std::move(object));
			DataModelHistory data{ 2 * data_index_size_ + 1,iter->persistentId(),iter->dataType(),"" };
			//std::shared_ptr<AdapterObject> object = getObjectInData(iter);
			std::ostringstream fout;

			iter->writeStream(fout);

			data.data = fout.str().c_str();

			oldObjects_.push_back(data);
		}
	}


	void UndoManager::nextStep(std::vector<AdapterObject*> curData, std::vector<AdapterObject*> oldData)
	{
		if (curData.empty() && oldData.empty())
			return;

		oldObjects_.clear();

		addBegin(oldData);
		addEnd(curData);

		//std::vector<DataModelHistory> data;
		//std::vector<std::shared_ptr<AdapterObject>>  curVector, oldVector

		nextStep(oldObjects_);
		oldObjects_.clear();
		//data_.push_back({ curVector,oldVector });
	}

	void UndoManager::nextStep(const std::vector<DataModelHistory>& data)
	{
		if (data_index_size_ != data_size_)
		{
			QSqlQuery query;
			query.prepare("delete from model where step >= ?");
			query.addBindValue(data_index_size_ * 2);
			query.exec();
		}

		execStore(data);

		data_size_ = ++data_index_size_;

		//data_.push_back(data);
		func_();
	}

	bool UndoManager::canUndo()
	{
		return data_index_size_ > 0;
	}

	bool UndoManager::canRedo()
	{
		return data_index_size_ < data_size_;
	}


	void UndoManager::Undo()
	{
		if (!canUndo()) {
			std::cout << "can not Undo!!!" << std::endl;
			return;
		}
		clearModify();


		--data_index_size_;

		QSqlQuery query;
		query.prepare("select persistentId from model where step = ?");
		query.addBindValue(2 * data_index_size_ + 1);
		if (query.exec())
		{
			while (query.next())
			{
				deleteObject_UndoManage(query.value(0).toInt());
			}
		}

		query.prepare("select persistentId from model where step = ?");
		query.addBindValue(2 * data_index_size_);
		if (query.exec())
		{
			while (query.next())
			{
				QSqlQuery query2;
				query2.prepare("select persistentId,type,data from model where persistentId = ? and step < ? and step % 2 = 1 order by step desc limit 1");
				query2.addBindValue(query.value(0).toInt());
				query2.addBindValue(2 * data_index_size_);

				if (query2.exec() && query2.next())
				{
					AdapterObject* object = new AdapterObject;
					object->setPersistentId(query2.value(0).toInt());

					DataType type = (DataType)query2.value(1).toInt();
					object->setDataType(type);
					std::istringstream fin(query2.value(2).toString().toStdString());

					if (object->readStream(fin))
					{
						addObject_UndoManage(object);
					}
					else
					{
						delete object;
					}


				}

				///deleteObject_UndoManage(query.value(0).toInt());
			}
		}

		sortObject_UndoManage();

		func_();

	}
	void UndoManager::Redo()
	{
		if (!canRedo()) {
			std::cout << "can not Redo!!!" << std::endl;
			return;
		}
		clearModify();

		QSqlQuery query;
		query.prepare("select persistentId from model where step = ?");
		query.addBindValue(2 * data_index_size_);
		if (query.exec())
		{
			while (query.next())
			{
				deleteObject_UndoManage(query.value(0).toInt());
			}
		}


		query.prepare("select persistentId,type,data from model where step = ?");
		query.addBindValue(2 * data_index_size_ + 1);
		if (query.exec())
		{
			while (query.next())
			{

				AdapterObject* object = new AdapterObject;
				object->setPersistentId(query.value(0).toInt());

				DataType type = (DataType)query.value(1).toInt();
				object->setDataType(type);

				std::istringstream fin(query.value(2).toString().toStdString());

				if (object->readStream(fin))
				{
					addObject_UndoManage(object);
				}
				else
				{
					delete object;
				}

			}
		}

		++data_index_size_;

		sortObject_UndoManage();


		func_();
	}

	void UndoManager::clearHistory()
	{
		if (data_index_size_ == 0 && data_size_ == 0)
		{
			return;
		}

		data_index_size_ = data_size_ = 0;


		QSqlQuery query;
		query.prepare("delete from model where step >= ?");
		query.addBindValue(data_index_size_ * 2);
		query.exec();


		func_();

	}

	void UndoManager::setFunc(std::function<void(void)> func)
	{
		func_ = func;
	}

	////void UndoManager::getObject()
	//std::shared_ptr<AdapterObject> UndoManager::getObjectInData(AdapterObject* object)
	//{
	//	///std::shared_ptr<AdapterObject> result;

	//	for (auto iter = data_.rbegin(); iter != data_.rend(); iter++)
	//	{
	//		for (auto& iter1 : iter->curObjects)
	//		{
	//			if (iter1->persistentId() == object->persistentId())
	//			{
	//				return iter1;
	//			}
	//		}
	//	}
	//	std::shared_ptr<AdapterObject> result = std::make_shared<AdapterObject>(*object, false);
	//	return result;
	//}

}