#include "UndoManage.h"
#include <iostream>

namespace acamcad
{
	UndoManager::UndoManager() :data_index_(0)
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

		for (auto& iter : oldData) {
			std::shared_ptr<AdapterObject> object = getObjectInData(iter);
			oldObjects_.push_back(std::move(object));
		}

	}
	void UndoManager::RecordModifyEnd(std::vector<AdapterObject*> curData)
	{
		if (!curData.empty() && !oldObjects_.empty())
		{
			Data data{ {} , oldObjects_ };

			for (auto& iter : curData) {
				std::shared_ptr<AdapterObject> object = std::make_shared<AdapterObject>(*iter, false);
				data.curObjects.push_back(std::move(object));
			}
			nextStep(data);
		}

		oldObjects_.clear();
	}


	void UndoManager::nextStep(std::vector<AdapterObject*> curData, std::vector<AdapterObject*> oldData)
	{
		if (curData.empty() && oldData.empty())
			return;

		Data data;
		//std::vector<std::shared_ptr<AdapterObject>>  curVector, oldVector;

		for (auto& iter : curData) {
			std::shared_ptr<AdapterObject> object = std::make_shared<AdapterObject>(*iter, false);
			data.curObjects.push_back(std::move(object));
		}

		for (auto& iter : oldData) {
			std::shared_ptr<AdapterObject> object = getObjectInData(iter);
			data.oldObjects.push_back(std::move(object));
		}

		nextStep(data);
		//data_.push_back({ curVector,oldVector });
	}

	void UndoManager::nextStep(const Data& data)
	{
		if (data_index_ < data_.size()) {

			auto iter = data_.begin();

			for (int i = 0; i < data_index_; i++) {
				iter++;
			}

			data_.erase(iter, data_.end());

		}

		data_index_++;

		data_.push_back(data);
	}

	bool UndoManager::canUndo()
	{
		return data_index_ > 0;
	}
	bool UndoManager::canRedo()
	{
		return data_index_ < data_.size();
	}


	void UndoManager::Undo()
	{
		if (!canUndo()) {
			std::cout << "can not Undo!!!" << std::endl;
			return;
		}
		clearModify();


		--data_index_;

		//int index = 0;
		auto iter = data_.begin();

		for (int i = 0; i < data_index_; i++) {
			iter++;
		}

		for (auto cur = iter->curObjects.begin(); cur != iter->curObjects.end(); cur++) {
			deleteObject_UndoManage(cur->get());
		}

		for (auto old = iter->oldObjects.begin(); old != iter->oldObjects.end(); old++)
		{
			addObject_UndoManage(old->get());
		}

		sortObject_UndoManage();

	}
	void UndoManager::Redo()
	{
		if (!canRedo()) {
			std::cout << "can not Redo!!!" << std::endl;
			return;
		}
		clearModify();
		//int index = 0;
		auto iter = data_.begin();

		for (int i = 0; i < data_index_; i++) {
			iter++;
		}

		for (auto cur = iter->curObjects.begin(); cur != iter->curObjects.end(); cur++) {
			addObject_UndoManage(cur->get());
		}

		for (auto old = iter->oldObjects.begin(); old != iter->oldObjects.end(); old++)
		{
			deleteObject_UndoManage(old->get());
		}

		sortObject_UndoManage();
		++data_index_;


	}

	//void UndoManager::getObject()
	std::shared_ptr<AdapterObject> UndoManager::getObjectInData(AdapterObject* object)
	{
		///std::shared_ptr<AdapterObject> result;

		for (auto iter = data_.rbegin(); iter != data_.rend(); iter++)
		{
			for (auto& iter1 : iter->curObjects)
			{
				if (iter1->persistentId() == object->persistentId())
				{
					return iter1;
				}
			}
		}
		std::shared_ptr<AdapterObject> result = std::make_shared<AdapterObject>(*object, false);
		return result;
	}

}