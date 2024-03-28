#include "AdapterObject.h"
#include "BRepObject.h"
#include "MeshObject.h"
#include "TSplineObject.h"
#include "../Operation/MOperate.h"

#include <topology/BRepBoundingBox.hpp>
#include <polymesh/PolyMesh.hpp>
#include <polymeshAPI/MeshTool.hpp>
#include <tmeshSplineAPI/TMSplineTool.hpp>

namespace acamcad
{
	AdapterObject::AdapterObject()
		:BaseObject()
	{
		//setDataType(DataType::BREP_TYPE);
	}

	AdapterObject::AdapterObject(const AdapterObject& object, bool isDraw)
	{
		//copy data
		setPersistentId(object.persistentId());
		setId(object.id());
		setLabel(object.label());

		//copy shape
		if (object.bRep != NULL) {
			BaseObject::setDataType(object.dataType());
			bRep = std::make_unique<BRepObject>(this, *object.bRep, isDraw);

			if (isDraw)
				bRep->updateDraw();
		}

		if (object.mesh != NULL) {
			BaseObject::setDataType(object.dataType());
			mesh = std::make_unique<MeshObject>(this, *object.mesh, isDraw);

			if (isDraw)
				mesh->updateDraw();
		}

		if (object.tSpline != NULL) {
			BaseObject::setDataType(object.dataType());
			tSpline = std::make_unique<TSplineObject>(this, *object.tSpline, isDraw);

			if (isDraw)
				tSpline->updateDraw();

		}
	}

	AdapterObject::~AdapterObject()
	{
		//shape_.Nullify();
	}

	// select vert edge face
	void AdapterObject::selectVertWithFaceId(const AMCAX::Vector3& begin, const AMCAX::Vector3& end, int f_id, int& sv_id)
	{
		switch (dataType())
		{
		case acamcad::DataType::BREP_TYPE:
			bRep->selectVertWithFaceId(begin, end, f_id, sv_id);
			break;
		case acamcad::DataType::MESH_TYPE:
			mesh->selectVertWithFaceId(begin, end, f_id, sv_id);
			break;
		case acamcad::DataType::TSPLINEU_TYPE:
			tSpline->selectVertWithFaceId(begin, end, f_id, sv_id);
			break;
		default:
			break;
		}
	}
	void AdapterObject::selectEdgeWithFaceId(const AMCAX::Vector3& begin, const AMCAX::Vector3& end, int f_id, int& se_id)
	{
		switch (dataType())
		{
		case acamcad::DataType::BREP_TYPE:
			bRep->selectEdgeWithFaceId(begin, end, f_id, se_id);
			break;
		case acamcad::DataType::MESH_TYPE:
			mesh->selectEdgeWithFaceId(begin, end, f_id, se_id);
			break;
		case acamcad::DataType::TSPLINEU_TYPE:
			tSpline->selectEdgeWithFaceId(begin, end, f_id, se_id);
			break;
		default:
			break;
		}
	}

	void AdapterObject::mesh_OPvert_BackupVertes(const std::vector<int>& vert_id)
	{
		if (bRep)
			bRep->mesh_OPvert_Backup();

		if (mesh)
			mesh->mesh_OPvert_BackupVertes(vert_id);

		if (tSpline)
			tSpline->mesh_OPvert_BackupVertes(vert_id);
	}

	void AdapterObject::mesh_OPvert_BackupEdges(const std::vector<int>& edge_id)
	{
		if (bRep)
			bRep->mesh_OPvert_Backup();

		if (mesh)
			mesh->mesh_OPvert_BackupEdges(edge_id);

		if (tSpline)
			tSpline->mesh_OPvert_BackupEdges(edge_id);
	}

	void AdapterObject::mesh_OPvert_BackupFaces(const std::vector<int>& face_id)
	{
		if (bRep)
			bRep->mesh_OPvert_Backup();


		if (mesh)
			mesh->mesh_OPvert_BackupFaces(face_id);

		if (tSpline)
			tSpline->mesh_OPvert_BackupFaces(face_id);
	}

