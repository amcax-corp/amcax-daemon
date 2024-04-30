#include "CreateOperate_SingleFaceTSpline.h"

#include <tmeshSplineAPI/TMSplineAddFace.hpp>
#include "../Object/TSplineObject.h"

namespace acamcad
{
	CreateOperate_SingleFaceTSpline::CreateOperate_SingleFaceTSpline(const std::vector<AMCAX::Coord3>& point_list) :
		CreateOperate_SingleFace(point_list)
	{
	}

	bool CreateOperate_SingleFaceTSpline::DoOperate(AdapterObject* object)
	{
		object->tSpline->assignShape();

		std::vector<MPoint3> pList;
		for (auto& iter : point_list_)
		{
			pList.push_back(MPoint3(iter));
		}

		bool result = AMCAX::TMS::TMSplineAddFace().AddSingleFace(object->tSpline->getShape(), pList);

		//AMCAX::SubD::MeshInsertFace::AddSingleFace(object->mesh->mesh(), pList);

		object->updateDraw();



		return result;
	}

}