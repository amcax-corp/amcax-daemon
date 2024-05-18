#pragma once
#include "BaseObject.h"

#include <set>

#include <tmeshSpline/TMSpline.hpp>
#include "AdapterObjectParent.h"
//#include <TSpline/TSpline.h>
//#include <Math/M_Transform.h>
#include <memory>

namespace acamcad
{
	class TSplineObjectDraw;

	class TSplineObject : public AdapterObjectParent
	{
		friend class ObjectDraw;
		friend class TSplineObjectDraw;


	public:
		//TSplineObject();
		//TSplineObject(const TSplineObject& object);
		TSplineObject(AdapterObject* parent);
		TSplineObject(AdapterObject* parent, const TSplineObject& object, bool isDraw);

		virtual ~TSplineObject();

	public:
		//virtual void doOperate(MOperation* operate) override;
		virtual void updateDraw();

	public:
		AMCAX::TMS::TMSpline* getShape();
		const AMCAX::TMS::TMSpline* getShape() const;

		/// This function is Very danger, only using in creat operation, I am trying other ways to replace it 
		void setShape(AMCAX::TMS::TMSpline* tsp);

		void assignShape();

		// select vert edge face
		void selectVertWithFaceId(const AMCAX::Vector3& begin, const AMCAX::Vector3& end, int f_id, int& sv_id);
		void selectEdgeWithFaceId(const AMCAX::Vector3& begin, const AMCAX::Vector3& end, int f_id, int& se_id);

		//public:
			//void backSpline(AMCAX::TMS::TMSpline*& back_spline);
			//void applySpline(const AMCAX::TMS::TMSpline* apply_spline);

	//public:
	//	/// Load a mesh from the given file
	//	bool loadSpline(const QString& filename);
	//	/// Save a mesh to the given file
	//	bool saveSpline(const QString& filename);

	//	bool saveSplineSTL(const QString& filename);
	//	bool saveSplineOBJ(const QString& filename);

		//bool saveSplineFrep(const QString& filename);
		//std::set<int> frepFixFaceIndex;
		//std::set<int> frepForceFaceIndex;

	private:
		std::unique_ptr<AMCAX::TMS::TMSpline> spline_;

	public:
		void mesh_OPvert_Backup();
		void mesh_OPvert_BackupVertes(const std::vector<int>& vert_id);
		void mesh_OPvert_BackupEdges(const std::vector<int>& edge_id);
		void mesh_OPvert_BackupFaces(const std::vector<int>& face_id);
		void mesh_Clear_OPvertBackup();

		void meshScaleSingle(const AMCAX::Vector3& center, const AMCAX::Vector3& axis, double scale);
		void meshScalePlane(const AMCAX::Vector3& center, const AMCAX::Vector3& normal, double scale);
		void meshRotation(const AMCAX::Vector3& center, const AMCAX::Vector3& axis, double angle);
		///void meshMoveUsingNormal(double dis);
		void meshMove(const AMCAX::Vector3& v, double dis);

		void mirror(const AMCAX::Coord3& center, const AMCAX::Coord3& normal);

	private:
		///void applyTransForm(acamcad::M_Transform& trsf_);

		std::vector<int> mesh_v_id_back_;
		double scale_;
		//std::vector<MPoint3> mesh_v_position_back_;

	public:
		//bool combineObject(const TSplineObject* object);

		//void duplicateFaces(TSplineObject* object, std::vector<int>& f_list);

		//TSplineObject* mioorMeshObject(const MVector3& center, const MVector3& normal);

	public:
		const virtual ObjectDraw* getDrawTool() const;
	private:
		std::unique_ptr<TSplineObjectDraw> draw_tool_;

	public:
		/// Get all Info for the Object as a string
		//QString getObjectinfo();



	};

} //namespace acamcad