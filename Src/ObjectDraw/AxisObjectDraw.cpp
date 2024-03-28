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
#include "AxisObjectDraw.h"

#include <Windows.h>
#include <GL/glu.h>
#include "../Utils/MathUtils.h"
#include "../Utils/Quaternion.h"

using namespace acamcad;

AxisObjectDraw::AxisObjectDraw(BaseObject* object)
{
	axis_ = dynamic_cast<Axis*>(object);
}

AxisObjectDraw::~AxisObjectDraw()
{
	axis_ = nullptr;
}

void AxisObjectDraw::draw(const DrawModel& dmodel, const SelectModel& smodel) const
{
	draw(dmodel, false);
}

void AxisObjectDraw::draw(const DrawModel& dmodel, bool is_show_point) const
{
	if (!axis_->isVisible())
		return;

	GLboolean lighting, colorMaterial;
	glGetBooleanv(GL_LIGHTING, &lighting);
	glGetBooleanv(GL_COLOR_MATERIAL, &colorMaterial);

	glDisable(GL_COLOR_MATERIAL);

	float color[4];
	color[0] = 0.1f; color[1] = 0.1f; color[2] = 1.0f; color[3] = 1.0f;
	glColor3f(0.1f, 0.1f, 1.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
	drawArrow(axis_->axis_z());

	color[0] = 1.0f; color[1] = 0.1f; color[2] = 0.1f; color[3] = 1.0f;
	glColor3f(1.0f, 0.1f, 0.1f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
	drawArrow(axis_->axis_x());

	color[0] = 0.1f; color[1] = 1.0f; color[2] = 0.1f; color[3] = 1.0f;
	glColor3f(0.1f, 1.0f, 0.1f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
	drawArrow(axis_->axis_y());

	if (colorMaterial)
		glEnable(GL_COLOR_MATERIAL);
	if (!lighting)
		glDisable(GL_LIGHTING);
}



//void AxisObjectDraw::drawForSelect() const
//{
//
//}

void AxisObjectDraw::drawArrow(const Arrow& arrow) const
{
	AMCAX::Coord3 from(arrow.from());
	AMCAX::Coord3 dir(arrow.direction());
	AMCAX::Coord3 dir_len = dir * arrow.length();

	glPushMatrix();
	glTranslated(from[0], from[1], from[2]);
	glMultMatrixd(Quaternion(AMCAX::Coord3(0.0, 0.0, 1.0), dir).matrix());
	drawArrow(arrow.length(), arrow.radius());
	glPopMatrix();
}

void AxisObjectDraw::drawArrow(qreal length, qreal radius) const
{
	static GLUquadric* quadric = gluNewQuadric();

	radius = radius * 0.01;

	int nbSubdivisions = 10;
	const qreal head = 0.1;

	gluCylinder(quadric, radius, radius, length * (1.0 - head * 0.75), 10, 1);
	glTranslated(0.0, 0.0, length * (1.0 - head));
	gluCylinder(quadric, 3 * radius, 0.0, head * length, 10, 1);
	glTranslated(0.0, 0.0, -length * (1.0 - head));
}
