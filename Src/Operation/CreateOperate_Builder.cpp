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
#include "CreateOperate_Builder.h"

#include <common/PointT.hpp>
#include <common/DirectionT.hpp>
#include <common/VectorT.hpp>
#include <common/FrameT.hpp>
#include <math/Polygon2.hpp>
#include <math/CircleT.hpp>
#include <math/Plane.hpp>

#include <geometry/Geom3TrimmedCurve.hpp>
#include <geometry/MakeSegment.hpp>
#include <geometry/MakeArcOfCircle.hpp>

#include <topology/TopoWire.hpp>
#include <topology/TopoFace.hpp>
#include <topology/TopoShape.hpp>
#include <topology/TopoCompound.hpp>
#include <topology/TopoBuilder.hpp>

using namespace AMCAX;
using namespace acamcad;

CreateOperate_Builder::CreateOperate_Builder()
{
}

CreateOperate_Builder::~CreateOperate_Builder()
{
}

void CreateOperate_Builder::point(const double x, const double y, const double z)
{
	points.emplace_back(x, y, z);
}

void CreateOperate_Builder::direction(const double x, const double y, const double z)
{
	dirs.emplace_back(x, y, z);
}

void CreateOperate_Builder::vector(const double x, const double y, const double z)
{
	vecs.emplace_back(x, y, z);
}

std::tuple<double, double, double> CreateOperate_Builder::double3TakeOne(const TYPE tp)
{
	double x{ NAN }, y{ NAN }, z{ NAN };
	switch (tp)
	{
	case TYPE_DIR:
	{
		auto dir = dirs.back();
		dirs.pop_back();
		x = dir[0];
		y = dir[1];
		z = dir[2];
	}
	break;
	case TYPE_VEC:
	{
		auto vec = vecs.back();
		vecs.pop_back();
		x = vec[0];
		y = vec[1];
		z = vec[2];
	}
	break;
	case TYPE_PNT:
	{
		auto pnt = points.back();
		points.pop_back();
		x = pnt[0];
		y = pnt[1];
		z = pnt[2];
	}
	break;
	default:;
	}
	return std::make_tuple(x, y, z);
}

void CreateOperate_Builder::frame(TYPE tp0, TYPE tp1, TYPE tp2)
{
	// tp0 is the type of the first double3, which is the point,
	//     and is the lost one being executed (added to the list)
	//     and we're using the lists as stack, which means it is 
	//     the first one to be read (taken from the list).
	auto [x0, y0, z0] = double3TakeOne(tp0);
	auto [x1, y1, z1] = double3TakeOne(tp1);
	auto [x2, y2, z2] = double3TakeOne(tp2);

	frames.push_back(Frame3{ Point3{x0, y0, z0},
							Direction3{x1, y1, z1},
							Direction3{x2, y2, z2} });
}

bool CreateOperate_Builder::hasPosition() {
	return hasFrame() || hasPoint() || hasVec() || hasDir();
}

bool CreateOperate_Builder::hasFrame() {
	return frames.size() > 0;
}
bool CreateOperate_Builder::hasPoint() {
	return points.size() > 0;
}
bool CreateOperate_Builder::hasVec() {
	return vecs.size() > 0;
}
bool CreateOperate_Builder::hasDir() {
	return dirs.size() > 0;
}
bool CreateOperate_Builder::hasCircle() {
	return circles.size() > 0;
}
bool CreateOperate_Builder::hasEdge() {
	return edges.size() > 0;
}
bool CreateOperate_Builder::hasWire() {
	return wires.size() > 0;
}
bool CreateOperate_Builder::hasFace() {
	return faces.size() > 0;
}
int CreateOperate_Builder::frameSize() {
	return frames.size();
}
int CreateOperate_Builder::pointSize() {
	return points.size();
}
int CreateOperate_Builder::vecSize() {
	return vecs.size();
}
int CreateOperate_Builder::dirSize() {
	return dirs.size();
}
int CreateOperate_Builder::edgeSize() {
	return edges.size();
}
int CreateOperate_Builder::wireSize() {
	return wires.size();
}
int CreateOperate_Builder::faceSize() {
	return faces.size();
}
AMCAX::Vector3 CreateOperate_Builder::getLastVec() {
	AMCAX::Vector3 lastVec = vecs.back();
	vecs.pop_back();
	return lastVec;
}

