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
#include "../Operation/OperationDefine.h"

#include <cmath>
#include "UndoRedoHandler.h"
#include "../Utils/MathUtils.h"

#define M_EPSILON 1.0e-8

namespace acamcad
{
	bool CheckCubeParamOK(const AMCAX::Coord3& first, const AMCAX::Coord3& second, const bool suppress = false)
	{
		// check if the two MPoints are on a same panel
		if (std::fabs(first.X() - second.X()) < M_EPSILON ||
			std::fabs(first.Y() - second.Y()) < M_EPSILON ||
			std::fabs(first.Z() - second.Z()) < M_EPSILON)
		{
			if (!suppress) { std::cout << __func__ << " param error " << std::endl; }
			return false;
		}

		return true;
	}

	bool CheckSphereParamOK(const double radius, const double angle, const bool suppress = false)
	{
		// is a positive valid number
		if (!std::isnan(radius) && !std::isinf(radius) && radius > M_EPSILON &&
			!std::isnan(angle) && !std::isinf(angle) && angle > M_EPSILON && angle < 2.0 * M_PI + M_EPSILON)
		{
			return true;
		}
		if (!suppress) { std::cout << __func__ << " param error " << std::endl; }
		return false;
	}

	bool CheckCylinderParamOK(const AMCAX::Coord3& axis,
		const double radius, const double height, const double angle, const bool suppress = false)
	{
		// is a valid number
		bool arenum = (!std::isnan(radius)) && (!std::isnan(height)) && (!std::isnan(angle));
		arenum &= (!std::isinf(radius)) && (!std::isinf(height)) && (!std::isinf(angle));
		// is positive
		bool arepositive = (radius > M_EPSILON) && (height > M_EPSILON);
		// if the axis is valid
		bool axisvalid = axis.Norm() > M_EPSILON;
		bool anglevalid = (angle > M_EPSILON) && (angle < 2.0 * M_PI + M_EPSILON);

		if (arenum && arepositive && axisvalid && anglevalid)
		{
			return true;
		}
		if (!suppress) { std::cout << __func__ << " param error " << std::endl; }
		return false;
	}

	bool CheckCylinderParamOK(const AMCAX::Coord3& axis, const AMCAX::Coord3& xaxis,
		const double radius, const double height, const double angle, const double suppress = false)
	{
		bool othervalid = CheckCylinderParamOK(axis, radius, height, angle);
		bool xaxisvalid = xaxis.Norm() > M_EPSILON;
		bool orth = axis.Dot(xaxis) == 0;

		if (othervalid && xaxisvalid && orth)
		{
			return true;
		}
		if (!suppress) { std::cout << __func__ << " param error " << std::endl; }
		return false;
	}

	bool CheckConeParamOK(const AMCAX::Coord3& axis,
		const double r1, const double r2, const double h, const double angle, const bool suppress = false)
	{
		int cR1 = ::fpclassify(r1);
		int cR2 = ::fpclassify(r2);
		int cH = ::fpclassify(h);
		int cA = ::fpclassify(angle);

		bool arenum = (cR1 != FP_NAN) && (cR2 != FP_NAN) && (cH != FP_NAN) && (cA != FP_NAN);
		arenum &= (cR1 != FP_INFINITE) && (cR2 != FP_INFINITE) && (cH != FP_INFINITE) && (cA != FP_INFINITE);

		bool arepositive = ((r2 > M_EPSILON && r2 > M_EPSILON) || // both positive, truncated cone
			(cR1 == FP_ZERO && r2 > M_EPSILON) || (cR2 == FP_ZERO && r1 > M_EPSILON)) && // regular cone
			(h > M_EPSILON);

		// if it's a cylinder, call make cylinder instead
		bool cylinder = std::fabs(r1 - r2) < M_EPSILON;

		bool anglevalid = (angle > M_EPSILON) && (angle < 2.0 * M_PI + M_EPSILON);

		// if the axis is valid
		bool axisvalid = axis.Norm() > M_EPSILON;

		if (arenum && arepositive && !cylinder && anglevalid && axisvalid)
		{
			return true;
		}
		if (!suppress) { std::cout << __func__ << " param error " << std::endl; }
		return false;
	}

