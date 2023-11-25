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
#include "../Core/ACAMCore.h"

using namespace acamcad;

// ==================== BASE TAB ====================
void RenderViewer::slotRenderMode_Wire() {
    setDrawModel(DrawModel::WIRE_MODE);
    update();
}
void RenderViewer::slotRenderMode_Rendering() {
    setDrawModel(DrawModel::RENDERING_MODEL);
    update();
}
void RenderViewer::slotRenderMode_Shading() {
    setDrawModel(DrawModel::SHADING_MODEL);
    update();
}


void RenderViewer::slotSetScenesCenter() {
    setScenesCenter();
    update();
}

void RenderViewer::slotSelectMode_Vert() {
    setSelectModel(SelectModel::VERTEX_MODEL);
    clearSelected();
    update();

    selectCheckChanged();
}
void RenderViewer::slotSelectMode_Edge() {
    setSelectModel(SelectModel::EDGE_MODEL);
    clearSelected();
    update();

    selectCheckChanged();
}
void RenderViewer::slotSelectMode_Face() {
    setSelectModel(SelectModel::FACE_MODEL);
    clearSelected();
    update();

    selectCheckChanged();
}
void RenderViewer::slotSelectMode_Object() {
    setSelectModel(SelectModel::OBJECT_MODEL);
    cancelOperation(); // cancel
    update();

    selectCheckChanged();
}


void RenderViewer::slotCopyObject() {
    BaseObjectOperation(SelectModel::OBJECT_MODEL, OperationType::ENTER_OPERATION, 
            MeshOperationType::MeshCopy);
}

void RenderViewer::slotCombineObject() {
    BaseObjectOperation(SelectModel::OBJECT_MODEL, OperationType::ENTER_OPERATION, 
            MeshOperationType::MeshCombine);
}


