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

#include "../Object/BaseObject.h"
#include "../ObjectDraw/ObjectDraw.h"

#include "QGLViewer/camera.h"
#include <common/CoordT.hpp>

namespace acamcad
{
	class DataManager;

	class SelectTool
	{
	public:
		SelectTool();
		~SelectTool() {};

	public:
		bool select(qglviewer::Camera* camera, const SelectModel& s_model, const AMCAX::CoordT<int, 2>& point, bool is_select_muti);
		bool selectFrame(qglviewer::Camera* camera, const SelectModel& s_model, const AMCAX::CoordT<int, 2>& point, const PyramidInf& frustum);

	private:
		void beginSelection(const AMCAX::CoordT<int, 2>& point);
		void endSelection(const AMCAX::CoordT<int, 2>& point, const SelectModel& type);
		bool selectedDrawNeeded(const AMCAX::CoordT<int, 2>& point, bool is_select_muti);

		void selectDrawObject();
		//void selectDrawFace(const BaseObject* obj);

	private:
		void beginSelectionFrame(const AMCAX::CoordT<int, 2>& point);
		void endSelectionFrame(const AMCAX::CoordT<int, 2>& point, SelectModel type);
		bool selectedDrawNeededFrame(const AMCAX::CoordT<int, 2>& point, const PyramidInf& frustum);

	public:
		std::vector<SelectInfoWithCamera> getSelectInfoNew();
		void clearSelected();

		AMCAX::Coord3 getCenter();
	private:
		std::vector<SelectInfo> select_info_list_;
		std::vector<SelectInfoWithCamera> select_infoCamera_list_;

		std::vector<AMCAX::Coord3> select_center_list_;

	public:
		GLuint* selectBuffer() { return selectBuffer_; }
		/*! Default value is 400, 100 for 1 object with 4 size. */
		void setSelectBufferSize(int size);
		int selectBufferSize() const { return selectBufferSize_; }

		/*! Default value is 3 pixels. */
		int selectRegionWidth() const { return selectRegionWidth_; }
		/*! Default value is 3 pixels. */
		int selectRegionHeight() const { return selectRegionHeight_; }
		void setSelectRegionWidth(int width) { selectRegionWidth_ = width; }
		void setSelectRegionHeight(int height) { selectRegionHeight_ = height; }

		void setSelectRegionWidthDefault() { selectRegionWidth_ = 3; }
		void setSelectRegionHeightDefault() { selectRegionHeight_ = 3; }
	private:
		int selectRegionWidth_, selectRegionHeight_;
		GLuint* selectBuffer_;
		int selectBufferSize_;
		int selectedId_;
		int selectedSubsetId_;

		std::vector<int> selectedFrame_id_;

	private:
		qglviewer::Camera* camera_;
		void setSelectInfoCamera(const AMCAX::CoordT<int, 2>& pixel, acamcad::SelectInfoWithCamera& s_info);

	private:
		void selectedObject_Element(SelectInfoWithCamera& select_info, const SelectModel& type_);
		void selectedObject_Element_Frame(std::vector<SelectInfo>& select_info_list, const SelectModel& type_, const PyramidInf& frustum);

		void selectedObject_Center(SelectInfoWithCamera& select_info, const SelectModel& type_, AMCAX::Coord3& center);
		void selectedObject_Center(SelectInfo& select_info, const SelectModel& type_, AMCAX::Coord3& center);

	private:
		SelectModel select_model_;

	public:
		void setDataManger(DataManager* dataManager);
	private:
		DataManager* dataManager_;

	};



}