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
#include "ACAMCore.h"

#include "DataManager.h"
#include "UndoRedoHandler.h"

#include <set>

#include "../Operation/OperationDefine.h"
#include "../Utils/MathUtils.h"

using namespace acamcad;

AMCore::AMCore() : dataManager_(nullptr)
{
	init();

	op_datatype_ = OperationDataType::UNKNOW_TYPE;
	mesh_op_type_ = MeshOperationType::NO_operotion;
}

void AMCore::init()
{
	addNewDataTypeFile(FILE_EXT_AMCAX_BREP, DataType::BREP_TYPE);
	addNewDataTypeFile(FILE_EXT_STEP, DataType::BREP_TYPE);
	addNewDataTypeFile(FILE_EXT_STP, DataType::BREP_TYPE);

	inlineSingleOperation();
	inlineMultOperation();
}

void AMCore::inlineSingleOperation()
{
	single_operation_list_.clear();

//	SGLOperate_Delete* operate_delete = new SGLOperate_Delete(SelectModel::OBJECT_MODEL);
//	single_operation_list_.push_back(operate_delete);

	SGLOperate_ChamferEdge* operate_chamfer = new SGLOperate_ChamferEdge(SelectModel::OBJECT_MODEL);
	single_operation_list_.push_back(operate_chamfer);
	SGLOperate_FilletEdge* operate_fillet = new SGLOperate_FilletEdge(SelectModel::OBJECT_MODEL);
	single_operation_list_.push_back(operate_fillet);

}

void AMCore::inlineMultOperation()
{
	mult_operation_list_.clear();
	MultOperate_BoolCommon* operate_common = new MultOperate_BoolCommon(SelectModel::OBJECT_MODEL);
	mult_operation_list_.push_back(operate_common);
	MultOperate_BoolCut* operate_cut = new MultOperate_BoolCut(SelectModel::OBJECT_MODEL);
	mult_operation_list_.push_back(operate_cut);
	MultOperate_BoolFuse* operate_fuse = new MultOperate_BoolFuse(SelectModel::OBJECT_MODEL);
	mult_operation_list_.push_back(operate_fuse);
	MultOperate_Combine* operate_combine = new MultOperate_Combine(SelectModel::OBJECT_MODEL);
	mult_operation_list_.push_back(operate_combine);
}


AMCore::~AMCore()
{
	for (int i = 0; i < single_operation_list_.size(); i++)
	{
		delete single_operation_list_[i];
	}
	for (int i = 0; i < mult_operation_list_.size(); i++)
	{
		delete mult_operation_list_[i];
	}
}

//=================================== Load Save Method ===================================


//============================================================
//                   Select
//============================================================

AMCAX::Coord3 AMCore::getSelectedObject_Center(const SelectInfoWithCamera& select_info, SelectModel type_)
{
	BaseObject* object = dataManager_->getObjectByPersistentID(select_info.object_id_);
	if (object == nullptr)
		return AMCAX::Coord3(0.0, 0.0, 0.0);

	MOperation_getCenter mo_select(type_);
	mo_select.setSelectInfoCamera(select_info);
	object->doOperate(&mo_select);
	AMCAX::Coord3 c = mo_select.getCenter();
	return c;
}

AMCAX::Coord3 AMCore::getSelectedObject_Normal(const SelectInfoWithCamera& select_info, SelectModel type_)
{
	AMCAX::Coord3 n(0.0, 0.0, 1.0);
	std::vector<SelectInfo> s_info_list;
	s_info_list.push_back(SelectInfo(select_info.object_id_, select_info.object_subselect_id_));

	int object_id = select_info.object_id_;
	BaseObject* object = dataManager_->getObjectByPersistentID(object_id);

	MOperation_getDirection* mo_select = new MOperation_getDirection(type_);
	mo_select->setselectInfo(s_info_list);
	object->doOperate(mo_select);
	n = mo_select->getDirection();
	delete mo_select;

	return n;
}

void AMCore::getSelectedObject_BBox(const SelectInfoWithCamera& select_info, SelectModel type_, AMCAX::Coord3 & bbmin, AMCAX::Coord3 & bbmax)
{
	std::vector<SelectInfo> s_info_list;
	s_info_list.push_back(SelectInfo(select_info.object_id_, select_info.object_subselect_id_));

	int object_id = select_info.object_id_;
	BaseObject* object = dataManager_->getObjectByPersistentID(object_id);

	MOperation_getBoundingBox* mo_select = new MOperation_getBoundingBox(type_);
	mo_select->setselectInfo(s_info_list);
	object->doOperate(mo_select);

	mo_select->getBoundingBox(bbmin, bbmax);

	delete mo_select;
}

