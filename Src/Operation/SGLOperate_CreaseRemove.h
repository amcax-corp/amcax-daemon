#pragma once
#include "MOperate_SelectInfo.h"

namespace acamcad
{

	class SGLOperate_CreaseEdgeRemove : public MOperate_SingleObject
	{
	public:
		SGLOperate_CreaseEdgeRemove(const SelectModel& smodle);
		virtual ~SGLOperate_CreaseEdgeRemove() {};

	public:
		virtual bool DoOperate(AdapterObject* tspline_object) override;

	public:
		virtual MeshOperationType OperationType() { return MeshOperationType::MeshRemoveCreaseEdge; }
	};

} //namespace acamcad