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
#include "RenderViewer.h"
#include "../Core/DataManager.h"
#include "../ObjectDraw/ObjectDraw.h"
#include "../Utils/MathUtils.h"
#include "GL/freeglut.h"

// VIEW_LEN > AXES_LEN + ARROW_LEN
#define VIEW_LEN 180
#define AXES_LEN  120
#define ARROW_LEN  30
#define ARROW_RADIUS  18

//===================================================================
// This File is All Draw With renderViewer
//===================================================================

using namespace acamcad;
using namespace qglviewer;
using namespace acamcad;

void RenderViewer::drawWorkPlaneGrid(double size, int nbSubdivisions)
{
	AMCAX::Coord3& center = work_plane_->center();
	AMCAX::Coord3& d = work_plane_->normal();

	glPushMatrix();
	glTranslated(center[0], center[1], center[2]);
	glMultMatrixd(Quaternion(AMCAX::Coord3(0.0, 0.0, 1.0), d).matrix());
	RenderViewer::drawGrid(size, nbSubdivisions);
	glPopMatrix();
}

void RenderViewer::drawGrid(double size, int nbSubdivisions)
{
	glLineWidth(1.0);
	glColor3f(0.8, 0.8, 0.8);
	GLboolean lighting;
	glGetBooleanv(GL_LIGHTING, &lighting);
	glDisable(GL_LIGHTING);

	glBegin(GL_LINES);
	for (int i = 0; i <= nbSubdivisions; ++i) {
		const qreal pos = size * (2.0 * i / nbSubdivisions - 1.0);
		glVertex2d(pos, -size);
		glVertex2d(pos, +size);
		glVertex2d(-size, pos);
		glVertex2d(size, pos);
	}
	glEnd();

	if (lighting)
		glEnable(GL_LIGHTING);
}

void RenderViewer::drawWorldAxis()
{
	glLineWidth(3.0);

	double length = 5.0;

	//GLboolean lighting, colorMaterial;
	////glGetBooleanv(GL_LIGHTING, &lighting);
	//glGetBooleanv(GL_COLOR_MATERIAL, &colorMaterial);

	//glDisable(GL_COLOR_MATERIAL);

	//X R
	float color[4];
	color[0] = 1.0f; color[1] = 0.1f; color[2] = 0.1f; color[3] = 1.0f;
	glColor3f(1.0f, 0.1f, 0.1f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);

	glBegin(GL_LINES);
	glVertex3d(0, 0, 0);
	glVertex3d(length, 0, 0);
	glEnd();

	//Y G
	color[0] = 0.1f; color[1] = 1.0f; color[2] = 0.1f; color[3] = 1.0f;
	glColor3f(0.1f, 1.0f, 0.1f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);

	glBegin(GL_LINES);
	glVertex3d(0, 0, 0);
	glVertex3d(0, length, 0);
	glEnd();

	//Z B
	color[0] = 0.1f; color[1] = 0.1f; color[2] = 1.0f; color[3] = 1.0f;
	glColor3f(0.1f, 0.1f, 1.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);

	glBegin(GL_LINES);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, length);
	glEnd();

	//if (colorMaterial)
	//	glEnable(GL_COLOR_MATERIAL);
	//if (!lighting)
	//	glDisable(GL_LIGHTING);

}