	void AdapterObject::mesh_Clear_OPvertBackup()
	{

		if (bRep)
			bRep->mesh_Clear_OPvertBackup();


		if (mesh)
			mesh->mesh_Clear_OPvertBackup();

		if (tSpline)
			tSpline->mesh_Clear_OPvertBackup();
	}


	void AdapterObject::mesh_OPvert_Backup()
	{
		if (bRep)
			bRep->mesh_OPvert_Backup();


		if (mesh)
			mesh->mesh_OPvert_Backup();

		if (tSpline)
			tSpline->mesh_OPvert_Backup();
	}

	void AdapterObject::meshMove(const AMCAX::Coord3& v, double dis)
	{

		switch (dataType())
		{
		case acamcad::DataType::BREP_TYPE:
			bRep->meshMove(v, dis);
			break;
		case acamcad::DataType::MESH_TYPE:
		{
			AMCAX::Vector3 ve(v.X(), v.Y(), v.Z());
			mesh->meshMove(ve, dis);
		}
		break;
		case acamcad::DataType::TSPLINEU_TYPE:
		{
			AMCAX::Vector3 ve(v.X(), v.Y(), v.Z());
			tSpline->meshMove(ve, dis);
		}
		break;
		default:
			break;
		}
	}

	void AdapterObject::meshRotation(const AMCAX::Coord3& center, const AMCAX::Coord3& axis, double angle)
	{
		if (bRep)
			bRep->meshRotation(center, axis, angle);

		if (mesh) {
			AMCAX::Vector3 vCenter(center.X(), center.Y(), center.Z()), vAxis(axis.X(), axis.Y(), axis.Z());
			mesh->meshRotation(vCenter, vAxis, angle);
		}

		if (tSpline)
		{
			AMCAX::Vector3 vCenter(center.X(), center.Y(), center.Z()), vAxis(axis.X(), axis.Y(), axis.Z());
			tSpline->meshRotation(vCenter, vAxis, angle);
		}
	}

	void AdapterObject::meshScaleSingle(const AMCAX::Coord3& center, const AMCAX::Coord3& axis, double scale)
	{
		if (mesh)
		{
			AMCAX::Vector3 vCenter(center.X(), center.Y(), center.Z()), vAxis(axis.X(), axis.Y(), axis.Z());
			mesh->meshScaleSingle(vCenter, vAxis, scale);
		}

		if (tSpline)
		{
			AMCAX::Vector3 vCenter(center.X(), center.Y(), center.Z()), vAxis(axis.X(), axis.Y(), axis.Z());
			tSpline->meshScaleSingle(vCenter, vAxis, scale);
		}

	}
	void AdapterObject::meshScalePlane(const AMCAX::Coord3& center, const AMCAX::Coord3& normal, double scale)
	{
		if (mesh) {
			AMCAX::Vector3 vCenter(center.X(), center.Y(), center.Z()), vAxis(normal.X(), normal.Y(), normal.Z());
			mesh->meshScalePlane(vCenter, vAxis, scale);
		}

		if (tSpline) {
			AMCAX::Vector3 vCenter(center.X(), center.Y(), center.Z()), vAxis(normal.X(), normal.Y(), normal.Z());
			tSpline->meshScalePlane(vCenter, vAxis, scale);
		}
	}


	void AdapterObject::setDataType(DataType _type)
	{
		BaseObject::setDataType(_type);

		switch (_type)
		{
		case acamcad::DataType::BREP_TYPE:
			bRep = std::make_unique<BRepObject>(this);
			bRep->updateDraw();
			break;
		case acamcad::DataType::MESH_TYPE:
			mesh = std::make_unique<MeshObject>(this);
			//mesh->assignMesh();
			mesh->updateDraw();
			break;
		case acamcad::DataType::TSPLINEU_TYPE:
			tSpline = std::make_unique<TSplineObject>(this);
			tSpline->updateDraw();
			break;
		default:
			break;
		}
	}

