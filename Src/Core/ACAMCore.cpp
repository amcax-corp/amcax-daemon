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

#include <set>

#include "../Operation/OperationDefine.h"
#include "../Utils/MathUtils.h"

using namespace acamcad;

AMCore::AMCore() : dataManager_(nullptr), listener_(nullptr)
{
	init();

	op_datatype_ = OperationDataType::CUSTOM_TYPE;
	mesh_op_type_ = MeshOperationType::NO_operotion;
}

void AMCore::init()
{
	//addNewDataTypeFile(FILE_EXT_AMCAX_BREP, DataType::BREP_TYPE);
	//addNewDataTypeFile(FILE_EXT_STEP, DataType::BREP_TYPE);
	//addNewDataTypeFile(FILE_EXT_STP, DataType::BREP_TYPE);

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

	SGLOperate_TriangulateMesh* operate_tri = new SGLOperate_TriangulateMesh(SelectModel::OBJECT_MODEL);
	single_operation_list_.push_back(operate_tri);

	SGLOperate_EmbedFace* operate_face = new SGLOperate_EmbedFace(SelectModel::OBJECT_MODEL);
	single_operation_list_.push_back(operate_face);

	SGLOperate_Delete* operate_del = new SGLOperate_Delete(SelectModel::OBJECT_MODEL);
	single_operation_list_.push_back(operate_del);

	SGLOperate_Separate* operate_sep = new SGLOperate_Separate(SelectModel::OBJECT_MODEL);
	single_operation_list_.push_back(operate_sep);

	SGLOperate_WeldEdge* operate_weld = new SGLOperate_WeldEdge(SelectModel::OBJECT_MODEL);
	single_operation_list_.push_back(operate_weld);

	SGLOperate_SplitLoop* operate_split_loop = new SGLOperate_SplitLoop(SelectModel::OBJECT_MODEL);
	single_operation_list_.push_back(operate_split_loop);

	SGLOperate_SplitFaceByEdge* operate_split_by_edge = new SGLOperate_SplitFaceByEdge(SelectModel::OBJECT_MODEL);
	single_operation_list_.push_back(operate_split_by_edge);

	SGLOperate_SplitFaceVertex* opereate_split_by_vertex = new SGLOperate_SplitFaceVertex(SelectModel::OBJECT_MODEL);
	single_operation_list_.push_back(opereate_split_by_vertex);

	SGLOperate_Bridge* operate_bridge = new SGLOperate_Bridge(SelectModel::OBJECT_MODEL);
	single_operation_list_.push_back(operate_bridge);

	SGLOperate_Thicken* operate_thichen = new SGLOperate_Thicken(SelectModel::OBJECT_MODEL);
	single_operation_list_.push_back(operate_thichen);

	SGLOperate_ExtrudeEdgeH* opreate_extrude_H = new SGLOperate_ExtrudeEdgeH(SelectModel::OBJECT_MODEL);
	single_operation_list_.push_back(opreate_extrude_H);

	SGLOperate_ExtrudeEdgeV* opreate_extrude_V = new SGLOperate_ExtrudeEdgeV(SelectModel::OBJECT_MODEL);
	single_operation_list_.push_back(opreate_extrude_V);

	SGLOperate_ReverseNormal* opreate_reverse = new SGLOperate_ReverseNormal(SelectModel::OBJECT_MODEL);
	single_operation_list_.push_back(opreate_reverse);

	SGLOperate_CreaseEdgeRemove* operate_creaseRemove = new SGLOperate_CreaseEdgeRemove(SelectModel::OBJECT_MODEL);
	single_operation_list_.push_back(operate_creaseRemove);

	SGLOperate_CreaseEdge* operate_create = new SGLOperate_CreaseEdge(SelectModel::OBJECT_MODEL);
	single_operation_list_.push_back(operate_create);

	SGLOperate_Uniform* opreate_uniform = new SGLOperate_Uniform(SelectModel::OBJECT_MODEL);
	single_operation_list_.push_back(opreate_uniform);

	SGLOperate_Repair* opreate_repair = new SGLOperate_Repair(SelectModel::OBJECT_MODEL);
	single_operation_list_.push_back(opreate_repair);

	SGLOperate_SplitFace* operate_Splitface = new SGLOperate_SplitFace(SelectModel::OBJECT_MODEL);
	single_operation_list_.push_back(operate_Splitface);

	SGLOperate_ExtrudeFace* operate_ef = new SGLOperate_ExtrudeFace(SelectModel::OBJECT_MODEL);
	single_operation_list_.push_back(operate_ef);

	SGLOperate_FillHole* operate_fh = new SGLOperate_FillHole(SelectModel::OBJECT_MODEL);
	single_operation_list_.push_back(operate_fh);


	//SGLOperate_SplitEdge* operate_split_edge = new SGLOperate_SplitEdge(SelectModel::OBJECT_MODEL);
	//single_operation_list_.push_back(operate_split_edge);

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
	AdapterObject* object = dataManager_->getObjectByPersistentID(select_info.object_id_);
	if (object == nullptr)
		return AMCAX::Coord3(0.0, 0.0, 0.0);

	MOperation_getCenter mo_select(type_);
	mo_select.setSelectInfoCamera(select_info);
	//object->doOperate(&mo_select);

	mo_select.operate(object);
	AMCAX::Coord3 c = mo_select.getCenter();
	return c;
}

