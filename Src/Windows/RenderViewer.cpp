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
#include "QGLViewer/manipulatedCameraFrame.h"

#include <math.h>
#include <qmessagebox.h>
#include <QMouseEvent>

#include "../Core/ACAMCore.h"
#include "../Core/DataManager.h"
#include "SelectTool.h"
#include "../Utils/MathUtils.h"

using namespace acamcad;
using namespace qglviewer;

RenderViewer::RenderViewer(QWidget* parent, Qt::WindowFlags flags) :
    QGLViewer(parent, flags), selectBuffer_(nullptr), draw_model_(DrawModel::RENDERING_MODEL), 
    dataManager_(nullptr),coreCommand_(nullptr)
{
    work_plane_ = new PlaneObject();
    is_draw_grid_ = false;
    is_frame_selection_ = false;
    is_pick_move_select_ = false;

    o_axis_ = nullptr;

    setSelectBufferSize(2000);
    setSelectRegionWidthDefault();
    setSelectRegionHeightDefault();

    setSelectModel(SelectModel::OBJECT_MODEL);
    setOperateType(OperationType::NO_OPERATION);

    selectTool_ = new SelectTool();

    dialog = new ParamDialog(parent);
    // TODO: connect
    connect(dialog, &ParamDialog::finished, this, &RenderViewer::disconnectDialog);
    // Possibly a bug in Qt5 MSVC, which prevents the "new" style connect() from working.
    //connect(this, &RenderViewer::selectedModelChanged, this, &RenderViewer::selectCheckChanged);
    connect(this, SIGNAL(selectedModelChanged()), this, SLOT(selectCheckChanged()));
    
    initBRep();
}

// it will use in glInit;
void RenderViewer::init() 
{
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    glEnable(GL_LIGHT3);
    const GLfloat light_ambient[4] = { 0.4, 0.4, 0.4, 1.0 };
    const GLfloat light_specular[4] = { 1.0, 1.0, 1.0, 1.0 };
    const GLfloat light_diffuse[4] = { 1.0, 1.0, 1.0, 1.0 };

    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
    //glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
    //glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);

    glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient);
    //glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular);
    //glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse);

    glLightfv(GL_LIGHT3, GL_AMBIENT, light_ambient);
    //glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular);
    //glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse);

    //P1 = (-0.5, -sqrt(3)/6, -sqrt(6)/12);
    //P1 = (0.5, -sqrt(3)/6, -sqrt(6)/12);
    //P1 = (0, sqrt(3)/3, -sqrt(6)/12);
    //P1 = (0 0, sqrt(6)/4);

//    help();
}

void RenderViewer::draw() 
{
    qglviewer::Vec c = camera()->sceneCenter();
    double r = camera()->sceneRadius();
    float pos0[4] = { float(c.X()), float(c.Y()), float(c.Z() + 0.6*r), 1.0 };
    glLightfv(GL_LIGHT1, GL_POSITION, pos0);
    float pos1[4] = { float(c.X() - 0.5 * r), float(c.Y() - 0.3 * r), float(c.Z() - 0.2 * r), 1.0 };
    glLightfv(GL_LIGHT1, GL_POSITION, pos1);
    float pos2[4] = { float(c.X() + 0.5 * r), float(c.Y() - 0.3 * r), float(c.Z() - 0.2 * r), 1.0 };
    glLightfv(GL_LIGHT2, GL_POSITION, pos2);
    float pos3[4] = { float(c.X()), float(c.Y() + 0.6 * r), float(c.Z() -0.2 * r), 1.0 };
    glLightfv(GL_LIGHT2, GL_POSITION, pos3);

    QPoint pixel(width() / 2, height() / 2);
    camera()->convertClickToLine(pixel, view_ori_, view_dir_);
    for (ObjectIter o_it = dataManager_->objects_begin(); o_it != dataManager_->objects_end(); ++o_it)
    {
        BaseObject* object = *o_it;
        ObjectDraw* objectdraw = const_cast<ObjectDraw*>(object->getDrawTool());
        objectdraw->setCameraInfo(view_ori_, view_dir_);
        objectdraw->draw(draw_model_, select_model_);
    }

    drawSelectedElement();

    drawSelectMovePoint();

    if (select_model_ == SelectModel::CLICK_MODEL && !click_info_list_.empty())
    {
        drawClickedElement();
    }

    if (is_frame_selection_)
    {
        drawSelectionRectangle(frame_selection_rectangle_);
    }
}

/*! Called after draw() to draw viewer visual hints. */
void RenderViewer::postDraw() {
    // Reset model view matrix to world coordinates origin
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    camera()->loadModelViewMatrix();
    // TODO restore model loadProjectionMatrixStereo

    // Save OpenGL state
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    // Set neutral GL state
    glDisable(GL_TEXTURE_1D);
    glDisable(GL_TEXTURE_2D);
#ifdef GL_TEXTURE_3D // OpenGL 1.2 Only...
    glDisable(GL_TEXTURE_3D);
#endif

    glDisable(GL_TEXTURE_GEN_Q);
    glDisable(GL_TEXTURE_GEN_R);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);

#ifdef GL_RESCALE_NORMAL // OpenGL 1.2 Only...
    glEnable(GL_RESCALE_NORMAL);
