#pragma once
#include "MOperate_SelectInfo.h"

namespace acamcad
{

	class SGLOperate_ReverseNormal : public MOperate_SingleObject
	{
	public:
		SGLOperate_ReverseNormal(const SelectModel& smodle);
		virtual ~SGLOperate_ReverseNormal() {};

	public:
		virtual bool DoOperate(AdapterObject* mesh_object) override;
		///virtual void operateWithTSpline(TSplineObject* tspline_object) override;

	public:
		virtual MeshOperationType OperationType() { return MeshOperationType::MeshReverseNormal; }

	};

} //namespace acamcad