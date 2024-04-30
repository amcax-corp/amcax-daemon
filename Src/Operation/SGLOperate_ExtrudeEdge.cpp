#include "SGLOperate_ExtrudeEdge.h"

#include <polymesh/PolyMesh.hpp>
#include <polymeshAPI/MeshExtrude.hpp>
#include <polymeshAPI/MeshTool.hpp>
#include <tmeshSplineAPI/TMSplineExtrude.hpp>
#include <tmeshSplineAPI/TMSplineTool.hpp>

acamcad::SGLOperate_ExtrudeEdgeH::SGLOperate_ExtrudeEdgeH(const SelectModel& smodle)
	:MOperate_SingleObject(smodle)
{
}

bool acamcad::SGLOperate_ExtrudeEdgeH::DoOperate(AdapterObject* adapter)
{
	if (selmodel_ != SelectModel::EDGE_MODEL)
		return false;

	int object_id = adapter->persistentId();
	std::vector<int> s_id_list = getSelectSubList(s_info_list_);
	std::vector<int> s_id_list_new;

	dire_.SetCoord(0.0, 0.0, 0.0);


	bool result = false;

	switch (adapter->dataType())
	{
	case DataType::MESH_TYPE:
	{

		for (int i = 0; i < s_id_list.size(); i++)
		{
			std::vector<int> faces = AMCAX::SubD::MeshTool::EdgeFaceIndexs(adapter->mesh->getShape(),
				s_id_list[i]);

			AMCAX::Vector3 nor = AMCAX::SubD::MeshTool::FaceNormal(adapter->mesh->getShape(), faces[0]);

			int p0, p1;
			AMCAX::SubD::MeshTool::EdgeVertexIndexs(adapter->mesh->getShape(), s_id_list[i], p0, p1);
			AMCAX::Point3 position0 = AMCAX::SubD::MeshTool::Position(adapter->mesh->getShape(), p0),
				position1 = AMCAX::SubD::MeshTool::Position(adapter->mesh->getShape(), p1);

			AMCAX::Vector3 ve = AMCAX::Vector3(position1.Coord() - position0.Coord());

			dire_ += ve.Cross(nor).Coord();
		}

		dire_.Normalize();


		result = AMCAX::SubD::MeshExtrude::ExtrudeEdge(adapter->mesh->getShape(),
			s_id_list, s_id_list_new);

		if (result)
		{
			s_info_list_.resize(s_id_list_new.size());
			for (int i = 0; i < s_id_list_new.size(); i++)
			{
				s_info_list_[i].object_id_ = object_id;
				s_info_list_[i].object_subselect_id_ = s_id_list_new[i];
			}
		}
	}
	break;
	case DataType::TSPLINEU_TYPE:
	{
		for (int i = 0; i < s_id_list.size(); i++)
		{
			std::vector<int> faces = AMCAX::TMS::TMSplineTool::EdgeFaceIndexs(adapter->tSpline->getShape(),
				s_id_list[i]);

			//AMCAX::TMS::TMSplineTool::ControlFaceNormal(adapter->tSpline->getTSpline(), faces[0]);

			AMCAX::Vector3 nor = AMCAX::TMS::TMSplineTool::ControlFaceNormal(adapter->tSpline->getShape(), faces[0]);

			//EdgeFaceIndexs

			int p0, p1;
			AMCAX::TMS::TMSplineTool::EdgeVertexIndexs(adapter->tSpline->getShape(), s_id_list[i], p0, p1);
			AMCAX::Point3 position0 = AMCAX::TMS::TMSplineTool::ControlPosition(adapter->tSpline->getShape(), p0),
				position1 = AMCAX::TMS::TMSplineTool::ControlPosition(adapter->tSpline->getShape(), p1);

			AMCAX::Vector3 ve = AMCAX::Vector3(position1.Coord() - position0.Coord());

			dire_ += ve.Cross(nor).Coord();
		}

		dire_.Normalize();

		AMCAX::TMS::TMSplineExtrude tool;

		result = tool.CanExtrudeEdges(adapter->tSpline->getShape(), s_id_list);

		if (result)
		{
			tool.ExtrudeEdges(adapter->tSpline->getShape(), s_id_list, s_id_list_new);

			s_info_list_.resize(s_id_list_new.size());
			for (int i = 0; i < s_id_list_new.size(); i++)
			{
				s_info_list_[i].object_id_ = object_id;
				s_info_list_[i].object_subselect_id_ = s_id_list_new[i];
			}
		}
	}
	break;
	default:
		break;
	}

	if (result)
	{
		adapter->updateDraw();
	}

	return result;
}

//======================================================================================================


acamcad::SGLOperate_ExtrudeEdgeV::SGLOperate_ExtrudeEdgeV(const SelectModel& smodle)
	:MOperate_SingleObject(smodle)
{
}

bool acamcad::SGLOperate_ExtrudeEdgeV::DoOperate(AdapterObject* adapter)
{
	if (selmodel_ != SelectModel::EDGE_MODEL)
		return false;


	int object_id = adapter->persistentId();
	std::vector<int> s_id_list = getSelectSubList(s_info_list_);
	std::vector<int> s_id_list_new;

	dire_.SetCoord(0.0, 0.0, 0.0);
	bool result = false;

	switch (adapter->dataType())
	{
	case DataType::MESH_TYPE:
	{
		for (int i = 0; i < s_id_list.size(); i++)
		{
			std::vector<int> faces = AMCAX::SubD::MeshTool::EdgeFaceIndexs(adapter->mesh->getShape(),
				s_id_list[i]);

			AMCAX::Vector3 nor = AMCAX::SubD::MeshTool::FaceNormal(adapter->mesh->getShape(), faces[0]);

			dire_ += nor.Coord();
		}

		dire_.Normalize();

		result = AMCAX::SubD::MeshExtrude::ExtrudeEdge(adapter->mesh->getShape(),
			s_id_list, s_id_list_new);


		if (result)
		{
			s_info_list_.resize(s_id_list_new.size());
			for (int i = 0; i < s_id_list_new.size(); i++)
			{
				s_info_list_[i].object_id_ = object_id;
				s_info_list_[i].object_subselect_id_ = s_id_list_new[i];
			}
		}
	}
	break;
	case DataType::TSPLINEU_TYPE:
	{

		for (int i = 0; i < s_id_list.size(); i++)
		{
			std::vector<int> faces = AMCAX::TMS::TMSplineTool::EdgeFaceIndexs(adapter->tSpline->getShape(),
				s_id_list[i]);

			//AMCAX::TMS::TMSplineTool::ControlFaceNormal(adapter->tSpline->getTSpline(), faces[0]);

			AMCAX::Vector3 nor = AMCAX::TMS::TMSplineTool::ControlFaceNormal(adapter->tSpline->getShape(), faces[0]);

			dire_ += nor.Coord();
		}

		dire_.Normalize();
		AMCAX::TMS::TMSplineExtrude tool;

		result = tool.CanExtrudeEdges(adapter->tSpline->getShape(), s_id_list);

		if (result)
		{
			tool.ExtrudeEdges(adapter->tSpline->getShape(), s_id_list, s_id_list_new);

			s_info_list_.resize(s_id_list_new.size());
			for (int i = 0; i < s_id_list_new.size(); i++)
			{
				s_info_list_[i].object_id_ = object_id;
				s_info_list_[i].object_subselect_id_ = s_id_list_new[i];
			}
		}
	}
	break;
	default:
		break;
	}

	if (result)
	{
		adapter->updateDraw();
	}

	return result;
}