void AMCore::getSelectedObject_Plane(const std::vector<SelectInfoWithCamera>& select_info_list,
 SelectModel type_, AMCAX::Coord3 & center, AMCAX::Coord3 & normal)
{
	std::vector<SelectInfo> s_info_list;
	extractSelectInfoList(select_info_list, s_info_list);
	if (!isSelectOneObject(s_info_list))
	{
		std::cout << "can not set Plane with different object" << std::endl;
		return;
	}

	int object_id = s_info_list[0].object_id_;
	BaseObject* object = dataManager_->getObjectByPersistentID(object_id);
	if (object == nullptr)
		return;

	MOperation_getPlane mo_getplane(type_);
	mo_getplane.setselectInfo(s_info_list);
	object->doOperate(&mo_getplane);
	center = mo_getplane.getCenter();
	normal = mo_getplane.getDirection();
}

void AMCore::getSelectedObject_Element_Related(std::vector<SelectInfoWithCamera>& select_info_list, SelectModel type_)
{
	std::vector<SelectInfo> s_info_list;
	extractSelectInfoList(select_info_list, s_info_list);
	if (!isSelectOneObject(s_info_list))
	{
		std::cout << "can not Fast select different object" << std::endl;
		return;
	}

	int object_id = s_info_list[0].object_id_;
	BaseObject* object = dataManager_->getObjectByPersistentID(object_id);
	if (object == nullptr)
		return;

	select_info_list.resize(s_info_list.size());
	for (int i = 0; i < s_info_list.size(); i++)
	{
		select_info_list[i].object_id_ = s_info_list[i].object_id_;
		select_info_list[i].object_subselect_id_ = s_info_list[i].object_subselect_id_;
	}
}



//============================================================
//                Move Scale Rotation
//============================================================

void AMCore::beginOperationByAxis(std::vector<SelectInfoWithCamera>& select_info_list, SelectModel type_)
{
	if (type_ == SelectModel::OBJECT_MODEL)
	{
		for (int i = 0; i < select_info_list.size(); i++)
		{
			const SelectInfoWithCamera& s_info = select_info_list[i];
			BaseObject* object = dataManager_->getObjectByPersistentID(s_info.object_id_);

			if (object->dataType() == DataType::BREP_TYPE)
			{
				BRepObject* brep_object = dynamic_cast<BRepObject*> (object);
				brep_object->mesh_OPvert_Backup();
			}
		}
	}
	else if (type_ == SelectModel::VERTEX_MODEL || type_ == SelectModel::EDGE_MODEL || type_ == SelectModel::FACE_MODEL)
	{
		std::vector<int> s_object_list = getSelectObjectList(select_info_list);
		std::vector<std::vector<int>> s_s_object_list; s_s_object_list.clear();
		for (size_t si = 0; si < s_object_list.size(); si++)
		{
			std::vector<int> ss_id = getSelectSubListWithObject(select_info_list, s_object_list[si]);
			s_s_object_list.push_back(ss_id);
		}

		for (int si = 0; si < s_object_list.size(); si++)
		{
			BaseObject* obj = dataManager_->getObjectByPersistentID(s_object_list[si]);
			if (obj->dataType() == DataType::BREP_TYPE)
			{
				BRepObject* brep_object = dynamic_cast<BRepObject*> (obj);
				brep_object->mesh_OPvert_Backup();
			}
		}
	}
}

void AMCore::scaleSelectedObject(std::vector<SelectInfoWithCamera>& select_info_list,
 SelectModel type_, const AMCAX::Coord3 & center, const AMCAX::Coord3 & axis, double scale, int scale_type)
{
	std::vector<int> select_object_id = getSelectObjectList(select_info_list);

	for (int i = 0; i < select_object_id.size(); i++)
	{
		BaseObject* object = dataManager_->getObjectByPersistentID(select_object_id[i]);
		if (object->dataType() == DataType::BREP_TYPE)
		{
			std::cout << "Can not scale BRep shape Now";
			continue;
		}
	}
}

