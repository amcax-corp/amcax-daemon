#include "MeshObject.h"
#include <set>
// #include <PolyMesh_Operation/PolyMeshOperation.h>

#include "../ObjectDraw/MeshObjectDraw.h"
#include "../Operation/MOperate.h"

#include <common/FrameT.hpp>
#include <common/AxisT.hpp>
#include <common/DirectionT.hpp>
#include <common/CartesianCoordinateSystem.hpp>

#include <polymesh/PolyMesh.hpp>
#include <polymesh/PolyMeshIO.hpp>

#include <polymeshAPI/MeshTransform.hpp>
#include <polymeshAPI/MeshTool.hpp>
#include <polymeshAPI/MeshCopy.hpp>
#include <polymeshAPI/MeshSubdiveHE.hpp>
#include <polymeshAPI/MeshWeld.hpp>

#include "../Utils/MathUtils.h"

// #include <Math/Algorithms.h>
// #include <Math/M_Transform.h>

namespace acamcad
{

	MeshObject::MeshObject(AdapterObject *parent) : AdapterObjectParent(parent) //, mesh_(nullptr)//, mesh_subdive_(nullptr)
	{
		/// setDataType(DataType::MESH_TYPE);
		draw_tool_ = std::make_unique<MeshObjectDraw>(parent_);
	}

	MeshObject::MeshObject(AdapterObject *parent, const MeshObject &object, bool isDraw) : AdapterObjectParent(parent) //, mesh_(nullptr)//, mesh_subdive_(nullptr)
	{
		if (object.mesh_ != nullptr)
		{
			if (object.mesh_ != nullptr)
			{
				// mesh_ = new polymesh::PolyMesh();
				mesh_.reset(
					AMCAX::SubD::MeshCopy::CopyMesh(object.getShape()));
			}
		}

		// setDataType(DataType::MESH_TYPE);
		if (isDraw)
		{
			draw_tool_ = std::make_unique<MeshObjectDraw>(parent_);
			draw_tool_->updatedrawState();
		}
	}

	MeshObject::~MeshObject()
	{
		// if (mesh_ != nullptr)
		//{
		//	mesh_->clear();
		//	delete mesh_;
		// }
	}

	void MeshObject::setShape(polymesh::PolyMesh *mesh)
	{
		mesh_.reset(mesh);
	}

	polymesh::PolyMesh *MeshObject::getShape()
	{
		return mesh_.get();
	}

	const polymesh::PolyMesh *MeshObject::getShape() const
	{
		return mesh_.get();
	}

	//===========================================================

	void MeshObject::doOperate(MOperation *operate)
	{
		operate->operate(parent_);
	}

	//===========================================================

	void MeshObject::assignShape()
	{
		if (mesh_ == nullptr)
			mesh_ = std::make_unique<polymesh::PolyMesh>(); // new polymesh::PolyMesh();
	}

	//===========================================================

	bool MeshObject::combineObject(const MeshObject *mesh_object)
	{
		if (mesh_ == nullptr)
			return false;
		if (mesh_object->mesh_ == nullptr)
			return false;

		polymesh::MeshCopy::CombineMesh(getShape(), mesh_object->getShape());

		return true;
	}

	// void MeshObject::duplicateFaces(MeshObject* mesh_object, std::vector<int>& f_list)
	//{
	//	std::sort(f_list.begin(), f_list.end(), std::greater<int>());
	//	f_list.erase(std::unique(f_list.begin(), f_list.end()), f_list.end());

	//	if (mesh_object->mesh_ == nullptr)
	//		mesh_object->mesh_ = std::make_unique<polymesh::PolyMesh>();

	//	//polymesh::copyMeshFaces(mesh_, mesh_object->mesh_, f_list);
	//	mesh_object->mesh_.reset(polymesh::MeshCopy::ExtractMeshFaces(getShape(), f_list));
	//}

