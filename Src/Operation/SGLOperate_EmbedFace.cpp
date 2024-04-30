#include "SGLOperate_EmbedFace.h"

#include <polymesh/PolyMesh.hpp>
#include <polymeshAPI/MeshInsertFace.hpp>
#include <tmeshSplineAPI/TMSplineAddFace.hpp>
#include <tmeshSplineAPI/TMSplineTool.hpp>

namespace acamcad
{
	SGLOperate_EmbedFace::SGLOperate_EmbedFace(const SelectModel& smodle)
		:MOperate_SingleObject(smodle)
	{
	}

	bool SGLOperate_EmbedFace::DoOperate(AdapterObject* adapter)
	{

		bool result = false;

		switch (adapter->dataType())
		{
		case DataType::MESH_TYPE:
		{
			if (selmodel_ == SelectModel::VERTEX_MODEL)
			{
				if (s_info_list_.size() < 3) break;

				//result = true;
				std::vector<int> s_id_list = getSelectSubList(s_info_list_);

				result = AMCAX::SubD::MeshInsertFace::EmbedSingleFace(adapter->mesh->getShape(), s_id_list) != nullptr;
				adapter->updateDraw();
			}
			else if (selmodel_ == SelectModel::EDGE_MODEL)
			{
				if (s_info_list_.size() < 2) break;

				std::vector<int> s_id_list = getSelectSubList(s_info_list_);
				//polymesh::MEdge* e0 = mesh_object->mesh()->edge(s_id_list[0]);
				//polymesh::MEdge* e1 = mesh_object->mesh()->edge(s_id_list[1]);

				//polymesh::addFaceByEdge(mesh_object->mesh(), e0, e1);
				result = AMCAX::SubD::MeshInsertFace::AddFaceByEdge(adapter->mesh->getShape(), s_id_list[0], s_id_list[1]) != nullptr;
				adapter->updateDraw();


			}
		}
		break;
		case DataType::TSPLINEU_TYPE:
		{
			if (selmodel_ == SelectModel::VERTEX_MODEL)
			{
				if (s_info_list_.size() < 3) break;
				if (s_info_list_[0].object_id_ != s_info_list_[1].object_id_ ||
					s_info_list_[1].object_id_ != s_info_list_[2].object_id_)
					break;

				std::vector<int> s_id_list = getSelectSubList(s_info_list_);
				std::vector<AMCAX::Point3> points;
				for (int i = 0; i < s_id_list.size(); i++)
				{
					AMCAX::Point3 point =
						AMCAX::TMS::TMSplineTool::ControlPosition(adapter->tSpline->getShape(), s_id_list[i]);

					points.push_back(point);
				}


				AMCAX::TMS::TMSplineAddFace tool;

				result = tool.CanAddSingleFace(adapter->tSpline->getShape(), points);

				if (result) {
					tool.AddSingleFace(adapter->tSpline->getShape(), points);

					adapter->updateDraw();
				}


				//tspline::TO_AddFace addface;
				//addface.addSingleFace(spline_object->getTSpline(), vp_list);


			}
			else if (selmodel_ == SelectModel::EDGE_MODEL)
			{
				if (s_info_list_.size() < 2) break;
				if (s_info_list_[0].object_id_ != s_info_list_[1].object_id_)
					break;

				std::vector<int> s_id_list = getSelectSubList(s_info_list_);

				AMCAX::TMS::TMSplineAddFace tool;

				result = tool.CanAddFaceByEdge(adapter->tSpline->getShape(), s_id_list[0], s_id_list[1]);

				if (result) {
					tool.AddFaceByEdge(adapter->tSpline->getShape(), s_id_list[0], s_id_list[1]);
					adapter->updateDraw();
				}
			}
			//spline_object->updateDraw();
		}
		break;
		default:
			break;
		}

		return result;
	}
}