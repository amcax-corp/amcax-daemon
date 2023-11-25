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
#include "SelectTool.h"

#include "../Core/DataManager.h"
#include "../Operation/MOperate_Select.h"
#include "../Operation/MOperate_getCenter.h"

using namespace acamcad;

acamcad::SelectTool::SelectTool()
    :dataManager_(nullptr), selectBuffer_(nullptr)
{
    setSelectBufferSize(2000);
    setSelectRegionWidthDefault();
    setSelectRegionHeightDefault();
}

//===================================================================

bool SelectTool::select(qglviewer::Camera* camera, const SelectModel& s_model, const AMCAX::CoordT<int, 2>& point, bool is_select_muti)
{
    camera_ = camera;
    select_model_ = s_model;
    return selectedDrawNeeded(point, is_select_muti);
}

bool SelectTool::selectFrame(qglviewer::Camera* camera, const SelectModel& s_model, const AMCAX::CoordT<int, 2>& point, const PyramidInf & frustum)
{
    camera_ = camera;
    select_model_ = s_model;
    return selectedDrawNeededFrame(point, frustum);
}

//===================================================================

void SelectTool::beginSelection(const AMCAX::CoordT<int, 2>& point)
{
    // Prepare the selection mode
    glSelectBuffer(selectBufferSize(), selectBuffer());
    glRenderMode(GL_SELECT);
    glInitNames();

    // Loads the matrices
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    static GLint viewport[4];
    camera_->getViewport(viewport);
    gluPickMatrix(point.X(), point.Y(), selectRegionWidth(), selectRegionHeight(), viewport);

    // loadProjectionMatrix() first resets the GL_PROJECTION matrix with a glLoadIdentity()
    // The false parameter prevents this and hence multiplies the matrices.
    camera_->loadProjectionMatrix(false);
    // Reset the original (world coordinates) modelview matrix
    camera_->loadModelViewMatrix();
}

void SelectTool::endSelection(const AMCAX::CoordT<int, 2>& point, const SelectModel& type)
{
    Q_UNUSED(point)

    // Flush GL buffers
    glFlush();

    // Get the number of objects that were seen through the pick matrix frustum.
    // Reset GL_RENDER mode.
    GLint nbHits = glRenderMode(GL_RENDER);

    if (nbHits <= 0)
        selectedId_ = -1;
    else {
        // Interpret results: each object created 4 values in the selectBuffer().
        // selectBuffer[4*i+1] is the object minimum depth value, while
        // selectBuffer[4*i+3] is the id pushed on the stack. Of all the objects
        // that were projected in the pick region, we select the closest one (zMin comparison). 
        // This code needs to be modified if you use several stack levels. 

        if (type == SelectModel::OBJECT_MODEL)
        {
            GLuint zMin = (selectBuffer())[1];
            selectedId_ = int((selectBuffer())[3]);
            for (int i = 1; i < nbHits; ++i)
            {
                if ((selectBuffer())[4 * i + 1] < zMin)
                {
                    zMin = (selectBuffer())[4 * i + 1];
                    selectedId_ = int((selectBuffer())[4 * i + 3]);
                }
            }
        }
        else if (type == SelectModel::FACE_MODEL)
        {
            GLuint zMin = (selectBuffer())[1];
            selectedSubsetId_ = int((selectBuffer())[3]);
            for (int i = 1; i < nbHits; ++i)
            {
                if ((selectBuffer())[4 * i + 1] < zMin)
                {
                    zMin = (selectBuffer())[4 * i + 1];
                    selectedSubsetId_ = int((selectBuffer())[4 * i + 3]);
                }
            }
        }
        else
        {

            selectedId_ = -1;
            selectedSubsetId_ = -1;
        }
    }
}