	void MeshObject::mirrorMeshObject(const MVector3 &center, const MVector3 &normal)
	{
		// if (mesh_ == nullptr)
		//	return nullptr;

		// AMCAX::SubD::PolyMesh* copy_mesh = AMCAX::SubD::MeshCopy::CopyMesh(mesh_);

		// AdapterObject* adapter_object = new AdapterObject;
		// MeshObject* mesh_object_new = new MeshObject(adapter_object);
		// mesh_object_new->mesh_ = AMCAX::SubD::MeshCopy::CopyMesh(mesh_);
		/// mesh_object_new->mesh_ = polymesh::meshMioor(mesh_, center.point(), normal);

		AMCAX::Point3 ct(center.X(), center.Y(), center.Z());
		AMCAX::Frame3 ft(ct, AMCAX::Direction3(normal));
		AMCAX::SubD::MeshTransform trsfF;
		AMCAX::Transformation3 trsfMove;
		trsfMove.SetMirror(ft);
		trsfF.SetTransformation(trsfMove);
		trsfF.TranformMesh(getShape());

		// return mesh_object_new;
	}

	//===========================================================
	void MeshObject::mesh_OPvert_Backup()
	{
		// AMCAX::SubD::Mes
		scale_ = 1;
		mesh_v_id_back_.resize(mesh_->numVertices());
		// mesh_v_position_back_.resize(mesh_->numVertices());
		for (int i = 0; i < mesh_->numVertices(); i++)
		{
			mesh_v_id_back_[i] = i;
			// mesh_v_position_back_[i] = AMCAX::SubD::MeshTool::Position(mesh_, i);// mesh_->vert(i)->position();
		}
	}

	void MeshObject::mesh_OPvert_BackupVertes(const std::vector<int> &vert_id)
	{
		mesh_v_id_back_ = vert_id;
		scale_ = 1;
		// mesh_v_position_back_.resize(vert_id.size());
		// for (int i = 0; i < vert_id.size(); i++)
		//{
		//	mesh_v_position_back_[i] = AMCAX::SubD::MeshTool::Position(mesh_, i);//mesh_->vert(vert_id[i])->position();
		// }
	}

	void MeshObject::mesh_OPvert_BackupEdges(const std::vector<int> &edge_id)
	{
		scale_ = 1;
		std::vector<int> vert_id;
		vert_id.clear();
		vert_id.reserve(edge_id.size() * 2);
		for (size_t i = 0; i < edge_id.size(); i++)
		{
			// polymesh::MEdge* e = mesh_->edge(edge_id[i]);
			// vert_id.push_back(e->getVert(0)->index());
			// vert_id.push_back(e->getVert(1)->index());
			int id0, id1;
			AMCAX::SubD::MeshTool::EdgeVertexIndexs(getShape(), edge_id[i], id0, id1);
			vert_id.push_back(id0);
			vert_id.push_back(id1);
		}

		std::sort(vert_id.begin(), vert_id.end());
		vert_id.erase(std::unique(vert_id.begin(), vert_id.end()), vert_id.end());

		mesh_v_id_back_ = vert_id;
		// mesh_v_position_back_.resize(vert_id.size());
		// for (int i = 0; i < vert_id.size(); i++)
		//{
		//	mesh_v_position_back_[i] = AMCAX::SubD::MeshTool::Position(mesh_, i);//mesh_->vert(vert_id[i])->position();
		// }
	}

	void MeshObject::mesh_OPvert_BackupFaces(const std::vector<int> &face_id)
	{
		scale_ = 1;
		std::vector<int> vert_id;
		vert_id.clear();
		vert_id.reserve(face_id.size() * 4);
		for (size_t i = 0; i < face_id.size(); i++)
		{
			auto ids = AMCAX::SubD::MeshTool::FaceVertexIndexs(getShape(), face_id[i]);

			for (auto iter : ids)
			{
				vert_id.push_back(iter);
			}
		}
		std::sort(vert_id.begin(), vert_id.end());
		vert_id.erase(std::unique(vert_id.begin(), vert_id.end()), vert_id.end());

		mesh_v_id_back_ = vert_id;
		// mesh_v_position_back_.resize(vert_id.size());
		// for (int i = 0; i < vert_id.size(); i++)
		//{
		//	mesh_v_position_back_[i] = AMCAX::SubD::MeshTool::Position(mesh_, i);
		// }
	}

	void MeshObject::mesh_Clear_OPvertBackup()
	{
		mesh_v_id_back_.clear();
		// mesh_v_position_back_.clear();
		scale_ = 1;

		AMCAX::SubD::MeshTool::UpdateNormal(getShape());
	}