AMCAX::Coord3 AMCore::getSelectedObject_Normal(const SelectInfoWithCamera& select_info, SelectModel type_)
{
	AMCAX::Coord3 n(0.0, 0.0, 1.0);
	std::vector<SelectInfo> s_info_list;
	s_info_list.push_back(SelectInfo(select_info.object_id_, select_info.object_subselect_id_));

	int object_id = select_info.object_id_;
	AdapterObject* object = dataManager_->getObjectByPersistentID(object_id);

	MOperation_getDirection* mo_select = new MOperation_getDirection(type_);
	mo_select->setselectInfo(s_info_list);
	//object->doOperate(mo_select);
	mo_select->operate(object);
	n = mo_select->getDirection();
	delete mo_select;

	return n;
}

void AMCore::getSelectedObject_BBox(const SelectInfoWithCamera& select_info, SelectModel type_, AMCAX::Coord3& bbmin, AMCAX::Coord3& bbmax)
{
	std::vector<SelectInfo> s_info_list;
	s_info_list.push_back(SelectInfo(select_info.object_id_, select_info.object_subselect_id_));

	int object_id = select_info.object_id_;
	AdapterObject* object = dataManager_->getObjectByPersistentID(object_id);

	MOperation_getBoundingBox* mo_select = new MOperation_getBoundingBox(type_);
	mo_select->setselectInfo(s_info_list);
	//object->doOperate(mo_select);
	mo_select->operate(object);

	mo_select->getBoundingBox(bbmin, bbmax);

	delete mo_select;
}

void AMCore::getSelectedObject_Plane(const std::vector<SelectInfoWithCamera>& select_info_list,
	SelectModel type_, AMCAX::Coord3& center, AMCAX::Coord3& normal)
{
	std::vector<SelectInfo> s_info_list;
	extractSelectInfoList(select_info_list, s_info_list);
	if (!isSelectOneObject(s_info_list))
	{
		std::cout << "can not set Plane with different object" << std::endl;
		return;
	}

	int object_id = s_info_list[0].object_id_;
	AdapterObject* object = dataManager_->getObjectByPersistentID(object_id);
	if (object == nullptr)
		return;

	MOperation_getPlane mo_getplane(type_);
	mo_getplane.setselectInfo(s_info_list);
	//object->doOperate(&mo_getplane);
	mo_getplane.operate(object);
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


			AdapterObject* brep_object = dynamic_cast<AdapterObject*> (object);
			brep_object->mesh_OPvert_Backup();

		}
	}
	else if (type_ == SelectModel::VERTEX_MODEL || type_ == SelectModel::EDGE_MODEL || type_ == SelectModel::FACE_MODEL)
	{

		void (AdapterObject:: * ptr[3])(const std::vector<int>&) {
			&AdapterObject::mesh_OPvert_BackupVertes, & AdapterObject::mesh_OPvert_BackupEdges,
				& AdapterObject::mesh_OPvert_BackupFaces
		};

		std::vector<int> s_object_list = getSelectObjectList(select_info_list);
		std::vector<std::vector<int>> s_s_object_list; s_s_object_list.clear();
		for (size_t si = 0; si < s_object_list.size(); si++)
		{
			std::vector<int> ss_id = getSelectSubListWithObject(select_info_list, s_object_list[si]);
			s_s_object_list.push_back(ss_id);
		}

		for (int si = 0; si < s_object_list.size(); si++)
		{
			AdapterObject* adapter = dataManager_->getObjectByPersistentID(s_object_list[si]);

			//AdapterObject* adapter = dynamic_cast<AdapterObject*> (obj);

			(adapter->*ptr[(size_t)type_])(s_s_object_list[si]);

		}
	}

}