void AMCore::rotateSelectedObject(std::vector<SelectInfoWithCamera>& select_info_list,
 SelectModel type_, const AMCAX::Coord3 & center, const AMCAX::Coord3 & axis, double scale)
{
	std::vector<int> select_object_id = getSelectObjectList(select_info_list);

	for (int i = 0; i < select_object_id.size(); i++)
	{
		BaseObject* object = dataManager_->getObjectByPersistentID(select_object_id[i]);
		if (object->dataType() == DataType::BREP_TYPE)
		{
			if (type_ != SelectModel::OBJECT_MODEL)
			{
				std::cout << "Can not rotate BRep sub shape";
				continue;
			}

			BRepObject* brep_object = dynamic_cast<BRepObject*> (object);
			brep_object->meshRotation(center, axis, scale);
		}
	}
}

void AMCore::rotateSelectedObject(std::vector<BaseObject*>& object_list,
 SelectModel type_, const AMCAX::Coord3 & center, const AMCAX::Coord3 & axis, double scale, const std::string& label)
{
	for (auto *object: object_list)
	{
		if (object->dataType() == DataType::BREP_TYPE)
		{
			if (type_ != SelectModel::OBJECT_MODEL)
			{
				std::cout << "Can not rotate BRep sub shape";
				continue;
			}

			BRepObject* brep_object = dynamic_cast<BRepObject*> (object);
			brep_object->meshRotation(center, axis, scale);
		}
		if (label != "") // default empty label
		{
			object->setLabel(label);
		}
	}
}


void AMCore::moveSelectedObject(std::vector<BaseObject *>& object_list, SelectModel type_, const AMCAX::Coord3 & trans, double distance, const std::string &label)
{
	for (auto *object: object_list)
	{
		if (object->dataType() == DataType::BREP_TYPE)
		{
			if (type_ != SelectModel::OBJECT_MODEL)
			{
				std::cout << "Can not rotate BRep sub shape";
				continue;
			}

			BRepObject* brep_object = dynamic_cast<BRepObject*> (object);
			brep_object->meshMove(trans, distance);
		}
		if (label != "") // default empty label
		{
			object->setLabel(label);
		}
	}
}


void AMCore::moveSelectedObject(std::vector<SelectInfoWithCamera>& select_info_list, SelectModel type_, const AMCAX::Coord3 & trans, double distance)
{
	std::vector<int> select_object_id = getSelectObjectList(select_info_list);
	std::vector<BaseObject *> object_list(select_object_id.size());

	std::transform(select_object_id.begin(), select_object_id.end(),
				   object_list.begin(), [&](const int &id) { return dataManager_->getObjectByPersistentID(id); });

	moveSelectedObject(object_list, type_, trans, distance);
}

void AMCore::endOperationByAxis(std::vector<SelectInfoWithCamera>& select_info_list, SelectModel type_)
{
	std::vector<int> select_object_id = getSelectObjectList(select_info_list);

	for (int i = 0; i < select_object_id.size(); i++)
	{
		BaseObject* object = dataManager_->getObjectByPersistentID(select_object_id[i]);

	}
}


//============================================================
//                 Operation
//============================================================

void AMCore::OperateSelectedObject(const SelectModel& s_model, const SelectInfoWithCamera& s_info)
{
	if (s_model != SelectModel::OBJECT_MODEL)
		return;

	BaseObject* obj = dataManager_->getObjectByPersistentID(s_info.object_id_);

	for (int si = 0; si < single_operation_list_.size(); si++)
	{
		if (single_operation_list_[si]->OperationType() == mesh_op_type_)
		{
			single_operation_list_[si]->setSelectModel(s_model);
			obj->doOperate(single_operation_list_[si]);
			createBackupNormalOp(obj);
			return;
		}
	}

	if (mesh_op_type_ == MeshOperationType::MeshCopy) {
		BaseObject* copy_object = dataManager_->copyObject(obj);
		if (copy_object != nullptr)
			undoRedoHandler_->recordOperation(copy_object, ActionType::AddObject); 
	}
	else if (mesh_op_type_ == MeshOperationType::MeshDelete) {
		if (obj != nullptr) {
			record_->recordDelete(obj);
			undoRedoHandler_->recordOperation(obj, ActionType::DeleteObject);
		}
		dataManager_->deleteObject(obj);
	}
}