AMCAX::Point3 CreateOperate_Builder::getLastPoint() {
	AMCAX::Point3 lastPoint = points.back();
	points.pop_back();
	return lastPoint;
}
AMCAX::Direction3 CreateOperate_Builder::getLastDir() {
	AMCAX::Direction3 lastDir = dirs.back();
	dirs.pop_back();
	return lastDir;
}
std::vector<AMCAX::Point3> CreateOperate_Builder::getPoints() {
	return points;
}
void CreateOperate_Builder::frame(TYPE tp0, TYPE tp1) {
	auto [x0, y0, z0] = double3TakeOne(tp0);
	auto [x1, y1, z1] = double3TakeOne(tp1);

	frames.push_back(AMCAX::Frame3{ AMCAX::Point3{x0, y0, z0},
		AMCAX::Direction3{x1, y1, z1} });
}

void CreateOperate_Builder::edgePnt()
{
	auto point0 = points.back();
	points.pop_back();
	auto point1 = points.back();
	points.pop_back();
	edges.push_back(AMCAX::MakeEdge(point0, point1));
}
void CreateOperate_Builder::edgeCircle()
{
	auto circ = circles.back();
	circles.pop_back();

	edges.push_back(AMCAX::MakeEdge(circ));
}
void CreateOperate_Builder::edgeCircle(const double angle1, const double angle2)
{
	auto circ = circles.back();
	circles.pop_back();

	edges.push_back(AMCAX::MakeEdge(circ, angle1 * M_PI, angle2 * M_PI));
}
void CreateOperate_Builder::edgeCirclePnt() {
	auto circ = circles.back();
	circles.pop_back();
	auto pnt0 = points.back();
	points.pop_back();
	auto pnt1 = points.back();
	points.pop_back();

	edges.push_back(AMCAX::MakeEdge(circ, pnt0, pnt1));
}

void CreateOperate_Builder::circle(const double radius) {
	buildFrame();
	if (hasFrame())
	{
		auto frame = frames.back();
		frames.pop_back();

		circles.push_back(AMCAX::Circle3(frame, radius));
	}
}

void CreateOperate_Builder::wire(const size_t edgenum) {
	AMCAX::MakeWire mw;
	for (auto i = 0; i < edgenum; ++i)
	{
		mw.Add(edges.back());
		edges.pop_back();
	}
	wires.push_back(mw);
}

void CreateOperate_Builder::face() {
	if (hasEdge())
	{
		wires.push_back(AMCAX::MakeWire(edges.back()));
		edges.pop_back();
	}
	if (hasWire())
	{
		faces.push_back(AMCAX::MakeFace(wires.back()));
		wires.pop_back();
	}
}

void CreateOperate_Builder::faceToFace() {
	if (hasFace())
	{
		auto f = faces.back();
		faces.pop_back();
		faces.push_back(AMCAX::MakeFace(f));
	}
}

void CreateOperate_Builder::circleToFace() {
	if (circles.size())
	{
		auto cir = circles.back();
		circles.pop_back();
		faces.push_back(AMCAX::MakeFace());
	}
}

void CreateOperate_Builder::polygon(const size_t pointnum) {
	std::list<AMCAX::TopoShape> edges;
	for (int i = points.size() - pointnum + 1; i < points.size(); ++i)
	{
		edges.push_back(AMCAX::MakeEdge(points[i - 1], points[i]));
	}
	edges.push_back(AMCAX::MakeEdge(points.back(), points[points.size() - pointnum]));
	for (int i = 0; i < pointnum; ++i) {
		points.pop_back();
	}
	AMCAX::MakeWire mw;
	mw.Add(edges);

	faces.push_back(AMCAX::MakeFace(mw.Wire()));
}

