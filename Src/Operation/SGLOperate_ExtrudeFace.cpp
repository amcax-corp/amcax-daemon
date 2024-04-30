#include "SGLOperate_ExtrudeFace.h"

#include <polymeshAPI/MeshTool.hpp>
#include <polymeshAPI/MeshExtrude.hpp>
#include <tmeshSplineAPI/TMSplineTool.hpp>
#include <tmeshSplineAPI/TMSplineExtrude.hpp>


using namespace acamcad;

SGLOperate_ExtrudeFace::SGLOperate_ExtrudeFace(const SelectModel& smodle)
	:MOperate_SingleObject(smodle)
{
}

bool SGLOperate_ExtrudeFace::DoOperate(AdapterObject* object)
{
	if (selmodel_ != SelectModel::FACE_MODEL)
		return false;

	int object_id = object->persistentId();
	std::vector<int> s_id_list = getSelectSubList(s_info_list_);
	std::vector<int> s_id_list_new;

	dire_.SetCoord(0.0, 0.0, 0.0);


	bool result = false;

	switch (object->dataType())
	{
	case DataType::MESH_TYPE:
	{
		for (int i = 0; i < s_id_list.size(); i++)
		{
			//std::vector<int> faces = AMCAX::SubD::MeshTool::EdgeFaceIndexs(object->mesh->mesh(),
			//	s_id_list[i]);

			AMCAX::Vector3 nor = AMCAX::SubD::MeshTool::FaceNormal(object->mesh->getShape(), s_id_list[i]);

			dire_ += nor.Coord();
		}

		dire_.Normalize();

		result = AMCAX::SubD::MeshExtrude::ExtrudeFace(object->mesh->getShape(),
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
			//std::vector<int> faces = AMCAX::TMS::TMSplineTool::EdgeFaceIndexs(object->tSpline->getTSpline(),
			//	s_id_list[i]);

			//AMCAX::TMS::TMSplineTool::ControlFaceNormal(adapter->tSpline->getTSpline(), faces[0]);

			AMCAX::Vector3 nor = AMCAX::TMS::TMSplineTool::ControlFaceNormal(object->tSpline->getShape(), s_id_list[i]);

			dire_ += nor.Coord();
		}

		dire_.Normalize();
		AMCAX::TMS::TMSplineExtrude tool;

		result = tool.CanExtrudeFaces(object->tSpline->getShape(), s_id_list);

		if (result)
		{
			tool.ExtrudeFaces(object->tSpline->getShape(), s_id_list, s_id_list_new);

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
		object->updateDraw();
	}

	return result;
}

//void SGLOperate_ExtrudeFace::operateWithMesh(MeshObject* mesh_object)
//{
//	if (selmodel_ != SelectModel::FACE_MODEL)
//		return;
//
//	int object_id = mesh_object->persistentId();
//	std::vector<int> s_id_list = getSelectSubList(s_info_list_);
//	std::vector<int> s_id_list_new;
//
//	//Mesh_SelectOperate::extrudeFace(mesh_object->mesh(), s_id_list, s_id_list_new, dire_);
//
//	s_info_list_.resize(s_id_list_new.size());
//	for (int i = 0; i < s_id_list_new.size(); i++)
//	{
//		s_info_list_[i].object_id_ = object_id;
//		s_info_list_[i].object_subselect_id_ = s_id_list_new[i];
//	}
//}
//
//void SGLOperate_ExtrudeFace::operateWithTSpline(TSplineUObject* tspline_object)
//{
//	if (selmodel_ != SelectModel::FACE_MODEL)
//		return;
//
//	int object_id = tspline_object->persistentId();
//	std::vector<int> s_id_list = getSelectSubList(s_info_list_);
//	std::vector<int> s_id_list_new;
//
//	//TSpline_SelectOperate::extrudeFace(tspline_object->getTSpline(), s_id_list, s_id_list_new, dire_);
//
//	s_info_list_.resize(s_id_list_new.size());
//	for (int i = 0; i < s_id_list_new.size(); i++)
//	{
//		s_info_list_[i].object_id_ = object_id;
//		s_info_list_[i].object_subselect_id_ = s_id_list_new[i];
//	}
//
//	tspline_object->updateDraw();
//}