	void MeshObject::meshScaleSingle(const MVector3 &center, const MVector3 &axis, double scale)
	{
		scale_ = scale / scale_;

		if (std::isnan(scale_))
		{
			scale_ = 1;
			return;
		}

		/// std::cout << "meshScaleSingle center:(" << center.X() << "," << center.Y() << "," << center.Z() << ") scale:" << scale << std::endl;

		AMCAX::Point3 point(center.X(), center.Y(), center.Z());
		AMCAX::Direction3 dir(axis.X(), axis.Y(), axis.Z());
		// AMCAX::Axis3
		AMCAX::Frame3 ct(point, dir);
		// AMCAX::Frame3 ft(ct, AMCAX::Direction3(normal));
		AMCAX::SubD::MeshTransform trsfF;
		// AMCAX::Transformation3 trfScale;
		AMCAX::GeneralTransformation3 trfScale;
		// trfRotation.SetRotation(AMCAX::Axis3(ct, AMCAX::Direction3(axis.X(), axis.Y(), axis.Z())), angle);
		// trfScale.SetScale(ct, scale);
		// trfScale.Set
		//	trsfF.SetGTransformation(trfScale);

		trfScale.SetAffinity(ct, scale_);
		trsfF.SetGTransformation(trfScale);
		trsfF.TransformMeshVertices(getShape(), mesh_v_id_back_);

		scale_ = scale;
	}

	void MeshObject::meshScalePlane(const MVector3 &center, const MVector3 &normal, double scale)
	{
		/// std::cout << "meshScalePlane is error,scale:" << scale << std::endl;
		scale_ = scale / scale_;

		if (std::isnan(scale_))
		{
			scale_ = 1;
			return;
		}

		AMCAX::Point3 point(center.X(), center.Y(), center.Z());
		AMCAX::Direction3 dir(normal.X(), normal.Y(), normal.Z());
		// AMCAX::Axis3
		AMCAX::Axis3 ct(point, dir);
		// AMCAX::Frame3 ft(ct, AMCAX::Direction3(normal));
		AMCAX::SubD::MeshTransform trsfF;
		// AMCAX::Transformation3 trfScale;
		AMCAX::GeneralTransformation3 trfScale;
		// trfRotation.SetRotation(AMCAX::Axis3(ct, AMCAX::Direction3(axis.X(), axis.Y(), axis.Z())), angle);
		// trfScale.SetScale(ct, scale);
		// trfScale.Set
		//	trsfF.SetGTransformation(trfScale);

		trfScale.SetAffinity(ct, scale_);
		trsfF.SetGTransformation(trfScale);
		trsfF.TransformMeshVertices(getShape(), mesh_v_id_back_);

		scale_ = scale;
	}

	void MeshObject::meshRotation(const MVector3 &center, const MVector3 &axis, double angle)
	{
		// std::cout << "MeshObject::meshRotation angle:" << angle << std::endl;
		AMCAX::Point3 ct(center.X(), center.Y(), center.Z());
		// AMCAX::Frame3 ft(ct, AMCAX::Direction3(normal));
		AMCAX::SubD::MeshTransform trsfF;
		AMCAX::Transformation3 trfRotation;
		trfRotation.SetRotation(AMCAX::Axis3(ct, AMCAX::Direction3(axis.X(), axis.Y(), axis.Z())), angle);
		trsfF.SetTransformation(trfRotation);

		trsfF.TransformMeshVertices(getShape(), mesh_v_id_back_);

		/*

		AMCAX::SubD::MeshTransform trsfF;
		AMCAX::Transformation3 trfMove;
		trfMove.SetTranslation(dist * v);
		trsfF.SetTransformation(trfRotation);

		trsfF.TransformMeshVertices(mesh, mesh_v_id_back_);


		*/
	}

	void MeshObject::meshMoveUsingNormal(double dis)
	{
		std::cout << "MeshObject::meshMoveUsingNormal" << std::endl;
	}

	void MeshObject::meshMove(const MVector3 &v, double dis)
	{
		AMCAX::Vector3 vh(v.Normalized() * dis);
		AMCAX::Transformation3 trshMove;
		// AMCAX::Axis3 ax;
		trshMove.SetTranslation(vh);

		AMCAX::SubD::MeshTransform trsf;
		trsf.SetTransformation(trshMove);

		trsf.TransformMeshVertices(getShape(), mesh_v_id_back_);
	}

