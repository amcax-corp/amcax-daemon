#pragma once
#include "MOperate_SelectInfo.h"

namespace acamcad
{

	class SGLOperate_SplitLoop : public MOperate_SingleObject
	{
	public:
		SGLOperate_SplitLoop(const SelectModel& smodle);
		virtual ~SGLOperate_SplitLoop() {};

	public:
		virtual bool DoOperate(AdapterObject* mesh_object) override;
		//virtual void operateWithTSpline(TSplineObject* tspline_object) override;

	public:
		virtual MeshOperationType OperationType() { return MeshOperationType::MeshSplitLoop; }

		virtual void setParameter(const std::vector<AMCAX::Coord3>& related_point) override;

	private:
		std::vector<AMCAX::Coord3> re_points_;
	};

} //namespace acamcad