void CreateOperate_Builder::polygon(const std::vector<AMCAX::Point3> pointlist) {
	std::list<AMCAX::TopoShape> edgelist;
	for (int i = 0; i < pointlist.size() - 1; ++i)
	{
		edgelist.push_back(AMCAX::MakeEdge(pointlist[i + 1], pointlist[i]));
	}
	edgelist.push_back(AMCAX::MakeEdge(pointlist.back(), pointlist[0]));
	AMCAX::MakeWire mw;
	mw.Add(edgelist);

	faces.push_back(AMCAX::MakeFace(mw.Wire()));
}

void CreateOperate_Builder::prismVec() {
	auto face = faces.back();
	faces.pop_back();
	auto vec = vecs.back();
	vecs.pop_back();

	shapes.push_back(AMCAX::MakePrism(face, vec));
}

void CreateOperate_Builder::prismDir() {
	auto face = faces.back();
	faces.pop_back();
	auto dir = dirs.back();
	dirs.pop_back();

	shapes.push_back(MakePrism(face, dir));
}

void CreateOperate_Builder::transform()
{
	// TODO
}

// take no points
void CreateOperate_Builder::box(const double x0, const double y0, const double z0,
	const double x1, const double y1, const double z1)
{
	AMCAX::Point3 bmin(x0, y0, z0);
	AMCAX::Point3 bmax(x1, y1, z1);
	shapes.push_back(AMCAX::MakeBox(bmin, bmax));
}

// take one point
void CreateOperate_Builder::boxP(const double x1, const double y1, const double z1)
{
	//gp_Pnt()
	buildFrame();
	if (hasFrame()) {
		auto fr = frames.back();
		frames.pop_back();
		shapes.push_back(AMCAX::MakeBox(fr, x1, y1, z1));
	}
	else {
		auto origin{ points.back() };
		points.pop_back();
		shapes.push_back(AMCAX::MakeBox(origin, x1, y1, z1));
	}
}

// default origin pnt(0, 0, 0)
void CreateOperate_Builder::box(const double dx, const double dy, const double dz)
{
	buildFrame();
	if (hasFrame()) {
		auto fr = frames.back();
		frames.pop_back();
		shapes.emplace_back(AMCAX::MakeBox(fr, dx, dy, dz));
	}
	else
	{
		AMCAX::Point3 origin(0.0, 0.0, 0.0);
		shapes.emplace_back(AMCAX::MakeBox(origin, dx, dy, dz));
	}
}

// take 2 points
void CreateOperate_Builder::box()
{
	auto bmin = points.back();
	points.pop_back();
	auto bmax = points.back();
	points.pop_back();
	shapes.push_back(AMCAX::MakeBox(bmin, bmax));
}

void CreateOperate_Builder::cylinderFrame(const double radius, const double height, const double angle)
{
	auto fr = frames.back();
	frames.pop_back();
	shapes.push_back(AMCAX::MakeCylinder(fr, radius, height, angle * M_PI));
}
void CreateOperate_Builder::cylinder(const double radius, const double height, const double angle)
{
	buildFrame();
	if (hasFrame())
	{
		auto fr = frames.back();
		frames.pop_back();
		shapes.push_back(AMCAX::MakeCylinder(fr, radius, height, angle * M_PI));
	}
	else {
		shapes.push_back(AMCAX::MakeCylinder(radius, height, angle * M_PI));
	}

}
void CreateOperate_Builder::cylinderFrame(const double radius, const double height)
{
	auto fr = frames.back();
	frames.pop_back();
	shapes.push_back(AMCAX::MakeCylinder(fr, radius, height));
}
void CreateOperate_Builder::cylinder(const double radius, const double height)
{
	buildFrame();
	if (hasFrame())
	{
		auto fr = frames.back();
		frames.pop_back();
		shapes.push_back(AMCAX::MakeCylinder(fr, radius, height));
	}
	else {
		shapes.push_back(AMCAX::MakeCylinder(radius, height));
	}
}

