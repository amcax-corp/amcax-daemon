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
#include "AxisOperateDraw.h"

#include <Windows.h>
#include <GL/glu.h>
#include "../Utils/MathUtils.h"
#include "../Utils/Quaternion.h"

using namespace acamcad;

void AxisOperateDraw::drawOAxisArrow(const AMCAX::Coord3 & center, const AMCAX::Coord3 & dir, double off_len)
{
    AMCAX::Coord3 dirz(0.0, 0.0, 1.0);
    AMCAX::Coord3 dir_(dir[0], dir[1], dir[2]);
    static GLUquadric* quadric = gluNewQuadric();

    glPushMatrix();
    glTranslated(center[0], center[1], center[2]);
    glMultMatrixd(Quaternion(dirz, dir_).matrix());
    glTranslated(0.0, 0.0, off_len);
    gluCylinder(quadric, 0.04, 0.0, 0.2, 10, 1);
    glTranslated(0.0, 0.0, -off_len);
    glPopMatrix();
}

void AxisOperateDraw::drawOAxisSphere(const AMCAX::Coord3 & center, const AMCAX::Coord3 & dir, double off_len)
{
    AMCAX::Coord3 dirz(0.0, 0.0, 1.0);
    AMCAX::Coord3 dir_(dir[0], dir[1], dir[2]);
    static GLUquadric* quadric = gluNewQuadric();

    glPushMatrix();
    glTranslated(center[0], center[1], center[2]);
    glMultMatrixd(Quaternion(dirz, dir_).matrix());
    glTranslated(0.0, 0.0, off_len);
    gluSphere(quadric, 0.04, 4, 3);
    glTranslated(0.0, 0.0, -off_len);
    glPopMatrix();
}

void AxisOperateDraw::drawCycleRing(const std::vector<AMCAX::Coord3>& move_plane)
{
    for (int i = 0; i < 10; i++)
    {
        glBegin(GL_POLYGON);
        glVertex3dv(move_plane[i].Data());
        glVertex3dv(move_plane[i + 11].Data());
        glVertex3dv(move_plane[i + 12].Data());
        glVertex3dv(move_plane[i + 1].Data());
        glEnd();
    }
}

void AxisOperateDraw::drawOperateAxisPart(const AxisOperation* axis, const std::string& type, int part)
{
    std::vector<AMCAX::Coord3> move_plane(4);

    switch (part)
    {
    case 0:
        if (type == "MOVE" || type == "SCALE" || type == "ROTATE")
        {
            drawOAxisSphere(axis->center(), AMCAX::Coord3(0.0, 0.0, 0.0), 0.0);
        }
        break;
    case 1:
        if (type == "MOVE") {
            drawOAxisArrow(axis->center(), axis->dirX());
        }
        else if (type == "SCALE") {
            drawOAxisSphere(axis->center(), axis->dirX(), axis->lenX());
        }
        else if (type == "ROTATE")
        {
            axis->planeCyCleOffset(0, move_plane);
            drawCycleRing(move_plane);
        }
        break;
    case 2:
        if (type == "MOVE") {
            drawOAxisArrow(axis->center(), axis->dirY());
        }
        else if (type == "SCALE") {
            drawOAxisSphere(axis->center(), axis->dirY(), axis->lenY());
        }
        else if (type == "ROTATE")
        {
            axis->planeCyCleOffset(1, move_plane);
            drawCycleRing(move_plane);
        }
        break;
    case 3:
        if (type == "MOVE") {
            drawOAxisArrow(axis->center(), axis->dirZ());
        }
        else if (type == "SCALE") {
            drawOAxisSphere(axis->center(), axis->dirZ(), axis->lenZ());
        }
        else if (type == "ROTATE")
        {
            axis->planeCyCleOffset(2, move_plane);
            drawCycleRing(move_plane);
        }
        break;
    case 4:
        if (type == "MOVE") {
            axis->planeOffset(0, move_plane);
            glBegin(GL_POLYGON);
            glVertex3dv(move_plane[0].Data());
            glVertex3dv(move_plane[1].Data());
            glVertex3dv(move_plane[2].Data());
            glVertex3dv(move_plane[3].Data());
            glEnd();
        }
        else if (type == "SCALE") {
            axis->planeScaleOffset(0, move_plane);
            glBegin(GL_POLYGON);
            glVertex3dv(move_plane[0].Data());
            glVertex3dv(move_plane[1].Data());
            glVertex3dv(move_plane[2].Data());
            glVertex3dv(move_plane[3].Data());
            glEnd();
        }
        break;
    case 5:
        if (type == "MOVE") {
            axis->planeOffset(1, move_plane);
            glBegin(GL_POLYGON);
            glVertex3dv(move_plane[0].Data());
            glVertex3dv(move_plane[1].Data());
            glVertex3dv(move_plane[2].Data());
            glVertex3dv(move_plane[3].Data());
            glEnd();
        }
        else if (type == "SCALE") {
            axis->planeScaleOffset(1, move_plane);
            glBegin(GL_POLYGON);
            glVertex3dv(move_plane[0].Data());
            glVertex3dv(move_plane[1].Data());
            glVertex3dv(move_plane[2].Data());
            glVertex3dv(move_plane[3].Data());
            glEnd();
        }
        break;
    case 6:
        if (type == "MOVE") {
            axis->planeOffset(2, move_plane);
            glBegin(GL_POLYGON);
            glVertex3dv(move_plane[0].Data());
            glVertex3dv(move_plane[1].Data());
            glVertex3dv(move_plane[2].Data());
            glVertex3dv(move_plane[3].Data());
            glEnd();
        }
        else if (type == "SCALE") {
            axis->planeScaleOffset(2, move_plane);
            glBegin(GL_POLYGON);
            glVertex3dv(move_plane[0].Data());
            glVertex3dv(move_plane[1].Data());
            glVertex3dv(move_plane[2].Data());
            glVertex3dv(move_plane[3].Data());
            glEnd();
        }
        break;
    default:
        break;
    }

}

