#include "MeshObjectDraw.h"
#include "../Object/AdapterObject.h"
#include <polymesh/PolyMesh.hpp>
#include <polymeshAPI/MeshCheck.hpp>
#include <polymeshAPI/MeshTool.hpp>

using namespace acamcad;

MeshObjectDraw::MeshObjectDraw(AdapterObject* object) :ObjectDraw(object)
{
}

MeshObjectDraw::~MeshObjectDraw()
{
	//mesh_object_ = nullptr;
}

void MeshObjectDraw::draw(const DrawModel& dmodel, const SelectModel& smodel) const
{
	bool is_show_point = false;
	if (smodel == SelectModel::VERTEX_MODEL)
		is_show_point = true;

	draw(dmodel, is_show_point);
}

void MeshObjectDraw::draw(const DrawModel& dmodel, bool is_show_point) const
{
	if (!parent_->isVisible())
		return;

	const polymesh::PolyMesh* mesh = parent_->mesh->mesh_;

	if (mesh == nullptr) { return; }
	if (mesh->numVertices() == 0) { return; }

	bool is_selected = parent_->isSelect();

	switch (dmodel)
	{
	case DrawModel::WIRE_MODE:
		glDisable(GL_LIGHTING);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		draw_mesh_wireframe(mesh, is_selected, is_show_point);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case DrawModel::RENDERING_MODEL:
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0f, 1.0f);
		glEnable(GL_LIGHTING);
		glShadeModel(GL_FLAT);
		draw_mesh_solidflat(mesh, is_selected);
		glDisable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_LIGHTING);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		draw_mesh_wireframe(mesh, is_selected, is_show_point);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case DrawModel::SHADING_MODEL:
		glEnable(GL_LIGHTING);
		draw_mesh_smooth(mesh, is_selected);
		glDisable(GL_LIGHTING);
		break;
	default:
		break;
	}
}

//void MeshObjectDraw::drawForSelect() const
//{
//	if (!parent_->isVisible())
//		return;
//
//	const polymesh::PolyMesh* mesh = parent_->mesh->mesh_;
//
//	if (mesh == nullptr) { return; }
//	if (mesh->numVertices() == 0) { return; }
//
//	glEnable(GL_POLYGON_OFFSET_FILL);
//	glPolygonOffset(1.0f, 1.0f);
//	glEnable(GL_LIGHTING);
//	glShadeModel(GL_FLAT);
//	draw_mesh_solidflat(mesh, false);
//	glDisable(GL_POLYGON_OFFSET_FILL);
//	glDisable(GL_LIGHTING);
//}
//
//void MeshObjectDraw::drawForSelectPart() const
//{
//	polymesh::PolyMesh* mesh = parent_->mesh->mesh();
//
//	//auto indexs = AMCAX::SubD::MeshTool::FaceVertexIndexs(mesh, i);
//	for (int i = 0; i < mesh->numPolygons(); i++) {
//
//		auto indexs = AMCAX::SubD::MeshTool::FaceVertexIndexs(mesh, i);
//
//		glPushName(i);
//		glBegin(GL_POLYGON);
//
//		for (int j = 0; j < indexs.size(); j++) {
//			auto point = AMCAX::SubD::MeshTool::Position(mesh, indexs[j]);
//			//double d[3] = { point.X(),point.Y(),point.Z() };
//			glVertex3dv(point.Coord().Data());
//		}
//
//		glEnd();
//		glPopName();
//	}
//
//
//
//	//for (auto iter = mesh->const_polyfaces_begin(); iter != mesh->const_polyfaces_end(); iter++) {
//		//glPushName(iter->index());
//	//}
//}