void AMCore::scaleSelectedObject(std::vector<SelectInfoWithCamera>& select_info_list,
	SelectModel type_, const AMCAX::Coord3& center, const AMCAX::Coord3& axis, double scale, int scale_type)
{
	std::vector<int> select_object_id = getSelectObjectList(select_info_list);

	for (int i = 0; i < select_object_id.size(); i++)
	{
		AdapterObject* object = dataManager_->getObjectByPersistentID(select_object_id[i]);
		dataManager_->RecordModifyBegin({ object });
		switch (scale_type)
		{
		case 0:
		{
			object->meshScaleSingle(center, axis, scale);
		}
		break;
		case 1:
		{
			object->meshScalePlane(center, axis, scale);
		}
		break;
		default:
			break;
		}
	}

}

void AMCore::rotateSelectedObject(std::vector<SelectInfoWithCamera>& select_info_list,
	SelectModel type_, const AMCAX::Coord3& center, const AMCAX::Coord3& axis, double scale)
{
	std::vector<int> select_object_id = getSelectObjectList(select_info_list);

	for (int i = 0; i < select_object_id.size(); i++)
	{
		AdapterObject* object = dataManager_->getObjectByPersistentID(select_object_id[i]);

		dataManager_->RecordModifyBegin({ object });

		object->meshRotation(center, axis, scale);
	}
}

void AMCore::rotateSelectedObject(std::vector<BaseObject*>& object_list,
	SelectModel type_, const AMCAX::Coord3& center, const AMCAX::Coord3& axis, double scale, const std::string& label)
{
	for (auto* object : object_list)
	{


		AdapterObject* brep_object = dynamic_cast<AdapterObject*> (object);
		brep_object->meshRotation(center, axis, scale);

		if (label != "") // default empty label
		{
			object->setLabel(label);
		}
	}


}


void AMCore::moveSelectedObject(std::vector<AdapterObject*>& object_list, SelectModel type_, const AMCAX::Coord3& trans, double distance, const std::string& label)
{

	//bool empty = dataManager_->isEmptyAuto();


	dataManager_->RecordModifyBegin(object_list);


	for (auto* object : object_list)
	{
		//if (object->dataType() == DataType::BREP_TYPE || object->dataType() == DataType::MESH_TYPE)
			//{
				//AdapterObject* brep_object = dynamic_cast<AdapterObject*> (object);
		object->meshMove(trans, distance);
		//}
		if (label != "") // default empty label
		{
			object->setLabel(label);
		}
	}


}


void AMCore::moveSelectedObject(std::vector<SelectInfoWithCamera>& select_info_list, SelectModel type_, const AMCAX::Coord3& trans, double distance)
{
	std::vector<int> select_object_id = getSelectObjectList(select_info_list);
	std::vector<AdapterObject*> object_list(select_object_id.size());

	std::transform(select_object_id.begin(), select_object_id.end(),
		object_list.begin(), [&](const int& id) { return dataManager_->getObjectByPersistentID(id); });

	moveSelectedObject(object_list, type_, trans, distance);
}

void AMCore::getSelectedObject_Element_closet(const SelectInfoWithCamera& select_info, SelectModel type_, AMCAX::Coord3& op_v)
{
	if (type_ == SelectModel::OBJECT_MODEL)
		return;

	AdapterObject* object = dataManager_->getObjectByPersistentID(select_info.object_id_);

	MOperation_SelectCloset mo_select(type_);
	mo_select.setSelectInfoCamera(select_info);
	//object->doOperate(&mo_select);
	mo_select.operate(object);

	MPoint3 op_mp = mo_select.getCloset();
	//MathUtils::CopyMVec2Coord(op_mp, op_v);
	op_v = op_mp.Coord();
}

void AMCore::endOperationByAxis(std::vector<SelectInfoWithCamera>& select_info_list, SelectModel type_)
{
	std::vector<int> select_object_id = getSelectObjectList(select_info_list);

	for (int i = 0; i < select_object_id.size(); i++)
	{
		AdapterObject* object = dataManager_->getObjectByPersistentID(select_object_id[i]);
		object->mesh_Clear_OPvertBackup();
		dataManager_->RecordModifyEnd({ object });
		object->updateDraw();

	}
}


