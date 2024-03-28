#pragma once
#include "MOperate_SelectInfo.h"

namespace acamcad
{

	class SGLOperate_Bridge : public MOperate_SingleObject
	{
	public:
		SGLOperate_Bridge(const SelectModel& smodle);
		virtual ~SGLOperate_Bridge() {};

	public:
		virtual bool DoOperate(AdapterObject* adapter) override;
		//virtual void operateWithTSpline(TSplineObject* spline_object) override;

	public:
		virtual MeshOperationType OperationType() { return MeshOperationType::MeshBridgeEdge; }
	};

} //namespace acamcad