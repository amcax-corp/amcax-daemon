#include "TSplineObject.h"

#include <tmeshSplineAPI/TMSplineCopy.hpp>
#include <tmeshSpline/TMSplineIO.hpp>
#include <tmeshSplineAPI/TMSplineTool.hpp>
#include <tmeshSplineAPI/TMSplineTransform.hpp>

#include <common/AxisT.hpp>
#include <common/FrameT.hpp>

#include "../ObjectDraw/TSplineObjectDraw.h"
#include "../Operation/MOperate.h"
#include "../Utils/MathUtils.h"

namespace acamcad
{

	TSplineObject::TSplineObject(AdapterObject* parent) :AdapterObjectParent(parent),
		spline_(nullptr), scale_(1)
	{
		//setDataType(DataType::TSPLINEU_TYPE);
		draw_tool_ = std::make_unique<TSplineObjectDraw>(parent);
	}

	TSplineObject::TSplineObject(AdapterObject* parent, const TSplineObject& object, bool isDraw) :
		AdapterObjectParent(parent), spline_(nullptr), scale_(1)
		//BaseObject(object), spline_(nullptr)
	{
		if (object.spline_ != nullptr)
		{
			//AMCAX::TMS::TMSplineCopy copy;
			spline_ = AMCAX::TMS::TMSplineCopy().CloneTMSpline(object.spline_);
			//tcopy.copyTspline(object.spline_, spline_);
		}
		if (isDraw) {
			//setDataType(DataType::TSPLINEU_TYPE);
			draw_tool_ = std::make_unique<TSplineObjectDraw>(parent);
			draw_tool_->updatedrawState();
		}
	}

	TSplineObject::~TSplineObject()
	{
		if (spline_ != nullptr)
		{
			spline_->clear();
			delete spline_;
		}
	}

	AMCAX::TMS::TMSpline* TSplineObject::getTSpline()
	{
		return spline_;
	}

	const AMCAX::TMS::TMSpline* TSplineObject::getTSpline() const
	{
		return spline_;
	}

	void TSplineObject::setTSpline(AMCAX::TMS::TMSpline* tsp)
	{
		if (spline_ != nullptr)
		{
			delete spline_;
			spline_ = nullptr;

			draw_tool_->clear();
		}

		spline_ = tsp;
	}

	void TSplineObject::assignTSpline()
	{
		if (spline_ == nullptr)
			spline_ = new AMCAX::TMS::TMSpline();
	}

	// select vert edge face
	void TSplineObject::selectVertWithFaceId(const AMCAX::Vector3& begin, const AMCAX::Vector3& end, int f_id, int& sv_id)
	{
		sv_id = -1;

		//auto ids = AMCAX::SubD::MeshTool::FaceVertexIndexs(mesh_, f_id);
		auto ids = AMCAX::TMS::TMSplineTool::FaceVertexIndexs(spline_, f_id);
		AMCAX::Coord3 coord;
		double len_min;

		for (int i = 0; i < ids.size(); i++)
		{

			AMCAX::Point3 position = AMCAX::TMS::TMSplineTool::ControlPosition(spline_, ids[i]);
			double len = MathUtils::distPointLineSquared(position.Coord(), begin.Coord(), end.Coord(), coord);

			if (len > 0.1) {
				continue;
			}


			if (sv_id == -1 || len_min > len)
			{
				sv_id = ids[i];
				len_min = len;
			}
		}

	}

	void TSplineObject::selectEdgeWithFaceId(const AMCAX::Vector3& begin, const AMCAX::Vector3& end, int f_id, int& se_id)
	{
		se_id = -1;
		auto ids = AMCAX::TMS::TMSplineTool::FaceEdgeIndexs(spline_, f_id);
		AMCAX::Coord3 coord0, coord1;
		double len_min = 0;

		for (int i = 0; i < ids.size(); i++) {
			int id0, id1;
			AMCAX::TMS::TMSplineTool::EdgeVertexIndexs(spline_, ids[i], id0, id1);
			AMCAX::Point3 p0 = AMCAX::TMS::TMSplineTool::ControlPosition(spline_, id0),
				p1 = AMCAX::TMS::TMSplineTool::ControlPosition(spline_, id1);
			double len = MathUtils::distLine_SLineSquared(p0.Coord(), p1.Coord(), begin.Coord(), end.Coord(), coord0, coord1);

			if (len > 0.1) {
				continue;
			}

			if (se_id == -1 || len_min > len)
			{
				se_id = ids[i];
				len_min = len;
			}

		}

	}

	//void TSplineObject::backSpline(AMCAX::TMS::TMSpline*& back_spline)
	//{
	//	if (spline_ != nullptr)
	//	{
	//		//if (back_spline == nullptr)
	//		//	back_spline = new AMCAX::TMS::TMSpline();