void CreateOperate_Builder::coneFrame(const double r1, const double r2, const double height, const double angle)
{
	auto fr = frames.back();
	frames.pop_back();
	shapes.push_back(AMCAX::MakeCone(fr, r1, r2, height, angle * M_PI));
}
void CreateOperate_Builder::coneFrame(const double r1, const double r2, const double height)
{
	auto fr = frames.back();
	frames.pop_back();
	shapes.push_back(AMCAX::MakeCone(fr, r1, r2, height));
}
void CreateOperate_Builder::cone(const double r1, const double r2, const double height, const double angle)
{
	buildFrame();
	if (hasFrame())
	{
		auto fr = frames.back();
		frames.pop_back();
		shapes.push_back(AMCAX::MakeCone(fr, r1, r2, height, angle * M_PI));
	}
	else {
		shapes.push_back(AMCAX::MakeCone(r1, r2, height, angle * M_PI));
	}
}
void CreateOperate_Builder::cone(const double r1, const double r2, const double height)
{
	buildFrame();
	if (hasFrame())
	{
		auto fr = frames.back();
		frames.pop_back();
		shapes.push_back(AMCAX::MakeCone(fr, r1, r2, height));
	}
	else {
		shapes.push_back(AMCAX::MakeCone(r1, r2, height));
	}
}
void CreateOperate_Builder::torusFrame(const double r1, const double r2) {
	auto fr = frames.back();
	frames.pop_back();
	shapes.push_back(AMCAX::MakeTorus(fr, r1, r2));
}
void CreateOperate_Builder::torusFrame(const double r1, const double r2, const double angle) {
	auto fr = frames.back();
	frames.pop_back();
	shapes.push_back(AMCAX::MakeTorus(fr, r1, r2, angle * M_PI));
}
void CreateOperate_Builder::torus(const double r1, const double r2) {
	buildFrame();
	if (hasFrame())
	{
		auto fr = frames.back();
		frames.pop_back();
		shapes.push_back(AMCAX::MakeTorus(fr, r1, r2));
	}
	else {
		shapes.push_back(AMCAX::MakeTorus(r1, r2));
	}
}
void CreateOperate_Builder::torus(const double r1, const double r2, const double angle) {
	buildFrame();
	if (hasFrame())
	{
		auto fr = frames.back();
		frames.pop_back();
		shapes.push_back(AMCAX::MakeTorus(fr, r1, r2, angle * M_PI));
	}
	else {
		shapes.push_back(AMCAX::MakeTorus(r1, r2, angle * M_PI));
	}
}

void CreateOperate_Builder::sphereP(const double r, const double angle) {
	if (hasPoint()) {
		auto origin{ points.back() };
		points.pop_back();
		shapes.push_back(AMCAX::MakeSphere(origin, r, angle));
	}
}

void CreateOperate_Builder::sphereP(const double r) {
	if (hasPoint()) {
		auto origin{ points.back() };
		points.pop_back();
		shapes.push_back(AMCAX::MakeSphere(origin, r));
	}
}

void CreateOperate_Builder::sphereFrame(const double r, const double angle) {
	if (hasFrame())
	{
		auto fr = frames.back();
		frames.pop_back();
		shapes.push_back(MakeSphere(fr, r, angle));
	}
}

void CreateOperate_Builder::sphereFrame(const double r) {
	if (hasFrame())
	{
		auto fr = frames.back();
		frames.pop_back();
		shapes.push_back(AMCAX::MakeSphere(fr, r));
	}
}

