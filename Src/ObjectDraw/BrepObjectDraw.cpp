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
#include "BrepObjectDraw.h"

#include <topology/TopoExplorerTool.hpp>
#include <topology/TopoTool.hpp>
#include <topology/TopoVertex.hpp>
#include <topology/TopoEdge.hpp>
#include <topology/TopoFace.hpp>
#include <topomesh/BRepMeshIncrementalMesh.hpp>
#include <common/VectorT.hpp>

#include <modeling/MakeShapeTool.hpp>
#include <geometry/Geom3Curve.hpp>
#include <geometry/Geom3BSplineCurve.hpp>
#include "../Object/AdapterObject.h"

acamcad::BRepObjectDraw::BRepObjectDraw(AdapterObject* object) :ObjectDraw(object)
{
	//brep_object_ = object;
}

acamcad::BRepObjectDraw::~BRepObjectDraw()
{
	//brep_object_ = nullptr;
}

void acamcad::BRepObjectDraw::draw(const DrawModel& dmodel, const SelectModel& smodel) const
{
	bool is_show_point = false;
	if (smodel == SelectModel::VERTEX_MODEL)
		is_show_point = true;

	if (!parent_->isVisible())
		return;

	draw(dmodel, is_show_point);
}

void acamcad::BRepObjectDraw::drawSelected(const SelectModel& s_model, int s_id) const
{
	if (!parent_->bRep)
		return;

	if (s_model == SelectModel::VERTEX_MODEL)
	{
		glColor3f(0.85, 0.85, 0);
		glPointSize(8);

		const AMCAX::IndexSet<AMCAX::TopoShape>& vertexs = parent_->bRep->getShapeVertices();
		const AMCAX::TopoVertex& v = static_cast<const AMCAX::TopoVertex&>(vertexs[s_id]);

		if (!v.IsNull())
		{
			AMCAX::Point3 vp = AMCAX::TopoTool::Point(v);

			glBegin(GL_POINTS);
			glVertex3d(vp.X(), vp.Y(), vp.Z());
			glEnd();
		}
	}
	if (s_model == SelectModel::EDGE_MODEL)
	{
		glColor3f(0.85, 0.85, 0);
		glLineWidth(6);

		if (s_id < 0 || s_id > edgeInfos_.size())
		{
			std::cout << "error brep edge index " << std::endl;
			return;
		}

		std::shared_ptr<AMCAX::TriangularMesh> mesh = edgeInfos_[s_id].triMesh;
		std::shared_ptr<AMCAX::PolygonOnTriangularMesh> poly = edgeInfos_[s_id].polyMesh;
		AMCAX::Transformation3 tr = edgeInfos_[s_id].loc.Transformation();

		for (int pid = 1; pid < poly->NVertices(); ++pid)
		{
			const AMCAX::Point3& p0 = mesh->Vertex(poly->Vertex(pid - 1)).Transformed(tr);
			const AMCAX::Point3& p1 = mesh->Vertex(poly->Vertex(pid)).Transformed(tr);

			glBegin(GL_LINES);
			glVertex3d(p0.X(), p0.Y(), p0.Z());
			glVertex3d(p1.X(), p1.Y(), p1.Z());
			glEnd();
		}
	}
	if (s_model == SelectModel::FACE_MODEL)
	{
		if (s_id < 0 || s_id > edgeInfos_.size())
		{
			std::cout << "error brep edge index " << std::endl;
			return;
		}

		glColor3f(0.85, 0.85, 0);

		std::shared_ptr<AMCAX::TriangularMesh> mesh = faceInfos_[s_id].triMesh;
		AMCAX::Transformation3 tr = faceInfos_[s_id].loc.Transformation();
		AMCAX::OrientationType fOri = faceInfos_[s_id].ori;

		for (int fid = 0; fid < mesh->NTriangles(); ++fid)
		{
			AMCAX::Triangle tri = mesh->Face(fid);
			if (fOri == AMCAX::OrientationType::Reversed)
				std::swap(tri[1], tri[2]);

			const AMCAX::Point3& p0 = mesh->Vertex(tri[0]).Transformed(tr);
			const AMCAX::Point3& p1 = mesh->Vertex(tri[1]).Transformed(tr);
			const AMCAX::Point3& p2 = mesh->Vertex(tri[2]).Transformed(tr);

			glBegin(GL_POLYGON);
			glVertex3d(p0.X(), p0.Y(), p0.Z());
			glVertex3d(p1.X(), p1.Y(), p1.Z());
			glVertex3d(p2.X(), p2.Y(), p2.Z());
			glEnd();
		}
	}

}