	bool CheckConeParamOK(const AMCAX::Coord3& axis, const AMCAX::Coord3& xaxis,
		const double r1, const double r2, const double h, const double angle, const bool suppress = false)
	{
		bool othervalid = CheckConeParamOK(axis, r1, r2, h, angle, true);
		bool xaxisvalid = xaxis.Norm() > M_EPSILON;
		bool orth = axis.Dot(xaxis) == 0;

		if (othervalid && xaxisvalid && orth)
		{
			return true;
		}
		if (!suppress) { std::cout << __func__ << " param error " << std::endl; }
		return false;
	}

	bool CheckTorusParamOK(const AMCAX::Coord3& axis,
		const double r1, const double r2, const double angle, const bool suppress = false)
	{
		int cR1 = ::fpclassify(r1);
		int cR2 = ::fpclassify(r2);
		int cA = ::fpclassify(angle);

		bool arenum = (cR1 != FP_NAN) && (cR2 != FP_NAN) && (cA != FP_NAN)
			&& (cR1 != FP_INFINITE) && (cR2 != FP_INFINITE) && (cA != FP_INFINITE);

		bool arepositive = (r1 > M_EPSILON && r2 > M_EPSILON);

		bool anglevalid = (angle > M_EPSILON) && (angle < 2.0 * M_PI + M_EPSILON);

		bool axisvalid = axis.Norm() > M_EPSILON;

		if (arenum && arepositive && anglevalid && axisvalid)
		{
			return true;
		}
		if (!suppress) { std::cout << __func__ << "0 param error " << std::endl; }
		return false;
	}

	bool CheckTorusParamOK(const AMCAX::Coord3& axis, const AMCAX::Coord3& xaxis,

		const double r1, const double r2, const double angle, const bool suppress = false)
	{
		bool othervalid = CheckTorusParamOK(axis, r1, r2, angle, true);
		bool xaxisvalid = xaxis.Norm() > M_EPSILON;
		bool orth = axis.Dot(xaxis) == 0;

		if (othervalid && xaxisvalid && orth)
		{
			return true;
		}
		if (!suppress) { std::cout << __func__ << "1 param error " << std::endl; }
		return false;
	}

	bool CheckPolygonParamOK(const std::vector<AMCAX::Coord3d>& points, const bool suppress = false)
	{
		if (points.size() >= 3 && MathUtils::isOnSamePlane(points))
		{
			return true;
		}
		if (!suppress) { std::cout << __func__ << " param error " << std::endl; }
		return false;
	}

	bool CheckPrismParamOK(const std::vector<AMCAX::Coord3d>& points, const AMCAX::Coord3d& prism, const bool suppress = false)
	{
		if (CheckPolygonParamOK(points, true) && prism.Norm() > M_EPSILON)
		{
			return true;
		}
		if (!suppress) { std::cout << __func__ << " param error " << std::endl; }
		return false;
	}
}

namespace acamcad
{
	//=================================== BRep ===================================

	void AMCore::createBRepObject(MOperation* operate, const std::string& label)
	{
		AdapterObject* adapter_object = new AdapterObject;
		adapter_object->setDataType(acamcad::DataType::BREP_TYPE);
		//adapter_object->bRep = std::make_unique<BRepObject>(adapter_object);

		//BRepObject* newObject = new BRepObject();
		adapter_object->setLabel(label);
		operate->operate(adapter_object);
		//adapter_object->doOperate(operate);

		dataManager_->addObject(adapter_object);
		dataManager_->RecordAddObject({ adapter_object });
		//undoRedoHandler_->recordOperation(adapter_object, ActionType::AddObject);

	}

	int AMCore::getLastPersistentId() {
		return dataManager_->getLastPersistentId();
	}

	void AMCore::createPlaneBRepObject(const AMCAX::Coord3& first, const AMCAX::Coord3& second, const std::string& label)
	{
		AMCAX::Coord2 p0p(first.X(), first.Y());
		AMCAX::Coord2 p1p(second.X(), second.Y());

		CreateOperate_Plane cPlane(p0p, p1p, 1, 1);

		createBRepObject(&cPlane, label);
	}

	void AMCore::createCubeBRepObject(const AMCAX::Coord3& first, const AMCAX::Coord3& second, const std::string& label)
	{
		if (!CheckCubeParamOK(first, second))
			return;

		CreateOperate_Cube cCube(first, second, 1, 1, 1);


		createBRepObject(&cCube, label);
	}

