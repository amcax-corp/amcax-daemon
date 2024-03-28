#pragma once
#include "MOperate_SelectInfo.h"

namespace acamcad
{

	class SGLOperate_CreaseEdge : public MOperate_SingleObject
	{
	public:
		SGLOperate_CreaseEdge(const SelectModel& smodle);
		//virtual ~SGLOperate_CreaseEdge() {};

	public:
		virtual bool DoOperate(AdapterObject* tspline_object) override;

	public:
		virtual MeshOperationType OperationType() { return MeshOperationType::MeshAddCreaseEdge; }
	};

} //namespace acamcad