#endif

    glDisable(GL_COLOR_MATERIAL);
    glColor4f(foregroundColor().redF(), foregroundColor().greenF(),
        foregroundColor().blueF(), foregroundColor().alphaF());

    if (is_draw_grid_)
    {
        drawWorkPlaneGrid();
    }

    drawWorldAxis();

    // Restore foregroundColor
    float color[4];
    color[0] = foregroundColor().red() / 255.0f;
    color[1] = foregroundColor().green() / 255.0f;
    color[2] = foregroundColor().blue() / 255.0f;
    color[3] = 1.0f;
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    // Restore GL state
    glPopAttrib();
    glPopMatrix();
}

QString RenderViewer::helpString() const {
    QString text("<h2>S e l e c t</h2>");
    text += " Liuhao test ";
    return text;
}

//==========================================================
//                Mouse And Key Event
//==========================================================

void RenderViewer::mousePressEvent(QMouseEvent* e)
{
    pressPos_ = e->pos(); prevPos_ = e->pos();
    click_buttom = e->button();

    if (draw_model_ == DrawModel::SHADING_MODEL && e->button() == Qt::LeftButton && select_model_ != SelectModel::OBJECT_MODEL)
        return;

    bool is_select = false;

    bool ctrl_left = (e->modifiers() == Qt::ControlModifier) && (e->button() == Qt::LeftButton);
    bool alt_left = (e->modifiers() == Qt::AltModifier) && (e->button() == Qt::LeftButton);
    if (ctrl_left && 
        (operate_type_ == OperationType::NO_OPERATION || operate_type_ == OperationType::CTRL_OPERATION))
    {      
        if (select_info_list_.empty())
        {
            select(e, false);
        }

        if (select_info_list_.size() > 0)
        {
            if (select_model_ == SelectModel::FACE_MODEL)
            {
                coreCommand_->setMeshOperationType(MeshOperationType::MeshExtrudeFace);
            }
            else if (select_model_ == SelectModel::EDGE_MODEL)
            {
                if (coreCommand_->getMeshOperationType() == MeshOperationType::NO_operotion)
                    coreCommand_->setMeshOperationType(MeshOperationType::MeshExtrudeEdgeHorizontal);
            }
            setOperateType(OperationType::CTRL_OPERATION);

            coreCommand_->OperateSelectedObject_Subset_Extrude(select_model_, select_info_list_, operation_vector_);
        }
        return;
    }

    if (alt_left &&
        (operate_type_ == OperationType::NO_OPERATION || operate_type_ == OperationType::ALT_OPERATION))
    {
        if (select_info_list_.empty())
        {
            select(e, false);

            if (select_info_list_.size() > 0)
            {

            }
        }

        return;
    }

    bool no_left = (e->modifiers() == Qt::NoModifier) && (e->button() == Qt::LeftButton);
    bool shift_left = (e->modifiers() == Qt::ShiftModifier) && (e->button() == Qt::LeftButton);
    switch (operate_type_)
    {
    case acamcad::OperationType::MOVE_OPERATION:    //process the move with axis
    case acamcad::OperationType::ROTATE_OPERATION:  //process the rotation with axis
    case acamcad::OperationType::SCALE_OPERATION:   //process the scale with axis
        if (no_left)
        {
            is_select = selectOpAxis(e);
            if (is_select)
            {
                coreCommand_->beginOperationByAxis(select_info_list_, select_model_);
            }
        }
        if (ctrl_left)          
            selectOpAxis(e);    //move the axis
        break;
    case acamcad::OperationType::NO_OPERATION:      //process the windows select operation
        if (no_left)
            is_select = select(e, false);
        else if (shift_left)
            is_select = select(e, true);

        //if not select any object, this is select_frame
        if (no_left && !is_select)
        {
            setFrameSelection(true);
            frame_selection_rectangle_ = QRect(e->pos(), e->pos());
        }

        coreCommand_->beginOperationByAxis(select_info_list_, select_model_);

        update();
        break;
    case acamcad::OperationType::ENTER_OPERATION:
        if (no_left || shift_left) 
            select(e, true);

        update();
        break;
    case acamcad::OperationType::CTRL_OPERATION:
        if (no_left || shift_left)
            select(e, true);

        update();
        break;
    case acamcad::OperationType::PICK_OPERATION_SINGLE:
        if (no_left)
        {
            select(e, false);
            beginPickOperationSingle();
        }
        break;
    case acamcad::OperationType::PICK_OPERATION_MOVE:
        if (no_left)
        {
            if (select(e, false))
            {
                is_pick_move_select_ = true;

                SelectInfoWithCamera s_info = select_info_list_[0];
                setSelectInfoCamera(e->pos(), s_info);
                operation_point_.resize(1);
            }
        }
        else if (e->button() == Qt::RightButton)    //取消之前的选择
        {
            is_pick_move_select_ = false;
            operation_point_.clear();
            cancelOperation();
            update();
        }
        break;
    case acamcad::OperationType::PICK_OPERATION_MOVE_MULTI:
        //TODO 
        break;
    case acamcad::OperationType::CLICK_OPERATION:
        if (no_left) {
            click(e);
        }
        else if (e->button() == Qt::RightButton){
            clearClicked();
        }
        break;
    case acamcad::OperationType::CLICK_OPERATION_SELECT:
        if (no_left)
        {
            setSelectModel(SelectModel::VERTEX_MODEL);
            if (select(e, true))
            {
                size_t s_end = select_info_list_.size() - 1;
                const AMCAX::Coord3& v = coreCommand_->getSelectedObject_Center(select_info_list_[s_end], select_model_);
                ClickInfo c_info; c_info.point_ = v;
                click_info_list_.push_back(c_info);
            }
            else
            {
                setSelectModel(SelectModel::CLICK_MODEL);
                click(e);
            }
            setSelectModel(SelectModel::CLICK_MODEL);
        }
        else if (e->button() == Qt::RightButton) {
            clearSelected();
            clearClicked();
        }
        std::cout << "select_info_list_.size()" << select_info_list_.size() << std::endl;
        break;
    default:
        break;
    }

    if(e->button() == Qt::RightButton)
        QGLViewer::mousePressEvent(e);
}

