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

namespace acamcad
{
typedef std::vector<acamcad::BaseObject*>::iterator ObjectIter;
typedef std::vector<acamcad::BaseObject*>::const_iterator CObjectIter;
typedef std::vector<acamcad::BaseObject*>::reverse_iterator ObjectRIter;
typedef std::vector<acamcad::BaseObject*>::const_reverse_iterator CObjectRIter;

class DataManager
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

	BaseObject* getObjectByPersistentID(int o_id);	//use object persistent to get the object
	int  getLastPersistentId();

	void addObject(BaseObject* object);		//正常的添加单元
	void deleteObject(ObjectIter& it);

	BaseObject* copyObject(BaseObject* object);
	void deleteObject(BaseObject* object);

	void restoreObject(BaseObject* object);	//恢复一个被删除的单元，在备份的时候使用

public:
	void clearSelected();
	void getSelectedList(std::vector<BaseObject*>& selected); 

private:
	std::vector<BaseObject*> objects_;
private:
	int data_persistent_;
};


} //namespace acamcad