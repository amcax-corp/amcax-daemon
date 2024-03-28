#pragma once
#include "MOperate_SelectInfo.h"

namespace acamcad
{

	class SGLOperate_WeldEdge : public MOperate_SingleObject
	{
	public:
		SGLOperate_WeldEdge(const SelectModel& smodle);
		virtual ~SGLOperate_WeldEdge() {};

	public:
		virtual bool DoOperate(AdapterObject* mesh_object) override;
		//virtual void operateWithTSpline(TSplineObject* tspline_object) override;

	public:
		virtual MeshOperationType OperationType() { return MeshOperationType::MeshWeldEdge; }
	};

} //namespace acamcad