//void acamcad::BRepObjectDraw::drawForSelect() const
//{
//	const AMCAX::TopoShape& shape = parent_->bRep->shape_;
//	if (parent_->bRep->getShape().IsNull())
//		return;
//	if (parent_->bRep->getShapeVertices().size() == 0)
//		return;
//
//	glEnable(GL_POLYGON_OFFSET_FILL);
//	glPolygonOffset(1.0f, 1.0f);
//	glEnable(GL_LIGHTING);
//	glShadeModel(GL_FLAT);
//	draw_mesh_solidflat(false);
//	draw_curve();
//	glDisable(GL_POLYGON_OFFSET_FILL);
//	glDisable(GL_LIGHTING);
//}
//
//void acamcad::BRepObjectDraw::drawForSelectPart() const
//{
//	for (int i = 0; i < faceInfos_.size(); ++i)
//	{
//		std::shared_ptr<AMCAX::TriangularMesh> mesh = faceInfos_[i].triMesh;
//		AMCAX::Transformation3 tr = faceInfos_[i].loc.Transformation();
//		AMCAX::OrientationType fOri = faceInfos_[i].ori;
//
//		glPushName(i);
//
//		for (int fid = 0; fid < mesh->NTriangles(); ++fid)
//		{
//			AMCAX::Triangle tri = mesh->Face(fid);
//			if (fOri == AMCAX::OrientationType::Reversed)
//				std::swap(tri[1], tri[2]);
//
//			const AMCAX::Point3& p0 = mesh->Vertex(tri[0]).Transformed(tr);
//			const AMCAX::Point3& p1 = mesh->Vertex(tri[1]).Transformed(tr);
//			const AMCAX::Point3& p2 = mesh->Vertex(tri[2]).Transformed(tr);
//
//			glBegin(GL_POLYGON);
//			glVertex3d(p0.X(), p0.Y(), p0.Z());
//			glVertex3d(p1.X(), p1.Y(), p1.Z());
//			glVertex3d(p2.X(), p2.Y(), p2.Z());
//			glEnd();
//		}
//
//		glPopName();
//	}
//}

void acamcad::BRepObjectDraw::drawWithObjectID() const
{
	const AMCAX::TopoShape& shape = parent_->bRep->shape_;
	if (parent_->bRep->getShape().IsNull())
		return;
	if (parent_->bRep->getShapeVertices().size() == 0)
		return;

	glPushName(parent_->persistentId());

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);
	glEnable(GL_LIGHTING);
	glShadeModel(GL_FLAT);
	draw_mesh_solidflat(false);
	draw_curve();
	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_LIGHTING);

	glPopName();
}

