#pragma once
#include "MOperate_SelectInfo.h"

namespace acamcad
{

	class SGLOperate_ExtrudeEdgeH : public MOperate_SingleObject
	{
	public:
		SGLOperate_ExtrudeEdgeH(const SelectModel& smodle);
		virtual ~SGLOperate_ExtrudeEdgeH() {};

	public:
		virtual bool DoOperate(AdapterObject* mesh_object) override;
		//virtual void operateWithTSpline(TSplineObject* tspline_object);

	public:
		virtual MeshOperationType OperationType() { return MeshOperationType::MeshExtrudeEdgeHorizontal; }

		virtual AMCAX::Coord3 getDirection() override { return dire_; }

	private:
		AMCAX::Coord3 dire_;
	};

	class SGLOperate_ExtrudeEdgeV : public MOperate_SingleObject
	{
	public:
		SGLOperate_ExtrudeEdgeV(const SelectModel& smodle);
		virtual ~SGLOperate_ExtrudeEdgeV() {};

	public:
		virtual bool DoOperate(AdapterObject* adapter) override;
		//virtual void operateWithTSpline(TSplineObject* tspline_object);

	public:
		virtual MeshOperationType OperationType() { return MeshOperationType::MeshExtrudeEdgeVertical; }

		virtual AMCAX::Coord3 getDirection() override { return dire_; }

	private:
		AMCAX::Coord3 dire_;
	};



} //namespace acamcad