void RenderViewer::mouseMoveEvent(QMouseEvent* e)
{
    // std::cout << e->button() << std::endl;       这个始终是0
    // std::cout << e->modifiers() << std::endl;    这个会根据按键不同有不同的值

    if(click_buttom == Qt::LeftButton)
    {
        const QPoint e_point = e->pos();
        /* 处理挤出的鼠标移动 */
        if (operate_type_ == OperationType::CTRL_OPERATION && e->modifiers() == Qt::ControlModifier)
        {
            if (!select_info_list_.empty())
            {
                AMCAX::Coord3 oCenter = select_object_center_;
                AMCAX::Coord3 sp_tr = mouseMoveTrans(e_point, oCenter);
                double len = sp_tr.Norm(); 
                operation_vector_.Normalize(); sp_tr.Normalize();

                if (operation_vector_.Dot(sp_tr) < 0)
                    len = -len;
                AMCAX::Coord3 tr = len * operation_vector_;

                if (select_model_ == SelectModel::EDGE_MODEL || select_model_ == SelectModel::FACE_MODEL)
                {
                    double len = tr.Norm();
                    coreCommand_->moveSelectedObject(select_info_list_, select_model_, tr.Normalized(), len);

                    //if (select_info_list_.size() == 1)
                    //    coreCommand_->moveSelectedObject_Element(tr, select_info_list_[0], select_model_);
                    //else if (select_info_list_.size() > 1)
                    //    coreCommand_->moveSelectedObject_Element(tr, select_info_list_, select_model_);

                    update();
                }
            }
        }
        if (operate_type_ == OperationType::ALT_OPERATION && e->modifiers() == Qt::AltModifier)
        {
            if (!select_info_list_.empty())
            {
                AMCAX::Coord3d oCenter = select_object_center_;
                AMCAX::Coord3d sp_tr = mouseMoveTrans(e_point, oCenter);
                double len = sp_tr.Norm();
                operation_vector_ = AMCAX::Coord3d(0.0, 0.0, 1.0); sp_tr.Normalize();

                if (operation_vector_.Dot(sp_tr) < 0)
                    len = -len;

                if (select_model_ == SelectModel::FACE_MODEL)
                {
                    update();
                }
            }
        }
        /* 处理框选的鼠标移动 */
        else if (is_frame_selection_ && e->modifiers() == Qt::NoModifier)
        {
            frame_selection_rectangle_.setBottomRight(e->pos());
            update();
        }
        /* 处理点击后立刻移动物体 */
        else if (operate_type_ == OperationType::NO_OPERATION && e->modifiers() == Qt::NoModifier)
        {
            if (!select_info_list_.empty())
            {
                AMCAX::Coord3 oCenter = select_object_center_;
                AMCAX::Coord3 tr = mouseMoveTrans(e_point, oCenter);
                double len = tr.Norm();

                if (select_model_ == SelectModel::OBJECT_MODEL || select_model_ == SelectModel::VERTEX_MODEL
                    || select_model_ == SelectModel::EDGE_MODEL || select_model_ == SelectModel::FACE_MODEL)
                {
                    coreCommand_->moveSelectedObject(select_info_list_, select_model_, tr.Normalized(), len);
                }

                std::vector<int> select_object_id = getSelectObjectList(select_info_list_);
                for (int i : select_object_id)
                {
                    record.recordMove(i, tr);
                }

                update();
            }
        }
        /* 处理操作轴引起的移动 */
        else if (operate_type_ == OperationType::MOVE_OPERATION && e->modifiers() == Qt::NoModifier)
        {
            if (!select_info_list_.empty() && o_axis_ != nullptr && o_axis_->selectPart() > -1)
            {
                AMCAX::Coord3d oCenter = o_axis_->center();
                AMCAX::Coord3d sp_tr = mouseMoveTrans(e_point, oCenter);
                double len = sp_tr.Norm(); sp_tr.Normalize();

                AMCAX::Coord3d tr = o_axis_->operateMovePart(sp_tr, len);

                if (select_model_ == SelectModel::OBJECT_MODEL || select_model_ == SelectModel::VERTEX_MODEL
                    || select_model_ == SelectModel::EDGE_MODEL || select_model_ == SelectModel::FACE_MODEL)
                {                
                    coreCommand_->moveSelectedObject(select_info_list_, select_model_, tr.Normalized(), len);
                }

                o_axis_->setCenter(oCenter + tr);
                update();
            }
        }
        /* 处理操作轴的缩放 */
        else if (operate_type_ == OperationType::SCALE_OPERATION && e->modifiers() == Qt::NoModifier)
        {
            if (!select_info_list_.empty() && o_axis_ != nullptr && o_axis_->selectPart() > 0)
            {
                AMCAX::Coord3d oCenter = o_axis_->center();
                AMCAX::Coord3d sp_tr = mouseMoveTrans(e_point, oCenter);
                double len = sp_tr.Norm(); sp_tr.Normalize();
                AMCAX::Coord3d s_arix = o_axis_->operateScalePart(sp_tr, len);
                double scale = o_axis_->operateScalePart_scale();

                if (select_model_ == SelectModel::OBJECT_MODEL || select_model_ == SelectModel::VERTEX_MODEL
                    || select_model_ == SelectModel::EDGE_MODEL || select_model_ == SelectModel::FACE_MODEL)
                {
                    if (o_axis_->selectPart() == 1 || o_axis_->selectPart() == 2 || o_axis_->selectPart() == 3)
                    {
                        coreCommand_->scaleSelectedObject(select_info_list_,
 select_model_,
                            oCenter, s_arix, scale);
                    }
                    else if (o_axis_->selectPart() == 4 || o_axis_->selectPart() == 5 || o_axis_->selectPart() == 6)
                    {
                        coreCommand_->scaleSelectedObject(select_info_list_,
 select_model_,
                            oCenter, s_arix, scale, 1);
                    }                  
                }
                update();
            }
        }
        /* 处理操作轴的旋转 */
        else if (operate_type_ == OperationType::ROTATE_OPERATION && e->modifiers() == Qt::NoModifier)
        {
            if (!select_info_list_.empty() && o_axis_ != nullptr && o_axis_->selectPart() > 0)
            {
                AMCAX::Coord3 oCenter = o_axis_->center();
                Vec c(oCenter[0], oCenter[1], oCenter[2]);
                Vec sc = camera()->projectedCoordinatesOf(c);

                qreal px = (prevPos_.x() - sc.X()) / camera()->screenWidth();
                qreal py = (sc.Y() - prevPos_.y()) / camera()->screenHeight();
                qreal dx = (e_point.x() - sc.X()) / camera()->screenWidth();
                qreal dy = (sc.Y() - e_point.y()) / camera()->screenHeight();

                double u = (px - dy + py - dx); u = u / std::abs(u);

                const Vec p1(px, py, 1.0);
                const Vec p2(dx, dy, 1.0);
                const Vec axis = MathUtils::cross(p2, p1);
                const Vec axisn = axis / axis.Norm();
                AMCAX::Coord3 s_arix = o_axis_->operateRotation(u);

                const qreal angle = 5.0 * u * asin(std::sqrt(axis.SquaredNorm() / p1.SquaredNorm() / p2.SquaredNorm()));

                if (select_model_ == SelectModel::OBJECT_MODEL || select_model_ == SelectModel::VERTEX_MODEL
                    || select_model_ == SelectModel::EDGE_MODEL || select_model_ == SelectModel::FACE_MODEL)
                {
                    coreCommand_->rotateSelectedObject(select_info_list_,
 select_model_,
                        oCenter, s_arix, angle);
                }
                update();
            }
        }

        if ((operate_type_ == OperationType::MOVE_OPERATION || operate_type_ == OperationType::SCALE_OPERATION
            || operate_type_ == OperationType::ROTATE_OPERATION) && e->modifiers() == Qt::ControlModifier)
        {
            if (!select_info_list_.empty() && o_axis_ != nullptr && o_axis_->selectPart() == 0)
            {
                AMCAX::Coord3 oCenter = o_axis_->center();
                AMCAX::Coord3 sp_tr = mouseMoveTrans(e_point, oCenter);
                double len = sp_tr.Norm(); sp_tr.Normalize();
                AMCAX::Coord3 tr = o_axis_->operateMovePart(sp_tr, len);
                o_axis_->setCenter(oCenter + tr);
                update();
            }
        }

        /* 处理移动后操作的鼠标移动，例如split等，让点在边上移动 */
        if (operate_type_ == OperationType::PICK_OPERATION_MOVE && is_pick_move_select_)
        {
            SelectInfoWithCamera s_info = select_info_list_[0];
            setSelectInfoCamera(e->pos(), s_info);

            update();
        }
        else if (operate_type_ == OperationType::PICK_OPERATION_MOVE_MULTI && is_pick_move_select_)
        {
            SelectInfoWithCamera s_info = select_info_list_.back();
            setSelectInfoCamera(e->pos(), s_info);

            update();
        }
    }

    prevPos_ = e->pos();

    QGLViewer::mouseMoveEvent(e);
}