void acamcad::BRepObjectDraw::drawWithFaceID() const
{
	for (int i = 0; i < faceInfos_.size(); ++i)
	{
		std::shared_ptr<AMCAX::TriangularMesh> mesh = faceInfos_[i].triMesh;
		AMCAX::Transformation3 tr = faceInfos_[i].loc.Transformation();
		AMCAX::OrientationType fOri = faceInfos_[i].ori;

		glPushName(i);

		for (int fid = 0; fid < mesh->NTriangles(); ++fid)
		{
			AMCAX::Triangle tri = mesh->Face(fid);
			if (fOri == AMCAX::OrientationType::Reversed)
				std::swap(tri[1], tri[2]);

			const AMCAX::Point3& p0 = mesh->Vertex(tri[0]).Transformed(tr);
			const AMCAX::Point3& p1 = mesh->Vertex(tri[1]).Transformed(tr);
			const AMCAX::Point3& p2 = mesh->Vertex(tri[2]).Transformed(tr);

			glBegin(GL_POLYGON);
			glVertex3d(p0.X(), p0.Y(), p0.Z());
			glVertex3d(p1.X(), p1.Y(), p1.Z());
			glVertex3d(p2.X(), p2.Y(), p2.Z());
			glEnd();
		}

		glPopName();
	}
}

void acamcad::BRepObjectDraw::drawWithEdgeID() const
{
	const AMCAX::IndexSet<AMCAX::TopoShape>& edges = parent_->bRep->getShapeEdges();

	for (int i = 0; i < edges.size(); ++i)
	{
		std::shared_ptr<AMCAX::TriangularMesh> mesh = edgeInfos_[i].triMesh;
		std::shared_ptr<AMCAX::PolygonOnTriangularMesh> poly = edgeInfos_[i].polyMesh;
		AMCAX::Transformation3 tr = edgeInfos_[i].loc.Transformation();
		if (mesh == nullptr || poly == nullptr)
		{
			glPushName(i);
			draw_curve();
			glPopName();

			continue;
		}
		glPushName(i);
		for (int pid = 1; pid < poly->NVertices(); ++pid)
		{
			const AMCAX::Point3& p0 = mesh->Vertex(poly->Vertex(pid - 1)).Transformed(tr);
			const AMCAX::Point3& p1 = mesh->Vertex(poly->Vertex(pid)).Transformed(tr);

			glBegin(GL_LINES);
			glVertex3d(p0.X(), p0.Y(), p0.Z());
			glVertex3d(p1.X(), p1.Y(), p1.Z());
			glEnd();
		}
		glPopName();
	}

}

void acamcad::BRepObjectDraw::drawWithVertexID() const
{
	const AMCAX::IndexSet<AMCAX::TopoShape>& vertexs = parent_->bRep->getShapeVertices();
	for (int i = 0; i < vertexs.size(); ++i)
	{
		const AMCAX::TopoVertex& v = static_cast<const AMCAX::TopoVertex&>(vertexs[i]);

		AMCAX::Point3 vp = AMCAX::TopoTool::Point(v);

		glPushName(i);

		glBegin(GL_POINTS);
		glVertex3d(vp.X(), vp.Y(), vp.Z());
		glEnd();

		glPopName();
	}
}

