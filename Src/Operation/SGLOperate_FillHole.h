#pragma once
#include "MOperate_SelectInfo.h"

namespace acamcad
{

	class SGLOperate_FillHole : public MOperate_SingleObject
	{
	public:
		SGLOperate_FillHole(const SelectModel& smodle);
		virtual ~SGLOperate_FillHole() {};

	public:
		virtual bool DoOperate(AdapterObject* adapter) override;

	public:
		virtual MeshOperationType OperationType() { return MeshOperationType::MeshFillHole; }

	};

}