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
#include "BRepObject.h"
#include "../ObjectDraw/BrepObjectDraw.h"
#include "../Operation/MOperate.h"

#include <topology/TopoExplorerTool.hpp>
#include <modeling/CopyShape.hpp>
#include <modeling/TransformShape.hpp>
#include <io/STLTool.hpp>
#include <io/OBJTool.hpp>
#include <common/VectorT.hpp>
#include <common/AxisT.hpp>
#include <common/FrameT.hpp>
#include <common/TransformationT.hpp>

namespace acamcad
{
	BRepObject::BRepObject()
		:BaseObject(), draw_tool_(nullptr)
	{
		shape_.Nullify();
		setDataType(DataType::BREP_TYPE);
		draw_tool_ = new BRepObjectDraw(this);
		updateShapeInfo();
		//draw_tool_->updatedrawState();
	}

	BRepObject::BRepObject(const BRepObject& object)
	{
		if (!object.shape_.IsNull())
			shape_ = AMCAX::CopyShape(object.shape_);

		setDataType(DataType::BREP_TYPE);
		draw_tool_ = new BRepObjectDraw(this);

		updateShapeInfo();
		draw_tool_->updatedrawState();
	}


	BRepObject::~BRepObject()
	{
		shape_.Nullify();
	}

	void BRepObject::doOperate(MOperation* operate)
	{
		operate->operateWithBRep(this);
		updateShapeInfo();
		draw_tool_->updatedrawState();
	}

	void BRepObject::updateDraw()
	{
		updateShapeInfo();
		draw_tool_->updatedrawState();
	}

	AMCAX::TopoShape& BRepObject::getShape()
	{
		return shape_;
	}

	const AMCAX::TopoShape& BRepObject::getShape() const
	{
		return shape_;
	}

	const AMCAX::IndexSet<AMCAX::TopoShape>& BRepObject::getShapeVertices()
	{
		return shapeVertices_;
	}

	const AMCAX::IndexSet<AMCAX::TopoShape>& BRepObject::getShapeEdges()
	{
		return shapeEdges_;
	}

	const AMCAX::IndexSet<AMCAX::TopoShape>& BRepObject::getShapeFaces()
	{
		return shapeFaces_;
	}

	void BRepObject::assignBRep()
	{
		shape_ = AMCAX::TopoShape();
	}

	void BRepObject::SetShape(const AMCAX::TopoShape& shape)
	{
		shape_ = shape;
	}

	void BRepObject::backBRep(AMCAX::TopoShape& back_brep)
	{
		if (!shape_.IsNull())
		{
			back_brep = AMCAX::CopyShape(shape_);
		}
		else
			back_brep.Nullify();
	}

	void BRepObject::applyBRep(const AMCAX::TopoShape& apply_brep)
	{
		shape_ = AMCAX::CopyShape(apply_brep);
	}

	bool BRepObject::loadBRep(const QString& _filename)
	{
		setFromFileName(_filename);

		setName(name());

		std::string	filename = _filename.toStdString();
		
        bool ok = /*AMCAX::BRepTool::Read(shape_, filename, -1);
		if (ok)
		{
            updateDraw();
			show();
        }*/ true;

		return ok;
	}

	bool BRepObject::saveBRep(const QString& _filename)
	{
		std::string filename = _filename.toStdString();

        bool ok = /*AMCAX::BRepTool::Write(shape_, filename, -1);*/ true;
		
        return ok;
	}

	bool BRepObject::saveBRepSTL(const QString& filename)
	{
		updateDraw();
		return AMCAX::STLTool::WriteShape(shape_, filename.toStdString());
	}

	bool BRepObject::saveBRepOBJ(const QString& filename)
	{
		updateDraw();
		return AMCAX::OBJTool::WriteShape(shape_, filename.toStdString());
	}

	void BRepObject::mesh_OPvert_Backup()
	{
		brep_trans_back_ = AMCAX::CopyShape(shape_);
	}

	void BRepObject::mesh_Clear_OPvertBackup()
	{
		brep_trans_back_.Nullify();
	}

	void BRepObject::meshRotation(const AMCAX::Coord3& center, const AMCAX::Coord3& axis, double angle)
	{
		AMCAX::Point3 P(center.X(), center.Y(), center.Z());
		AMCAX::Direction3 v(axis.X(), axis.Y(), axis.Z());

		AMCAX::Transformation3 tr;
		tr.SetRotation(AMCAX::Axis3(P, v), angle);
		AMCAX::TransformShape trans(shape_, tr);
		shape_ = trans.Shape();

		updateShapeInfo();
		draw_tool_->updatedrawState();
	}

	void BRepObject::meshMove(const AMCAX::Coord3& vec, double dis)
	{
		AMCAX::VectorT<double, 3> v(dis * vec.X(), dis * vec.Y(), dis * vec.Z());

		AMCAX::Transformation3 tr;
		tr.SetTranslation(v);
		AMCAX::TransformShape trans(shape_, tr);
		shape_ = trans.Shape();
		
		updateShapeInfo();
		draw_tool_->updatedrawState();
	}

	BRepObject* BRepObject::mirrorMeshObject(const AMCAX::Coord3& center, const AMCAX::Coord3& normal)
	{
		AMCAX::Point3 P(center.X(), center.Y(), center.Z());
		AMCAX::Direction3 v(normal.X(), normal.Y(), normal.Z());

		AMCAX::Transformation3 tr;
		tr.SetMirror(AMCAX::Frame3(P, v));
		AMCAX::TransformShape trans(brep_trans_back_, tr); 

		BRepObject* brep_object_new = new BRepObject();
	//	brep_object_new->shape_ = trans.Shape();
		brep_object_new->updateDraw();
		return brep_object_new;
	}

	void BRepObject::updateShapeInfo()
	{
		shapeFaces_.clear();
		shapeEdges_.clear();
		shapeVertices_.clear();
		AMCAX::TopoExplorerTool::MapShapes(shape_, AMCAX::ShapeType::Face, shapeFaces_);
		AMCAX::TopoExplorerTool::MapShapes(shape_, AMCAX::ShapeType::Edge, shapeEdges_);
		AMCAX::TopoExplorerTool::MapShapes(shape_, AMCAX::ShapeType::Vertex, shapeVertices_);
	}

	QString BRepObject::getObjectinfo()
	{
		QString output;

		updateShapeInfo();

		output += "========================================================================\n";
		output += BaseObject::getObjectinfo();

		output += "Object Contains BRep : ";
		output += QString::number(shapeVertices_.size()) + " vertices, ";
		output += QString::number(shapeEdges_.size()) += " edges ";
		output += QString::number(shapeFaces_.size()) += " faces.\n";

		output += "========================================================================\n";
		return output;
	}

}