void acamcad::BRepObjectDraw::updatedrawState()
{
	vert_position_.clear();
	vert_normal_.clear();
	face_vertices_.clear();
	edge_vertices_.clear();

	AMCAX::BRepMeshIncrementalMesh IMesh(parent_->bRep->shape_, 0.005, true);
	AMCAX::MakeShapeTool::EnsureNormalConsistency(parent_->bRep->getShape());

	const AMCAX::IndexSet<AMCAX::TopoShape>& faces = parent_->bRep->getShapeFaces();
	faceInfos_.resize(faces.size());
	for (int i = 0; i < faces.size(); ++i)
	{
		const AMCAX::TopoFace& f = static_cast<const AMCAX::TopoFace&>(faces[i]);
		AMCAX::TopoLocation loc;
		faceInfos_[i].triMesh = AMCAX::TopoTool::Triangulation(f, loc);
		faceInfos_[i].loc = loc;
		faceInfos_[i].ori = f.Orientation();
	}

	const AMCAX::IndexSet<AMCAX::TopoShape>& edges = parent_->bRep->getShapeEdges();
	edgeInfos_.resize(edges.size());
	for (int i = 0; i < edges.size(); ++i)
	{
		const AMCAX::TopoEdge& e = static_cast<const AMCAX::TopoEdge&>(edges[i]);
		AMCAX::TopoLocation loc;
		AMCAX::TopoTool::PolygonOnTriangulation(e, edgeInfos_[i].polyMesh, edgeInfos_[i].triMesh, loc);
		edgeInfos_[i].loc = loc;

		//step curve
		if (edgeInfos_[i].polyMesh == nullptr)
		{
			double ef, el;
			std::shared_ptr<AMCAX::Geom3Curve> curve = AMCAX::TopoTool::Curve(e, ef, el);
			if (curve == nullptr)
				continue;
			int nPoints = 10;
			if (curve->Type() == AMCAX::CurveType::Line)
			{
				nPoints = 2;
			}
			else if (curve->Type() == AMCAX::CurveType::BSplineCurve)
			{
				std::shared_ptr<AMCAX::Geom3BSplineCurve> spline = std::dynamic_pointer_cast<AMCAX::Geom3BSplineCurve>(curve);
				nPoints = spline->NKnots() * spline->Degree() * 10;
			}

			curvePointsInfo_.resize(nPoints);
			for (int j = 0; j < nPoints; j++)
			{
				double t = (double)j / ((double)nPoints - 1);
				AMCAX::Point3 point = curve->Value((1 - t) * ef + t * el);
				curvePointsInfo_[j] = point;
			}
		}
	}

	int vNum = 0;
	int fNum = 0;
	for (int i = 0; i < faceInfos_.size(); ++i)
	{
		if (faceInfos_[i].triMesh != nullptr)
		{
			vNum += faceInfos_[i].triMesh->NVertices();
			fNum += faceInfos_[i].triMesh->NTriangles();
		}
	}

	vert_position_.resize(vNum * 3);
	face_vertices_.resize(fNum);
	vert_normal_.resize(vNum * 3);
	int vOffset = 0;
	int fOffset = 0;
	for (int i = 0; i < faceInfos_.size(); ++i)
	{
		std::shared_ptr<AMCAX::TriangularMesh> mesh = faceInfos_[i].triMesh;
		AMCAX::Transformation3 tr = faceInfos_[i].loc.Transformation();
		AMCAX::OrientationType fOri = faceInfos_[i].ori;

		AMCAX::Matrix3 mat = tr.HVectorialPart();
		mat.Invert().Transpose();
		AMCAX::Transformation3 trn;
		trn.SetValues(mat(0, 0), mat(0, 1), mat(0, 2), 0.0,
			mat(1, 0), mat(1, 1), mat(1, 2), 0.0,
			mat(2, 0), mat(2, 1), mat(2, 2), 0.0);

		if (mesh == nullptr)
		{
			continue;
		}

		for (int vid = 0; vid < mesh->NVertices(); ++vid)
		{
			const AMCAX::Point3& p = mesh->Vertex(vid).Transformed(tr);
			AMCAX::Direction3& n = mesh->Normal(vid).Transformed(trn);

			if (fOri == AMCAX::OrientationType::Reversed) { n = n.Reversed(); }

			int id = vid + vOffset;
			vert_position_[3 * id + 0] = p.X(); vert_position_[3 * id + 1] = p.Y(); vert_position_[3 * id + 2] = p.Z();
			vert_normal_[3 * id + 0] = n.X(); vert_normal_[3 * id + 1] = n.Y(); vert_normal_[3 * id + 2] = n.Z();
		}
		for (int fid = 0; fid < mesh->NTriangles(); ++fid)
		{
			const AMCAX::Triangle& tri = mesh->Face(fid);
			if (fOri == AMCAX::OrientationType::Reversed)
				face_vertices_[fid + fOffset] = { tri[0] + vOffset, tri[2] + vOffset, tri[1] + vOffset };
			else
				face_vertices_[fid + fOffset] = { tri[0] + vOffset, tri[1] + vOffset, tri[2] + vOffset };
		}

		vOffset += mesh->NVertices();
		fOffset += mesh->NTriangles();
	}

	for (int i = 0; i < edgeInfos_.size(); ++i)
	{
		std::shared_ptr<AMCAX::TriangularMesh> mesh = edgeInfos_[i].triMesh;
		std::shared_ptr<AMCAX::PolygonOnTriangularMesh> poly = edgeInfos_[i].polyMesh;
		AMCAX::Transformation3 tr = edgeInfos_[i].loc.Transformation();
		if (mesh == nullptr || poly == nullptr)
		{
			continue;
		}

		for (int pid = 0; pid < poly->NVertices(); ++pid)
		{
			const AMCAX::Point3& p = mesh->Vertex(poly->Vertex(pid)).Transformed(tr);
			const AMCAX::Direction3& n = mesh->Normal(poly->Vertex(pid));

			vert_position_.push_back(p.X()); vert_position_.push_back(p.Y()); vert_position_.push_back(p.Z());
			vert_normal_.push_back(n.X()); vert_normal_.push_back(n.Y()); vert_normal_.push_back(n.Z());
		}

		for (int pid = 1; pid < poly->NVertices(); ++pid)
		{
			std::vector<int> ev = { pid - 1 + vOffset, pid + vOffset };
			edge_vertices_.push_back(ev);
		}

		vOffset += poly->NVertices();
	}
}


