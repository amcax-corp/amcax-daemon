/* ===================================================================
* Copyright (C) 2023 Hefei Jiushao Intelligent Technology Co., Ltd.
* All rights reserved.
*
* This software is licensed under the GNU Affero General Public License
* v3.0 (AGPLv3.0) or a commercial license. You may choose to use this
* software under the terms of either license.
*
* For more information about the AGPLv3.0 license, please visit:
* https://www.gnu.org/licenses/agpl-3.0.html
* For licensing inquiries or to obtain a commercial license, please
* contact Hefei Jiushao Intelligent Technology Co., Ltd.
* ===================================================================
* Author:
*/
#ifndef CREATEOPERATE_BUILDER_H
#define CREATEOPERATE_BUILDER_H

#include "MOperate.h"


#include <modeling/MakeFace.hpp>
#include <modeling/MakeWire.hpp>
#include <modeling/MakeEdge.hpp>
#include <modeling/MakePrism.hpp>
#include <modeling/MakeBox.hpp>
#include <modeling/MakeCylinder.hpp>
#include <modeling/MakeSphere.hpp>
#include <modeling/MakeCone.hpp>
#include <modeling/MakeWedge.hpp>
#include <modeling/MakeTorus.hpp>
#include <math/CircleT.hpp>
//#include <modeling/MakeSweep.hpp>
//#include <modeling/MakeShape.hpp>
//#include <modeling/MakeEdge2d.hpp>

#include <memory>

#include <list>
#include <vector>

namespace AMCAD
{
	template<typename T, int DIM>
	class PointT;
	template<typename T, int DIM>
	class DirectionT;
	template<typename T, int DIM>
	class VectorT;
	template<typename T, int DIM>
	class FrameT;
	//template<typename T, int DIM>
	//class CircleT;

	//    class MakeEdge;
	//    class MakeWire;
	//    class MakeFace;
	//    class MakePolygon;
	//    class MakePrism;
	//    class MakeBox;
	//    class MakeCylinder;
	//    class MakeCone;
	//    class MakeWedge;
	//    class MakeShape;
	//    class MakeSphere;
	//    class MakeTorus;
	//    class MakeSweep;
	//    class MakeEdge2d;
}

namespace acamcad
{
	class CreateOperate_Builder : public MOperation
	{
	private:
		enum OP
		{
			OP_NONE, OP_PNT, OP_DIR, OP_VEC, OP_FRAME, OP_PLANE, OP_TRSF,
		};
	public:
		enum TYPE
		{
			TYPE_NONE, TYPE_PNT, TYPE_DIR, TYPE_VEC,
		};

	public:
		CreateOperate_Builder();
		~CreateOperate_Builder();

		virtual bool DoOperate(AdapterObject* brep_object) override;

	public:
		void point(const double x, const double y, const double z);

		void direction(const double x, const double y, const double z);

		void vector(const double x, const double y, const double z);

		std::tuple<double, double, double> double3TakeOne(const TYPE tp);

		void frame(TYPE tp0, TYPE tp1, TYPE tp2);
		void frame(TYPE tp0, TYPE tp1);
		bool hasPosition();
		bool hasFrame();
		bool hasPoint();
		bool hasVec();
		bool hasDir();
		bool hasCircle();
		bool hasEdge();
		bool hasWire();
		bool hasFace();
		int frameSize();
		int pointSize();
		int dirSize();
		int vecSize();
		int edgeSize();
		int wireSize();
		int faceSize();
		AMCAX::Vector3 getLastVec();
		AMCAX::Point3 getLastPoint();
		AMCAX::Direction3 getLastDir();
		std::vector<AMCAX::Point3> getPoints();

		void edgePnt(); // using the last two pnt
		void edgeCircle(); // using the last circ
		void edgeCircle(const double angle1, const double angle2); // using the last circ
		void edgeCirclePnt(); // using the last circ and the last two gp_Pnt

		void circle(const double radius);

		void wire(const size_t edgenum); // take the last edgenum edges

		void face(); // take the last wire
		void faceToFace();
		void circleToFace();

		void polygon(const size_t pointnum); // take the last pointnum points; 
		void polygon(const std::vector<AMCAX::Point3> pointlist);
		void prismVec(); // take the last face and the last vec/dir
		void prismDir(); // take the last face and the last vec/dir
		void prism();

		void transform();

		void box(const double x0, const double y0, const double z0,
			const double x1, const double y1, const double z1); // take no points
		void boxP(const double x1, const double y1, const double z1); // take one point
		void box(const double dx, const double dy, const double dz); // default origin pnt(0, 0, 0)
		void box(); // take 2 points

		void cylinderFrame(const double radius, const double height, const double angle);
		void cylinder(const double radius, const double height, const double angle);
		void cylinderFrame(const double radius, const double height);
		void cylinder(const double radius, const double height);

		void coneFrame(const double r1, const double r2, const double height, const double angle);
		void coneFrame(const double r1, const double r2, const double height);
		void cone(const double r1, const double r2, const double height, const double angle);
		void cone(const double r1, const double r2, const double height);

		void torusFrame(const double r1, const double r2);
		void torusFrame(const double r1, const double r2, const double angle);
		void torus(const double r1, const double r2);
		void torus(const double r1, const double r2, const double angle);

		void sphere(const double r);
		void sphere(const double r, const double angle);
		void sphere(const double r, const double angle1, const double angle2);
		void sphere(const double r, const double angle1, const double angle2, const double angle3);
		void sphereP(const double r);
		void sphereP(const double r, const double angle);
		void sphereFrame(const double r);
		void sphereFrame(const double r, const double angle);

		void wedge(const double dx, const double dy, const double dz, const double ltx);
		void wedgeFrame(const double dx, const double dy, const double dz, const double ltx);
		void wedge(const double dx, const double dy, const double dz, const double xmin, const double zmin, const double xmax, const double zmax);
		void wedgeFrame(const double dx, const double dy, const double dz, const double xmin, const double zmin, const double xmax, const double zmax);

		void plane(const double a, const double b, const double c, const double d, const double ulength, const double vlength);
		void plane(const double ulength, const double vlength);
		void plane();
		void pyramid();

		void buildFrame();

	private:
		// std::list<enum OP> order;
		std::vector<AMCAX::PointT<double, 3>> points;
		std::vector<AMCAX::DirectionT<double, 3>> dirs;
		std::vector<AMCAX::VectorT<double, 3>> vecs;
		std::vector<AMCAX::FrameT<double, 3>> frames;

		std::vector<AMCAX::CircleS<3>> circles;

		std::vector<AMCAX::MakeEdge> edges;
		std::vector<AMCAX::MakeWire> wires;
		std::vector<AMCAX::MakeFace> faces;

		std::vector<AMCAX::TopoShape> shapes;
	};
}

#endif // CREATEOPERATE_BUILDER_H