void AMCore::deleteObject(BaseObject* obj) {
	if (obj != nullptr)
		undoRedoHandler_->recordOperation(obj, ActionType::DeleteObject);
	dataManager_->deleteObject(obj);
}

BaseObject *AMCore::getObjectByPersistentID(const int pid) const
{
	for (auto it = dataManager_->objects_begin(); it != dataManager_->objects_end(); ++it)
	{
		if ((*it)->persistentId() == pid) 
		{
			return *it;
		}
	}
    return nullptr;
}

std::vector<BaseObject *> AMCore::getObjectByLabel(const std::string &label) const
{
    std::vector<BaseObject *> objs{};
	for (auto it = dataManager_->objects_begin(); it != dataManager_->objects_end(); ++it)
	{
		if ((*it)->label() == label)
		{
			objs.push_back(*it);
		}
	}
	return objs;
}

MultOperate *AMCore::getMultOperateByOperationType(const MeshOperationType &ot)
{
	for (int i = 0; i < mult_operation_list_.size(); i++)
	{
		if (mult_operation_list_[i]->OperationType() == ot)
		{
			return mult_operation_list_[i];
		}
	}
	return nullptr;
}

void AMCore::OperateSelectedObject(std::vector<BaseObject *> objectList, MultOperate *operate, const std::string &label)
{
	operate->setObjectList(objectList);
	operate->checkOjbectTypeUnanimous();
	if (operate->checkOjbectTypeUnanimous())
	{
		BaseObject* newObject = nullptr;
		std::vector<int> persistent_id_list;
		for (int i = 0; i < objectList.size(); i++)
		{
			persistent_id_list.emplace_back(objectList[i]->persistentId());
		}
		DataType type = operate->ObjectType();
		if (type == DataType::BREP_TYPE)
			newObject = operate->operateWithBRep();
		if (label != "") // default empty label
		{
			newObject->setLabel(label);
		} // else the object will take the label of the first object in the list

		for (int i = 0; i < objectList.size(); i++)
		{
			std::cerr <<"persistentId:" << objectList[i]->persistentId() << "\n";
			std::cerr << "label:" << objectList[i]->label() << "\n";
			undoRedoHandler_->recordOperation(objectList[i], ActionType::DeleteObject);
			dataManager_->deleteObject(objectList[i]);
		}

		dataManager_->addObject(newObject);
		undoRedoHandler_->recordOperation(newObject, ActionType::AddObject);
		if (record_ != 0)
		{
			switch (operate->OperationType())
			{
			case MeshOperationType::MeshCommon: {
				record_->recordCommon(dataManager_->getLastPersistentId(), persistent_id_list);
			}break;
			case MeshOperationType::MeshFuse: {
				record_->recordFuse(dataManager_->getLastPersistentId(), persistent_id_list);
			}break;
			case MeshOperationType::MeshCut: {
				record_->recordCut(dataManager_->getLastPersistentId(), persistent_id_list);
			}break;
			default:;
			}
		}
	}
	
	operate->reInline();
}


void AMCore::OperateSelectedObject(const SelectModel& s_model, const std::vector<SelectInfoWithCamera>& s_info_vector)
{
	if (s_model != SelectModel::OBJECT_MODEL)
		return;

    auto operate = getMultOperateByOperationType(mesh_op_type_);
    if (operate)
	{
		std::vector<BaseObject*> objectList(s_info_vector.size());
		for (int i = 0; i < s_info_vector.size(); i++)
		{
			objectList[i] = dataManager_->getObjectByPersistentID(s_info_vector[i].object_id_);
		}
		
		OperateSelectedObject(objectList, operate);
		return;
	}

	for (int i = 0; i < s_info_vector.size(); i++)
	{
		OperateSelectedObject(s_model, s_info_vector[i]);
	}
}

void AMCore::OperateSelectedObject(const SelectModel& s_model, const SelectInfoWithCamera& s_info, const AMCAX::Coord3 & center, const AMCAX::Coord3 & axis)
{
	if (s_model != SelectModel::OBJECT_MODEL)
		return;

	BaseObject* obj = dataManager_->getObjectByPersistentID(s_info.object_id_);

}