//============================================================
//                 Operation
//============================================================

void AMCore::OperateSelectedObject(const SelectModel& s_model, const SelectInfoWithCamera& s_info)
{
	if (s_model != SelectModel::OBJECT_MODEL)
		return;

	AdapterObject* obj = dataManager_->getObjectByPersistentID(s_info.object_id_);

	for (int si = 0; si < single_operation_list_.size(); si++)
	{
		if (single_operation_list_[si]->OperationType() == mesh_op_type_)
		{
			single_operation_list_[si]->setSelectModel(s_model);
			dataManager_->RecordModifyBegin({ obj });
			//obj->doOperate(single_operation_list_[si]);
			single_operation_list_[si]->operate(obj);
			dataManager_->RecordModifyEnd({ obj });
			//createBackupNormalOp(obj);
			return;
		}
	}

	switch (mesh_op_type_)
	{
	case acamcad::MeshOperationType::MeshCopy:
	{
		AdapterObject* copy_object = dataManager_->copyObject(obj);
		if (copy_object != nullptr)
			dataManager_->RecordAddObject({ copy_object });
		//undoRedoHandler_->recordOperation(copy_object, ActionType::AddObject);
	}
	break;
	case acamcad::MeshOperationType::MeshDelete:
	{
		if (obj != nullptr) {
			//record_->recordDelete(obj);
			dataManager_->RecordDeleteObject({ obj });

			//undoRedoHandler_->recordOperation(obj, ActionType::DeleteObject);
		}
		dataManager_->deleteObject(obj);

	}
	break;
	case acamcad::MeshOperationType::MeshSubdiceCC:
	{
		if (obj->dataType() == DataType::MESH_TYPE) {
			//AdapterObject* adapter = dynamic_cast<AdapterObject*>(obj);
			///obj->mesh->mesh()->numPolygons();

			dataManager_->RecordModifyBegin({ obj });

			obj->mesh->subdivemeshCC();

			dataManager_->RecordModifyEnd({ obj });
			//adapter->updateDraw();


			///dataManager_->addObject()
		}
		else {
			std::cout << "acamcad::MeshOperationType::MeshSubdiceCC do not support undo redo" << std::endl;
		}

	}
	break;
	case acamcad::MeshOperationType::MeshSubdiveLoop:
	{
		if (obj->dataType() == DataType::MESH_TYPE) {
			dataManager_->RecordModifyBegin({ obj });
			//AdapterObject* adapter = dynamic_cast<AdapterObject*>(obj);
			obj->mesh->subdivemeshLoop();
			//adapter->updateDraw();
			//AdapterObject* news = new AdapterObject(*obj);
			dataManager_->RecordModifyEnd({ obj });
			///dataManager_->addObject()
		}
		else
		{
			std::cout << "acamcad::MeshOperationType::MeshSubdiceCC do not support undo redo" << std::endl;
		}
	}
	break;
	default:
		break;
	}
}

void AMCore::deleteObject(BaseObject* obj) {
	//if (obj != nullptr)
	//	undoRedoHandler_->recordOperation(obj, ActionType::DeleteObject);

	AdapterObject* adapter = dynamic_cast<AdapterObject*>(obj);


	dataManager_->RecordDeleteObject({ adapter });
	dataManager_->deleteObject(adapter);
}

BaseObject* AMCore::getObjectByPersistentID(const int pid) const
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

std::vector<BaseObject*> AMCore::getObjectByLabel(const std::string& label) const
{
	std::vector<BaseObject*> objs{};
	for (auto it = dataManager_->objects_begin(); it != dataManager_->objects_end(); ++it)
	{
		if ((*it)->label() == label)
		{
			objs.push_back(*it);
		}
	}
	return objs;
}

MultOperate* AMCore::getMultOperateByOperationType(const MeshOperationType& ot)
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

