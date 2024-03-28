#pragma once
#include "ObjectDraw.h"
#include "../Object/MeshObject.h"

namespace acamcad
{
	class AdapterObject;

	class MeshObjectDraw : public ObjectDraw
	{
	public:
		MeshObjectDraw(AdapterObject* object);
		virtual ~MeshObjectDraw();

	public:
		virtual void draw(const DrawModel& dmodel, const SelectModel& smodel) const override;

		virtual void drawSelected(const SelectModel& smodel, int d_id) const override;

	public:
		//virtual void drawForSelect() const override;
		//virtual void drawForSelectPart() const override;

		virtual void drawWithObjectID() const override;
		virtual void drawWithFaceID() const override;
		virtual void drawWithEdgeID() const override;
		virtual void drawWithVertexID() const override;

	public:
		virtual void updatedrawState();

	private:
		void draw(const DrawModel& dmodel, bool is_show_point) const;

	private:
		void draw_mesh_wireframe(const polymesh::PolyMesh* mesh, bool is_selected, bool showPoint) const;

		void draw_mesh_solidflat(const polymesh::PolyMesh* mesh, bool is_selected) const;

		void draw_mesh_smooth(const polymesh::PolyMesh* mesh, bool is_selected) const;

	private:
		//AdapterObject* parent_;

		std::vector<GLdouble> vert_position_;
		std::vector<GLdouble> vert_normal_;
		std::vector<std::vector<size_t>> face_vertices_;
		std::vector<std::vector<size_t>> edge_vertices_;
	};

} //namespace acamcad