void AxisOperateDraw::drawOperateAxis(const AxisOperation* axis, const std::string& type)
{
    GLboolean lighting, colorMaterial, depth;
    glGetBooleanv(GL_LIGHTING, &lighting);
    glGetBooleanv(GL_COLOR_MATERIAL, &colorMaterial);
    glGetBooleanv(GL_DEPTH_TEST, &depth);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_DEPTH_TEST);

    bool a = (type == "ROTATE");

    {
        glLineWidth(3);
        glColor3f(1.0f, 0.1f, 0.1f);
        glBegin(GL_LINES);
        glVertex3dv(axis->centerOffset(0).Data());
        glVertex3dv(axis->centerFinal(0).Data());
        glEnd();

        glColor3f(0.1f, 1.0f, 0.1f);
        glBegin(GL_LINES);
        glVertex3dv(axis->centerOffset(1).Data());
        glVertex3dv(axis->centerFinal(1).Data());
        glEnd();

        glColor3f(0.1f, 0.1f, 1.0f);
        glBegin(GL_LINES);
        glVertex3dv(axis->centerOffset(2).Data());
        glVertex3dv(axis->centerFinal(2).Data());
        glEnd();
    }

    //画中间的框，只再选择的模式有用
    glColor3f(0.1f, 0.1f, 0.1f);
    drawOperateAxisPart(axis, type, 0);

    glColor3f(1.0f, 0.1f, 0.1f);
    drawOperateAxisPart(axis, type, 1);
    drawOperateAxisPart(axis, type, 6);

    glColor3f(0.1f, 1.0f, 0.1f);
    drawOperateAxisPart(axis, type, 2);
    drawOperateAxisPart(axis, type, 5);

    glColor3f(0.1f, 0.1f, 1.0f);
    drawOperateAxisPart(axis, type, 3);
    drawOperateAxisPart(axis, type, 4);

    if (colorMaterial)
        glEnable(GL_COLOR_MATERIAL);
    if (!lighting)
        glDisable(GL_LIGHTING);
    if (depth)
        glEnable(GL_DEPTH_TEST);
}

void AxisOperateDraw::drawOperateAxisInSelect(const AxisOperation* axis, const std::string& type)
{
    glPushName(0);
    drawOperateAxisPart(axis, type, 0);
    glPopName();

    glPushName(1);
    drawOperateAxisPart(axis, type, 1);
    glPopName();

    glPushName(6);
    drawOperateAxisPart(axis, type, 6);
    glPopName();

    glPushName(2);
    drawOperateAxisPart(axis, type, 2);
    glPopName();

    glPushName(5);
    drawOperateAxisPart(axis, type, 5);
    glPopName();

    glPushName(3);
    drawOperateAxisPart(axis, type, 3);
    glPopName();

    glPushName(4);
    drawOperateAxisPart(axis, type, 4);
    glPopName();
}
