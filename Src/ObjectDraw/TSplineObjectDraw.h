#pragma once
#include "ObjectDraw.h"

#include <memory>

#include <tmeshSpline/TMSpline.hpp>
#include <tmeshSpline/TMSplineMeshing.hpp>

namespace acamcad
{

	//class TSplineObject;

	class AdapterObject;

	class TSplineObjectDraw : public ObjectDraw
	{
	public:
		TSplineObjectDraw(AdapterObject* object);
		virtual ~TSplineObjectDraw();

	public:
		virtual void draw(const DrawModel& dmodel, const SelectModel& smodel) const override;

		virtual void drawSelected(const SelectModel& smodel, int d_id) const override;

		virtual void clear() override;
	public:
		virtual void drawWithObjectID() const override;
		virtual void drawWithFaceID() const override;
		virtual void drawWithEdgeID() const override;
		virtual void drawWithVertexID() const override;

	public:
		virtual void updatedrawState() override;
		virtual void updatedrawStateFast(bool is_topology_change = true) override;

	public:
		virtual void clearCurvature() override;
		virtual void updatedrawZebra() override;
		virtual void updatedrawCurvatureGraph(std::vector<int> face_index) override;

	public:
		bool saveSTL(const std::string& filename);
		bool saveOBJ(const std::string& filename);
		//bool saveFrepMesh(const std::string& filename, const std::vector<int>& fixIndex, const std::vector<int>& forceIndex);

	private:
		void draw(const DrawModel& dmodel, bool is_show_point) const;

	private:
		void draw_mesh_wireframe(AMCAX::TMS::TMSpline* spline, bool is_selected, bool showPoint) const;

		void draw_mesh_solidflat(AMCAX::TMS::TMSpline* spline, bool is_selected) const;

		void draw_mesh_smooth(bool is_selected) const;

		void draw_mesh_zebra() const;

		void draw_mesh_curvature_comb() const;

		void initMesh(int subTime);

	private:
		//TSplineObject* tspline_object_;

		std::unique_ptr<AMCAX::TMS::TMSplineMeshing> tsp_mesh_;

		std::vector<GLdouble> vert_position_;
		std::vector<GLdouble> vert_normal_;
		std::vector<std::vector<size_t>> face_vertices_;
		std::vector<std::vector<size_t>> edge_vertices_;
	};



}