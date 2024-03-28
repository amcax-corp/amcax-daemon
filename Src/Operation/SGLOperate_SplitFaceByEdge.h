#pragma once
#include "MOperate_SelectInfo.h"

namespace acamcad
{

	class SGLOperate_SplitFaceByEdge : public MOperate_SingleObject
	{
	public:
		SGLOperate_SplitFaceByEdge(const SelectModel& smodle);
		virtual ~SGLOperate_SplitFaceByEdge() {};

	public:
		virtual bool DoOperate(AdapterObject* adapter) override;
		//virtual bool operateWithTSpline(TSplineObject* tspline_object) override;

	public:
		virtual MeshOperationType OperationType() { return MeshOperationType::MeshSplitFaceByEdge; }

		virtual void setParameter(const std::vector<AMCAX::Coord3>& related_point) override;

	private:
		std::vector<AMCAX::Coord3> re_points_;
	};

} //namespace acamcad