	void AMCore::createSphereBRepObject(const AMCAX::Coord3& center, double radius,
		double angle1, double angle2, double angle3,
		const std::string& label)
	{
		if (!CheckSphereParamOK(radius, angle1))
		{
			return; // angle2 and angle3 is currently not used. range unknown.
		}

		CreateOperate_Sphere cSphere(center, radius, angle1, angle2, angle3);

		createBRepObject(&cSphere, label);
	}

	void AMCore::createCylinderBRepObject(const AMCAX::Coord3& b_center, const AMCAX::Coord3& axis,
		double radius, double height, double angle,
		const std::string& label)
	{
		if (!CheckCylinderParamOK(axis, radius, height, angle))
		{
			return;
		}

		CreateOperate_Cylinder cCylinder(b_center, radius, height, axis, angle);

		createBRepObject(&cCylinder, label);
	}

	void AMCore::createCylinderBRepObject(const AMCAX::Coord3& b_center, const AMCAX::Coord3& axis, const AMCAX::Coord3& xaxis,
		double radius, double height, double angle,
		const std::string& label)
	{
		if (!CheckCylinderParamOK(axis, xaxis, radius, height, angle))
		{
			return;
		}

		CreateOperate_Cylinder cCylinder(b_center, radius, height, axis, xaxis, angle);

		createBRepObject(&cCylinder, label);
	}

	void AMCore::createConeBRepObject(const AMCAX::Coord3& b_center, const AMCAX::Coord3& axis,
		double radiusR, double radiusr, double height,
		double angle, const std::string& label)
	{
		if (!CheckConeParamOK(axis, radiusR, radiusr, height, angle))
		{
			return;
		}

		CreateOperate_Cone cTCone(b_center, axis, radiusR, radiusr, height, angle);

		createBRepObject(&cTCone, label);
	}

	void AMCore::createConeBRepObject(const AMCAX::Coord3& b_center, const AMCAX::Coord3& axis, const AMCAX::Coord3& xaxis,
		double radiusR, double radiusr, double height, double angle, const std::string& label)
	{
		if (!CheckConeParamOK(axis, xaxis, radiusR, radiusr, height, angle))
		{
			return;
		}

		CreateOperate_Cone cTCone(b_center, axis, xaxis, radiusR, radiusr, height, angle);

		createBRepObject(&cTCone, label);
	}

	void AMCore::createTorusBRepObject(const AMCAX::Coord3& center, const AMCAX::Coord3& axis,
		double radius0, double radius1,
		double angle,
		const std::string& label)
	{
		if (!CheckTorusParamOK(axis, radius0, radius1, angle))
		{
			return;
		}
		CreateOperate_Torus cTorus(center, radius0, radius1, axis, angle);

		createBRepObject(&cTorus, label);
	}

	void AMCore::createTorusBRepObject(const AMCAX::Coord3& center, const AMCAX::Coord3& axis, const AMCAX::Coord3& xaxis,
		double radius0, double radius1, double angle,
		const std::string& label)
	{
		if (!CheckTorusParamOK(axis, xaxis, radius0, radius1, angle))
		{
			return;
		}

		CreateOperate_Torus cTorus(center, radius0, radius1, axis, xaxis, angle);

		createBRepObject(&cTorus, label);
	}

	void AMCore::createPolygonBRepObject(const std::vector<AMCAX::Coord3d>& points, const std::string& label)
	{
		if (!CheckPolygonParamOK(points))
		{
			return;
		}
		CreateOperate_Polygon cPolygon(points);

		createBRepObject(&cPolygon, label);
	}

	void AMCore::createPrismBRepObject(const std::vector<AMCAX::Coord3d>& points, const AMCAX::Coord3d& prism_axis, const std::string& label)
	{
		if (!CheckPrismParamOK(points, prism_axis))
		{
			return;
		}
		CreateOperate_Prism cPrism(points, prism_axis);

		createBRepObject(&cPrism, label);
	}

	void AMCore::createRoundedPolygonBRepObject(const std::vector<AMCAX::Coord3>& points, const std::string& label)
	{
		// TODO: CheckParamOk
		CreateOperate_RoundedPolygon cRPolygon(points);

		createBRepObject(&cRPolygon, label);
	}