	//		back_spline = AMCAX::TMS::TMSplineCopy().CloneTMSpline(spline_);

	//		//tspline::TO_Duplicate tcopy;
	//		//tcopy.cloneTspline(spline_, back_spline);
	//	}
	//	else
	//	{
	//		back_spline = nullptr;
	//	}
	//}

	//===========================================================================

	bool TSplineObject::loadSpline(const QString& _filename)
	{
		parent_->setFromFileName(_filename);

		// call the local function to update names
		//parent_->setName(parent_->name());

		std::string filename = _filename.toStdString();

		AMCAX::TMS::TMSplineIO io;

		bool ok = io.LoadTMSpline(filename, spline_);
		if (ok)
		{
			updateDraw();
			parent_->show();
		}
		else
		{
			std::cout << "Load File fall" << std::endl;
		}

		return ok;
	}

	bool TSplineObject::saveSpline(const QString& _filename)
	{
		std::string filename = _filename.toStdString();

		AMCAX::TMS::TMSplineIO io;

		bool ok = io.WriteTMSpline(filename, spline_);

		if (!ok)
		{
			std::cout << "Save File fall" << std::endl;
		}
		return ok;
	}

	bool TSplineObject::saveSplineSTL(const QString& filename)
	{
		//TSplineObjectDraw* dt = dynamic_cast<TSplineObjectDraw*>(draw_tool_);
		return draw_tool_->saveSTL(filename.toStdString());
	}

	bool TSplineObject::saveSplineOBJ(const QString& filename)
	{
		//TSplineObjectDraw* dt = dynamic_cast<TSplineObjectDraw*>(draw_tool_);
		return draw_tool_->saveOBJ(filename.toStdString());
	}

	//bool TSplineObject::saveSplineFrep(const QString& filename)
	//{
	//	TSplineObjectDraw* dt = dynamic_cast<TSplineObjectDraw*>(draw_tool_);
	//	std::vector<int> fixIndex; fixIndex.reserve(frepFixFaceIndex.size());
	//	for (auto iter = frepFixFaceIndex.begin(); iter != frepFixFaceIndex.end(); ++iter)
	//		fixIndex.push_back(*iter);
	//	std::vector<int> forceIndex; forceIndex.reserve(frepForceFaceIndex.size());
	//	for (auto iter = frepForceFaceIndex.begin(); iter != frepForceFaceIndex.end(); ++iter)
	//		forceIndex.push_back(*iter);

	//	return dt->saveFrepMesh(filename.toStdString(), fixIndex, forceIndex);
	//}

	//===========================================================================

	void TSplineObject::updateDraw()
	{
		draw_tool_->updatedrawState();

		if (spline_)
			AMCAX::TMS::TMSplineTool::UpdateNormal(spline_);
	}

	//===========================================================================

	void TSplineObject::mesh_OPvert_Backup()
	{
		scale_ = 1;
		size_t vert_num = spline_->numVertices();
		mesh_v_id_back_.resize(vert_num);
		//mesh_v_position_back_.resize(vert_num);
		for (size_t i = 0; i < vert_num; i++)
		{
			mesh_v_id_back_[i] = i;
			//mesh_v_position_back_[i] = spline_->getVertControl(i);
		}
	}

	void TSplineObject::mesh_OPvert_BackupVertes(const std::vector<int>& vert_id)
	{
		scale_ = 1;
		mesh_v_id_back_.resize(vert_id.size());
		for (size_t i = 0; i < vert_id.size(); i++)
		{
			mesh_v_id_back_[i] = vert_id[i];
		}

		std::sort(mesh_v_id_back_.begin(), mesh_v_id_back_.end());
		mesh_v_id_back_.erase(std::unique(mesh_v_id_back_.begin(), mesh_v_id_back_.end()), mesh_v_id_back_.end());
	}

	void TSplineObject::mesh_OPvert_BackupEdges(const std::vector<int>& edge_id)
	{
		scale_ = 1;
		mesh_v_id_back_.clear();
		//for()
		for (int i = 0; i < edge_id.size(); i++) {
			int v0, v1;
			AMCAX::TMS::TMSplineTool::EdgeVertexIndexs(spline_, edge_id[i], v0, v1);

			mesh_v_id_back_.push_back(v0);
			mesh_v_id_back_.push_back(v1);
		}

		std::sort(mesh_v_id_back_.begin(), mesh_v_id_back_.end());
		mesh_v_id_back_.erase(std::unique(mesh_v_id_back_.begin(), mesh_v_id_back_.end()), mesh_v_id_back_.end());

	}

