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
#include "operatetoolbar.h"

#include <QDebug>
#include <chrono>
#include <ctime>

#include <algorithm>

#include "RenderViewer.h"
#include "../Core/ACAMCore.h"

#include "RenderViewer.h"

using namespace acamcad;

// action template: 
// {"", QObject::tr(""), "", false, &OperateToolBar::},
// {"Title", "tooltip", icon, checkable, slot}
const QVector<ToolBarTabFillOut> OperateToolBar::tabs{
    { // Base tab
        "Base", tr("View"), {
            {"WireFrame", QObject::tr("Show in wirefram model"), "", &RenderViewer::slotRenderMode_Wire},
            {"Render", QObject::tr("Show in Shading model"), "", &RenderViewer::slotRenderMode_Rendering},
            {"Smooth", QObject::tr("Show in Smooth model"), "", &RenderViewer::slotRenderMode_Shading},
            {""}, // empty for separator
            {"SetCenter", QObject::tr("Set secnce center in select object center"), ":images/images/SetCenter.png", &RenderViewer::slotSetScenesCenter},
            {""}, // empty for separator
            {"Vert", QObject::tr("Select Vert model"), ":images/images/SelectVert.png", &RenderViewer::slotSelectMode_Vert, true, SelectModel::VERTEX_MODEL},
            {"Edge", QObject::tr("Select Edge model"), ":images/images/SelectEdge.png", &RenderViewer::slotSelectMode_Edge, true, SelectModel::EDGE_MODEL},
            {"Face", QObject::tr("Select Face model"), ":images/images/SelectFace.png", &RenderViewer::slotSelectMode_Face, true, SelectModel::FACE_MODEL},
            {"Object", QObject::tr("Select Object model"), ":images/images/SelectObject.png", &RenderViewer::slotSelectMode_Object, true, SelectModel::OBJECT_MODEL},
       
            {""}, // empty for separator
            {"Copy Object", QObject::tr("Copy an Object"), ":images/images/copy.png", &RenderViewer::slotCopyObject},
            {"Combine Object", QObject::tr("Combine more than one Object to a one Object, final get only one object"), ":images/images/Combine.png", &RenderViewer::slotCombineObject},
    }},


    { // BRep tab
        "BRep", tr("BRep"), {
            {"Create Single Face", QObject::tr("Create a mesh has a single face, click the left mouse button in turn, enter sure"), ":images/images/SingalMeshFace.png", &RenderViewer::slotCreateSingleFaceBRep},
            {"Create a BRep Plane", QObject::tr("Create a plane brep"), ":images/images/Plane.png", &RenderViewer::slotCreatPlaneBRep},
            {"Create a BRep Polygon", QObject::tr("Create a polygon brep"), ":images/images/Polygon.png", &RenderViewer::slotCreatePolygonBRep},
            {""},
            {"Create a BRep Cube", QObject::tr("Create a cube brep"), ":images/images/Cube.png", &RenderViewer::slotCreatCubeBRep},
            {"Create a BRep Sphere", QObject::tr("Create a sphere brep"), ":images/images/UCSphere.png", &RenderViewer::slotCreateSphereBRep},
            {"Create a BRep Cylinder", QObject::tr("Create a cylinder brep"), ":images/images/Cylinder.png", &RenderViewer::slotCreateCylinderBRep},
            {"Create a BRep Cone", QObject::tr("Create a cone brep"), ":images/images/Cone.png", &RenderViewer::slotCreateConeBrep},
            {"Create a BRep Torus", QObject::tr("Create a torus brep"), ":images/images/Torus.png", &RenderViewer::slotCreateTorusBRep},
            {"Create a BRep Prism", QObject::tr("Create a prism brep"), ":images/images/Prism.png", &RenderViewer::slotCreatePrismBRep},
    }},

};

OperateToolBar::OperateToolBar(RenderViewer *viewer, QWidget *parent): 
        QTabWidget(parent)
{
    setMovable(true);
    setFocusPolicy(Qt::NoFocus);
    
    std::for_each(tabs.begin(), tabs.end(), [&](const ToolBarTabFillOut& tab) {
        auto *toolBar = new QToolBar(tab.title);
        toolBars.append(toolBar);
        addTab(toolBar, tab.name);
        std::for_each(tab.actions.begin(), tab.actions.end(), [&](const ToolBarTabAction &ta) {
            auto *action = ta.toQAction();
            if (action) {
                if (ta.actionSlot) {
                    connect(action, &QAction::triggered, viewer, ta.actionSlot);
                }
                if (ta.checkable) {
                    viewer->addSelector(ta.selectModel, action);
                }
                toolBar->addAction(action);
            } else {
                toolBar->addSeparator();
            }
        });
    });
}