	void AMCore::createRoundedPrismBRepObject(const std::vector<AMCAX::Coord3>& points, const AMCAX::Coord3& prism_axis, const std::string& label)
	{
		// TODO: CheckParamOk
		CreateOperate_RoundedPrism cRPrism(points, prism_axis);

		createBRepObject(&cRPrism, label);
	}

	void AMCore::createWedgeBRepObject(const double dx, const double dy, const double dz, const double ltx,
		const std::string& label)
	{
		CreateOperate_Wedge cWedge(dx, dy, dz, ltx);

		createBRepObject(&cWedge, label);
	}

	void AMCore::createWedgeBRepObject(const AMCAX::Coord3& center,
		const double dx, const double dy, const double dz, const double ltx,
		const std::string& label)
	{
		CreateOperate_Wedge cWedge(center, dx, dy, dz, ltx);

		createBRepObject(&cWedge, label);
	}

	void AMCore::createWedgeBRepObject(const AMCAX::Coord3& center, const AMCAX::Coord3& axis,
		const double dx, const double dy, const double dz, const double ltx,
		const std::string& label)
	{
		CreateOperate_Wedge cWedge(center, axis, dx, dy, dz, ltx);
		createBRepObject(&cWedge, label);
	}

	void AMCore::createWedgeBRepObject(const double dx, const double dy, const double dz,
		const double xmin, const double zmin, const double xmax, const double zmax,
		const std::string& label)
	{
		CreateOperate_Wedge cWedge(dx, dy, dz, xmin, zmin, xmax, zmax);

		createBRepObject(&cWedge, label);
	}

	void AMCore::createWedgeBRepObject(const AMCAX::Coord3& center,
		const double dx, const double dy, const double dz,
		const double xmin, const double zmin, const double xmax, const double zmax,
		const std::string& label)
	{
		CreateOperate_Wedge cWedge(center, dx, dy, dz, xmin, zmin, xmax, zmax);

		createBRepObject(&cWedge, label);
	}

	void AMCore::createWedgeBRepObject(const AMCAX::Coord3& center, const AMCAX::Coord3& axis,
		const double dx, const double dy, const double dz, const double xmin, const double zmin,
		const double xmax, const double zmax, const std::string& label)
	{
		CreateOperate_Wedge cWedge(center, axis, dx, dy, dz, xmin, zmin, xmax, zmax);

		createBRepObject(&cWedge, label);
	}

	//Mesh
	void AMCore::createPlaneMeshObject(MPoint3& p0, MPoint3& p1, size_t u_num, size_t v_num)
	{
		//MPlane plane;
		AMCAX::Coord2 p0p(p0.X(), p0.Y());
		AMCAX::Coord2 p1p(p1.X(), p1.Y());
		CreateOperate_PlaneMesh cPlane(p0p, p1p, u_num, v_num);

		AdapterObject* adapter = new AdapterObject;
		adapter->setDataType(acamcad::DataType::MESH_TYPE);

		//adapter->doOperate(&cPlane);

		cPlane.operate(adapter);


		//MeshObject* newObject = new MeshObject();
		//newObject->doOperate(&cPlane);

		dataManager_->addObject(adapter);
		//createBackupAdd(newObject);
		//undoRedoHandler_->recordOperation(adapter, ActionType::AddObject);
		dataManager_->RecordAddObject({ adapter });
	}

	void AMCore::createCubeMeshObject(const MPoint3& first, const MPoint3& second,
		size_t x_num, size_t y_num, size_t z_num)
	{
		AMCAX::Coord3 c0(first.X(), first.Y(), first.Z()), c1(second.X(), second.Y(), second.Z());

		CreateOperate_CubeMesh cCube(c0, c1, x_num, y_num, z_num);

		AdapterObject* adapter = new AdapterObject;
		adapter->setDataType(acamcad::DataType::MESH_TYPE);

		cCube.operate(adapter);


		dataManager_->addObject(adapter);
		//dataManager_->addObject(adapter);
		//createBackupAdd(newObject);
		//undoRedoHandler_->recordOperation(adapter, ActionType::AddObject);
		dataManager_->RecordAddObject({ adapter });
	}

