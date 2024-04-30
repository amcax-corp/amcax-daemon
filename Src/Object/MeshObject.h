#pragma once
#include "BaseObject.h"

//#include <PolyMesh/IOManager.h>
#include <common/PointT.hpp>
#include <polymesh/PolyMesh.hpp>
#include <common/VectorT.hpp>

#include "AdapterObjectParent.h"
#include <memory>

/// 
#define polymesh  AMCAX::SubD
typedef AMCAX::Point3 MPoint3;
typedef AMCAX::Vector3 MVector3;


namespace acamcad {
	class AdapterObject;
	class AdapterObjectParent;

	class MeshObject : public AdapterObjectParent
	{
		friend class ObjectDraw;
		friend class MeshObjectDraw;

	public:

		MeshObject(AdapterObject* parent);
		MeshObject(AdapterObject* parent, const MeshObject& object, bool isDraw);

		virtual ~MeshObject();

	public:
		void setShape(polymesh::PolyMesh* mesh);
		polymesh::PolyMesh* getShape();
		const polymesh::PolyMesh* getShape() const;

	private:
		std::unique_ptr<polymesh::PolyMesh> mesh_;
		//polymesh::PolyMesh* mesh_subdive_;

		std::vector<int> mesh_v_id_back_;
		//std::vector<MPoint3> mesh_v_position_back_;
		//std::vector<MVector3> mesh_normal_back_;
		float scale_ = 1;

		//public:
		//	/// Load a mesh from the given file
		//	bool loadMesh(QString filename);
		//	/// Save a mesh to the given file
		//	bool saveMesh(QString filename);
		//	bool saveTSpline(QString filename);

	public:
		const virtual ObjectDraw* getDrawTool() const { return draw_tool_.get(); }

	private:
		std::unique_ptr<ObjectDraw> draw_tool_;

	public:
		virtual void doOperate(MOperation* operate);

	public:
		void assignShape();

		//public:
		//	///void backMesh(polymesh::PolyMesh*& bmesh);
		//	void applyMesh(const polymesh::PolyMesh* amesh);

	public:
		bool combineObject(const MeshObject* mesh_object);

		//void duplicateFaces(MeshObject* mesh_object, std::vector<int>& f_list);

		void mirrorMeshObject(const MVector3& center, const MVector3& normal);

	public:
		//存储要移动，缩放，旋转的单元的原来的顶点 -- 暂时只有缩放和旋转用了
		//TODO： 结构升级
		void mesh_OPvert_Backup();
		void mesh_OPvert_BackupVertes(const std::vector<int>& vert_id);
		void mesh_OPvert_BackupEdges(const std::vector<int>& edge_id);
		void mesh_OPvert_BackupFaces(const std::vector<int>& face_id);
		void mesh_Clear_OPvertBackup();

		//在有了原来的顶点之后，所有的缩放都是一样的，没有体，边，面，点的区别了。
		void meshScaleSingle(const MVector3& center, const MVector3& axis, double scale);
		void meshScalePlane(const MVector3& center, const MVector3& normal, double scale);

		void meshRotation(const MVector3& center, const MVector3& axis, double angle);

		void meshMoveUsingNormal(double dis);
		void meshMove(const MVector3& v, double dis);


		// select vert edge face
		void selectVertWithFaceId(const MVector3& begin, const MVector3& end, int f_id, int& sv_id);
		void selectEdgeWithFaceId(const MVector3& begin, const MVector3& end, int f_id, int& se_id);
		//void selectFaceWithFaceId(const MVector3& begin, const MVector3& end, int f_id, int& sf_id);



	public:
		void meshTriangulating();
		//void meshQuadrilateralizatuon();

		///void reverseMeshNormal();

		void subdivemeshCC(int sub_time = 3);
		void subdivemeshLoop(int sub_time = 3);

		void updateDraw();

	public:
		void weldEdge(std::vector<int>& e_list);
		///void shellFace(const std::vector<int>& face_id_list, std::vector<int>& face_id_new);


		//next function have been replace;
		/*
	private:
		void createPlane(const MPoint3& p0, const MPoint3& p1, size_t u_num, size_t v_num);
		void createCube(const MPoint3& getCenter, double length);
		void createCube(const MPoint3& First, const MPoint3& second);
		void createUVSphere(MPoint3& getCenter, double radius, size_t rf_num, size_t vf_num);
		void createSubdSphere(MPoint3& getCenter, double radius, size_t subtime);
		void createCylinder(MPoint3& b_center, MVector3& axis, double radius, double height,
			size_t rf_num, size_t vf_num, bool top, bool bottom);
		void createCone(MPoint3& b_center, MVector3& axis, double radius, double height,
			size_t rf_num, size_t vf_num, bool bottom);
		void createPolyFace(const std::vector<MPoint3>& point_list);

		//type 0:mesh center, 1:vert center, 2:edge center, 3:face center
		void getLocalCenter(MVector3& c, int s_id = -1, int type = 0);
		void getLocalNormal(MVector3& c, int s_id = -1, int type = 0);
		void getLocalBBox(MVector3& bb_min, MVector3& bb_max, int s_id = -1, int type = 0);
		void MeshPlaneVert(const std::vector<int>& vert_id, MVector3& center, MVector3& normal);
		void MeshPlaneEdge(const std::vector<int>& edge_id, MVector3& center, MVector3& normal);
		void MeshPlaneFace(const std::vector<int>& face_id, MVector3& center, MVector3& normal);

		//返回给定选中元素与射线的最近点
		void closestPointWithEdge(const int& edge_id, const MVector3& begin, const MVector3& end, MVector3& clost);

		void deleteVerts(std::vector<int>& v_list);
		void deleteEdges(std::vector<int>& e_list);
		void deleteFaces(std::vector<int>& f_list);

		void bridgeEdge(int e0_id, int e1_id);
		void embedFaceVert(std::vector<int>& v_list);
		void embedFaceEdge(int e0_id, int e1_id);
		void separateEdge(std::vector<int>& e_list);
		///type == 0, Vertical, type == 1, Horizontal
		void extrudeEdge(const std::vector<int>& edge_id_list, MVector3& dire, std::vector<int>& edge_id_new, int type = 1);
		void extrudeFace(const std::vector<int>& face_id_list, MVector3& dire, std::vector<int>& face_id_new);
		*/

	public:
		/// Get all Info for the Object as a string
		QString getObjectinfo();

		//AdapterObject* getParent() { return parent_; };


	//private:
	//	AdapterObject* parent_;


	};

} //acamcad