bool SelectTool::selectedDrawNeeded(const AMCAX::CoordT<int, 2>& point, bool is_select_muti)
{
    beginSelection(point);
    selectDrawObject();
    endSelection(point, SelectModel::OBJECT_MODEL);

    BaseObject* object = dataManager_->getObjectByPersistentID(selectedId_);

    if (object == nullptr)
    {
        if(!is_select_muti)
            clearSelected();

        return false;
    }

    if (!is_select_muti)
    {
        clearSelected();
    }

    SelectInfoWithCamera s_info_camera;
    s_info_camera.object_id_ = selectedId_;
    s_info_camera.object_subpart_id_ = -1;

    AMCAX::Coord3 sc = AMCAX::Coord3(0.0, 0.0, 0.0);
    if (select_model_ == SelectModel::OBJECT_MODEL)
    {
        object->setSelect(true);
        selectedObject_Center(s_info_camera, select_model_, sc);
    }
    else if (select_model_ == SelectModel::FACE_MODEL ||
        select_model_ == SelectModel::VERTEX_MODEL || select_model_ == SelectModel::EDGE_MODEL)
    {
        object->setSelectSub(true);

        beginSelection(point);
        selectDrawFace(object);
        endSelection(point, SelectModel::FACE_MODEL);

        if (selectedSubsetId_ >= 0)
        {
            s_info_camera.object_subpart_id_ = selectedSubsetId_;
            setSelectInfoCamera(point, s_info_camera);
            selectedObject_Element(s_info_camera, select_model_);

            if (s_info_camera.object_subselect_id_ > -1) {
                selectedObject_Center(s_info_camera, select_model_, sc);
            }
            else{
                return false;
            }
        }
    }

    SelectInfo s_info(s_info_camera.object_id_, s_info_camera.object_subselect_id_);

    int fid = findSelect(select_model_, select_info_list_, s_info);
    if (fid > -1)
    {
        if (is_select_muti)
        {
            select_info_list_.erase(select_info_list_.begin() + fid);
            select_center_list_.erase(select_center_list_.begin() + fid);

            select_infoCamera_list_.erase(select_infoCamera_list_.begin() + fid);
            return false;
        }
    }
    else
    {
        select_info_list_.push_back(s_info);
        select_center_list_.push_back(sc);

        select_infoCamera_list_.push_back(s_info_camera);
        return true;
    }

    return false;
}

void SelectTool::selectDrawObject()
{
    for (ObjectIter o_it = dataManager_->objects_begin(); o_it != dataManager_->objects_end(); ++o_it)
    {
        BaseObject* object = *o_it;
        glPushName(object->persistentId());

        object->getDrawTool()->drawForSelect();

        glPopName();
    }
}

void SelectTool::selectDrawFace(const BaseObject* object)
{
    object->getDrawTool()->drawForSelectPart();
}

//===================================================================

void SelectTool::beginSelectionFrame(const AMCAX::CoordT<int, 2>& point)
{
    beginSelection(point);
}

void SelectTool::endSelectionFrame(const AMCAX::CoordT<int, 2>& point, SelectModel type)
{
    Q_UNUSED(point)

        // Flush GL buffers
        glFlush();

    // Get the number of objects that were seen through the pick matrix frustum.
    // Reset GL_RENDER mode.
    GLint nbHits = glRenderMode(GL_RENDER);

    selectedFrame_id_.clear();
    if (nbHits > 0)
    {
        // Same as endSelection
        for (int i = 0; i < nbHits; ++i)
        {
            int id = (selectBuffer())[4 * i + 3];
            selectedFrame_id_.push_back(id);
        }
    }
}

bool SelectTool::selectedDrawNeededFrame(const AMCAX::CoordT<int, 2>& point, const PyramidInf & frustum)
{
    beginSelection(point);
    selectDrawObject();
    endSelectionFrame(point, SelectModel::OBJECT_MODEL);

    std::vector<int> object_sid_list = selectedFrame_id_;
    std::vector< BaseObject* > select_object_list; select_object_list.clear();
    for (int i = 0; i < object_sid_list.size(); i++)
    {
        BaseObject* object = dataManager_->getObjectByPersistentID(object_sid_list[i]);
        if (object != nullptr)
        {
            if (select_model_ == SelectModel::OBJECT_MODEL)
                object->setSelect(true);
            else
                object->setSelectSub(true);
            select_object_list.push_back(object);
        }
    }

    if (select_object_list.empty())
    {
        clearSelected();
        return false;
    }

    AMCAX::Coord3d sc = AMCAX::Coord3d(0.0, 0.0, 0.0);
    if (select_model_ == SelectModel::OBJECT_MODEL)
    {
        for (int i = 0; i < select_object_list.size(); i++)
        {
            SelectInfoWithCamera s_info_camera;
            s_info_camera.object_id_ = selectedFrame_id_[i];
            s_info_camera.object_subpart_id_ = -1;
            SelectInfo s_info;
            s_info.object_id_ = selectedFrame_id_[i];
            s_info.object_subselect_id_ = -1;

            selectedObject_Center(s_info_camera, select_model_, sc);

            select_center_list_.push_back(sc);
            select_info_list_.push_back(s_info);
            select_infoCamera_list_.push_back(s_info_camera);
        }
    }
    else if (select_model_ == SelectModel::FACE_MODEL ||
        select_model_ == SelectModel::VERTEX_MODEL || select_model_ == SelectModel::EDGE_MODEL)
    {
        SelectInfo s_info;
        std::vector<SelectInfo> select_info_object_;
        for (int i = 0; i < select_object_list.size(); i++)
        {
            beginSelection(point);
            selectDrawFace(select_object_list[i]);
            endSelectionFrame(point, SelectModel::FACE_MODEL);

            s_info.object_id_ = object_sid_list[i];
            select_info_object_.clear();
            if (!selectedFrame_id_.empty())
            {
                for (int j = 0; j < selectedFrame_id_.size(); j++)
                {
                    s_info.object_subselect_id_ = selectedFrame_id_[j];
                    select_info_object_.push_back(s_info);
                }

                selectedObject_Element_Frame(select_info_object_, select_model_, frustum);

                for (int j = 0; j < select_info_object_.size(); j++)
                {
                    SelectInfo& s_info = select_info_object_[j];
                    selectedObject_Center(s_info, select_model_, sc);

                    SelectInfoWithCamera s_info_camera;
                    s_info_camera.object_id_ = s_info.object_id_;
                    s_info_camera.object_subselect_id_ = s_info.object_subselect_id_;

                    select_center_list_.push_back(sc);
                    select_info_list_.push_back(s_info);
                    select_infoCamera_list_.push_back(s_info_camera);
                }
            }
        }
    }

    if (!select_info_list_.empty())
    {
        return true;
    }
    else
    {
        return false;
    }
}