void RenderViewer::drawAxis()
{

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLboolean lighting;
	glGetBooleanv(GL_LIGHTING, &lighting);

	if (lighting)
		glDisable(GL_LIGHTING);

	glViewport(0, 0, VIEW_LEN, VIEW_LEN);

	glClear(GL_DEPTH_BUFFER_BIT);

	glOrtho(-VIEW_LEN, VIEW_LEN, -VIEW_LEN, VIEW_LEN, -VIEW_LEN, VIEW_LEN);
	//gluPerspective(45.0, 1.0f, 0.1f, 500.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	auto vec = camera()->upVector();
	auto coord = camera()->viewDirection();

	auto point0 = -coord;
	point0.Normalize();

	gluLookAt(point0.X(), point0.Y(), point0.Z(),
		0, 0, 0,
		vec.X(), vec.Y(), vec.Z());

	//确定坐标系原点
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 0.0f);
	gluSphere(glu_cylinder_, 12, 12, 12);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.1f, 0.1f, 1.0f);
	//glRotatef(90, 0.0, 0.0, 1.0);
	//glutSolidSphere(0.25, 6, 6);
	gluCylinder(glu_cylinder_, 6, 6, AXES_LEN, 6, 3);            //z
	glTranslatef(0, 0, AXES_LEN);
	gluCylinder(glu_cylinder_, ARROW_RADIUS, 0, ARROW_LEN, 6, 3);  //z arrow
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.0f, 1.0f, 0.0f);
	glRotatef(90, -1.0, 0.0, 0.0);
	gluCylinder(glu_cylinder_, 6, 6, AXES_LEN, 6, 3);             //Y
	glTranslatef(0, 0, AXES_LEN);
	gluCylinder(glu_cylinder_, ARROW_RADIUS, 0, ARROW_LEN, 6, 3);   //Y arrow
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 0.1f, 0.1f);
	glRotatef(90, 0.0, 1.0, 0.0);
	gluCylinder(glu_cylinder_, 6, 6, AXES_LEN, 6, 3);              //X
	glTranslatef(0, 0, AXES_LEN);
	gluCylinder(glu_cylinder_, ARROW_RADIUS, 0, ARROW_LEN, 6, 3);    //X arrow
	glPopMatrix();

	//SwapBuffers()
	glRasterPos3f(150, -12, 0);
	//glColor3f(1.0f, 0.1f, 0.1f);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'x');

	glRasterPos3f(-12, 150, 0);
	//glColor3f(0.1f, 1.0f, 0.1f);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'y');

	glRasterPos3f(-12, -12, 150);
	//glColor3f(0.1f, 0.1f, 1.0f);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'z');

	//glFlush(); // remember to flush GL output!

	if (lighting)
		glEnable(GL_LIGHTING);

}

//====================================================================

void RenderViewer::drawSelectedElement()
{
	if (select_info_list_.empty())
		return;

	for (int i = 0; i < select_info_list_.size(); i++)
	{
		const SelectInfoWithCamera& s_info = select_info_list_[i];
		BaseObject* object = dataManager_->getObjectByPersistentID(s_info.object_id_);

		const ObjectDraw* objectdraw = object->getDrawTool();
		objectdraw->drawSelected(select_model_, s_info.object_subselect_id_);
	}

	if (o_axis_ != nullptr)
	{
		std::string type;
		if (operate_type_ == OperationType::MOVE_OPERATION)
			type = "MOVE";
		else if (operate_type_ == OperationType::ROTATE_OPERATION)
			type = "ROTATE";
		else if (operate_type_ == OperationType::SCALE_OPERATION)
			type = "SCALE";
		else
			return;

		o_axis_->draw(type);
	}

}

//====================================================================

void RenderViewer::drawSelectMovePoint() const
{
	// 绘制具体的选择的点，用于显示选中后的移动，例如，选中边split，点在边上move选中一个好的位置
	if (operate_type_ == OperationType::PICK_OPERATION_MOVE || operate_type_ == OperationType::PICK_OPERATION_MOVE_MULTI)
	{
		glColor3f(0.85, 0.0, 0.0);
		glPointSize(8);

		for (int i = 0; i < operation_point_.size(); i++)
		{
			glBegin(GL_POINTS);
			glVertex3dv(operation_point_[i].Data());
			glEnd();
		}
	}
}

void RenderViewer::drawClickedElement() const
{
	glColor3f(0.15, 0.15, 0.15);
	glPointSize(5);

	for (int i = 0; i < click_info_list_.size(); i++)
	{
		const AMCAX::Coord3& p = click_info_list_[i].point_;
		glBegin(GL_POINTS);
		glVertex3dv(p.Data());
		glEnd();
	}

	if (click_info_list_.size() > 1)
	{
		glColor3f(0.15, 0.15, 0.15);
		glLineWidth(3);

		for (size_t i = 0; i < click_info_list_.size() - 1; i++)
		{
			const AMCAX::Coord3& p0 = click_info_list_[i].point_;
			const AMCAX::Coord3& p1 = click_info_list_[i + 1].point_;
			glBegin(GL_LINES);
			glVertex3dv(p0.Data());
			glVertex3dv(p1.Data());
			glEnd();
		}
	}
}

void RenderViewer::drawSelectionRectangle(QRect& rectangle_) const {
	startScreenCoordinatesSystem();
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);

	glLineWidth(2.0);
	glColor4f(0.66f, 0.66f, 0.66f, 0.66f);
	glBegin(GL_LINE_LOOP);
	glVertex2i(rectangle_.left(), rectangle_.top());
	glVertex2i(rectangle_.right(), rectangle_.top());
	glVertex2i(rectangle_.right(), rectangle_.bottom());
	glVertex2i(rectangle_.left(), rectangle_.bottom());
	glEnd();

	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	stopScreenCoordinatesSystem();
}


//===================================================================