	// select vert edge face
	void MeshObject::selectVertWithFaceId(const MVector3 &begin, const MVector3 &end, int f_id, int &sv_id)
	{
		sv_id = -1;

		auto ids = AMCAX::SubD::MeshTool::FaceVertexIndexs(getShape(), f_id);
		AMCAX::Coord3 coord;
		double len_min;

		for (int i = 0; i < ids.size(); i++)
		{
			AMCAX::Point3 position = AMCAX::SubD::MeshTool::Position(getShape(), ids[i]);
			double len = MathUtils::distPointLineSquared(position.Coord(), begin.Coord(), end.Coord(), coord);

			if (len > 0.1)
			{
				continue;
			}

			if (sv_id == -1 || len_min > len)
			{
				sv_id = ids[i];
				len_min = len;
			}
		}
	}

	void MeshObject::selectEdgeWithFaceId(const MVector3 &begin, const MVector3 &end, int f_id, int &se_id)
	{
		se_id = -1;
		auto ids = AMCAX::SubD::MeshTool::FaceEdgeIndexs(getShape(), f_id);
		AMCAX::Coord3 coord0, coord1;
		double len_min;

		for (int i = 0; i < ids.size(); i++)
		{
			int id0, id1;
			AMCAX::SubD::MeshTool::EdgeVertexIndexs(getShape(), ids[i], id0, id1);
			AMCAX::Point3 p0 = AMCAX::SubD::MeshTool::Position(getShape(), id0),
						  p1 = AMCAX::SubD::MeshTool::Position(getShape(), id1);
			double len = MathUtils::distLine_SLineSquared(p0.Coord(), p1.Coord(), begin.Coord(), end.Coord(), coord0, coord1);

			if (len > 0.1)
			{
				continue;
			}

			if (se_id == -1 || len_min > len)
			{
				se_id = ids[i];
				len_min = len;
			}
		}
	}

	////===========================================================

	// void MeshObject::backMesh(polymesh::PolyMesh*& bmesh)
	//{
	//	if (mesh_ != nullptr)
	//	{
	//		if (bmesh == nullptr)
	//			bmesh = new polymesh::PolyMesh();

	//		bmesh = AMCAX::SubD::MeshCopy::CopyMesh(getShape());

	//		//polymesh::CopyMesh(mesh_, bmesh);
	//	}
	//	else
	//	{
	//		bmesh = nullptr;
	//	}
	//}

	// void MeshObject::applyMesh(const polymesh::PolyMesh* amesh)
	//{
	//	mesh_->clear();
	//	mesh_.reset(AMCAX::SubD::MeshCopy::CopyMesh(amesh));
	// }

	//===========================================================

	void MeshObject::meshTriangulating()
	{
		// polymesh::meshTriangles(mesh_);
		AMCAX::SubD::MeshTool::MeshTriangles(getShape());
	}

	// void MeshObject::meshQuadrilateralizatuon()
	//{
	//	//polymesh::meshQuadrilate(mesh_);
	//	std::cout << "MeshObject::meshQuadrilateralizatuon error" << std::endl;
	// }

	void MeshObject::subdivemeshCC(int sub_time)
	{
		// polymesh::catmullclark_Subdive(mesh_, sub_time);

		AMCAX::SubD::MeshSubdiveHE::CatmullClarkSubdive(getShape(), sub_time);
		draw_tool_->updatedrawState();
	}

	void MeshObject::subdivemeshLoop(int sub_time)
	{

		// if (mesh_->isTriangleMesh()) error

		AMCAX::SubD::MeshSubdiveHE::LoopSubdive(getShape(), sub_time);
		draw_tool_->updatedrawState();
		// polymesh::loop_Subdive(mesh_, sub_time);
	}

	void MeshObject::updateDraw()
	{
		draw_tool_->updatedrawState();
	}

	//===========================================================

	void MeshObject::weldEdge(std::vector<int> &e_list)
	{
		// polymesh::meshWeldEdges(mesh_, e_list);
		AMCAX::SubD::MeshWeld::MeshWeldEdges(getShape(), e_list);
	}

	//===========================================================

	QString MeshObject::getObjectinfo()
	{
		QString output;

		output += "========================================================================\n";
		output += parent_->getObjectinfo();

		output += "Object Contains Poly Mesh : ";
		output += QString::number(getShape()->numVertices()) + " vertices, ";
		output += QString::number(getShape()->numEdges()) += " edges ";
		output += QString::number(getShape()->numPolygons()) += " faces.\n";

		output += "========================================================================\n";
		return output;
	}

	//===========================================================
}