//================================================================

std::vector<SelectInfoWithCamera> acamcad::SelectTool::getSelectInfoNew()
{
    return select_infoCamera_list_;
}

AMCAX::Coord3 acamcad::SelectTool::getCenter()
{
    AMCAX::Coord3 c(0.0, 0.0, 0.0);
    for (int i = 0; i < select_center_list_.size(); i++)
    {
        c += select_center_list_[i];
    }
    c = c / select_center_list_.size();
    return c;
}

void SelectTool::clearSelected()
{
    dataManager_->clearSelected();

    select_info_list_.clear();
    select_infoCamera_list_.clear();
    select_center_list_.clear();
}

void SelectTool::setSelectInfoCamera(const AMCAX::CoordT<int, 2>& pixel, acamcad::SelectInfoWithCamera& s_info)
{
    QPoint p(pixel.X(), pixel.Y());

    AMCAX::Coord3 view_ori_, view_dir_;
    camera_->convertClickToLine(p, view_ori_, view_dir_);
    s_info.c_begin_ = view_ori_;
    s_info.c_end_ = view_ori_ + 100.f * view_dir_;
}

//================================================================

void SelectTool::selectedObject_Element(SelectInfoWithCamera& select_info, const SelectModel& type_)
{
    BaseObject* object = dataManager_->getObjectByPersistentID(select_info.object_id_);

    MOperation_Select* mo_select = new MOperation_Select(type_);
    mo_select->setOperateId(select_info);
    object->doOperate(mo_select);
    select_info = mo_select->getSelectInfoNew();
    delete mo_select;
}

void SelectTool::selectedObject_Element_Frame(std::vector<SelectInfo>& select_info_list, const SelectModel& type_, const PyramidInf & frustum)
{
    if (!isSelectOneObject(select_info_list))
    {
        std::cout << "can not Fast select different object" << std::endl;
        return;
    }

}

void SelectTool::selectedObject_Center(SelectInfoWithCamera& select_info, const SelectModel& type_, AMCAX::Coord3 & center)
{
    BaseObject* object = dataManager_->getObjectByPersistentID(select_info.object_id_);

    MOperation_getCenter* mo_select = new MOperation_getCenter(type_);
    mo_select->setSelectInfoCamera(select_info);
    object->doOperate(mo_select);
    center = mo_select->getCenter();
    delete mo_select;
}
void SelectTool::selectedObject_Center(SelectInfo& select_info, const SelectModel& type_, AMCAX::Coord3 & center)
{
    BaseObject* object = dataManager_->getObjectByPersistentID(select_info.object_id_);

    MOperation_getCenter* mo_select = new MOperation_getCenter(type_);
    mo_select->setOperateId(select_info);
    object->doOperate(mo_select);
    center = mo_select->getCenter();
    delete mo_select;
}

//================================================================

void SelectTool::setSelectBufferSize(int size) 
{
    if (selectBuffer_)
        delete[] selectBuffer_;
    selectBufferSize_ = size;
    selectBuffer_ = new GLuint[selectBufferSize()];
}

//================================================================

void acamcad::SelectTool::setDataManger(DataManager* dataManager)
{
    dataManager_ = dataManager;
}