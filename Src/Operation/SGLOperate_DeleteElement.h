#pragma once
#include "MOperate_SelectInfo.h"

namespace acamcad
{

	class SGLOperate_Delete : public MOperate_SingleObject
	{
	public:
		SGLOperate_Delete(const SelectModel& smodle);
		virtual ~SGLOperate_Delete() {};

	public:
		virtual bool DoOperate(AdapterObject* mesh_object) override;
		//virtual void operateWithTSpline(TSplineObject* tspline_object) override;

	public:
		virtual MeshOperationType OperationType() { return MeshOperationType::MeshDeleteElement; }
	};

} //namespace acamcad