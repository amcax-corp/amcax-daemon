#include "SGLOperate_Thicken.h"

#include <polymeshAPI/MeshTool.hpp>
#include <polymeshAPI/MeshOffset.hpp>

#include <tmeshSplineAPI/TMSplineThicken.hpp>

namespace acamcad
{
	SGLOperate_Thicken::SGLOperate_Thicken(const SelectModel& smodle) :
		MOperate_SingleObject(smodle)
	{
	}

	bool SGLOperate_Thicken::DoOperate(AdapterObject* adapter)
	{
		bool result = false;

		switch (adapter->dataType())
		{
		case DataType::MESH_TYPE:
			//AMCAX::SubD::MeshTool::MeshTriangles(adapter->mesh->mesh());
			//result = true;
			result = AMCAX::SubD::MeshOffset::ThickenMesh(adapter->mesh->getShape(), 0.2);
			break;
		case DataType::TSPLINEU_TYPE:
		{
			result = AMCAX::TMS::TMSplineThicken().ThickenTMSplins(adapter->tSpline->getShape(), 0.2);

		}
		break;
		default:
			break;
		}


		if (result) {
			adapter->updateDraw();
		}

		return result;


	}
}