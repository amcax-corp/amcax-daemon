#include "CreateOperate_SingleFaceMesh.h"
#include <polymeshAPI/MeshInsertFace.hpp>

namespace acamcad
{
	CreateOperate_SingleFaceMesh::CreateOperate_SingleFaceMesh(const std::vector<AMCAX::Coord3>& point_list) :
		CreateOperate_SingleFace(point_list)
	{
	}

	bool CreateOperate_SingleFaceMesh::DoOperate(AdapterObject* object)
	{
		object->mesh->assignShape();

		std::vector<MPoint3> pList;
		for (auto& iter : point_list_)
		{
			pList.push_back(MPoint3(iter));
		}

		AMCAX::SubD::MeshInsertFace::AddSingleFace(object->mesh->getShape(), pList);

		object->updateDraw();

		return true;
	}

}