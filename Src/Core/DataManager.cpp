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
#include "LocalDataManager.h"

using namespace acamcad;

DataManager::DataManager() : data_persistent_(0)
{
	//local.initHistory();
}

DataManager::~DataManager()
{
	deleteAllObject();
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

AdapterObject* DataManager::getObjectByPersistentID(int id)
{
	if (id < 0) return nullptr;

	AdapterObject* obj = nullptr;
	for (CObjectIter co_it = objects_begin(); co_it != objects_end(); co_it++)
	{
		if ((*co_it)->persistentId() == id)
			return *co_it;
	}
	return obj;
}
int DataManager::getLastPersistentId() {
	if (objects_.empty()) {
		return -1;
	}
	return objects_.back()->persistentId();
}
void DataManager::addObject(AdapterObject* object)
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

AdapterObject* DataManager::copyObject(AdapterObject* object)
{
	if (object == nullptr)
		return nullptr;

	if (object->dataType() != DataType::CUSTOM_TYPE)
	{
		//AdapterObject* brep_object = dynamic_cast<AdapterObject*> (object);
		AdapterObject* new_object = new AdapterObject(*object, true);

		addObject(new_object);
		return new_object;
	}

	return nullptr;
}

void DataManager::deleteObject(AdapterObject* object)
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

void DataManager::deleteAllObject()
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

void DataManager::restoreObject(AdapterObject* object)
{
	object->setPersistentId(data_persistent_); data_persistent_++;
	object->setId(objects_.size());
	objects_.push_back(object);
}


void DataManager::addObject_UndoManage(AdapterObject* object)
{
	AdapterObject* new_object = object;

	new_object->setVisible(true);
	new_object->updateDraw();

	objects_.push_back(new_object);
}


void DataManager::deleteObject_UndoManage(int persistentId)
{
	for (ObjectIter o_it = objects_begin(); o_it != objects_end(); o_it++)
	{
		if ((*o_it)->persistentId() == persistentId)
		{
			AdapterObject* obj = *o_it;
			delete obj;

			objects_.erase(o_it);
			break;
		}
	}

}


void DataManager::sortObject_UndoManage()
{
	int id = 0;
	for (ObjectIter o_it = objects_begin(); o_it != objects_end(); ++o_it)
	{
		(*o_it)->setId(id);
		id++;
	}
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

void DataManager::getSelectedList(std::vector<AdapterObject*>& selected)
{
	for (ObjectIter o_it = objects_begin(); o_it != objects_end(); ++o_it)
	{
		if ((*o_it)->isSelect()) {
			selected.push_back(*o_it);
		}
	}
}