std::vector<int> getSelectObjectList(const std::vector<SelectInfoWithCamera>& s_info_list)
{
    std::set<int> s_id_list;
    for (int i = 0; i < s_info_list.size(); i++)
    {
        s_id_list.insert(s_info_list[i].object_id_);
    }
    std::vector<int> so_id_list(s_id_list.begin(), s_id_list.end());
    return so_id_list;
}

void RenderViewer::mouseReleaseEvent(QMouseEvent* e)
{
    if (is_frame_selection_)
    {
        is_frame_selection_ = false;
        frame_selection_rectangle_ = frame_selection_rectangle_.normalized();
        QPoint p = frame_selection_rectangle_.center();
        QPoint rv = frame_selection_rectangle_.bottomRight() - frame_selection_rectangle_.topLeft();
        QPoint c = frame_selection_rectangle_.center();

        camera()->convertClickToLine(p, view_ori_, view_dir_);

        PyramidInf frustum;
        frustum.ori_ = AMCAX::Coord3(view_ori_.X(), view_ori_.Y(), view_ori_.Z());
        frustum.dir_.resize(4);
        if (rv.x() > 0 && rv.y() > 0)
        {
            camera()->convertClickToLine(frame_selection_rectangle_.topLeft(), view_ori_, view_dir_);
            frustum.dir_[0] = view_dir_;
            camera()->convertClickToLine(frame_selection_rectangle_.topRight(), view_ori_, view_dir_);
            frustum.dir_[1] = view_dir_;
            camera()->convertClickToLine(frame_selection_rectangle_.bottomRight(), view_ori_, view_dir_);
            frustum.dir_[2] =view_dir_;
            camera()->convertClickToLine(frame_selection_rectangle_.bottomLeft(), view_ori_, view_dir_);
            frustum.dir_[3] = view_dir_;
        }
        else
        {
            return;
        }

        selectTool_->setSelectRegionWidth(frame_selection_rectangle_.width());
        selectTool_->setSelectRegionHeight(frame_selection_rectangle_.height());

        selectFrame(frame_selection_rectangle_.center(), frustum);

        selectTool_->setSelectRegionWidthDefault();
        selectTool_->setSelectRegionHeightDefault();

        createOperateAxis();
    }
    else if (operate_type_ == OperationType::NO_OPERATION && click_buttom == Qt::LeftButton)
    {
        if( createOperateAxis() )
        {
            coreCommand_->createBackupSInfo(select_info_list_);
        }
        coreCommand_->endOperationByAxis(select_info_list_, select_model_);
    }
    else if (operate_type_ == OperationType::MOVE_OPERATION && click_buttom == Qt::LeftButton)
    {
        coreCommand_->endOperationByAxis(select_info_list_, select_model_);
        coreCommand_->createBackupSInfo(select_info_list_);
    }
    else if (operate_type_ == OperationType::SCALE_OPERATION && click_buttom == Qt::LeftButton)
    {
        if (o_axis_ != nullptr)
        {
            o_axis_->reSetLen();

            coreCommand_->endOperationByAxis(select_info_list_, select_model_);
            coreCommand_->createBackupSInfo(select_info_list_);
        }
    }
    else if (operate_type_ == OperationType::ROTATE_OPERATION && click_buttom == Qt::LeftButton)
    {
        coreCommand_->endOperationByAxis(select_info_list_, select_model_);
        coreCommand_->createBackupSInfo(select_info_list_);
    }


    if (operate_type_ == OperationType::PICK_OPERATION_MOVE && is_pick_move_select_)
    {
        if (select_info_list_.size() > 0)
        {
            coreCommand_->OperateSelectedObject_Subset_Split(select_model_, select_info_list_, operation_point_);
            coreCommand_->createBackupSInfo(select_info_list_);
            clearSelected();
        }
        operation_point_.clear();
        is_pick_move_select_ = false;
        update();
    }

    if (operate_type_ == OperationType::CTRL_OPERATION)
    {
        coreCommand_->endOperationByAxis(select_info_list_, select_model_);
        coreCommand_->createBackupSInfo(select_info_list_);
        clearSelected();
        cancelOperation();
        update();
    }
    else if (operate_type_ == OperationType::ALT_OPERATION)
    {
        coreCommand_->endOperationByAxis(select_info_list_, select_model_);
        coreCommand_->createBackupSInfo(select_info_list_);
        clearSelected();
        cancelOperation();
        update();
    }

    if (!select_info_list_.empty())
    {
        std::cout << "select id is ";
        for (size_t i = 0; i < select_info_list_.size(); i++)
        {
            std::cout << select_info_list_[i].object_id_ << "::" << select_info_list_[i].object_subselect_id_ << "  ";
        }
        std::cout << std::endl;
    }
  
    QGLViewer::mouseReleaseEvent(e);
}

