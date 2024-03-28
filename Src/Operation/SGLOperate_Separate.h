#pragma once
#include "MOperate_SelectInfo.h"

namespace acamcad
{

	class SGLOperate_Separate : public MOperate_SingleObject
	{
	public:
		SGLOperate_Separate(const SelectModel& smodle);
		virtual ~SGLOperate_Separate() {};

	public:
		virtual bool DoOperate(AdapterObject* mesh_object) override;
		///virtual void operateWithTSpline(TSplineObject* tspline_object) override;

	public:
		virtual MeshOperationType OperationType() { return MeshOperationType::MeshSeparate; }
	};

} //namespace acamcad