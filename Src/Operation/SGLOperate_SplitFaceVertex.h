#pragma once
#include "MOperate_SelectInfo.h"

namespace acamcad
{

	class SGLOperate_SplitFaceVertex : public MOperate_SingleObject
	{
	public:
		SGLOperate_SplitFaceVertex(const SelectModel& smodle);
		virtual ~SGLOperate_SplitFaceVertex() {};

	public:
		virtual bool DoOperate(AdapterObject* mesh_object) override;
		//virtual void operateWithTSpline(TSplineObject* tspline_object);

	public:
		virtual MeshOperationType OperationType() { return MeshOperationType::MeshSplitFaceVertex; }
	};

} //namespace acamcad