void AMCore::OperateSelectedObject(const SelectModel& s_model, const std::vector<SelectInfoWithCamera>& s_info_vector,
	const AMCAX::Coord3& center, const AMCAX::Coord3& axis)
{
	if (mesh_op_type_ != MeshOperationType::MeshMirror && mesh_op_type_ != MeshOperationType::MeshProject)
		return;

	if (mesh_op_type_ == MeshOperationType::MeshMirror)
	{
		for (int i = 0; i < s_info_vector.size(); i++)
		{
			OperateSelectedObject(s_model, s_info_vector[i], center, axis);
		}
	}
	else
	{
		std::vector<SelectInfo> s_info_list(s_info_vector.size());
		for (int i = 0; i < s_info_vector.size(); i++)
		{
			s_info_list[i].object_id_ = s_info_vector[i].object_id_;
			s_info_list[i].object_subselect_id_ = s_info_vector[i].object_subselect_id_;
		}
		if (!isSelectOneObject(s_info_list))
		{
			std::cout << "can not Operate select different object" << std::endl;
			return;
		}
		int object_id = s_info_list[0].object_id_;
		BaseObject* obj = dataManager_->getObjectByPersistentID(object_id);

		if (obj != nullptr)
		{
			createBackupNormalOp(obj);
		}
	}
}

void AMCore::OperateSelectedObject_Subset(const SelectModel& s_model, std::vector<SelectInfoWithCamera>& s_info_vector)
{
	std::vector<SelectInfo> s_info_list(s_info_vector.size());
	for (int i = 0; i < s_info_vector.size(); i++)
	{
		s_info_list[i].object_id_ = s_info_vector[i].object_id_;
		s_info_list[i].object_subselect_id_ = s_info_vector[i].object_subselect_id_;
	}
	if (!isSelectOneObject(s_info_list))
	{
		std::cout << "can not Operate select different object" << std::endl;
		return;
	}
	int object_id = s_info_list[0].object_id_;
	for (int si = 0; si < single_operation_list_.size(); si++)
	{
		if (single_operation_list_[si]->OperationType() == mesh_op_type_)
		{
			BaseObject* obj = dataManager_->getObjectByPersistentID(object_id);

			single_operation_list_[si]->setSelectModel(s_model);
			single_operation_list_[si]->setselectInfo(s_info_list);
			obj->doOperate(single_operation_list_[si]);
			createBackupNormalOp(obj);
		}
	}


	if (s_model == SelectModel::FACE_MODEL && mesh_op_type_ == MeshOperationType::MeshCopy)
	{
		std::vector<int> object_list = getSelectObjectList(s_info_vector);
		if (object_list.size() > 2)
			return;

		std::vector<int> s_id = getSelectSubList(s_info_vector);
		if (s_id.empty())
			return;

		BaseObject* obj = dataManager_->getObjectByPersistentID(object_list[0]);

	}
}

void AMCore::OperateSelectedObject_Subset_Split(const SelectModel& s_model,
 const std::vector<SelectInfoWithCamera>& s_info_vector, std::vector<AMCAX::Coord3>& point_list)
{
	//在调用时备份了，不一定对

	std::vector<SelectInfo> s_info_list(s_info_vector.size());
	for (int i = 0; i < s_info_vector.size(); i++)
	{
		s_info_list[i].object_id_ = s_info_vector[i].object_id_;
		s_info_list[i].object_subselect_id_ = s_info_vector[i].object_subselect_id_;
	}
	if (!isSelectOneObject(s_info_list))
	{
		std::cout << "can not Operate select different object" << std::endl;
		return;
	}
	int object_id = s_info_list[0].object_id_;
	for (int si = 0; si < single_operation_list_.size(); si++)
	{
		if (single_operation_list_[si]->OperationType() == mesh_op_type_)
		{
			BaseObject* obj = dataManager_->getObjectByPersistentID(object_id);

			single_operation_list_[si]->setSelectModel(s_model);
			single_operation_list_[si]->setselectInfo(s_info_list);

			single_operation_list_[si]->setParameter(point_list);
			obj->doOperate(single_operation_list_[si]);
		}
	}


	//MeshSplitEdge
	//MeshSplitLoop
	//MeshSplitFaceByEdge
}