void RenderViewer::mouseDoubleClickEvent(QMouseEvent* e)
{
    bool shift_left = (e->modifiers() == Qt::ShiftModifier) && (e->button() == Qt::LeftButton);

    if (shift_left && select_model_ == SelectModel::EDGE_MODEL)
    {
        int s_size = select_info_list_.size();
        if (s_size == 1 )
        {
            coreCommand_->getSelectedObject_Element_Related(select_info_list_, select_model_);
            createOperateAxis();
        }
    }

    QGLViewer::mouseDoubleClickEvent(e);
}

//==========================================================

void RenderViewer::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == 0) {
        e->ignore();
        return;
    }

    const Qt::Key key = Qt::Key(e->key());
    const Qt::KeyboardModifiers modifiers = e->modifiers();

    //取消当前的操作( Cancel the all operation) 
    if (key == Qt::Key_Escape)
    {
        cancelOperation();
        clearClicked();
        update();
    }
    //enter with differente Operation type
    else if (key == Qt::Key_Return && operate_type_ == OperationType::ENTER_OPERATION) {
        beginEnterOperation();
    }
    else if (key == Qt::Key_Return && operate_type_ == OperationType::PICK_OPERATION_MOVE_MULTI)
    {
        if (select_info_list_.size() > 0)
        {
            coreCommand_->OperateSelectedObject_Subset_Split(select_model_, select_info_list_, operation_point_);
            coreCommand_->createBackupSInfo(select_info_list_);
            clearSelected();
        }
        operation_point_.clear();
        is_pick_move_select_ = false;
        update();
    }
    else if (key == Qt::Key_Return && operate_type_ == OperationType::CLICK_OPERATION_SELECT) 
    {
        beginEnterClickOperation();
    }
    // control , with undo / redo
    else if (modifiers == Qt::ControlModifier && key == Qt::Key_Z)
    {
        slotUndo();
    }
    else if (modifiers == Qt::ControlModifier && key == Qt::Key_Y)
    {
        slotRedo();
    }
    //开关工作平面显示
    else if (modifiers == Qt::NoModifier && key == Qt::Key_G) {
        changeGridIsDrawn();
        update();
    }
    //移动模式
    else if (modifiers == Qt::NoModifier && key == Qt::Key_W)
    {
        setOperateType(OperationType::MOVE_OPERATION);
        update();
    }
    //旋转模式
    else if (modifiers == Qt::NoModifier && key == Qt::Key_R)
    {
        setOperateType(OperationType::ROTATE_OPERATION);
        update();
    }
    //缩放模式
    else if (modifiers == Qt::NoModifier && key == Qt::Key_E)
    {
        setOperateType(OperationType::SCALE_OPERATION);
        update();
    }
    //取消操作轴
    else if (modifiers == Qt::NoModifier && key == Qt::Key_Q)
    {
        setOperateType(OperationType::NO_OPERATION);
        update();
    }
    //顶点模式
    else if (modifiers == Qt::NoModifier && key == Qt::Key_A)
    {
        setSelectModel(SelectModel::VERTEX_MODEL);
        clearSelected();
        emit selectedModelChanged();
        update();
    }   
    //边模式
    else if (modifiers == Qt::NoModifier && key == Qt::Key_S)
    {
        setSelectModel(SelectModel::EDGE_MODEL);
        clearSelected();
        emit selectedModelChanged();
        update();
    }
    //面模式
    else if (modifiers == Qt::NoModifier && key == Qt::Key_D)
    {
        setSelectModel(SelectModel::FACE_MODEL);
        clearSelected();
        emit selectedModelChanged();
        update();
    }
    //物体模式
    else if (modifiers == Qt::NoModifier && key == Qt::Key_F)
    {
        setSelectModel(SelectModel::OBJECT_MODEL);
        clearSelected();
        emit selectedModelChanged();
        update();
    }
    //切换样条显示
    else if (modifiers == Qt::NoModifier && key == Qt::Key_Tab)
    {
        if (draw_model_ == DrawModel::RENDERING_MODEL)
            setDrawModel(DrawModel::SHADING_MODEL);
        else if(draw_model_ == DrawModel::SHADING_MODEL)
            setDrawModel(DrawModel::RENDERING_MODEL);
    }
    // 投影操作
    else if (modifiers == Qt::NoModifier && key == Qt::Key_P)
    {
        coreCommand_->setMeshOperationType(MeshOperationType::MeshProject);
        coreCommand_->OperateSelectedObject(select_model_, select_info_list_, work_plane_->center(), work_plane_->normal());
        cancelOperation();
        update();
    }
    //删除操作 (delete)
    else if (key == Qt::Key_Delete)
    {
        if (select_model_ == SelectModel::OBJECT_MODEL)
        {
            if (!select_info_list_.empty())
            {
                coreCommand_->setMeshOperationType(MeshOperationType::MeshDelete);
                beginEnterOperation();
            }
        }
        else if (select_model_ == SelectModel::FACE_MODEL || 
            select_model_ == SelectModel::EDGE_MODEL || select_model_ == SelectModel::VERTEX_MODEL)
        {
            if (!select_info_list_.empty())
            {
                coreCommand_->setMeshOperationType(MeshOperationType::MeshDeleteElement);
                beginEnterOperation();
                cancelOperation();
            }
        }
    }
    //保存操作
    else if (modifiers == Qt::ControlModifier && key == Qt::Key_S)
    {
        QString f = "AMCAD_work.obj";
        coreCommand_->saveObjects(f);
    }
    else
    {
        QGLViewer::keyPressEvent(e);
    }
}

