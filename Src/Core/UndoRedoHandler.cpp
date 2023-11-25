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
#include "UndoRedoHandler.h"
#include "../Object/BRepObject.h"
#include "DataManager.h"
#include <modeling/CopyShape.hpp>
#include "../Object/ObjectType.h"
#include <iostream>

namespace acamcad {

	ContextHolder::ContextHolder(int opId): operationId_(opId), objectPid_(-1) {
		backshape_.Nullify(); 
	}

	ContextHolder::ContextHolder(int opId, BaseObject* object, ActionType actionType): operationId_(opId), objectPid_(-1) {
		if (nullptr == object || object->dataType() != DataType::BREP_TYPE) {
			return; 
		}

		objectPid_ = object->persistentId();

		BRepObject* object_old = dynamic_cast<BRepObject*> (object);
		backshape_ = AMCAX::CopyShape(object_old->getShape());
	}

	ContextHolder::~ContextHolder()
	{
		backshape_.Nullify(); 
	}


	UndoRedoHandler::UndoRedoHandler(DataManager* dataManager, int max_queue_size) : dataManager_(dataManager), max_queue_size_(max_queue_size), operation_seq_(0)
	{
	}

	UndoRedoHandler::~UndoRedoHandler() {
		while (!undo_queue_.empty()) {
			OperateContext item = undo_queue_.back();
			undo_queue_.pop_back();
			releaseQueueItem(item);
		}

		while (!redo_queue_.empty()) {
			OperateContext item = redo_queue_.back();
			redo_queue_.pop_back();
			releaseQueueItem(item);
		}
	}

	void UndoRedoHandler::increaseSequence()
	{
		operation_seq_++;
	}

	void UndoRedoHandler::recordOperation(BaseObject* object, ActionType actionType, bool incSeq)
	{
		if (incSeq) {
			operation_seq_++; 
		}

		ContextHolder* holder = new ContextHolder(operation_seq_, object, actionType); 
		OperateContext context = std::make_pair(actionType, holder); 
		undo_queue_.push_back(context); 

		if (undo_queue_.size() >= max_queue_size_) {
			OperateContext item = undo_queue_.back();
			undo_queue_.pop_front();
			releaseQueueItem(item); 
		}
	}

	void UndoRedoHandler::revokeOp(const OperateContext& context)
	{
		ActionType op_type = context.first;
		ContextHolder* p_holder = context.second; 

		if (p_holder == nullptr) {
			return; 
		}

		if (op_type == ActionType::AddObject) {		//revoke add, need to find the object in data manager and delete it. 
			std::cout << "undo add object" << std::endl;

			BaseObject* obj = dataManager_->getObjectByPersistentID(p_holder->getObjectPid());
			if (obj == nullptr) {
				//Object gone, cannot revoke add operation; 
				return; 
			}
			dataManager_->deleteObject(obj); 
		}
		else if (op_type == ActionType::DeleteObject) { // revoke delete, need to restore the object from shape and add into data manager; and add another copy
			std::cout << "undo delete object" << std::endl;

			BaseObject* obj = dataManager_->getObjectByPersistentID(p_holder->getObjectPid());
			if (obj != nullptr) {
				return;			//Object already exists, no need to revoke delete now. 
			}
			//ONLY support bRep object now; 
			BRepObject* brep_obj = new BRepObject();
			brep_obj->applyBRep(p_holder->getBackShape());
			dataManager_->addObject(brep_obj); 
			brep_obj->setPersistentId(p_holder->getObjectPid());
			brep_obj->updateDraw();
		}
		else {		//revoke operate, need to find the backup data and the object in data manager, apply the old data to the object; 

		}
	}

	void UndoRedoHandler::applyOp(const OperateContext& context)
	{
		ActionType actionType = context.first;
		ContextHolder* p_holder = context.second;

		if (p_holder == nullptr) {
			return;
		}

		if (actionType == ActionType::AddObject) {		//redo add, need to restore the object from shape and add into data manager; 
			std::cout << "redo add object" << std::endl; 
			BaseObject* obj = dataManager_->getObjectByPersistentID(p_holder->getObjectPid());
			if (obj != nullptr) {
				return;			//Object already exists, no need to revoke delete now. 
			}
			//ONLY support bRep object now; 
			BRepObject* brep_obj = new BRepObject();
			brep_obj->applyBRep(p_holder->getBackShape());
			dataManager_->addObject(brep_obj);
			brep_obj->setPersistentId(p_holder->getObjectPid());
			brep_obj->updateDraw();
		}
		else if (actionType == ActionType::DeleteObject) {		//redo delete, need to find the object in data manager and delete it. 
			std::cout << "redo delete object" << std::endl;

			BaseObject* obj = dataManager_->getObjectByPersistentID(p_holder->getObjectPid());
			if (obj == nullptr) {
				//Object gone, cannot revoke add operation; 
				return;
			}
			dataManager_->deleteObject(obj);
		}

	}

	void UndoRedoHandler::undo()
	{
		if (undo_queue_.empty()) {
			return; 
		}

		bool hasNext = false; 
		do {
			OperateContext context = undo_queue_.back();
			int currentSeq = (context.second)->getOperationId();
			revokeOp(context);

			undo_queue_.pop_back(); 
			redo_queue_.push_back(context);
			if (redo_queue_.size() >= max_queue_size_) {

				OperateContext item = redo_queue_.front();
				redo_queue_.pop_front();
				releaseQueueItem(item);
			}

			if (undo_queue_.empty()) {
				return; 
			}
			OperateContext next_context = undo_queue_.back();
			int nextSeq = (next_context.second)->getOperationId();
			hasNext = (nextSeq == currentSeq);
		} while (hasNext);
	
	}

	void UndoRedoHandler::redo()
	{
		
		if (redo_queue_.empty()) {
			return;
		}

		bool hasNext = false;
		do {
			OperateContext context = redo_queue_.back();
			int currentSeq = (context.second)->getOperationId();
			applyOp(context);

			redo_queue_.pop_back();
			undo_queue_.push_back(context);
			if (undo_queue_.size() >= max_queue_size_) {

				OperateContext item = undo_queue_.front();
				undo_queue_.pop_front();
				releaseQueueItem(item);
			}

			if (redo_queue_.empty()) {
				return;
			}
			OperateContext next_context = redo_queue_.back();
			int nextSeq = (next_context.second)->getOperationId();
			hasNext = (nextSeq == currentSeq);
		} while (hasNext);
	}

	bool UndoRedoHandler::canUndo()
	{
		return (!undo_queue_.empty()); 
	}

	bool UndoRedoHandler::canRedo()
	{
		return (!redo_queue_.empty());
	}

	void UndoRedoHandler::releaseQueueItem(OperateContext& context)
	{
		if (context.second != nullptr) {
			delete context.second; 
			context.second = nullptr; 
		}
	}
}