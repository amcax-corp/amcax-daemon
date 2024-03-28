#pragma once
#include "MOperate_SelectInfo.h"

namespace acamcad
{

	class SGLOperate_ExtrudeFace : public MOperate_SingleObject
	{
	public:
		SGLOperate_ExtrudeFace(const SelectModel& smodle);
		virtual ~SGLOperate_ExtrudeFace() {};

	public:
		virtual bool DoOperate(AdapterObject* adapter) override;
		///virtual void operateWithTSpline(TSplineUObject* tspline_object) override;
		//virtual void operateWithBRep(BRepObject* brep_object) override;

	public:
		virtual MeshOperationType OperationType() { return MeshOperationType::MeshExtrudeFaceNew; }

		//virtual void setDirection(const MVector3& vec3) { dire_ = vec3; }
		virtual AMCAX::Coord3 getDirection() override { return dire_; }

	private:
		AMCAX::Coord3 dire_;
	};

} //namespace acamcad