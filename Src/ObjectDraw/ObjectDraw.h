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
#pragma once
#if _WIN32 || _WIN64
#include <Windows.h>
#endif
#include <GL/glu.h>

#include "../Core/CoreDefine.h"
#include <common/CoordT.hpp>
#include <vector>
#include "../Object/AdapterObjectParent.h"

namespace acamcad
{
	class BaseObject;
	//class BaseObjectParent;

	class AdapterObject;
	class AdapterObjectParent;

	class ObjectDraw : public AdapterObjectParent
	{
	public:
		ObjectDraw();
		ObjectDraw(AdapterObject* parent);
		virtual ~ObjectDraw();

	public:
		virtual void draw(const DrawModel& dmodel, const SelectModel& smodel) const = 0;

		virtual void drawSelected(const SelectModel& smodel, int d_id) const {};

		virtual void clear() {};

	public:
		virtual void setCameraInfo(const AMCAX::Coord3& c_begin, const AMCAX::Coord3& c_dir)
		{
			camera_begin = c_begin;
			camera_dir = c_dir;
		}
	protected:
		AMCAX::Coord3 camera_begin;
		AMCAX::Coord3 camera_dir;

	public:
		//virtual void drawForSelect() const = 0;
		//virtual void drawForSelectPart() const {};

		virtual void drawWithObjectID() const {};
		virtual void drawWithFaceID() const {};
		virtual void drawWithEdgeID() const {};
		virtual void drawWithVertexID() const {};

	public:
		virtual void updatedrawState() = 0;
		virtual void updatedrawStateFast(bool is_topology_change = true) {}

	public:
		virtual void clearCurvature() {}
		virtual void updatedrawZebra() {}
		virtual void updatedrawCurvatureAnalysis(int type) {}
		virtual void updatedrawCurvatureGraph(std::vector<int> face_index) {}

		//AdapterObject* getParent();

	protected:
		void colorDefault(bool is_select) const;
		void materialSolidFlatDefault(bool is_select) const;
		void materialSmoothDefault(bool is_select) const;

		//protected:
		//	AdapterObject* parent_;

	};


} //namespace acamcad