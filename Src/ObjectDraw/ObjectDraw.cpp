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
#include "ObjectDraw.h"
#include "../Object/AdapterObject.h"

#include <Windows.h>
#include <GL/glu.h>

acamcad::ObjectDraw::ObjectDraw() :AdapterObjectParent(NULL)
{
}

acamcad::ObjectDraw::ObjectDraw(AdapterObject* object) : AdapterObjectParent(object)
{
}

acamcad::ObjectDraw::~ObjectDraw()
{
}

void acamcad::ObjectDraw::colorDefault(bool is_selected) const
{
	if (is_selected)
		glColor3f(0.85, 0.85, 0);
	else
		glColor3f(0.1, 0.1, 0.1);
}

void acamcad::ObjectDraw::materialSolidFlatDefault(bool is_selected) const
{
	glDisable(GL_COLOR_MATERIAL);   //有光照的情况下不使用颜色进行绘制

	if (is_selected)
	{
		GLfloat mat_a[] = { 0.2f, 0.2f, 0.2f, 1.0f };
		GLfloat mat_d[] = { 0.8f, 0.8f, 0.0f, 1.0f };
		GLfloat mat_s[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		GLfloat shine[] = { 120.0f };

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_a);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_d);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_s);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shine);
	}
	else
	{
		GLfloat mat_a[] = { 0.2f, 0.2f, 0.2f, 1.0f };
		GLfloat mat_d[] = { 0.66f, 0.66f, 0.66f, 1.0f };
		GLfloat mat_s[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		GLfloat shine[] = { 120.0f };

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_a);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_d);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_s);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shine);
	}
}

void acamcad::ObjectDraw::materialSmoothDefault(bool is_selected) const
{
	if (is_selected)
	{
		GLfloat mat_a[] = { 0.2f, 0.2f, 0.2f, 1.0f };
		GLfloat mat_d[] = { 0.8f, 0.8f, 0.0f, 1.0f };
		GLfloat mat_s[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		GLfloat shine[] = { 120.0f };

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_a);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_d);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_s);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shine);
	}
	else
	{
		GLfloat mat_a[] = { 0.2f, 0.2f, 0.2f, 1.0f };
		GLfloat mat_d[] = { 0.66f, 0.66f, 0.66f, 1.0f };
		GLfloat mat_s[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		GLfloat shine[] = { 120.0f };

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_a);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_d);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_s);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shine);
	}
}

