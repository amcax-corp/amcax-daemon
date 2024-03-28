#include "SGLOperate_TriangulateMesh.h"

#include <polymeshAPI/MeshTool.hpp>

namespace acamcad
{
	SGLOperate_TriangulateMesh::SGLOperate_TriangulateMesh(const SelectModel& smodle) :
		MOperate_SingleObject(smodle)
	{
	}

	bool SGLOperate_TriangulateMesh::DoOperate(AdapterObject* adapter)
	{
		if (selmodel_ != SelectModel::OBJECT_MODEL)
			return false;

		bool result = false;

		switch (adapter->dataType())
		{
		case DataType::MESH_TYPE:
			AMCAX::SubD::MeshTool::MeshTriangles(adapter->mesh->mesh());
			result = true;
			break;
		default:
			break;
		}

		return result;


	}
}