void AMCore::OperateSelectedObject(std::vector<AdapterObject*> objectList, MultOperate* operate, const std::string& label)
{
	operate->setObjectList(objectList);
	//operate->checkOjbectTypeUnanimous();
	if (operate->checkOjbectTypeUnanimous())
	{
		AdapterObject* newObject = nullptr;
		std::vector<int> persistent_id_list;
		for (int i = 0; i < objectList.size(); i++)
		{
			persistent_id_list.emplace_back(objectList[i]->persistentId());
		}
		//DataType type = operate->ObjectType();
		//if (type == DataType::BREP_TYPE)
		newObject = dynamic_cast<AdapterObject*>(operate->operate());
		if (!newObject)
			return;

		if (label != "") // default empty label
		{
			newObject->setLabel(label);
		} // else the object will take the label of the first object in the list

		dataManager_->RecordModifyBegin(objectList);
		for (int i = 0; i < objectList.size(); i++)
		{
			std::cerr << "persistentId:" << objectList[i]->persistentId() << "\n";
			std::cerr << "label:" << objectList[i]->label() << "\n";
			//undoRedoHandler_->recordOperation(objectList[i], ActionType::DeleteObject);
			//dataManager_->RecordModifyOldObject()
			dataManager_->deleteObject(dynamic_cast<AdapterObject*>(objectList[i]));
		}

		dataManager_->addObject(newObject);
		///undoRedoHandler_->recordOperation(newObject, ActionType::AddObject);
		dataManager_->RecordModifyEnd({ newObject });
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
		std::vector<AdapterObject*> objectList(s_info_vector.size());
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

void AMCore::OperateSelectedObject(const SelectModel& s_model, const SelectInfoWithCamera& s_info, const AMCAX::Coord3& center, const AMCAX::Coord3& axis)
{
	if (s_model != SelectModel::OBJECT_MODEL)
		return;

	AdapterObject* obj = dataManager_->getObjectByPersistentID(s_info.object_id_);

	switch (mesh_op_type_)
	{
	case acamcad::MeshOperationType::MeshMirror:
	{
		dataManager_->RecordModifyBegin({ obj });
		obj->mirror(center, axis);
		dataManager_->RecordModifyEnd({ obj });
	}
	break;
	default:
		break;
	}

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
			AdapterObject* obj = dataManager_->getObjectByPersistentID(object_id);

			single_operation_list_[si]->setSelectModel(s_model);
			single_operation_list_[si]->setselectInfo(s_info_list);
			dataManager_->RecordModifyBegin({ obj });
			//obj->doOperate(single_operation_list_[si]);
			std::vector<AdapterObject*> vec;

			bool bOperate = single_operation_list_[si]->operate(obj);
			if (bOperate)
				vec.push_back(obj);

			dataManager_->RecordModifyEnd(vec);
			//else
			//	dataManager_->RecordModifyNewObject({  });
			//createBackupNormalOp(obj);
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

		AdapterObject* obj = dataManager_->getObjectByPersistentID(object_list[0]);

		AdapterObject* copy_obj = obj->duplicateFaces(s_id);

		if (copy_obj) {
			copy_obj->updateDraw();
			dataManager_->addObject(copy_obj);
			dataManager_->RecordAddObject({ copy_obj });

		}
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
			AdapterObject* obj = dataManager_->getObjectByPersistentID(object_id);

			single_operation_list_[si]->setSelectModel(s_model);
			single_operation_list_[si]->setselectInfo(s_info_list);

			single_operation_list_[si]->setParameter(point_list);
			//obj->doOperate(single_operation_list_[si]);

			dataManager_->RecordModifyBegin({ obj });
			std::vector<AdapterObject*> vec;
			bool flag = single_operation_list_[si]->operate(obj);
			if (flag) {
				vec.push_back(obj);
			}
			dataManager_->RecordModifyEnd(vec);
		}
	}


	//MeshSplitEdge
	//MeshSplitLoop
	//MeshSplitFaceByEdge
}

void AMCore::OperateSelectedObject_Subset_Extrude(const SelectModel& s_model,
	std::vector<SelectInfoWithCamera>& s_info_vector, AMCAX::Coord3& vector)
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
			AdapterObject* obj = dataManager_->getObjectByPersistentID(object_id);

			single_operation_list_[si]->setSelectModel(s_model);
			single_operation_list_[si]->setselectInfo(s_info_list);
			dataManager_->RecordModifyBegin({ obj });
			bool result = single_operation_list_[si]->operate(obj);
			if (!result)
			{
				dataManager_->clearModify();
				continue;
			}
			//obj->doOperate(single_operation_list_[si]);

			s_info_list = single_operation_list_[si]->getSelectInfoNew();
			std::vector<int> s_list = single_operation_list_[si]->getOperateIdNew();
			vector = single_operation_list_[si]->getDirection();

			s_info_vector.resize(s_info_list.size());
			for (int i = 0; i < s_info_list.size(); i++)
			{
				s_info_vector[i].object_id_ = s_info_list[i].object_id_;
				s_info_vector[i].object_subselect_id_ = s_info_list[i].object_subselect_id_;
			}

			switch (s_model)
			{
			case SelectModel::EDGE_MODEL:
			{
				obj->mesh_OPvert_BackupEdges(s_list);
			}
			break;
			case SelectModel::FACE_MODEL:
			{
				obj->mesh_OPvert_BackupFaces(s_list);
			}
			break;
			default:
				break;
			}

		}
	}

	//MeshOperationType::MeshExtrudeEdgeHorizontal
	//MeshOperationType::MeshExtrudeFace
}