void MeshObjectDraw::drawSelected(const SelectModel& s_model, int s_id) const
{
	polymesh::PolyMesh* mesh = parent_->mesh->mesh_;

	if (s_model == SelectModel::VERTEX_MODEL)
	{
		glColor3f(0.85, 0.85, 0);
		glPointSize(8);

		auto point = AMCAX::SubD::MeshTool::Position(mesh, s_id);
		//double d[3]{ point.X(),point.Y(),point.Z() };

		glBegin(GL_POINTS);
		glVertex3dv(point.Coord().Data());
		glEnd();

	}

	if (s_model == SelectModel::EDGE_MODEL)
	{
		int v0, v1;
		AMCAX::SubD::MeshTool::EdgeVertexIndexs(mesh, s_id, v0, v1);

		auto point0 = AMCAX::SubD::MeshTool::Position(mesh, v0);
		auto point1 = AMCAX::SubD::MeshTool::Position(mesh, v1);

		//double d0[3]{ point0.X(),point0.Y(),point0.Z() }, d1[3]{ point1.X(),point1.Y(),point1.Z() };

		glColor3f(0.85, 0.85, 0);
		glLineWidth(6);

		glBegin(GL_LINES);
		glVertex3dv(point0.Coord().Data());
		glVertex3dv(point1.Coord().Data());
		glEnd();

	}
	if (s_model == SelectModel::FACE_MODEL)
	{
		auto indexs = AMCAX::SubD::MeshTool::FaceVertexIndexs(mesh, s_id);

		glColor3f(0.85, 0.85, 0);
		glBegin(GL_POLYGON);

		for (int i = 0; i < indexs.size(); i++) {
			auto point = AMCAX::SubD::MeshTool::Position(mesh, indexs[i]);
			//double d[3] = { point.X(),point.Y(),point.Z() };
			glVertex3dv(point.Coord().Data());
		}
		glEnd();
	}
}

void MeshObjectDraw::drawWithObjectID() const
{
	if (!parent_->isVisible())
		return;

	const polymesh::PolyMesh* mesh = parent_->mesh->mesh_;

	if (mesh == nullptr) { return; }
	if (mesh->numVertices() == 0) { return; }

	glPushName(parent_->persistentId());

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);
	glEnable(GL_LIGHTING);
	glShadeModel(GL_FLAT);
	draw_mesh_solidflat(mesh, false);
	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_LIGHTING);

	glPopName();
}

void MeshObjectDraw::drawWithFaceID() const
{
	polymesh::PolyMesh* mesh = parent_->mesh->mesh();

	//auto indexs = AMCAX::SubD::MeshTool::FaceVertexIndexs(mesh, i);
	for (int i = 0; i < mesh->numPolygons(); i++) {

		auto indexs = AMCAX::SubD::MeshTool::FaceVertexIndexs(mesh, i);

		glPushName(i);
		glBegin(GL_POLYGON);

		for (int j = 0; j < indexs.size(); j++) {
			auto point = AMCAX::SubD::MeshTool::Position(mesh, indexs[j]);
			//double d[3] = { point.X(),point.Y(),point.Z() };
			glVertex3dv(point.Coord().Data());

		}

		glEnd();
		glPopName();

	}
}

void MeshObjectDraw::drawWithEdgeID() const
{
	auto mesh = parent_->mesh->mesh();

	for (int i = 0; i < mesh->numEdges(); i++)
	{
		int v0, v1;
		AMCAX::SubD::MeshTool::EdgeVertexIndexs(mesh, i, v0, v1);

		auto point0 = AMCAX::SubD::MeshTool::Position(mesh, v0),
			point1 = AMCAX::SubD::MeshTool::Position(mesh, v1);

		glPushName(i);
		glBegin(GL_LINES);
		glVertex3dv(point0.Coord().Data());
		glVertex3dv(point1.Coord().Data());
		glEnd();
		glPopName();
	}
}

void MeshObjectDraw::drawWithVertexID() const
{
	auto mesh = parent_->mesh->mesh();


	for (int i = 0; i < mesh->numVertices(); i++)
	{
		auto& point = AMCAX::SubD::MeshTool::Position(mesh, i);
		glPushName(i);
		glBegin(GL_POINTS);
		glVertex3dv(point.Coord().Data());
		glEnd();
		glPopName();
	}

}