void acamcad::BRepObjectDraw::draw(const DrawModel& dmodel, bool is_show_point) const
{
	const AMCAX::TopoShape& shape = parent_->bRep->shape_;
	if (parent_->bRep->getShape().IsNull())
		return;
	if (parent_->bRep->getShapeVertices().size() == 0)
		return;

	bool is_selected = parent_->isSelect();

	switch (dmodel)
	{
	case DrawModel::WIRE_MODE:
		glDisable(GL_LIGHTING);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		draw_mesh_wireframe(is_selected, is_show_point);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case DrawModel::RENDERING_MODEL:
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0f, 1.0f);
		glEnable(GL_LIGHTING);
		glShadeModel(GL_FLAT);
		draw_mesh_solidflat(is_selected);
		glDisable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_LIGHTING);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		draw_mesh_wireframe(is_selected, is_show_point);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case DrawModel::SHADING_MODEL:
		glEnable(GL_LIGHTING);
		draw_mesh_smooth(is_selected);
		glDisable(GL_LIGHTING);
		break;
	case DrawModel::ZEBRA_MODE:
		break;
	case DrawModel::C_GRAPH_MODE:
		break;
	default:
		break;
	}

}

void acamcad::BRepObjectDraw::draw_mesh_wireframe(bool is_selected, bool showPoint) const
{
	if (showPoint)
	{
		colorDefault(is_selected);
		glPointSize(4);

		const AMCAX::IndexSet<AMCAX::TopoShape>& vertexs = parent_->bRep->getShapeVertices();
		for (int i = 0; i < vertexs.size(); ++i)
		{
			const AMCAX::TopoVertex& v = static_cast<const AMCAX::TopoVertex&>(vertexs[i]);

			AMCAX::Point3 vp = AMCAX::TopoTool::Point(v);

			glBegin(GL_POINTS);
			glVertex3d(vp.X(), vp.Y(), vp.Z());
			glEnd();
		}
	}

	colorDefault(is_selected);
	glLineWidth(1);

	const AMCAX::IndexSet<AMCAX::TopoShape>& edges = parent_->bRep->getShapeEdges();

	for (int i = 0; i < edges.size(); ++i)
	{
		std::shared_ptr<AMCAX::TriangularMesh> mesh = edgeInfos_[i].triMesh;
		std::shared_ptr<AMCAX::PolygonOnTriangularMesh> poly = edgeInfos_[i].polyMesh;
		AMCAX::Transformation3 tr = edgeInfos_[i].loc.Transformation();
		if (mesh == nullptr || poly == nullptr)
		{
			draw_curve();
			continue;
		}

		for (int pid = 1; pid < poly->NVertices(); ++pid)
		{
			const AMCAX::Point3& p0 = mesh->Vertex(poly->Vertex(pid - 1)).Transformed(tr);
			const AMCAX::Point3& p1 = mesh->Vertex(poly->Vertex(pid)).Transformed(tr);

			glBegin(GL_LINES);
			glVertex3d(p0.X(), p0.Y(), p0.Z());
			glVertex3d(p1.X(), p1.Y(), p1.Z());
			glEnd();
		}
	}
}

