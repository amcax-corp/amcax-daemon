#include "CreateOperate_CubeTSpline.h"


#include <tmeshSplineAPI/TMSplineMakeCube.hpp>
#include <common/PointT.hpp>


namespace acamcad
{

	CreateOperate_CubeTSpline::CreateOperate_CubeTSpline(const AMCAX::Coord3& bb_min, const AMCAX::Coord3& bb_max, size_t segX, size_t segY, size_t segZ)
		:CreateOperate_Cube(bb_min, bb_max, segX, segY, segZ)
	{
	}

	bool CreateOperate_CubeTSpline::DoOperate(AdapterObject* adapter)
	{


		AMCAX::Point3 bb_min = AMCAX::Point3(bb_min_.X(), bb_min_.Y(), bb_min_.Z());
		AMCAX::Point3 bb_max = AMCAX::Point3(bb_max_.X(), bb_max_.Y(), bb_max_.Z());

		AMCAX::TMS::TMSplineMakeCube cube(bb_min, bb_max, segX_, segY_, segZ_);

		adapter->tSpline->setShape(cube.BuildSpline());
		//adapter->mesh->SetMesh(cube.BuildMesh());
		adapter->updateDraw();

		return adapter->tSpline->getShape();
		//AMCAX::Point3 bb_min, bb_max_;
		//brep_object->SetShape(*AMCAX::SubD::MeshMakeCube(bb_min, bb_max, segX_, segY_, segZ_).BuildMesh());
	}

}