	void AdapterObject::GetBoundingBox(AMCAX::Coord3& min, AMCAX::Coord3& max)
	{
		switch (dataType())
		{
		case acamcad::DataType::BREP_TYPE:
		{
			AMCAX::BoundingBox3 box;
			AMCAX::BRepBoundingBox::AddToBox(bRep->getShape(), box);

			min = box.CornerMin().Coord();
			max = box.CornerMax().Coord();
		}
		break;
		case acamcad::DataType::MESH_TYPE:
		{
			auto box = AMCAX::SubD::MeshTool::MeshBoundingBox(mesh->mesh());

			min = box.CornerMin().Coord();
			max = box.CornerMax().Coord();
		}
		break;
		case acamcad::DataType::TSPLINEU_TYPE:
		{
			AMCAX::BoundingBox3 box;
			AMCAX::TMS::TMSplineTool::UpdateTMSplineBndBox(tSpline->getTSpline(), box);

			min = box.CornerMin().Coord();
			max = box.CornerMax().Coord();
			break;
		}
		default:
			break;
		}
	}
	void AdapterObject::GetBoundingBoxByFace(int face_id, AMCAX::Coord3& min, AMCAX::Coord3& max)
	{
		switch (dataType())
		{
		case acamcad::DataType::BREP_TYPE:
		{
			AMCAX::BoundingBox3 box;
			AMCAX::BRepBoundingBox::AddToBox(bRep->getShapeFaces()[face_id], box);

			min = box.CornerMin().Coord();
			max = box.CornerMax().Coord();
		}
		break;
		case acamcad::DataType::MESH_TYPE:
		{
			auto vs = AMCAX::SubD::MeshTool::FaceVertexIndexs(mesh->mesh(), face_id);
			double x_min(0), y_min(0), z_min(0), x_max(0), y_max(0), z_max(0);

			for (int i = 0; i < vs.size(); i++)
			{
				auto& p = AMCAX::SubD::MeshTool::Position(mesh->mesh(), vs[i]);

				if (i == 0)
				{
					x_min = x_max = p.X();
					y_min = y_max = p.Y();
					z_min = z_max = p.Z();
				}
				else {
					x_min = std::min(p.X(), x_min);
					y_min = std::min(p.Y(), y_min);
					z_min = std::min(p.Z(), z_min);

					x_max = std::max(p.X(), x_max);
					y_max = std::max(p.Y(), y_max);
					z_max = std::max(p.Z(), z_max);
				}

			}

			min.SetCoord(x_min, y_min, z_min);
			max.SetCoord(x_max, y_max, z_max);

			//center_.SetX((x_min + x_max) / 2);
			//center_.SetY((y_min + y_max) / 2);
			//center_.SetZ((z_min + z_max) / 2);
		}
		break;
		case acamcad::DataType::TSPLINEU_TYPE:
		{
			auto ids = AMCAX::TMS::TMSplineTool::FaceVertexIndexs(tSpline->getTSpline(), face_id);
			double x_min(0), y_min(0), z_min(0), x_max(0), y_max(0), z_max(0);

			for (int i = 0; i < ids.size(); i++)
			{
				auto& p = AMCAX::TMS::TMSplineTool::ControlPosition(tSpline->getTSpline(), ids[i]);
				if (i == 0)
				{
					x_min = x_max = p.X();
					y_min = y_max = p.Y();
					z_min = z_max = p.Z();
				}
				else {
					x_min = std::min(p.X(), x_min);
					y_min = std::min(p.Y(), y_min);
					z_min = std::min(p.Z(), z_min);

					x_max = std::max(p.X(), x_max);
					y_max = std::max(p.Y(), y_max);
					z_max = std::max(p.Z(), z_max);
				}


			}
			min.SetCoord(x_min, y_min, z_min);
			max.SetCoord(x_max, y_max, z_max);

		}
		break;
		default:
			break;
		}
	}
	void AdapterObject::GetBoundingBoxByEdge(int edge_id, AMCAX::Coord3& min, AMCAX::Coord3& max)
	{
		switch (dataType())
		{
		case acamcad::DataType::BREP_TYPE:
		{
			AMCAX::BoundingBox3 box;
			AMCAX::BRepBoundingBox::AddToBox(bRep->getShapeEdges()[edge_id], box);

			min = box.CornerMin().Coord();
			max = box.CornerMax().Coord();
		}
		break;
		case acamcad::DataType::MESH_TYPE:
		{
			int v0, v1;
			AMCAX::SubD::MeshTool::EdgeVertexIndexs(mesh->mesh(), edge_id, v0, v1);
			auto& p0 = AMCAX::SubD::MeshTool::Position(mesh->mesh(), v0),
				& p1 = AMCAX::SubD::MeshTool::Position(mesh->mesh(), v1);

			min.SetCoord(std::min(p0.X(), p1.X()), std::min(p0.Y(), p1.Y()), std::min(p0.Z(), p1.Z()));
			max.SetCoord(std::max(p0.X(), p1.X()), std::max(p0.Y(), p1.Y()), std::max(p0.Z(), p1.Z()));

			//center_.SetX((p0.X() + p1.X()) / 2);
			//center_.SetY((p0.Y() + p1.Y()) / 2);
			//center_.SetZ((p0.X() + p1.Y()) / 2);
		}
		break;
		case acamcad::DataType::TSPLINEU_TYPE:
		{
			int v0, v1;
			AMCAX::TMS::TMSplineTool::EdgeVertexIndexs(tSpline->getTSpline(), edge_id, v0, v1);
			auto p0 = AMCAX::TMS::TMSplineTool::ControlPosition(tSpline->getTSpline(), v0),
				p1 = AMCAX::TMS::TMSplineTool::ControlPosition(tSpline->getTSpline(), v1);


			min.SetCoord(std::min(p0.X(), p1.X()), std::min(p0.Y(), p1.Y()), std::min(p0.Z(), p1.Z()));
			max.SetCoord(std::max(p0.X(), p1.X()), std::max(p0.Y(), p1.Y()), std::max(p0.Z(), p1.Z()));

		}
		break;
		default:
			break;
		}
	}
	void AdapterObject::GetBoundingBoxByVetex(int vetex_id, AMCAX::Coord3& position)
	{
		switch (dataType())
		{
		case acamcad::DataType::BREP_TYPE:
		{
			bRep->getShapeVertices();
			AMCAX::BoundingBox3 box;
			AMCAX::BRepBoundingBox::AddToBox(bRep->getShapeVertices()[vetex_id], box);

			//min = box.CornerMin().Coord();
			position = (box.CornerMax().Coord() + box.CornerMin().Coord()) / 2;
		}
		break;
		case acamcad::DataType::MESH_TYPE:
		{
			auto& p = AMCAX::SubD::MeshTool::Position(mesh->mesh(), vetex_id);

			position = p.Coord();
		}
		break;
		case acamcad::DataType::TSPLINEU_TYPE:
		{
			auto& p = AMCAX::TMS::TMSplineTool::ControlPosition(tSpline->getTSpline(), vetex_id);
			position = p.Coord();
		}
		break;
		default:
			break;
		}
	}

	void AdapterObject::mirror(const AMCAX::Coord3& center, const AMCAX::Coord3& axis)
	{
		switch (dataType())
		{
		case acamcad::DataType::MESH_TYPE:
		{
			AMCAX::Vector3 c_v(center), c_a(axis);
			mesh->mirrorMeshObject(c_v, c_a);
		}
		break;
		case acamcad::DataType::BREP_TYPE:
		{
			bRep->mirrorMeshObject(center, axis);
		}
		break;
		case acamcad::DataType::TSPLINEU_TYPE:
		{
			tSpline->mirror(center, axis);
			//std::cout << "" << std::endl;
		}
		break;
		default:
			break;
		}

	}


	const ObjectDraw* AdapterObject::getDrawTool() const
	{
		if (bRep)
			return bRep->getDrawTool();

		if (mesh)
			return mesh->getDrawTool();

		if (tSpline)
			return tSpline->getDrawTool();
	}


	void AdapterObject::updateDraw()
	{
		if (bRep) {
			bRep->updateDraw();
		}

		if (mesh)
			mesh->updateDraw();

		if (tSpline)
			tSpline->updateDraw();
	}


}