void AMCore::OperateSelectedObjectFaceExtrude(const SelectModel& s_model, std::vector<SelectInfoWithCamera>& s_info_vector, AMCAX::Coord3d& vector)
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
	AdapterObject* obj = dataManager_->getObjectByPersistentID(object_id);

	if (mesh_op_type_ == MeshOperationType::MeshExtrudeFace)
	{
		dataManager_->RecordModifyBegin({ obj });

		SGLOperate_ExtrudeShape* exface = new SGLOperate_ExtrudeShape(SelectModel::FACE_MODEL);
		exface->setDirection(vector);

		//obj->doOperate(exface);
		if (exface->operate(obj))
			dataManager_->RecordModifyEnd({ obj });
		else
			dataManager_->clearModify();

		delete exface;
	}
}


//======================================================================================
//							Back Up
//=======================================================================================


void AMCore::createBackupSInfo(std::vector<SelectInfoWithCamera>& select_info)
{

}

void AMCore::Redo()
{
	//undoRedoHandler_->redo();
	dataManager_->Redo();

}

void AMCore::Undo()
{
	//undoRedoHandler_->undo();
	dataManager_->Undo();
}

void AMCore::createSubdSphereObject(const MPoint3& center, double radius, size_t subtime)
{
	if (subtime <= 0)
		return;

	//MSphere sphere(center, radius);
	CreateOperate_QuadballTSpline cQuadball(center, radius, subtime);
	createObject(&cQuadball, DataType::TSPLINEU_TYPE);



}

void AMCore::createCylinderObject(const MPoint3& b_center, const AMCAX::Vector3& axis, double radius, double height,
	size_t rf_num, size_t vf_num, bool top, bool bottom)
{
	if (vf_num <= 0 || rf_num <= 0)
		return;

	//AMCAX::Frame3 frame(b_center, AMCAX::Direction3(axis.Coord()));
	//MCylinder cylinder(b_center, axis, radius, height);
	CreateOperate_CylinderTSpline cCylinder(b_center, axis, radius, height, rf_num, vf_num, top, bottom);
	createObject(&cCylinder, DataType::TSPLINEU_TYPE);


}

void AMCore::createConeObject(const MPoint3& b_center, const AMCAX::Vector3& axis, double radius, double height,
	size_t rf_num, size_t vf_num, bool bottom)
{
	if (rf_num <= 0 || vf_num <= 0)
		return;
	//MCone cone(b_center, axis, radius, height);
	CreateOperate_ConeTSpline cCone(b_center, axis, radius, height, rf_num, vf_num, bottom);
	createObject(&cCone, DataType::TSPLINEU_TYPE);


}

void AMCore::creatTourObject(const MPoint3& center, double radius0, double radius1, size_t rf_num, size_t vf_num)
{
	if (rf_num <= 0 || vf_num <= 0)
		return;

	//MTorus tours(center, radius0, radius1);
	AMCAX::Coord3 coord(center.Coord());
	CreateOperate_TorusTSpline cTorus(coord, AMCAX::Coord3(0.0, 0.0, 1.0), radius0, radius1, rf_num, vf_num);
	createObject(&cTorus, DataType::TSPLINEU_TYPE);


}

void AMCore::createCircular(const MPoint3& center, double radius, double angle, size_t c_num)
{
	if (c_num <= 0)
		return;


	CreateOperate_CircularTSpline cCircular(center.Coord(), AMCAX::Coord3(0.0, 0.0, 1.0), radius, angle, c_num);
	createObject(&cCircular, DataType::TSPLINEU_TYPE);


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

DataManager* AMCore::getDataManager() const
{
	return dataManager_;
}