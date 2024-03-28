#pragma once
#include "MOperate_SelectInfo.h"

namespace acamcad
{

	class SGLOperate_Uniform : public MOperate_SingleObject
	{
	public:
		SGLOperate_Uniform(const SelectModel& smodle);
		virtual ~SGLOperate_Uniform() {};

	public:
		virtual bool DoOperate(AdapterObject* object) override;

	public:
		virtual MeshOperationType OperationType() { return MeshOperationType::TSplineUniform; }

	};

} //namespace acamcad