	void TSplineObject::mesh_OPvert_BackupFaces(const std::vector<int>& face_id)
	{
		scale_ = 1;
		mesh_v_id_back_.clear();
		//mesh_v_position_back_.resize(vert_id.size());


		//std::sort(vert_id.begin(), vert_id.end());
		//vert_id.erase(std::unique(vert_id.begin(), vert_id.end()), vert_id.end());

		for (int i = 0; i < face_id.size(); i++) {
			std::vector<int> vert_id = AMCAX::TMS::TMSplineTool::FaceVertexIndexs(spline_, face_id[i]);

			//mesh_v_id_back_.push_back( vert_id);

			for (auto iter : vert_id) {

				mesh_v_id_back_.push_back(iter);
			}
		}

		std::sort(mesh_v_id_back_.begin(), mesh_v_id_back_.end());
		mesh_v_id_back_.erase(std::unique(mesh_v_id_back_.begin(), mesh_v_id_back_.end()), mesh_v_id_back_.end());

	}

	void TSplineObject::mesh_Clear_OPvertBackup()
	{

		mesh_v_id_back_.clear();

		scale_ = 1;


	}

	void TSplineObject::meshScaleSingle(const AMCAX::Vector3& center, const AMCAX::Vector3& axis, double scale)
	{
		scale_ = scale / scale_;

		if (std::isnan(scale_)) {
			scale_ = 1;
			return;
		}

		AMCAX::Point3 point(center.Coord());
		AMCAX::Direction3 dir(axis.Coord());
		AMCAX::Frame3 ct(point, dir);

		AMCAX::TMS::TMSplineTransform trf;
		AMCAX::GeneralTransformation3 trfScale;
		trfScale.SetAffinity(ct, scale_);
		trf.SetGTransformation(trfScale);

		trf.TransformTMSplineVertices(spline_, mesh_v_id_back_);
		scale_ = scale;
		trf.TransformReprocessing(spline_);
	}

	void TSplineObject::meshScalePlane(const AMCAX::Vector3& center, const AMCAX::Vector3& normal, double scale)
	{
		//M_Transform trsf_;
		//trsf_.setScalePlane(center.point(), normal, scale);
		//applyTransForm(trsf_);

		scale_ = scale / scale_;

		if (std::isnan(scale_)) {
			scale_ = 1;
			return;
		}

		AMCAX::Point3 point(center.Coord());
		AMCAX::Direction3 dir(normal.Coord());
		AMCAX::Axis3 ct(point, dir);

		AMCAX::TMS::TMSplineTransform trf;
		AMCAX::GeneralTransformation3 trfScale;
		trfScale.SetAffinity(ct, scale_);
		trf.SetGTransformation(trfScale);

		trf.TransformTMSplineVertices(spline_, mesh_v_id_back_);
		scale_ = scale;
		trf.TransformReprocessing(spline_);
	}

	void TSplineObject::meshRotation(const AMCAX::Vector3& center, const AMCAX::Vector3& axis, double angle)
	{
		AMCAX::Point3 ct(center.X(), center.Y(), center.Z());

		AMCAX::TMS::TMSplineTransform trsf;
		AMCAX::Transformation3 trfRotation;
		trfRotation.SetRotation(AMCAX::Axis3(ct, AMCAX::Direction3(axis.X(), axis.Y(), axis.Z())), angle);

		trsf.SetTransformation(trfRotation);

		trsf.TransformTMSplineVertices(spline_, mesh_v_id_back_);
		trsf.TransformReprocessing(spline_);
	}

	void TSplineObject::meshMove(const AMCAX::Vector3& v, double dis)
	{
		AMCAX::Vector3 vh(v.Normalized() * dis);
		AMCAX::Transformation3 trMove;
		trMove.SetTranslation(vh);

		AMCAX::TMS::TMSplineTransform trf;
		trf.SetTransformation(trMove);

		trf.TransformTMSplineVertices(spline_, mesh_v_id_back_);

		trf.TransformReprocessing(spline_);
	}

	const ObjectDraw* TSplineObject::getDrawTool() const
	{
		return draw_tool_.get();
	}

	void TSplineObject::mirror(const AMCAX::Coord3& center, const AMCAX::Coord3& normal)
	{
		AMCAX::Point3 ct(center);
		AMCAX::Frame3 frame(ct, AMCAX::Direction3(normal));

		AMCAX::TMS::TMSplineTransform trf;
		AMCAX::Transformation3 trfMirror;
		trfMirror.SetMirror(frame);

		trf.SetTransformation(trfMirror);
		trf.TransformTMSpline(spline_);

		trf.TransformReprocessing(spline_);
	}

	//===========================================================




}
