#include "TSplineObjectDraw.h"
#include "../Object/TSplineObject.h"
#include "../Object/AdapterObject.h"

#include <tmeshSpline/TMSpline.hpp>
#include <tmeshSplineAPI/TMSplineTool.hpp>
#include <tmeshSpline/TMSplineMeshingIO.hpp>
#include <tmeshSpline/TMSTriangleMesh.hpp>
#include <tmeshSplineAPI/TMSplineCheck.hpp>

namespace acamcad
{
	TSplineObjectDraw::TSplineObjectDraw(AdapterObject* object) :ObjectDraw(object)
	{
		//tspline_object_ = dynamic_cast<TSplineObject*> (object);
		//tsp_mesh_ = nullptr;
	}

	TSplineObjectDraw::~TSplineObjectDraw()
	{
	}

	void TSplineObjectDraw::clear()
	{
		tsp_mesh_.release();
	}


	void TSplineObjectDraw::draw(const DrawModel& dmodel, const SelectModel& smodel) const
	{
		bool is_show_point = false;
		if (smodel == SelectModel::VERTEX_MODEL)
			is_show_point = true;

		if (!parent_->isVisible())
		{
			return;
		}
		draw(dmodel, is_show_point);
	}

	void TSplineObjectDraw::draw(const DrawModel& dmodel, bool is_show_point) const
	{
		bool is_selected = parent_->isSelect();
		auto mesh = parent_->tSpline->getShape();

		if (mesh == nullptr) { return; }
		if (mesh->numVertices() == 0) { return; }

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
			draw_mesh_smooth(is_selected);
			glDisable(GL_LIGHTING);
			break;
		case DrawModel::ZEBRA_MODE:

			glEnable(GL_LIGHTING);
			draw_mesh_zebra();
			glDisable(GL_LIGHTING);
			break;

		case DrawModel::C_GRAPH_MODE:
			glEnable(GL_LIGHTING);
			draw_mesh_smooth(is_selected);
			glDisable(GL_LIGHTING);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			draw_mesh_curvature_comb();
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			break;
		default:
			break;
		}
	}

	void TSplineObjectDraw::drawSelected(const SelectModel& s_model, int s_id) const
	{
		auto mesh = parent_->tSpline->getShape();

		if (s_model == SelectModel::VERTEX_MODEL)
		{
			glColor3f(0.85, 0.85, 0);
			glPointSize(8);

			auto pos = AMCAX::TMS::TMSplineTool::ControlPosition(mesh, s_id);
			//const tspline::TVert* v = mesh->vert(s_id);


			glBegin(GL_POINTS);
			glVertex3dv(pos.Coord().Data());
			glEnd();

		}
		if (s_model == SelectModel::EDGE_MODEL)
		{
			glColor3f(0.85, 0.85, 0);
			glLineWidth(6);

			//const tspline::TEdge* e = mesh->edge(s_id);
			int v0, v1;
			AMCAX::TMS::TMSplineTool::EdgeVertexIndexs(mesh, s_id, v0, v1);
			auto p0 = AMCAX::TMS::TMSplineTool::ControlPosition(mesh, v0),
				p1 = AMCAX::TMS::TMSplineTool::ControlPosition(mesh, v1);

			glBegin(GL_LINES);
			glVertex3dv(p0.Coord().Data());
			glVertex3dv(p1.Coord().Data());
			glEnd();

		}
		if (s_model == SelectModel::FACE_MODEL)
		{
			glColor3f(0.85, 0.85, 0);

			std::vector<int> v_ids = AMCAX::TMS::TMSplineTool::FaceVertexIndexs(mesh, s_id);

			//const tspline::TFace* cf = mesh->polyface(s_id);
			//if (cf != nullptr)
			{
				//tspline::TFace* f = const_cast<tspline::TFace*>(cf);
				glBegin(GL_POLYGON);
				for (auto& iter : v_ids)
				{
					auto p0 = AMCAX::TMS::TMSplineTool::ControlPosition(mesh, iter);
					glVertex3dv(p0.Coord().Data());
				}
				glEnd();
			}
		}
	}

	//void TSplineObjectDraw::drawForSelect() const
	//{
	//	if (!parent_->isVisible())
	//		return;

	//	auto mesh = parent_->tSpline->getTSpline();

	//	if (mesh == nullptr) { return; }
	//	if (mesh->numVertices() == 0) { return; }

	//	glEnable(GL_POLYGON_OFFSET_FILL);
	//	glPolygonOffset(1.0f, 1.0f);
	//	glEnable(GL_LIGHTING);
	//	glShadeModel(GL_FLAT);
	//	draw_mesh_solidflat(mesh, false);
	//	glDisable(GL_POLYGON_OFFSET_FILL);
	//	glDisable(GL_LIGHTING);
	//}

	//void TSplineObjectDraw::drawForSelectPart() const
	//{
	//	//const tspline::TSpline* mesh = tspline_object_->getTSpline();

	//	//for (tspline::TSpline::CFaceIter f_it = mesh->const_faces_begin(); f_it != mesh->const_faces_end(); ++f_it)
	//	//{
	//	//	tspline::TFace* f = *f_it;
	//	//	glPushName(f->index());

	//	//	glBegin(GL_POLYGON);
	//	//	
	//	//	for (tspline::TVert* v : mesh->polygonVertices(f))
	//	//	{
	//	//		glVertex3dv(v->controlPosition());
	//	//	}
	//	//	glEnd();

	//	//	glPopName();
	//	//}
	//	auto mesh = parent_->tSpline->getTSpline();

	//	int size = mesh->numFaces();
	//	for (int i = 0; i < size; i++) {
	//		auto ids = AMCAX::TMS::TMSplineTool::FaceVertexIndexs(mesh, i);
	//		glPushName(i);
	//		glBegin(GL_POLYGON);
	//		for (auto& iter : ids)
	//		{
	//			auto pos = AMCAX::TMS::TMSplineTool::ControlPosition(mesh, iter);
	//			glVertex3dv(pos.Coord().Data());
	//		}

	//		glEnd();
	//		glPopName();
	//	}
	//}

	void TSplineObjectDraw::drawWithObjectID() const
	{
		if (!parent_->isVisible())
			return;

		auto mesh = parent_->tSpline->getShape();

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
	void TSplineObjectDraw::drawWithFaceID() const
	{
		auto mesh = parent_->tSpline->getShape();

		int size = mesh->numFaces();
		for (int i = 0; i < size; i++) {
			auto ids = AMCAX::TMS::TMSplineTool::FaceVertexIndexs(mesh, i);
			glPushName(i);
			glBegin(GL_POLYGON);
			for (auto& iter : ids)
			{
				auto pos = AMCAX::TMS::TMSplineTool::ControlPosition(mesh, iter);
				glVertex3dv(pos.Coord().Data());
			}

			glEnd();
			glPopName();
		}

	}

	void TSplineObjectDraw::drawWithEdgeID() const
	{
		auto mesh = parent_->tSpline->getShape();

		for (int i = 0; i < mesh->numEdges(); i++)
		{
			int v0, v1;
			AMCAX::TMS::TMSplineTool::EdgeVertexIndexs(mesh, i, v0, v1);

			auto p0 = AMCAX::TMS::TMSplineTool::ControlPosition(mesh, v0),
				p1 = AMCAX::TMS::TMSplineTool::ControlPosition(mesh, v1);

			glPushName(i);
			glBegin(GL_LINES);
			glVertex3dv(p0.Coord().Data());
			glVertex3dv(p1.Coord().Data());
			glEnd();
			glPopName();
		}

	}

	void TSplineObjectDraw::drawWithVertexID() const
	{
		auto mesh = parent_->tSpline->getShape();

		for (int i = 0; i < mesh->numVertices(); i++)
		{
			auto pos = AMCAX::TMS::TMSplineTool::ControlPosition(mesh, i);

			glPushName(i);
			glBegin(GL_POINTS);
			glVertex3dv(pos.Coord().Data());
			glEnd();
			glPopName();
		}
	}

	void TSplineObjectDraw::draw_mesh_wireframe(AMCAX::TMS::TMSpline* mesh, bool is_selected, bool showPoint) const
	{
		if (showPoint)
		{
			colorDefault(is_selected);
			glPointSize(4);

			glBegin(GL_POINTS);
			//tspline::TSpline::CVertexIter cv_end = mesh->const_vertices_end();
			//for (tspline::TSpline::CVertexIter cv_it = mesh->const_vertices_begin(); cv_it != cv_end; ++cv_it)
			//{
			//	glVertex3dv((*cv_it)->controlPosition());
			//}
			int size = mesh->numVertices();
			for (int i = 0; i < size; i++)
			{
				auto pos = AMCAX::TMS::TMSplineTool::ControlPosition(mesh, i);
				glVertex3dv(pos.Coord().Data());
			}

			glEnd();
		}

		colorDefault(is_selected);
		glLineWidth(1);

		//tspline::TSpline::CFaceIter cf_end = mesh->const_faces_end();
		//for (tspline::TSpline::CFaceIter cf_it = mesh->const_faces_begin(); cf_it != cf_end; ++cf_it)
		//{
		//	glBegin(GL_POLYGON);
		//	for (tspline::TVert* v : mesh->polygonVertices(*cf_it))
		//	{
		//		glVertex3dv(v->controlPosition());
		//	}
		//	glEnd();
		//}
		int size = mesh->numFaces();
		for (int i = 0; i < size; i++) {
			auto ids = AMCAX::TMS::TMSplineTool::FaceVertexIndexs(mesh, i);

			glBegin(GL_POLYGON);
			for (auto& iter : ids)
			{
				auto pos = AMCAX::TMS::TMSplineTool::ControlPosition(mesh, iter);
				glVertex3dv(pos.Coord().Data());
			}

			glEnd();
		}
	}

	void TSplineObjectDraw::draw_mesh_solidflat(AMCAX::TMS::TMSpline* tsp, bool is_selected) const
	{
		glLineWidth(1);
		materialSolidFlatDefault(is_selected);
		//tspline::TSpline::CFaceIter cf_end = tsp->const_faces_end();
		//for (tspline::TSpline::CFaceIter cf_it = tsp->const_faces_begin(); cf_it != cf_end; ++cf_it)
		//{
		//	glBegin(GL_POLYGON);
		//	glNormal3dv((*cf_it)->normal());

		//	for (tspline::TVert* v : tsp->polygonVertices(*cf_it))
		//	{
		//		glVertex3dv(v->controlPosition());
		//	}
		//	glEnd();
		//}
		{//draw polygon
			int size = tsp->numFaces();
			for (int i = 0; i < size; i++) {
				auto ids = AMCAX::TMS::TMSplineTool::FaceVertexIndexs(tsp, i);
				auto nor = AMCAX::TMS::TMSplineTool::ControlFaceNormal(tsp, i);

				glBegin(GL_POLYGON);
				glNormal3dv(nor.Coord().Data());
				for (auto& iter : ids)
				{
					auto pos = AMCAX::TMS::TMSplineTool::ControlPosition(tsp, iter);
					glVertex3dv(pos.Coord().Data());
				}

				glEnd();
			}
		}
		glEnable(GL_COLOR_MATERIAL);
		glColor3f(0.1, 0.1, 0.1);
		glLineWidth(3);

		{//draw line
			int size = tsp->numEdges();

			for (int i = 0; i < size; i++) {
				//std::cout << "====" << i << "====" << r0 << "," << r1 << std::endl;
				if (AMCAX::TMS::TMSplineCheck::IsEdgeBoundary(tsp, i) ||
					AMCAX::TMS::TMSplineCheck::IsEdgeCrease(tsp, i)) {
					int v0, v1;
					AMCAX::TMS::TMSplineTool::EdgeVertexIndexs(tsp, i, v0, v1);

					auto p0 = AMCAX::TMS::TMSplineTool::ControlPosition(tsp, v0),
						p1 = AMCAX::TMS::TMSplineTool::ControlPosition(tsp, v1);
					glBegin(GL_LINES);
					glVertex3dv(p0.Coord().Data());
					glVertex3dv(p1.Coord().Data());
					glEnd();
				}

			}

		}

		glDisable(GL_COLOR_MATERIAL);
	}

	void TSplineObjectDraw::draw_mesh_smooth(bool is_selected) const
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
		//return;

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

	void TSplineObjectDraw::draw_mesh_zebra() const
	{
		static GLubyte T0[64][64];
		static GLuint Tname0 = 1;
		for (int i = 0; i < 64; i++)
		{
			for (int j = 0; j < 64; j++)
			{
				int c = 0;
				if (i < 16 || i >= 48)
					c = 255;

				T0[i][j] = (GLubyte)c;
			}
		}
		auto mesh = tsp_mesh_->GetCurvatureMesh();
		if (mesh == nullptr)
			return;
		//MVector3 L(1, 1, 0); L.normalize();

		materialSmoothDefault(false);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glGenTextures(1, &Tname0);
		glBindTexture(GL_TEXTURE_2D, Tname0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 64, 64, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, T0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, Tname0);

		int i = 0;
		for (const auto& fh : mesh->face_list_)
		{
			glBegin(GL_POLYGON);
			glNormal3dv(mesh->face_list_[i].normal_.Coord().Data());
			for (const auto& fvh : fh.vert_)
			{
				const AMCAX::Vector3& n = mesh->vert_lsit_[fvh].normal_;
				const AMCAX::Point3& p = mesh->vert_lsit_[fvh].position_;

				double alpha = (n.Coord().Dot(camera_dir) + 1) / 2;
				alpha = alpha * 16;                                 //16 different 

				glTexCoord2d(alpha, alpha);
				glVertex3dv(p.Coord().Data());
			}
			glEnd();
			i++;
		}

		glDisable(GL_TEXTURE_2D);
	}

	void TSplineObjectDraw::draw_mesh_curvature_comb() const
	{
		glColor3f(0.2, 0.2, 0.2);

		auto mesh = tsp_mesh_->GetCurvatureMesh();
		if (mesh == nullptr)
			return;
		size_t edge_num = mesh->edge_list_.size();

		double scale = 0.01;
		for (size_t i = 0; i < edge_num; i++)
		{
			auto e = mesh->edge_list_[i];
			auto v1 = mesh->vert_lsit_[e.v1_];
			auto v2 = mesh->vert_lsit_[e.v2_];
			AMCAX::Coord3 vn1 = v1.position_.Coord() + v1.Additional_[e.Additional_[0]] * v1.normal_.Coord() * scale;
			AMCAX::Coord3 vn2 = v2.position_.Coord() + v2.Additional_[e.Additional_[1]] * v2.normal_.Coord() * scale;
			//MVector3 vn1 = v1.position_ + v1.normal_ * scale;
			//MVector3 vn2 = v2.position_ + v2.normal_ * scale;

			//有一个重复绘制
			glBegin(GL_LINES);
			glVertex3dv(v1.position_.Coord().Data());
			glVertex3dv(vn1.Data());
			glEnd();

			glBegin(GL_LINES);
			glVertex3dv(v2.position_.Coord().Data());
			glVertex3dv(vn2.Data());
			glEnd();

			glBegin(GL_LINES);
			glVertex3dv(vn1.Data());
			glVertex3dv(vn2.Data());
			glEnd();
		}

		glColor3f(0.1, 0.1, 0.1);
	}

	//======================================================

	void TSplineObjectDraw::updatedrawState()
	{
		if (!parent_->tSpline)
			return;

		auto spl = parent_->tSpline->getShape();

		if (spl == nullptr)
		{
			vert_position_.clear();
			vert_normal_.clear();
			face_vertices_.clear();
			edge_vertices_.clear();
			return;
		}



		if (!tsp_mesh_)
		{
			//tsp_mesh_ = new tspline::TSplineMeshing(tspline_object_->spline_, 6);
			//tsp_mesh_->updateTSplineWatertightDraw();
			//tsp_mesh_->writeTsplineBezier("test_spline_bezier.obj");
			//tsp_mesh_->writeDrawMesh("Test_show_Mesh.obj");
			//tsp_mesh_ = std::make_unique<AMCAX::TMS::TMSplineMeshing>(spl, 6);
			initMesh(6);
			//p_mesh_->UpdateDrawMesh();
		}
		//else
		//{
		tsp_mesh_->UpdateDrawMesh();
		//tsp_mesh_->writeTsplineBezier("test_spline_bezier.obj");
		//tsp_mesh_->writeDrawMesh("Test_show_Mesh.obj");
		//}


		auto mesh = tsp_mesh_->GetDrawMesh();
		if (mesh == nullptr)
			return;
		//mesh->edge_list_[0].
		const size_t v_number = mesh->vert_lsit_.size();
		vert_position_.resize(v_number * 3);
		vert_normal_.resize(v_number * 3);

		for (size_t i = 0; i < v_number; i++)
		{
			const AMCAX::Point3& p = mesh->vert_lsit_[i].position_;
			const AMCAX::Vector3& n = mesh->vert_lsit_[i].normal_;

			vert_position_[3 * i + 0] = p.X();
			vert_position_[3 * i + 1] = p.Y();
			vert_position_[3 * i + 2] = p.Z();
			vert_normal_[3 * i + 0] = n.X();
			vert_normal_[3 * i + 1] = n.Y();
			vert_normal_[3 * i + 2] = n.Z();

			//if (i < 20) {
			//	std::cout << i << " p:" << p.X() << "," << p.Y() << "," << p.Z()
			//		<< " n:" << n.X() << "," << n.Y() << "," << n.Z() << std::endl;
			//}
		}



		face_vertices_.clear();
		face_vertices_.reserve(mesh->face_list_.size());
		std::vector<size_t> f_vertices; f_vertices.reserve(4);
		for (const auto& fh : mesh->face_list_)
		{
			f_vertices.clear();
			for (size_t i = 0; i < 3; i++)
			{
				f_vertices.push_back(fh.vert_[i]);
			}
			face_vertices_.push_back(f_vertices);
		}

		edge_vertices_.clear();
		edge_vertices_.reserve(mesh->edge_list_.size());
		for (const auto& eh : mesh->edge_list_)
		{
			size_t ev0 = eh.v1_;
			size_t ev1 = eh.v2_;
			edge_vertices_.push_back({ ev0 , ev1 });
		}

	}

	void TSplineObjectDraw::updatedrawStateFast(bool is_topology_change)
	{
		//auto mesh = parent_->spline->getTSpline();
		//mesh->updateNormals();

		initMesh(6);

	}

	//======================================================

	void TSplineObjectDraw::clearCurvature()
	{
		tsp_mesh_->ClearCurvatrueMesh();
	}

	void TSplineObjectDraw::updatedrawZebra()
	{
		if (tsp_mesh_ == nullptr)
		{
			initMesh(3);
			//p_mesh_ = std::make_unique<AMCAX::TMS::TMSplineMeshing>(parent_->spline->getTSpline(), 3);//  new tspline::TSplineMeshing(tspline_object_->spline_, 3);
			tsp_mesh_->SetSubtime(4);
			tsp_mesh_->UpdateMeshForZebra(4);
		}
		else
		{
			//tsp_mesh_->SetSubtime(4);
			tsp_mesh_->UpdateMeshForZebra(4);
		}

	}

	void TSplineObjectDraw::updatedrawCurvatureGraph(std::vector<int> face_index)
	{
		if (tsp_mesh_ == nullptr)
		{
			initMesh(3);
			//tsp_mesh_ = std::make_unique<AMCAX::TMS::TMSplineMeshing>(parent_->spline->getTSpline(), 3);
			//new tspline::TSplineMeshing(tspline_object_->spline_, 3);
			//tsp_mesh_->updateTsplineCurvatureGraph(face_index, 8);
			tsp_mesh_->UpdateMeshForCurvatureGraph(face_index, 8);
			//tsp_mesh_->writeCurvature("Test_curvature.obj", "Test_curvature.txt");
		}
		else
		{
			tsp_mesh_->UpdateMeshForCurvatureGraph(face_index, 8);
			//tsp_mesh_->writeCurvature("Test_curvature.obj", "Test_curvature.txt");
		}
#ifdef QT_DEBUG
		//formtTranslation::ExportJson_CurvatureGraph testjson;
		//testjson.exportJsonToFilePretty(tsp_mesh_, "test.json");
#endif
	}

	//======================================================

	bool TSplineObjectDraw::saveSTL(const std::string& filename)
	{

		initMesh(4);
		tsp_mesh_->SetSubtime(4);
		tsp_mesh_->UpdateDrawMesh();
		//formtTranslation::ExportSTL_TSplingMeshing stl_out;
#ifdef QT_DEBUG
		//stl_out.write_begin();
		//stl_out.input_TSplineMesh(tsp_mesh_);
		//stl_out.write_ToFile(filename);
#endif
		return AMCAX::TMS::TMSplineMeshingIO().ExportToSTL(filename, tsp_mesh_->GetDrawMesh());
	}

	bool TSplineObjectDraw::saveOBJ(const std::string& filename)
	{
		initMesh(4);
		tsp_mesh_->SetSubtime(4);
		tsp_mesh_->UpdateDrawMesh();

		///tsp_mesh_->writeDrawMesh(filename);
		return AMCAX::TMS::TMSplineMeshingIO().ExportToOBJ(filename, tsp_mesh_->GetDrawMesh());
	}

	//bool TSplineBjectDraw::saveFrepMesh(const std::string& filename, const std::vector<int>& fixIndex, const std::vector<int>& forceIndex)
	//{
	//	if (tsp_mesh_ == nullptr)
	//	{
	//		tsp_mesh_ = std::make_unique<AMCAX::TMS::TMSplineMeshing>(parent_->spline->getTSpline(), 4);
	//	}
	//	tsp_mesh_->SetSubtime(8);
	//	tsp_mesh_->UpdateDrawMesh();


	//	return true;
	//}
	void TSplineObjectDraw::initMesh(int subtime)
	{

		if (tsp_mesh_ == nullptr && parent_->tSpline->getShape())
		{

			tsp_mesh_ = std::make_unique<AMCAX::TMS::TMSplineMeshing>(parent_->tSpline->getShape(), subtime);
		}
	}

}