	void AMCore::createPlaneObject(MPoint3& p0, MPoint3& p1, size_t u_num, size_t v_num)
	{
		//MPlane plane;
		AMCAX::Coord2 p0p(p0.X(), p0.Y());
		AMCAX::Coord2 p1p(p1.X(), p1.Y());
		CreateOperate_PlaneTSpline cPlane(p0p, p1p, u_num, v_num);

		AdapterObject* adapter = new AdapterObject;
		adapter->setDataType(acamcad::DataType::TSPLINEU_TYPE);

		cPlane.operate(adapter);

		dataManager_->addObject(adapter);

		dataManager_->RecordAddObject({ adapter });

		//createBackupAdd(newObject);
	}

	void AMCore::createCubeObject(const MPoint3& first, const MPoint3& second, size_t x_num, size_t y_num, size_t z_num)
	{
		AMCAX::Coord3 min_bb = first.Coord(),
			max_bb = second.Coord();
		CreateOperate_CubeTSpline cCube(min_bb, max_bb, x_num, y_num, z_num);


		AdapterObject* adapter = new AdapterObject;
		adapter->setDataType(acamcad::DataType::TSPLINEU_TYPE);

		cCube.operate(adapter);

		dataManager_->addObject(adapter);

		dataManager_->RecordAddObject({ adapter });
	}


	void AMCore::createUVSphereObject(const MPoint3& center, double radius, size_t rf_num, size_t vf_num)
	{
		//MSphere sphere(center, radius);
		CreateOperate_SphereTSpline cSphere(center.Coord(), radius, rf_num, vf_num);

		AdapterObject* adapter = new AdapterObject;
		adapter->setDataType(acamcad::DataType::TSPLINEU_TYPE);

		cSphere.operate(adapter);

		dataManager_->addObject(adapter);

		dataManager_->RecordAddObject({ adapter });
	}

	//===========================================================
	void AMCore::OperatePointlistWithSelectedObject(const SelectModel& s_model, std::vector<ClickInfo>& c_info_vector,
		std::vector<SelectInfoWithCamera>& s_info_vector)
	{
		if (s_model == SelectModel::CLICK_MODEL && mesh_op_type_ == MeshOperationType::MeshFaceAppend)
		{

			///MOperation* single;
			acamcad::AdapterObject* newObject;

			switch (op_datatype_)
			{
			case DataType::BREP_TYPE:
			{
				std::vector<AMCAX::Coord3> point_list = getClickPointlist(c_info_vector);
				newObject = new acamcad::AdapterObject;
				newObject->setDataType(op_datatype_);
				CreateOperate_SingleFace single(point_list);
				if (!single.operate(newObject)) {
					delete newObject;
					return;
				}

				break;
			}
			case DataType::MESH_TYPE:
			{
				///std::vector<AMCAX::Coord3> point_list = getClickPointlist(c_info_vector);
				//single = new CreateOperate_SingleFace(point_list);
				std::vector<AMCAX::Coord3> point_list = getClickPointlist(c_info_vector);
				newObject = new acamcad::AdapterObject;
				newObject->setDataType(op_datatype_);
				CreateOperate_SingleFaceMesh single(point_list);
				if (!single.operate(newObject)) {
					delete newObject;
					return;
				}
				break;
			}
			case DataType::TSPLINEU_TYPE:
			{
				std::vector<AMCAX::Coord3> point_list = getClickPointlist(c_info_vector);
				newObject = new acamcad::AdapterObject;
				newObject->setDataType(op_datatype_);
				CreateOperate_SingleFaceTSpline single(point_list);
				if (!single.operate(newObject)) {
					delete newObject;
					return;
				}
				break;
			}
			default:
				return;
			}

			//std::vector<AMCAX::Coord3> point_list = getClickPointlist(c_info_vector);

			//BaseObject* newObject = nullptr;

			//CreateOperate_SingleFace* single = new CreateOperate_SingleFace(point_list);
			//single->setObjectType(op_datatype_);

			//newObject = single->operateWithBRep();

			dataManager_->addObject(newObject);
			//undoRedoHandler_->recordOperation(newObject, ActionType::AddObject);
			dataManager_->RecordAddObject({ newObject });

			//delete single;




		}
	}


}