void RenderViewer::keyReleaseEvent(QKeyEvent* e)
{
    QGLViewer::keyReleaseEvent(e);
}

//==========================================================
//                Select Function
//==========================================================

void RenderViewer::setSelectBufferSize(int size) {
    if (selectBuffer_)
        delete[] selectBuffer_;
    selectBufferSize_ = size;
    selectBuffer_ = new GLuint[selectBufferSize()];
}

bool RenderViewer::select(const QMouseEvent* ev, bool is_select_muti)
{
    makeCurrent();

    const QPoint point = ev->pos();
    AMCAX::CoordT<int, 2> p(point.x(), point.y());
    bool is_true = selectTool_->select(camera(), select_model_, p, is_select_muti);
    select_info_list_ = selectTool_->getSelectInfoNew();
    select_object_center_ = selectTool_->getCenter();
    return is_true;

    //const QPoint point = ev->pos();
    //return selectedDrawNeeded(point, is_select_muti);
}

void RenderViewer::beginSelection(const QPoint& point) {
    // Make OpenGL context current (may be needed with several viewers ?)
    makeCurrent();

    // Prepare the selection mode
    glSelectBuffer(selectBufferSize(), selectBuffer());
    glRenderMode(GL_SELECT);
    glInitNames();

    // Loads the matrices
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    static GLint viewport[4];
    camera()->getViewport(viewport);
    gluPickMatrix(point.x(), point.y(), selectRegionWidth(), selectRegionHeight(), viewport);

    // loadProjectionMatrix() first resets the GL_PROJECTION matrix with a glLoadIdentity()
    // The false parameter prevents this and hence multiplies the matrices.
    camera()->loadProjectionMatrix(false);
    // Reset the original (world coordinates) modelview matrix
    camera()->loadModelViewMatrix();
}

