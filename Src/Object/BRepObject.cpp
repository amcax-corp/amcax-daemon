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
#include "AdapterObject.h"
#include "AdapterObjectParent.h"

#include <topology/TopoExplorerTool.hpp>
#include <modeling/CopyShape.hpp>
#include <modeling/TransformShape.hpp>
#include <io/STLTool.hpp>
#include <io/OBJTool.hpp>
#include <common/VectorT.hpp>
#include <common/AxisT.hpp>
#include <common/FrameT.hpp>
#include <common/TransformationT.hpp>

#include <topology/TopoVertex.hpp>
#include <topology/TopoEdge.hpp>
#include <topology/TopoFace.hpp>
#include <topology/TopoExplorer.hpp>
#include <topology/TopoTool.hpp>
#include <math/PolygonOnTriangularMesh.hpp>
#include <math/TriangularMesh.hpp>
#include "../Utils/MathUtils.h"
#include <memory>

namespace acamcad
{
	BRepObject::BRepObject(AdapterObject* parent)
		:AdapterObjectParent(parent)//, draw_tool_(nullptr)
	{
		shape_.Nullify();
		//setDataType(DataType::BREP_TYPE);
		draw_tool_ = std::make_unique<BRepObjectDraw>(parent);
		//draw_tool_ = new BRepObjectDraw(parent);
		//updateShapeInfo();
		//draw_tool_->updatedrawState();
	}

	BRepObject::BRepObject(AdapterObject* parent, const BRepObject& object, bool isDraw) :AdapterObjectParent(parent)
	{
		if (!object.shape_.IsNull())
			shape_ = AMCAX::CopyShape(object.shape_);

		//setDataType(DataType::BREP_TYPE);
		if (isDraw)
			draw_tool_ = std::make_unique<BRepObjectDraw>(parent);

	}


	BRepObject::~BRepObject()
	{
		shape_.Nullify();

		//delete draw_tool_;
	}

	void BRepObject::doOperate(MOperation* operate)
	{
		operate->operate(parent_);
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
		parent_->setFromFileName(_filename);

		parent_->setName(parent_->name());

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

	void BRepObject::mirrorMeshObject(const AMCAX::Coord3& center, const AMCAX::Coord3& normal)
	{
		AMCAX::Point3 P(center.X(), center.Y(), center.Z());
		AMCAX::Direction3 v(normal.X(), normal.Y(), normal.Z());

		AMCAX::Transformation3 tr;
		tr.SetMirror(AMCAX::Frame3(P, v));
		AMCAX::TransformShape trans(brep_trans_back_, tr);

		shape_ = trans.Shape();

		//BRepObject* brep_object_new = new BRepObject();
		//brep_object_new->shape_ = trans.Shape();
		//brep_object_new->updateDraw();
		//return brep_object_new;
		//turn NULL;
	}

	// select vert edge face
	void BRepObject::selectVertWithFaceId(const AMCAX::Vector3& begin, const AMCAX::Vector3& end, int f_id, int& sv_id)
	{
		sv_id = -1;
		AMCAX::TopoVertex vMin;
		double min_dis = std::numeric_limits<double>::max();
		AMCAX::Coord3 ve_min;

		AMCAX::TopoShape f = getShapeFaces()[f_id];
		for (AMCAX::TopoExplorer expV(f, AMCAX::ShapeType::Vertex); expV.More(); expV.Next())
		{
			AMCAX::TopoVertex v = static_cast<const AMCAX::TopoVertex&>(expV.Current());
			AMCAX::Point3 p = AMCAX::TopoTool::Point(v);
			double len = MathUtils::distPointLineSquared(p.Coord(), begin.Coord(), end.Coord(), ve_min);
			if (min_dis > len)
			{
				min_dis = len;
				vMin = v;
			}
		}

		if (min_dis < 0.01)
			sv_id = getShapeVertices().index(vMin);
	}
	void BRepObject::selectEdgeWithFaceId(const AMCAX::Vector3& begin, const AMCAX::Vector3& end, int f_id, int& se_id)
	{
		se_id = -1;
		AMCAX::TopoEdge eMin;
		double min_dis = std::numeric_limits<double>::max();
		AMCAX::Coord3 ve_min(0.0, 0.0, 0.0), vL_min(0.0, 0.0, 0.0);

		AMCAX::TopoShape f = getShapeFaces()[f_id];
		for (AMCAX::TopoExplorer expE(f, AMCAX::ShapeType::Edge); expE.More(); expE.Next())
		{
			const AMCAX::TopoEdge& e = static_cast<const AMCAX::TopoEdge&>(expE.Current());
			AMCAX::TopoLocation loc;
			std::shared_ptr<AMCAX::TriangularMesh> triMesh;
			std::shared_ptr<AMCAX::PolygonOnTriangularMesh> polyMesh;
			AMCAX::TopoTool::PolygonOnTriangulation(e, polyMesh, triMesh, loc);
			const AMCAX::Transformation3& tr = loc.Transformation();
			for (int pid = 1; pid < polyMesh->NVertices(); ++pid)
			{
				const AMCAX::Point3& p0 = triMesh->Vertex(polyMesh->Vertex(pid - 1)).Transformed(tr);
				const AMCAX::Point3& p1 = triMesh->Vertex(polyMesh->Vertex(pid)).Transformed(tr);

				double len = MathUtils::distLine_SLineSquared(p0.Coord(), p1.Coord(), begin.Coord(), end.Coord(), ve_min, vL_min);
				if (len < min_dis)
				{
					min_dis = len;
					eMin = e;
				}
			}
		}

		if (min_dis < 0.01)
			se_id = getShapeEdges().index(eMin);
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
		output += parent_->getObjectinfo();

		output += "Object Contains BRep : ";
		output += QString::number(shapeVertices_.size()) + " vertices, ";
		output += QString::number(shapeEdges_.size()) += " edges ";
		output += QString::number(shapeFaces_.size()) += " faces.\n";

		output += "========================================================================\n";
		return output;
	}

}