void acamcad::BRepObjectDraw::draw_mesh_solidflat(bool is_selected) const
{
	materialSolidFlatDefault(is_selected);

	for (int i = 0; i < faceInfos_.size(); ++i)
	{
		std::shared_ptr<AMCAX::TriangularMesh> mesh = faceInfos_[i].triMesh;
		AMCAX::Transformation3 tr = faceInfos_[i].loc.Transformation();
		AMCAX::OrientationType fOri = faceInfos_[i].ori;
		if (mesh == nullptr)
		{
			continue;
		}

		for (int fid = 0; fid < mesh->NTriangles(); ++fid)
		{
			AMCAX::Triangle tri = mesh->Face(fid);
			if (fOri == AMCAX::OrientationType::Reversed)
				std::swap(tri[1], tri[2]);

			const AMCAX::Point3& p0 = mesh->Vertex(tri[0]).Transformed(tr);
			const AMCAX::Point3& p1 = mesh->Vertex(tri[1]).Transformed(tr);
			const AMCAX::Point3& p2 = mesh->Vertex(tri[2]).Transformed(tr);


			AMCAX::Vector3 n(AMCAX::Vector3(p0, p1).Cross(AMCAX::Vector3(p0, p2)));
			//if (n.Norm() < std::numeric_limits<double>::epsilon())
			//{
			//    std::cout << p0 << std::endl;
			//    std::cout << p1 << std::endl;
			//    std::cout << p2 << std::endl;
			//}

			n.Normalize();

			glBegin(GL_POLYGON);

			glNormal3d(n.X(), n.Y(), n.Z());

			glVertex3d(p0.X(), p0.Y(), p0.Z());
			glVertex3d(p1.X(), p1.Y(), p1.Z());
			glVertex3d(p2.X(), p2.Y(), p2.Z());
			glEnd();
		}
	}
}

void acamcad::BRepObjectDraw::draw_mesh_smooth(bool is_selected) const
{
	materialSmoothDefault(is_selected);

	if (vert_position_.empty())
		return;

	glShadeModel(GL_SMOOTH);
	glLoadName(static_cast<GLuint>(vert_position_.size()));
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_DOUBLE, 0, &vert_position_[0]);
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_DOUBLE, 0, &vert_normal_[0]);
	for (const auto& fh : face_vertices_)
	{
		glBegin(GL_POLYGON);
		for (const auto& fvh : fh)
		{
			glArrayElement(fvh);
		}
		glEnd();
	}

	glDisableClientState(GL_NORMAL_ARRAY);

	GLfloat mat_a[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat mat_d[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat mat_s[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat shine[] = { 120.0f };

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_a);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_d);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_s);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shine);

	glLineWidth(2.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for (const auto& eh : edge_vertices_)
	{
		glBegin(GL_LINES);
		glArrayElement(eh[0]);
		glArrayElement(eh[1]);
		glEnd();
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDisableClientState(GL_VERTEX_ARRAY);
}

void acamcad::BRepObjectDraw::draw_curve() const
{
	glLineWidth(1.2);

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < curvePointsInfo_.size(); i++)
	{
		glVertex3f(curvePointsInfo_[i].X(), curvePointsInfo_[i].Y(), curvePointsInfo_[i].Z());
	}
	glEnd();
}

