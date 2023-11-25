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
#include <QVector>

#include "CoreDefine.h"
#include "../Object/ObjectDefine.h"
#include "../Record/Record.h"

namespace acamcad
{
	const QString FILE_EXT_AMCAX_BREP("abr"); 
	const QString FILE_EXT_STEP("step");
	const QString FILE_EXT_STP("stp");

	class DataManager;
	class BackupManager;
	class MOperate_SingleObject;
	class MultOperate;
	class UndoRedoHandler; 

	class AMCore : public QObject
	{
		Q_OBJECT

	public:
		AMCore();

		void init();

		~AMCore();

	public slots:
		void loadObjects();
		void saveObjects();
		void saveObjects(const QString& filename);
		void saveSelectedObjects();

	private:
		void loadObjectsFromFile(const QString& filename);

	private:
		void saveObjectsToFile(const QString& filename, std::vector<acamcad::BaseObject*>::const_iterator& it_begin, std::vector<acamcad::BaseObject*>::const_iterator& it_end);

	public:

		//=============== BRep =======
		void createBRepObject(acamcad::MOperation* operate, const std::string& label);
		int getLastPersistentId();

		void createPlaneBRepObject(const AMCAX::Coord3& first, const AMCAX::Coord3& second, const std::string& label = "");
		void createCubeBRepObject(const AMCAX::Coord3& first, const AMCAX::Coord3& second, const std::string& label = "");
		void createSphereBRepObject(const AMCAX::Coord3& center, double radius, double angle1, double angle2, double angle3, const std::string& label = "");
		void createCylinderBRepObject(const AMCAX::Coord3& b_center, const AMCAX::Coord3& axis, double radius, double height, double angle, const std::string& label = "");
		void createCylinderBRepObject(const AMCAX::Coord3& b_center, const AMCAX::Coord3& axis, const AMCAX::Coord3& xaxis, double radius, double height, double angle, const std::string& label = "");
		void createConeBRepObject(const AMCAX::Coord3& b_center, const AMCAX::Coord3& axis, double radiusR, double radiusr, double height, double angle, const std::string& label = "");
		void createConeBRepObject(const AMCAX::Coord3& b_center, const AMCAX::Coord3& axis, const AMCAX::Coord3& xaxis, double radiusR, double radiusr, double height, double angle, const std::string& label = "");
		void createTorusBRepObject(const AMCAX::Coord3& center, const AMCAX::Coord3& axis, double radius0, double radius1, double angle, const std::string& label = "");
		void createTorusBRepObject(const AMCAX::Coord3& center, const AMCAX::Coord3& axis, const AMCAX::Coord3& xaxis, double radius0, double radius1, double angle, const std::string& label = "");
		void createPolygonBRepObject(const std::vector<AMCAX::Coord3d>& points, const std::string& label = "");
		void createPrismBRepObject(const std::vector<AMCAX::Coord3d>& points, const AMCAX::Coord3d& prism_axis, const std::string& label = "");
		void createRoundedPolygonBRepObject(const std::vector<AMCAX::Coord3>& points, const std::string& label = "");
		void createRoundedPrismBRepObject(const std::vector<AMCAX::Coord3>& points, const AMCAX::Coord3& axis, const std::string& label = "");

		void createWedgeBRepObject(const double dx, const double dy, const double dz, const double ltx, const std::string& label = "");
		void createWedgeBRepObject(const AMCAX::Coord3& center, const double dx, const double dy, const double dz, const double ltx, const std::string& label = "");
		void createWedgeBRepObject(const AMCAX::Coord3& center, const AMCAX::Coord3& axis, const double dx, const double dy, const double dz, const double ltx, const std::string& label = "");
		void createWedgeBRepObject(const double dx, const double dy, const double dz, const double xmin, const double zmin, const double xmax, const double zmax, const std::string& label = "");
		void createWedgeBRepObject(const AMCAX::Coord3& center, const double dx, const double dy, const double dz, const double xmin, const double zmin, const double xmax, const double zmax, const std::string& label = "");
		void createWedgeBRepObject(const AMCAX::Coord3& center, const AMCAX::Coord3& axis, const double dx, const double dy, const double dz, const double xmin, const double zmin, const double xmax, const double zmax, const std::string& label = "");

	public:
		// 根据已有的选择的信息，返回与选择的单元关联的单元，例如，根据一条边界边选中一整条边界。
		void getSelectedObject_Element_Related(std::vector<SelectInfoWithCamera>& select_info_list, SelectModel type_);
		void getSelectedObject_Plane(const std::vector<SelectInfoWithCamera>& select_info_list, SelectModel type_, AMCAX::Coord3& center, AMCAX::Coord3& normal);

		// 根据选择信息SelectInfo，返回选中单元的中心点
		AMCAX::Coord3 getSelectedObject_Center(const SelectInfoWithCamera& select_info, SelectModel type_);
		AMCAX::Coord3 getSelectedObject_Normal(const SelectInfoWithCamera& select_info, SelectModel type_);

		void getSelectedObject_BBox(const SelectInfoWithCamera& select_info, SelectModel type_, AMCAX::Coord3& bbmin, AMCAX::Coord3& bbmax);