void MeshObjectDraw::draw_mesh_wireframe(const polymesh::PolyMesh* cMesh, bool is_selected, bool showPoint) const
{

	AMCAX::SubD::PolyMesh* mesh = const_cast<AMCAX::SubD::PolyMesh*>(cMesh);
	if (showPoint)
	{
		colorDefault(is_selected);
		glPointSize(4);

		glBegin(GL_POINTS);

		for (int i = 0; i < mesh->numVertices(); i++)
		{
			auto& point = AMCAX::SubD::MeshTool::Position(mesh, i);
			//double d[3] = { point.X(),point.Y(),point.Z() };
			glVertex3dv(point.Coord().Data());
		}

		glEnd();
	}

	colorDefault(is_selected);
	glLineWidth(1);

	for (int i = 0; i < mesh->numPolygons(); i++) {

		auto indexs = AMCAX::SubD::MeshTool::FaceVertexIndexs(mesh, i);

		glBegin(GL_POLYGON);
		for (int j = 0; j < indexs.size(); j++) {
			auto& point = AMCAX::SubD::MeshTool::Position(mesh, indexs[j]);
			//double d[3]{ point.X(), point.Y(), point.Z() };
			glVertex3dv(point.Coord().Data());
		}
		glEnd();

	}

}

void MeshObjectDraw::draw_mesh_solidflat(const polymesh::PolyMesh* mesh, bool is_selected) const
{
	materialSolidFlatDefault(is_selected);

	for (int i = 0; i < mesh->numPolygons(); i++) {

		auto indexs = AMCAX::SubD::MeshTool::FaceVertexIndexs((polymesh::PolyMesh*)mesh, i);
		auto normal = AMCAX::SubD::MeshTool::FaceNormal((AMCAX::SubD::PolyMesh*)mesh, i).Normalized();

		//double v[3]{ normal.X(),normal.Y(),normal.Z() };

		glBegin(GL_POLYGON);
		glNormal3dv(normal.Coord().Data());
		for (int j = 0; j < indexs.size(); j++) {
			auto& point = AMCAX::SubD::MeshTool::Position((polymesh::PolyMesh*)mesh, indexs[j]);
			//double d[3]{ point.X(), point.Y(), point.Z() };
			glVertex3dv(point.Coord().Data());
		}
		glEnd();

	}

}

void MeshObjectDraw::draw_mesh_smooth(const polymesh::PolyMesh* mesh, bool is_selected) const
{
	if (vert_position_.empty())
		return;

	materialSmoothDefault(is_selected);

	glShadeModel(GL_SMOOTH);
	glLoadName(static_cast<GLuint>(mesh->numVertices()));
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_DOUBLE, 0, &vert_position_[0]);
	glEnableClientState(GL_NORMAL_ARRAY);
	//glNormalPointer(GL_DOUBLE, 0, &vert_normal_[0]);
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

	glLineWidth(1.5);
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


void MeshObjectDraw::updatedrawState()
{
	//return;
	//polymesh::PolyMesh* mesh = parent_->mesh->mesh_;
	//if (!mesh)
	//	return;
	//const size_t v_number = mesh->numVertices();
	//vert_position_.resize(v_number * 3);
	////vert_normal_.resize(v_number * 3);

	//for (size_t i = 0; i < v_number; i++)
	//{
	//	auto& point = AMCAX::SubD::MeshTool::Position(mesh, i);
	//	//double d[3];
	//	vert_position_[3 * i] = point.X();
	//	vert_position_[3 * i + 1] = point.Y();
	//	vert_position_[3 * i + 2] = point.Z();
	//}

	//face_vertices_.clear();
	//face_vertices_.reserve(mesh->numPolygons());
	//std::vector<size_t> f_vertices;

	//for (int i = 0; i < mesh->numPolygons(); i++) {
	//	f_vertices.clear();
	//	auto indexs = AMCAX::SubD::MeshTool::FaceVertexIndexs(mesh, i);

	//	for (int j = 0; j < indexs.size(); j++) {
	//		f_vertices.push_back(indexs[j]);
	//	}

	//	face_vertices_.push_back(f_vertices);


	//}

	//edge_vertices_.clear();
	//edge_vertices_.reserve(mesh->numEdges() / 2);

	//for (int i = 0; i < mesh->numEdges(); i++)
	//{
	//	//if (eh->isVisibility())
	//	int ev0, ev1;
	//	AMCAX::SubD::MeshTool::EdgeVertexIndexs(mesh, i, ev0, ev1);

	//	edge_vertices_.push_back({ (size_t)ev0,(size_t)ev1 });
	//}

}