void CreateOperate_Builder::sphere(const double r) {
	buildFrame();
	if (hasFrame())
	{
		auto fr = frames.back();
		frames.pop_back();
		shapes.push_back(AMCAX::MakeSphere(fr, r));
	}
	else if (hasPoint()) {
		auto origin{ points.back() };
		points.pop_back();
		shapes.push_back(AMCAX::MakeSphere(origin, r));
	}
	else {
		shapes.push_back(AMCAX::MakeSphere(r));
	}
}
void CreateOperate_Builder::sphere(const double r, const double angle) {
	buildFrame();
	if (hasFrame())
	{
		auto fr = frames.back();
		frames.pop_back();
		shapes.push_back(AMCAX::MakeSphere(fr, r, angle * M_PI));
	}
	else if (hasPoint()) {
		auto origin{ points.back() };
		points.pop_back();
		shapes.push_back(AMCAX::MakeSphere(origin, r, angle * M_PI));
	}
	else {
		shapes.push_back(AMCAX::MakeSphere(r, angle * M_PI));
	}
}
void CreateOperate_Builder::sphere(double r, const double angle1, const double angle2) {
	buildFrame();
	if (hasFrame())
	{
		auto fr = frames.back();
		frames.pop_back();
		shapes.push_back(AMCAX::MakeSphere(fr, r, angle1 * M_PI, angle2 * M_PI));
	}
	else if (hasPoint()) {
		auto origin{ points.back() };
		points.pop_back();
		shapes.push_back(AMCAX::MakeSphere(origin, r, angle1 * M_PI, angle2 * M_PI));
	}
	else {
		shapes.push_back(AMCAX::MakeSphere(r, angle1 * M_PI, angle2 * M_PI));
	}
}
void CreateOperate_Builder::sphere(const double r, const double angle1, const double angle2, const double angle3) {
	buildFrame();
	if (hasFrame())
	{
		auto fr = frames.back();
		frames.pop_back();
		shapes.push_back(AMCAX::MakeSphere(fr, r, angle1 * M_PI, angle2 * M_PI, angle3 * M_PI));
	}
	else if (hasPoint()) {
		auto origin{ points.back() };
		points.pop_back();
		shapes.push_back(AMCAX::MakeSphere(origin, r, angle1 * M_PI, angle2 * M_PI, angle3 * M_PI));
	}
	else {
		shapes.push_back(AMCAX::MakeSphere(r, angle1 * M_PI, angle2 * M_PI, angle3 * M_PI));
	}
}

void CreateOperate_Builder::buildFrame() {
	if (hasPoint() || hasDir())
	{
		AMCAX::Frame3 frame;
		if (hasPoint()) {
			AMCAX::Point3 center{ points.back() };
			points.pop_back();
			frame.SetLocation(center);
		}
		else {
			//default center(0,0,0)
			AMCAX::Point3 center(.0, .0, .0);
			frame.SetLocation(center);
		}
		if (hasDir())
		{
			if (dirSize() >= 2)
			{
				AMCAX::Direction3 xdir{ dirs.back() };
				dirs.pop_back();
				frame.SetXDirection(xdir);
			}
			AMCAX::Direction3 dir{ dirs.back() };
			dirs.pop_back();
			frame.SetDirection(dir);
		}
		else {
			//default direction(0,0,1)
			AMCAX::Direction3 dir(.0, .0, 1.0);
			frame.SetDirection(dir);
		}
		frames.push_back(frame);
	}
}

