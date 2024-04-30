#include "SGLOperate_ReverseNormal.h"

#include <polymeshAPI/MeshTool.hpp>
#include <tmeshSplineAPI/TMSplineTool.hpp>

acamcad::SGLOperate_ReverseNormal::SGLOperate_ReverseNormal(const SelectModel& smodle)
	:MOperate_SingleObject(smodle)
{
}

bool acamcad::SGLOperate_ReverseNormal::DoOperate(AdapterObject* adapter)
{
	if (selmodel_ != SelectModel::OBJECT_MODEL)
		return false;

	bool result = false;

	switch (adapter->dataType())
	{
	case DataType::MESH_TYPE:
	{
		result = true;
		AMCAX::SubD::MeshTool::ReverseMeshNormal(adapter->mesh->getShape());
	}
	break;
	case DataType::TSPLINEU_TYPE:
	{
		result = true;
		AMCAX::TMS::TMSplineTool::ReverseTMSNormal(adapter->tSpline->getShape());
	}
	break;
	default:
		break;
	}

	//mesh_object->mesh()->reverse_mesh();


	return true;
}