void AMCore::OperateSelectedObject_Subset_Extrude(const SelectModel& s_model,
 std::vector<SelectInfoWithCamera>& s_info_vector, AMCAX::Coord3 & vector)
{
	std::vector<SelectInfo> s_info_list(s_info_vector.size());
	for (int i = 0; i < s_info_vector.size(); i++)
	{
		s_info_list[i].object_id_ = s_info_vector[i].object_id_;
		s_info_list[i].object_subselect_id_ = s_info_vector[i].object_subselect_id_;
	}
	if (!isSelectOneObject(s_info_list))
	{
		std::cout << "can not Operate Extrude select different object" << std::endl;
		return;
	}
	int object_id = s_info_list[0].object_id_;
	for (int si = 0; si < single_operation_list_.size(); si++)
	{
		if (single_operation_list_[si]->OperationType() == mesh_op_type_)
		{
			BaseObject* obj = dataManager_->getObjectByPersistentID(object_id);

			single_operation_list_[si]->setSelectModel(s_model);
			single_operation_list_[si]->setselectInfo(s_info_list);
			obj->doOperate(single_operation_list_[si]);

			s_info_list = single_operation_list_[si]->getSelectInfoNew();
			std::vector<int> s_list = single_operation_list_[si]->getOperateIdNew();
			vector = single_operation_list_[si]->getCoord3Direction();

			s_info_vector.resize(s_info_list.size());
			for (int i = 0; i < s_info_list.size(); i++)
			{
				s_info_vector[i].object_id_ = s_info_list[i].object_id_;
				s_info_vector[i].object_subselect_id_ = s_info_list[i].object_subselect_id_;
			}

		}
	}

	//MeshOperationType::MeshExtrudeEdgeHorizontal
	//MeshOperationType::MeshExtrudeFace
}

void AMCore::OperateSelectedObjectFaceExtrude(const SelectModel& s_model, std::vector<SelectInfoWithCamera>& s_info_vector, AMCAX::Coord3d & vector)
{
	if (s_info_vector.size() != 1)
		return;	

	std::vector<SelectInfo> s_info_list(s_info_vector.size());
	for (int i = 0; i < s_info_vector.size(); i++)
	{
		s_info_list[i].object_id_ = s_info_vector[i].object_id_;
		s_info_list[i].object_subselect_id_ = s_info_vector[i].object_subselect_id_;
	}
	int object_id = s_info_list[0].object_id_;
	BaseObject* obj = dataManager_->getObjectByPersistentID(object_id);
	
	if (mesh_op_type_ == MeshOperationType::MeshExtrudeFace)
	{
		SGLOperate_ExtrudeShape* exface = new SGLOperate_ExtrudeShape(SelectModel::FACE_MODEL);
		exface->setDirection(vector);

		obj->doOperate(exface);
		
		createBackupNormalOp(obj);

		delete exface;
	}
}


//======================================================================================
//							Back Up
//=======================================================================================

void AMCore::createBackupNormalOp(BaseObject* object)
{
	undoRedoHandler_->recordOperation(object, ActionType::OperateObject); 
}

void AMCore::createBackupSInfo(std::vector<SelectInfoWithCamera>& select_info)
{
	return; 


	if (select_info.empty())
		return;

	std::set<int> object_id_set;
	for (int i = 0; i < select_info.size(); i++)
	{
		object_id_set.insert(select_info[i].object_id_);
	}
	std::vector<int> object_id;
	for (std::set<int>::iterator it = object_id_set.begin(); it != object_id_set.end(); ++it)
	{
		object_id.push_back(*it);
	}

	for (int i = 0; i < object_id.size(); i++)
	{
		BaseObject* object = dataManager_->getObjectByPersistentID(object_id[i]);
		createBackupNormalOp(object);
	}

}

void AMCore::Redo()
{
	undoRedoHandler_->redo(); 
}

void AMCore::Undo()
{
	undoRedoHandler_->undo();
}


//======================================================================================

void AMCore::setOperationDataType(OperationDataType type)
{
	op_datatype_ = type;
}
void AMCore::setMeshOperationType(MeshOperationType type)
{
	mesh_op_type_ = type;
	std::cout << "the operate type is " << type << std::endl;
}

void AMCore::setDataManager(DataManager* dataManager)
{
	dataManager_ = dataManager;
}

DataManager *AMCore::getDataManager() const
{
	return dataManager_;
}

void AMCore::setBackupManager(BackupManager* backManager)
{
	backupManager_ = backManager;
}

void AMCore::setUndoRedoHandler(UndoRedoHandler* undoRedoHandler)
{
	undoRedoHandler_ = undoRedoHandler;
}