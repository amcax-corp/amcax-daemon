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
#include "ObjectDraw.h"
#include "../Object/AxisObject.h"

namespace acamcad
{
	class Axis;

	class AxisObjectDraw : public ObjectDraw
	{
	public:
		AxisObjectDraw(BaseObject* object);
		virtual ~AxisObjectDraw();

	public:
		virtual void draw(const DrawModel& dmodel, const SelectModel& smodel) const override;

	public:
		///virtual void drawForSelect() const override;

	public:
		virtual void updatedrawState() {};

	private:
		void draw(const DrawModel& dmodel, bool is_show_point) const;

	private:
		void drawArrow(const Arrow& arrow) const;
		void drawArrow(qreal length, qreal radius) const;

	private:
		Axis* axis_;
	};

}