void RenderViewer::endSelection(const QPoint& point, SelectModel type)
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

//===================================================================

void RenderViewer::selectFrame(const QPoint& point, PyramidInf & frustum)
{
    AMCAX::CoordT<int, 2> p(point.x(), point.y());
    selectTool_->selectFrame(camera(), select_model_, p, frustum);
    select_info_list_ = selectTool_->getSelectInfoNew();
    select_object_center_ = selectTool_->getCenter();
}

//===================================================================

bool RenderViewer::selectOpAxis(const QMouseEvent* ev)
{
    const QPoint point = ev->pos();
    beginSelection(point);
    selectDrawAxis();
    endSelection(point, SelectModel::OBJECT_MODEL);

    if (selectedId_ >= 0){
        o_axis_->setSelectPart(selectedId_);
        return true;
    }
    else{
        if (o_axis_ != nullptr)
        {
            o_axis_->setSelectPart(-1);
        }
        return false;
    }
}

void RenderViewer::selectDrawAxis()
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

    if (o_axis_ == nullptr || select_info_list_.empty())
        return;

    o_axis_->drawForSelect(type);
}

void RenderViewer::endSelectionAxis(const QPoint& point)
{
    Q_UNUSED(point)

        // Flush GL buffers
        glFlush();

    // Get the number of objects that were seen through the pick matrix frustum.
    // Reset GL_RENDER mode.
    GLint nbHits = glRenderMode(GL_RENDER);

    if (nbHits <= 0)
        selectedId_ = -1;
    else 
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
}

//===================================================================

void RenderViewer::click(const QMouseEvent* ev)
{
    const QPoint pixel = ev->pos();

    camera()->convertClickToLine(pixel, view_ori_, view_dir_);
    AMCAX::Coord3 c_begin(view_ori_.X(), view_ori_.Y(), view_ori_.Z());
    AMCAX::Coord3 c_dir(view_dir_.X(), view_dir_.Y(), view_dir_.Z()); c_dir.Normalize();

    ClickInfo c_info;
    AMCAX::Coord3d w_point;
    work_plane_->lineInsertPlane(c_begin, c_dir, w_point);
    c_info.point_ = w_point;

    click_info_list_.push_back(c_info);
}

//===================================================================
//                  Operation Methods
//===================================================================

void RenderViewer::beginPickOperationSingle()
{
    if (select_info_list_.size() > 0)
    {
        coreCommand_->OperateSelectedObject_Subset(select_model_, select_info_list_);
        clearSelected();
        update();
    }
}

void RenderViewer::beginEnterOperation()
{
    if (select_model_ == SelectModel::OBJECT_MODEL)
    {
        if (select_info_list_.size() > 0)
        {
            coreCommand_->OperateSelectedObject(select_model_, select_info_list_);
            coreCommand_->OperateSelectedObject(select_model_, select_info_list_, work_plane_->center(), work_plane_->normal());

            coreCommand_->OperateSelectedObjectFaceExtrude(select_model_, select_info_list_, operation_vector_);

            cancelOperation();
            update();
        }
    }
    else if (select_model_ == SelectModel::VERTEX_MODEL || select_model_ == SelectModel::EDGE_MODEL 
        || select_model_ == SelectModel::FACE_MODEL)
    {
        if (select_info_list_.size() > 0)
        {
            coreCommand_->OperateSelectedObject_Subset(select_model_, select_info_list_);
            
            if (coreCommand_->getMeshOperationType() == MeshOperationType::MeshCopy)
            {
                setOperateType(OperationType::NO_OPERATION);
                coreCommand_->setOperationDataType(OperationDataType::UNKNOW_TYPE);
                coreCommand_->setMeshOperationType(MeshOperationType::NO_operotion);
                createOperateAxis();
                update();
            }
            else if (coreCommand_->getMeshOperationType() == MeshOperationType::MeshWeldEdge
                || coreCommand_->getMeshOperationType() == MeshOperationType::MeshSeparate
                || coreCommand_->getMeshOperationType() == MeshOperationType::MeshChamferEdge
                || coreCommand_->getMeshOperationType() == MeshOperationType::MeshFilletEdge)
            {
                cancelOperation();
                update();
            }
            else
            {
                clearSelected();
                update();
            }
        }
    }
}

void RenderViewer::beginEnterClickOperation()
{
    if (select_model_ == SelectModel::CLICK_MODEL)
    {
        if (click_info_list_.size() > 0)
        {
            coreCommand_->OperatePointlistWithSelectedObject(select_model_, click_info_list_, select_info_list_);

            cancelOperation();
            update();
        }
    }
}

void RenderViewer::cancelOperation()
{
    clearSelected();
    clearClicked();
    setOperateType(OperationType::NO_OPERATION);
    coreCommand_->setOperationDataType(OperationDataType::UNKNOW_TYPE);
    coreCommand_->setMeshOperationType(MeshOperationType::NO_operotion);

    setSelectModel(SelectModel::OBJECT_MODEL);

    Q_EMIT selectedModelChanged();
}

//====================================================================
//                   camera Methods 
//====================================================================