	public:
		void createBackupSInfo(std::vector<SelectInfoWithCamera>& select_info);

	public:
		void beginOperationByAxis(std::vector<SelectInfoWithCamera>& s_info_list, SelectModel type_);
		// 缩放物体，根据scale_type，0：单轴缩放；	1：平面缩放；		2：中心缩放
		void scaleSelectedObject(std::vector<SelectInfoWithCamera>& s_info_list, SelectModel type_, const AMCAX::Coord3& center, const AMCAX::Coord3& axis, double scale, int scale_type = 0);
		void rotateSelectedObject(std::vector<SelectInfoWithCamera>& s_info_list, SelectModel type_, const AMCAX::Coord3& center, const AMCAX::Coord3& axis, double alpha);
		void rotateSelectedObject(std::vector<BaseObject*>& object_list, SelectModel type_, const AMCAX::Coord3& center, const AMCAX::Coord3& axis, double scale, const std::string& label);
		void moveSelectedObject(std::vector<BaseObject*>& object_list, SelectModel type_, const AMCAX::Coord3& trans, double distance, const std::string& label = "");
		void moveSelectedObject(std::vector<SelectInfoWithCamera>& s_info_list, SelectModel type_, const AMCAX::Coord3& trans, double distance);

		void endOperationByAxis(std::vector<SelectInfoWithCamera>& s_info, SelectModel type_);

	public:
		void deleteObject(BaseObject* obj);
		BaseObject* getObjectByPersistentID(const int pid) const;
		std::vector<BaseObject*> getObjectByLabel(const std::string& label) const;

	public:
		// 注意每个函数之能做一些对应的操作，要根据情况来看。
		MultOperate* getMultOperateByOperationType(const MeshOperationType& ot);
		void OperateSelectedObject(std::vector<BaseObject*> objectList, MultOperate* operate, const std::string& label = "");
		// 对选中的物体做对应的operation 根据mesh_op_type_的类型
		void OperateSelectedObject(const SelectModel& s_model, const SelectInfoWithCamera& s_info);
		void OperateSelectedObject(const SelectModel& s_model, const std::vector<SelectInfoWithCamera>& s_info_vector);

		void OperateSelectedObject(const SelectModel& s_model, const SelectInfoWithCamera& s_info, const AMCAX::Coord3& center, const AMCAX::Coord3& axis);
		void OperateSelectedObject(const SelectModel& s_model, const std::vector<SelectInfoWithCamera>& s_info_vector, const AMCAX::Coord3& center, const AMCAX::Coord3& axis);

		// 不要直接调用上面的函数，调用下面的函数，因为现在的群组备份有问题
		void OperateSelectedObject_Subset(const SelectModel& s_model, std::vector<SelectInfoWithCamera>& s_info_vector);

		// 对选中物体的选中单元做对应的operation 根据mesh_op_type_的类型， 附加一个额外的参数
		void OperateSelectedObject_Subset_Split(const SelectModel& s_model, const std::vector<SelectInfoWithCamera>& s_info_list, std::vector<AMCAX::Coord3>& point_list);
		void OperateSelectedObject_Subset_Extrude(const SelectModel& s_model, std::vector<SelectInfoWithCamera>& s_info_vector, AMCAX::Coord3& vector);

		//temp function for BRepExtrude
		void OperateSelectedObjectFaceExtrude(const SelectModel& s_model, std::vector<SelectInfoWithCamera>& s_info_vector, AMCAX::Coord3d& vector);


	public:
		/// 主要处理添加面，如果没有物体则创建，有一个物体则添加，有多个物体则先合并，再添加 
		void OperatePointlistWithSelectedObject(const SelectModel& s_model, std::vector<ClickInfo>& c_info_vector, std::vector<SelectInfoWithCamera>& s_info_vector);

	public:
		void Redo();
		void Undo();

	private:
		void createBackupNormalOp(BaseObject* object);

	public:
		void addNewDataTypeFile(const QString&, const DataType&);

		void setOperationDataType(OperationDataType type);
		void setMeshOperationType(MeshOperationType type);

		OperationDataType getOperationDataType() const { return op_datatype_; }
		MeshOperationType getMeshOperationType() const { return mesh_op_type_; }

	private:
		std::map<QString, DataType> file_allowed_type_;			//允许的文件类型，string对应到type

		OperationDataType	op_datatype_;	//only use it for some creat function
		MeshOperationType	mesh_op_type_;
		acamcad::Record* record_;

	private:
		void inlineSingleOperation();
		void inlineMultOperation();

		std::vector< MOperate_SingleObject* > single_operation_list_;
		std::vector< MultOperate* > mult_operation_list_;

	public:
		void setUndoRedoHandler(UndoRedoHandler* undoRedoHandler); 
		void setDataManager(DataManager* dataManager);
		DataManager* getDataManager() const;
		void setBackupManager(BackupManager* backManager);
		void setRecord(Record& record) { record_ = &record; }

	private:
		DataManager* dataManager_;
		BackupManager* backupManager_;
		UndoRedoHandler* undoRedoHandler_;
	};

} //namespace acamcad;
