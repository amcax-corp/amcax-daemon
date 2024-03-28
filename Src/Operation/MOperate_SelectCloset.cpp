#include "MOperate_SelectCloset.h"

#include <polymesh/PolyMesh.hpp>
#include <polymeshAPI/MeshTool.hpp>

#include "../Utils/MathUtils.h"

acamcad::MOperation_SelectCloset::MOperation_SelectCloset(const SelectModel& smodle)
	:MOperation(smodle)
{
}

bool acamcad::MOperation_SelectCloset::DoOperate(AdapterObject* adapter)
{
	/*
	if (selmodel_ == SelectModel::VERTEX_MODEL)
	{
		Mesh_GetInfo::getVertCenter(mesh_object->mesh(), closet_, object_element_id_);
	}
	if (selmodel_ == SelectModel::EDGE_MODEL)
	{
		polymesh::MEdge* e = mesh_object->mesh()->edge(object_element_id_);

		MVector3 ev0 = e->getVert(0)->position();
		MVector3 ev1 = e->getVert(1)->position();
		MVector3 min_vL;

		MVector3 closet_temp;
		double dis = distLine_SLineSquared(ev0, ev1, begin_, end_, closet_temp, min_vL);
		closet_ = closet_temp.point();
	}
	if (selmodel_ == SelectModel::FACE_MODEL)
	{
		Mesh_GetInfo::getFaceCenter(mesh_object->mesh(), closet_, object_element_id_);
	}
	*/
	if (adapter->dataType() == DataType::BREP_TYPE)
		return false;


	switch (selmodel_)
	{
	case acamcad::SelectModel::VERTEX_MODEL:
	{
		//closet_ = AMCAX::SubD::MeshTool::Position(adapter->mesh->mesh(), object_element_id_);
		AMCAX::Coord3 coord0;
		adapter->GetBoundingBoxByVetex(object_element_id_, coord0);

		closet_.SetCoord(coord0);
	}
	break;
	case acamcad::SelectModel::EDGE_MODEL:
	{
		AMCAX::Coord3 point0, point1, coord0, coord1;

		adapter->GetBoundingBoxByEdge(object_element_id_, point0, point1);

		double len = MathUtils::distLine_SLineSquared(point0, point1,
			begin_, end_, coord0, coord1);
		closet_ = AMCAX::Point3(coord0);


	}
	break;
	case acamcad::SelectModel::FACE_MODEL:
	{
		AMCAX::Coord3  min, max;
		adapter->GetBoundingBoxByFace(object_element_id_, min, max);
		closet_ = AMCAX::Point3((min + max) / 2);
	}
	break;
	default:
		break;
	}




	return false;
}

/*
void acamcad::MOperation_SelectCloset::operateWithTSpline(TSplineObject* tspline_object)
{
	if (selmodel_ == SelectModel::VERTEX_MODEL)
	{
		closet_ = tspline_object->getTSpline()->getVertCenter(object_element_id_);
	}
	if (selmodel_ == SelectModel::EDGE_MODEL)
	{
		tspline::TEdge* e = tspline_object->getTSpline()->edge(object_element_id_);

		MVector3 ev0 = e->getVert(0)->controlPosition();
		MVector3 ev1 = e->getVert(1)->controlPosition();
		MVector3 min_vL;

		MVector3 closet_temp;
		double dis = distLine_SLineSquared(ev0, ev1, begin_, end_, closet_temp, min_vL);
		closet_ = closet_temp.point();
	}
	if (selmodel_ == SelectModel::FACE_MODEL)
	{
		closet_ = tspline_object->getTSpline()->getFaceCenter(object_element_id_);
	}
}
*/

void acamcad::MOperation_SelectCloset::setSelectInfoCamera(const SelectInfoWithCamera& s_info)
{
	object_id_ = s_info.object_id_;
	object_element_id_ = s_info.object_subselect_id_;
	begin_ = s_info.c_begin_;
	end_ = s_info.c_end_;
}
