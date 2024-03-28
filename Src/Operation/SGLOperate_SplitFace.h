#pragma once
#include "MOperate_SelectInfo.h"

namespace acamcad
{

	class SGLOperate_SplitFace : public MOperate_SingleObject
	{
	public:
		SGLOperate_SplitFace(const SelectModel& smodle);
		//virtual ~SGLOperate_SplitFace() {};

	public:
		virtual bool DoOperate(AdapterObject* tspline_object) override;

	public:
		virtual MeshOperationType OperationType() { return MeshOperationType::MeshSplitFace; }
	};

} //namespace acamcad