void RenderViewer::setScenesCenter()
{
    if (select_info_list_.empty())
    {
        setSceneCenter(qglviewer::Vec(0.0, 0.0, 0.0));
        camera()->fitSphere(qglviewer::Vec(0.0, 0.0, 0.0), 5.0);
    }
    else
    {
        AMCAX::Coord3 c = AMCAX::Coord3(0.0, 0.0, 0.0);
        AMCAX::Coord3 bb_min_f = AMCAX::Coord3(DBL_MAX, DBL_MAX, DBL_MAX);
        AMCAX::Coord3 bb_max_f = AMCAX::Coord3(-DBL_MAX, -DBL_MAX, -DBL_MAX);
        AMCAX::Coord3 bb_min = AMCAX::Coord3(0.0, 0.0, 0.0);
        AMCAX::Coord3 bb_max = AMCAX::Coord3(0.0, 0.0, 0.0);
        for (int i = 0; i < select_info_list_.size(); i++)
        {
            c += coreCommand_->getSelectedObject_Center(select_info_list_[i], select_model_);
            coreCommand_->getSelectedObject_BBox(select_info_list_[i], select_model_, bb_min, bb_max);

            if (bb_min[0] < bb_min_f[0]) bb_min_f[0] = bb_min[0];
            if (bb_min[1] < bb_min_f[1]) bb_min_f[1] = bb_min[1];
            if (bb_min[2] < bb_min_f[2]) bb_min_f[2] = bb_min[2];

            if (bb_max[0] > bb_max_f[0]) bb_max_f[0] = bb_max[0];
            if (bb_max[1] > bb_max_f[1]) bb_max_f[1] = bb_max[1];
            if (bb_max[2] > bb_max_f[2]) bb_max_f[2] = bb_max[2];
        }

        double r = (bb_max_f - bb_min_f).Norm() / 2;
        r = r * 1.2;
        if (r < 1.0) r = 1.0;

        setSceneCenter(qglviewer::Vec(c.X(), c.Y(), c.Z()));
        camera()->fitSphere(qglviewer::Vec(c.X(), c.Y(), c.Z()), r);
    }
}

void RenderViewer::setWorkPlane()
{
    if (select_info_list_.empty())
    {
        work_plane_->setCenter(AMCAX::Coord3(0.0, 0.0, 0.0));
        work_plane_->setNormal(AMCAX::Coord3(0.0, 0.0, 1.0));
    }
    else
    {
        AMCAX::Coord3 c, n;
        coreCommand_->getSelectedObject_Plane(select_info_list_, select_model_, c, n);
        work_plane_->setCenter(c);
        work_plane_->setNormal(n);
        cancelOperation();
    }
}

//====================================================================
//                   Auxiliary function
//====================================================================

bool RenderViewer::createOperateAxis()
{
    bool is_move = false;
    if (select_info_list_.empty())
    {
        if (o_axis_ != nullptr)
        {
            delete o_axis_;
            o_axis_ = nullptr;
        }
    }
    else
    {
        if (o_axis_ != nullptr)
        {
            delete o_axis_;
            o_axis_ = nullptr;
        }

        AMCAX::Coord3 c = AMCAX::Coord3(0.0, 0.0, 0.0);
        for (int i = 0; i < select_info_list_.size(); i++)
        {
            c += coreCommand_->getSelectedObject_Center(select_info_list_[i], select_model_);
        }
        c /= select_info_list_.size();

        if ((select_object_center_ - c).Norm() > 1e-4)
        {
            is_move = true;
        }

        if (select_info_list_.size() == 1)
        {
            AMCAX::Coord3 n = coreCommand_->getSelectedObject_Normal(select_info_list_[0], select_model_);
            o_axis_ = new AxisOperation(c, n);
        }
        else
        {
            o_axis_ = new AxisOperation(c);
        }
    }

    return is_move;
}

void RenderViewer::setSelectInfoCamera(const QPoint& pixel, acamcad::SelectInfoWithCamera& s_info)
{
    camera()->convertClickToLine(pixel, view_ori_, view_dir_);
    qglviewer::Vec space_point_ = view_ori_ + 100.f * view_dir_;
    s_info.c_begin_ = view_ori_;
    s_info.c_end_ = space_point_;
}

AMCAX::Coord3 RenderViewer::mouseMoveTrans(const QPoint& e_point, const AMCAX::Coord3& o_center)
{
    const QPoint delta = e_point - prevPos_;
    qglviewer::Vec trans = qglviewer::Vec((double)(delta.x()), (double)(-delta.y()), 0.0);
    qglviewer::Vec center(o_center.X(), o_center.Y(), o_center.Z());

    trans *= 2.0 * tan(camera()->fieldOfView() / 2.0) *
        fabs((camera()->frame()->coordinatesOf(center)).Z()) / camera()->screenHeight();
    trans = camera()->frame()->orientation().rotate(trans);
    return AMCAX::Coord3(trans.X(), trans.Y(), trans.Z());
}

void RenderViewer::clearSelected()
{
    dataManager_->clearSelected();
    select_info_list_.clear();
    selectTool_->clearSelected();
}

void RenderViewer::clearClicked()
{
    click_info_list_.clear();
    operation_point_.clear();
}

void RenderViewer::setDataManger(DataManager* dataManager)
{
    dataManager_ = dataManager;
    selectTool_->setDataManger(dataManager);
}

void RenderViewer::setCore(AMCore* core)
{
    coreCommand_ = core;
    core->setRecord(record);
}

void RenderViewer::slotUndo()
{
    clearSelected();
    coreCommand_->Undo();
    createOperateAxis();
    update();
}

void RenderViewer::slotRedo()
{
    clearSelected();
    coreCommand_->Redo();
    createOperateAxis();
    update();
}