void CreateOperate_Builder::wedge(const double dx, const double dy, const double dz, const double ltx) {
	buildFrame();
	if (hasFrame())
	{
		auto fr = frames.back();
		frames.pop_back();
		shapes.push_back(AMCAX::MakeWedge(fr, dx, dy, dz, ltx));
	}
	else {
		shapes.push_back(AMCAX::MakeWedge(dx, dy, dz, ltx));
	}
}
void CreateOperate_Builder::wedgeFrame(const double dx, const double dy, const double dz, const double ltx) {
	auto fr = frames.back();
	frames.pop_back();
	shapes.push_back(AMCAX::MakeWedge(fr, dx, dy, dz, ltx));
}
void CreateOperate_Builder::wedge(const double dx, const double dy, const double dz, const double xmin, const double zmin, const double xmax, const double zmax) {
	buildFrame();
	if (hasFrame())
	{
		auto fr = frames.back();
		frames.pop_back();
		shapes.push_back(AMCAX::MakeWedge(fr, dx, dy, dz, xmin, zmin, xmax, zmax));
	}
	else {
		shapes.push_back(AMCAX::MakeWedge(dx, dy, dz, xmin, zmin, xmax, zmax));
	}
}
void CreateOperate_Builder::wedgeFrame(const double dx, const double dy, const double dz, const double xmin, const double zmin, const double xmax, const double zmax) {
	auto fr = frames.back();
	frames.pop_back();
	shapes.push_back(AMCAX::MakeWedge(fr, dx, dy, dz, xmin, zmin, xmax, zmax));
}

void CreateOperate_Builder::plane(const double a, const double b, const double c, const double d, const double ulength, const double vlength) {
	faces.push_back(AMCAX::MakeFace(AMCAX::Plane(a, b, c, d)));
}

void CreateOperate_Builder::plane(const double ulength, const double vlength) {
	buildFrame();
	if (hasFrame())
	{
		auto fr = frames.back();
		frames.pop_back();
		AMCAX::Point3 center = fr.Location();
		faces.push_back(AMCAX::MakeFace(AMCAX::Plane(fr), center.X() - ulength / 2, center.X() + ulength / 2, center.Y() - vlength / 2, center.Y() + vlength / 2));
	}
}

void CreateOperate_Builder::plane() {
	if (pointSize() >= 2)
	{
		auto p0_{ points.back() };
		points.pop_back();
		auto p1_{ points.back() };
		points.pop_back();
		double xmin = p0_.X(), xmax = p1_.X();
		double ymin = p0_.Y(), ymax = p1_.Y();
		faces.push_back(AMCAX::MakeFace(AMCAX::Plane(), xmin, xmax, ymin, ymax));
	}
}

void coutPoint(AMCAX::Point3 p) {
	std::cerr << "(" << p.X() << "," << p.Y() << "," << p.Z() << ")\n";
}

void CreateOperate_Builder::pyramid() {
	if (pointSize() >= 4)
	{
		AMCAX::Point3 vertex = getLastPoint();
		for (int i = 0; i < pointSize() - 1; ++i)
		{
			std::vector<AMCAX::Point3> pointtemp;
			pointtemp.push_back(vertex);
			pointtemp.push_back(points[i]);
			pointtemp.push_back(points[i + 1]);
			polygon(pointtemp);
		}
		std::vector<AMCAX::Point3> pointlist;
		pointlist.push_back(vertex);
		pointlist.push_back(points[0]);
		pointlist.push_back(points.back());
		polygon(pointlist);
		polygon(points);
	}
	AMCAX::TopoCompound comp;
	AMCAX::TopoBuilder build;
	build.MakeCompound(comp);
	for (auto& mf : faces)
	{
		build.Add(comp, mf);
	}
	shapes.push_back(comp);
}

bool CreateOperate_Builder::DoOperate(AdapterObject* brep_object)
{
	if (shapes.size())
	{
		for (AMCAX::TopoShape& shape : shapes)
		{
			brep_object->bRep->getShape() = shape;
		}
	}
	else if (faces.size())
	{
		brep_object->bRep->getShape() = faces.back();
	}
	else if (wires.size())
	{
		brep_object->bRep->getShape() = wires.back();
	}
	else if (edges.size())
	{
		brep_object->bRep->getShape() = edges.back();
	}

	brep_object->updateDraw();

	return true;
}
