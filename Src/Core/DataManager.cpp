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
#include "DataManager.h"
#include "../Object/BaseObject.h"

using namespace acamcad;

DataManager::DataManager() : data_persistent_(0)
{

}

DataManager::~DataManager()
{
	if (objects_.size() != 0)
	{
		for (ObjectIter o_it = objects_begin(); o_it != objects_end(); ++o_it)
		{
			BaseObject* obj = *o_it;
			delete obj;
		}
	}
	objects_.clear();
}


//============================================================
//                  Object
//============================================================

ObjectIter DataManager::objects_begin()
{
	return objects_.begin();
}

ObjectRIter DataManager::objects_rbegin()
{
	return objects_.rbegin();
}

ObjectIter DataManager::objects_end()
{
	return objects_.end();
}

ObjectRIter DataManager::objects_rend()
{
	return objects_.rend();
}

CObjectIter DataManager::objects_begin() const
{
	return objects_.cbegin();
}

CObjectRIter DataManager::objects_rbegin() const
{
	return objects_.crbegin();
}

CObjectIter DataManager::objects_end() const
{
	return objects_.cend();
}

CObjectRIter DataManager::objects_rend() const
{
	return objects_.crend();
}

BaseObject* DataManager::getObjectByPersistentID(int id)
{
	if (id < 0) return nullptr;

	BaseObject* obj = nullptr;
	for (CObjectIter co_it = objects_begin(); co_it != objects_end(); co_it++)
	{
		if ((*co_it)->persistentId() == id)
			return *co_it;
	}
	return obj;
}
int  DataManager::getLastPersistentId() {
	if (objects_.empty()) {
		return -1; 
	}
	return objects_.back() -> persistentId();
}
void DataManager::addObject(BaseObject* object)
{
	object->setPersistentId(data_persistent_); data_persistent_++;
	object->setId(objects_.size());
	objects_.push_back(object);
}

void DataManager::deleteObject(ObjectIter& it)
{
	BaseObject* obj = *it;
	delete obj;

	objects_.erase(it);
	int id = 0;
	for (ObjectIter o_it = objects_begin(); o_it != objects_end(); ++o_it)
	{
		(*o_it)->setId(id);
		id++;
	}
}

BaseObject* DataManager::copyObject(BaseObject* object)
{
	if (object == nullptr)
		return nullptr;

	if (object->dataType() == DataType::BREP_TYPE)
	{
		BRepObject* brep_object = dynamic_cast<BRepObject*> (object);
		BRepObject* new_object  = new BRepObject(*brep_object);

		addObject(new_object);
		return new_object;
	}

	return nullptr;
}

void DataManager::deleteObject(BaseObject* object)
{
	for (ObjectIter o_it = objects_begin(); o_it != objects_end(); o_it++)
	{
		if ((*o_it)->persistentId() == object->persistentId())
		{
			deleteObject(o_it);
			return;
		}
	}
}

void DataManager::restoreObject(BaseObject* object)
{
	object->setPersistentId(data_persistent_); data_persistent_++;
	object->setId(objects_.size());
	objects_.push_back(object);
}

//============================================================
//                  Object Info
//============================================================

void DataManager::clearSelected()
{
	for (ObjectIter o_it = objects_begin(); o_it != objects_end(); ++o_it)
	{
		(*o_it)->setSelect(false);
		(*o_it)->setSelectSub(false);
	}
}

void DataManager::getSelectedList(std::vector<BaseObject*>& selected)
{
	for (ObjectIter o_it = objects_begin(); o_it != objects_end(); ++o_it)
	{
		if ((*o_it)->isSelect()) {
			selected.push_back(*o_it); 
		}
	}
}