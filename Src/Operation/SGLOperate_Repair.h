#pragma once
#include "MOperate_SelectInfo.h"

namespace acamcad
{

	class SGLOperate_Repair : public MOperate_SingleObject
	{
	public:
		SGLOperate_Repair(const SelectModel& smodle);
		virtual ~SGLOperate_Repair() {};

	public:
		virtual bool DoOperate(AdapterObject* adatper) override;

	public:
		virtual MeshOperationType OperationType() { return MeshOperationType::TSplineRepair; }

	};

} //namespace acamcad