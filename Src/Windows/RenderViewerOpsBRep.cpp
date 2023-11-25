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


// ==================== BRep TAB NON SLOT METHODS ====================
void RenderViewer::initBRep()
{
    objectFillOuts[OPERATE_BREP_PLANE] = QVector<ParamDialog::RequestUnit> {
        ParamDialog::RequestUnit(ParamDialog::TYPE_STRING, tr("Label (Group)"), ParamDialog::DataUnit{tr("plane")}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_MPOINT3_2, tr("first"), ParamDialog::DataUnit{ AMCAX::Coord3{.0, .0, .0}, ParamDialog::ParamType::TYPE_MPOINT3}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_MPOINT3_2, tr("second"), ParamDialog::DataUnit{ AMCAX::Coord3{2.0, 2.0, .0},  ParamDialog::ParamType::TYPE_MPOINT3}),
    };
    objectFillOuts[OPERATE_BREP_CUBE] = QVector<ParamDialog::RequestUnit> {
        ParamDialog::RequestUnit(ParamDialog::TYPE_STRING, tr("Label (Group)"), ParamDialog::DataUnit{tr("cube")}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_MPOINT3, tr("p0"), ParamDialog::DataUnit{ AMCAX::Coord3{.0, .0, .0},  ParamDialog::ParamType::TYPE_MPOINT3}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_MPOINT3, tr("p1"), ParamDialog::DataUnit{ AMCAX::Coord3{1.0, 1.0, 1.0},  ParamDialog::ParamType::TYPE_MPOINT3}),
    };
    objectFillOuts[OPERATE_BREP_SPHERE] = QVector<ParamDialog::RequestUnit> {
        ParamDialog::RequestUnit(ParamDialog::TYPE_STRING, tr("Label (Group)"), ParamDialog::DataUnit{tr("sphere")}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_MPOINT3, tr("center"), ParamDialog::DataUnit{ AMCAX::Coord3{.0, .0, .0}, ParamDialog::ParamType::TYPE_MPOINT3}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_DOUBLE, tr("radius"), ParamDialog::DataUnit{1.0}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_DOUBLE_PI, tr("angle1"), ParamDialog::DataUnit{2.0}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_DOUBLE_PI, tr("angle2 (NOP)"), ParamDialog::DataUnit{0.0}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_DOUBLE_PI, tr("angle3 (NOP)"), ParamDialog::DataUnit{0.0}),
    };
    objectFillOuts[OPERATE_BREP_CYLINDER] = QVector<ParamDialog::RequestUnit> {
        ParamDialog::RequestUnit(ParamDialog::TYPE_STRING, tr("Label (Group)"), ParamDialog::DataUnit{tr("cylinder")}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_MPOINT3, tr("b_center"), ParamDialog::DataUnit{  AMCAX::Coord3{.0, .0, .0},  ParamDialog::ParamType::TYPE_MPOINT3}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_MVEC, tr("axis"), ParamDialog::DataUnit{  AMCAX::Coord3{.0, .0, 1.0}, ParamDialog::ParamType::TYPE_MVEC3}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_MVEC, tr("X Axis direction"), ParamDialog::DataUnit{ AMCAX::Coord3{1.0, .0, .0}, ParamDialog::ParamType::TYPE_MVEC3}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_DOUBLE, tr("radius"), ParamDialog::DataUnit{1.0}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_DOUBLE, tr("height"), ParamDialog::DataUnit{1.0}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_DOUBLE_PI, tr("angle"), ParamDialog::DataUnit{2.0}),
    };
    objectFillOuts[OPERATE_BREP_CONE] = QVector<ParamDialog::RequestUnit> {
        ParamDialog::RequestUnit(ParamDialog::TYPE_STRING, tr("Label (Group)"), ParamDialog::DataUnit{tr("cone")}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_MPOINT3, tr("b_center"), ParamDialog::DataUnit{ AMCAX::Coord3{.0, .0, .0}, ParamDialog::ParamType::TYPE_MPOINT3}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_MVEC, tr("axis"), ParamDialog::DataUnit{ AMCAX::Coord3{.0, .0, 1.0}, ParamDialog::ParamType::TYPE_MVEC3}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_MVEC, tr("X Axis direction"), ParamDialog::DataUnit{ AMCAX::Coord3{1.0, .0, .0}, ParamDialog::ParamType::TYPE_MVEC3}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_DOUBLE, tr("radiuR"), ParamDialog::DataUnit{1.0}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_DOUBLE, tr("radiur"), ParamDialog::DataUnit{0.0}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_DOUBLE, tr("height"), ParamDialog::DataUnit{1.0}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_DOUBLE_PI, tr("angle"), ParamDialog::DataUnit{2.0}),
    };
    objectFillOuts[OPERATE_BREP_TORUS] = QVector<ParamDialog::RequestUnit> {
        ParamDialog::RequestUnit(ParamDialog::TYPE_STRING, tr("Label (Group)"), ParamDialog::DataUnit{tr("torus")}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_MPOINT3, tr("center"), ParamDialog::DataUnit{ AMCAX::Coord3{.0, .0, .0}, ParamDialog::ParamType::TYPE_MPOINT3}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_MVEC3, tr("axis"), ParamDialog::DataUnit{ AMCAX::Coord3{.0, .0, 1.0}, ParamDialog::ParamType::TYPE_MVEC3}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_MVEC, tr("X Axis direction"), ParamDialog::DataUnit{ AMCAX::Coord3{1.0, .0, .0}, ParamDialog::ParamType::TYPE_MVEC3}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_DOUBLE, tr("radiu0"), ParamDialog::DataUnit{2.0}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_DOUBLE, tr("radiu1"), ParamDialog::DataUnit{1.0}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_DOUBLE_PI, tr("angle"), ParamDialog::DataUnit{2.0}),
    };
    objectFillOuts[OPERATE_BREP_PRISM] = QVector<ParamDialog::RequestUnit> {
        ParamDialog::RequestUnit(ParamDialog::TYPE_STRING, tr("Label (Group)"), ParamDialog::DataUnit{tr("prism")}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_ARR_MPOINT3, tr("Point List"), ParamDialog::DataUnit{AMCAX::Coord3{.0, .0, .0}, ParamDialog::ParamType::TYPE_MPOINT3}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_MVEC3, tr("Extrude Axis"), ParamDialog::DataUnit{ AMCAX::Coord3{.0, .0, 2.0}, ParamDialog::ParamType::TYPE_MVEC3}),
    };
    objectFillOuts[OPERATE_BREP_EXTRUDE] = QVector<ParamDialog::RequestUnit> {
        std::make_tuple(ParamDialog::TYPE_MVEC, tr("dir"), ParamDialog::DataUnit{ AMCAX::Coord3{.0, .0, 1.0}, ParamDialog::ParamType::TYPE_MVEC3}),
    };
    objectFillOuts[OPERATE_BREP_POLYGON] = QVector<ParamDialog::RequestUnit> {
        ParamDialog::RequestUnit(ParamDialog::TYPE_STRING, tr("Label (Group)"), ParamDialog::DataUnit{tr("prism")}),
        ParamDialog::RequestUnit(ParamDialog::TYPE_ARR_MPOINT3, tr("Point List"), ParamDialog::DataUnit{AMCAX::Coord3{.0, .0, .0}, ParamDialog::ParamType::TYPE_MPOINT3}),
    };
}

// ==================== BRep TAB ====================
void RenderViewer::slotCreateSingleFaceBRep() {
    BRepObjectOperation(SelectModel::CLICK_MODEL, OperationType::CLICK_OPERATION_SELECT,
        MeshOperationType::MeshFaceAppend);
}

int RenderViewer::getLastPersistentId() {
    return coreCommand_->getLastPersistentId();
}
void RenderViewer::slotCreatPlaneBRep() 
{
    dialog->addElements(objectFillOuts[OPERATE_BREP_PLANE]);
    dialog->setWindowTitle(tr("Create a plane mesh"));
    connect(dialog, &ParamDialog::dataReady, this, &RenderViewer::slotCreatPlaneBRepAccepted);
    dialog->open();
}
void RenderViewer::slotCreatPlaneBRepAccepted(const ParamDialog::Response& data) 
{ 
    QString label(data[0].second.str);
    AMCAX::Coord3 p0(data[1].second.point);
    AMCAX::Coord3 p1(data[2].second.point);
    coreCommand_->createPlaneBRepObject(p0, p1, label.toStdString());
    record.recordPlane(getLastPersistentId(), p0, p1, label.toStdString());
    update();

    objectFillOuts[OPERATE_BREP_PLANE][0].def.str = label;
    objectFillOuts[OPERATE_BREP_PLANE][1].def.point = p0;
    objectFillOuts[OPERATE_BREP_PLANE][2].def.point = p1;
}

void RenderViewer::slotCreatCubeBRep() {
    dialog->addElements(objectFillOuts[OPERATE_BREP_CUBE]);
    dialog->setWindowTitle(tr("Create a cube BRep"));
    connect(dialog, &ParamDialog::dataReady, this, &RenderViewer::slotCreatCubeBRepAccepted);
    dialog->open();
}
void RenderViewer::slotCreatCubeBRepAccepted(const ParamDialog::Response& data) {
    QString label(data[0].second.str);
    AMCAX::Coord3 c(data[1].second.point);
    AMCAX::Coord3 s(data[2].second.point);
    coreCommand_->createCubeBRepObject(c, s, label.toStdString());
    record.recordCube(getLastPersistentId(), c, s, label.toStdString());
    update();

    objectFillOuts[OPERATE_BREP_CUBE][0].def.str = label;
    objectFillOuts[OPERATE_BREP_CUBE][1].def.point = c;
    objectFillOuts[OPERATE_BREP_CUBE][2].def.point = s;
}

void RenderViewer::slotCreateSphereBRep() {
    dialog->addElements(objectFillOuts[OPERATE_BREP_SPHERE]);
    dialog->setWindowTitle(tr("Create a Sphere BRep"));
    connect(dialog, &ParamDialog::dataReady, this, &RenderViewer::slotCreateSphereBRepAccepted);
    dialog->open();
}
void RenderViewer::slotCreateSphereBRepAccepted(const ParamDialog::Response& data) {
    QString label(data[0].second.str);
    AMCAX::Coord3 center = data[1].second.point;
    double radius = data[2].second.d;
    double angle1 = data[3].second.d;
    double angle2 = data[4].second.d;
    double angle3 = data[5].second.d;

    coreCommand_->createSphereBRepObject(center, radius, angle1 * M_PI, angle2 * M_PI,
 angle3 * M_PI,
                                 label.toStdString());
    record.recordSphere(getLastPersistentId(), center, radius, angle1, angle2,
 angle3,
        label.toStdString());
    update();

    objectFillOuts[OPERATE_BREP_SPHERE][0].def.str = label;
    objectFillOuts[OPERATE_BREP_SPHERE][1].def.point = center;
    objectFillOuts[OPERATE_BREP_SPHERE][2].def.d = radius;
    objectFillOuts[OPERATE_BREP_SPHERE][3].def.d = angle1;
    objectFillOuts[OPERATE_BREP_SPHERE][4].def.d = angle2;
    objectFillOuts[OPERATE_BREP_SPHERE][5].def.d = angle3;
}

void RenderViewer::slotCreateCylinderBRep() {
    dialog->addElements(objectFillOuts[OPERATE_BREP_CYLINDER]);
    dialog->setWindowTitle(tr("Create a Cylinder BRep"));
    connect(dialog, &ParamDialog::dataReady, this, &RenderViewer::slotCreateCylinderBRepAccepted);
    dialog->open();
}
void RenderViewer::slotCreateCylinderBRepAccepted(const ParamDialog::Response& data) {
    QString label(data[0].second.str);
    AMCAX::Coord3 b_center = data[1].second.point;
    AMCAX::Coord3 axis = data[2].second.vec;
    AMCAX::Coord3 xaxis = data[3].second.vec;
    double radius = data[4].second.d;
    double height = data[5].second.d;
    double angle = data[6].second.d;

    coreCommand_->createCylinderBRepObject(b_center, axis, xaxis, radius, height, angle * M_PI,
                                   label.toStdString());
    record.recordCylinder(getLastPersistentId(), b_center, axis, xaxis, radius,
 height, angle,
        label.toStdString());
    update();

    objectFillOuts[OPERATE_BREP_CYLINDER][0].def.str = label;
    objectFillOuts[OPERATE_BREP_CYLINDER][1].def.point = b_center;
    objectFillOuts[OPERATE_BREP_CYLINDER][2].def.vec = axis;
    objectFillOuts[OPERATE_BREP_CYLINDER][3].def.vec = xaxis;
    objectFillOuts[OPERATE_BREP_CYLINDER][4].def.d = radius;
    objectFillOuts[OPERATE_BREP_CYLINDER][5].def.d = height;
    objectFillOuts[OPERATE_BREP_CYLINDER][6].def.d = angle;
}

void RenderViewer::slotCreateConeBrep() {
    dialog->addElements(objectFillOuts[OPERATE_BREP_CONE]);
    dialog->setWindowTitle(tr("Create a Cone BRep"));
    connect(dialog, &ParamDialog::dataReady, this, &RenderViewer::slotCreateConeBrepAccepted);
    dialog->open();
}
void RenderViewer::slotCreateConeBrepAccepted(const ParamDialog::Response& data) {
    QString label(data[0].second.str);
    AMCAX::Coord3 b_center = data[1].second.point;
    AMCAX::Coord3 axis = data[2].second.vec;
    AMCAX::Coord3 xaxis = data[3].second.vec;
    double radiusR = data[4].second.d;
    double radiusr = data[5].second.d;
    double height = data[6].second.d;
    double angle = data[7].second.d;

    coreCommand_->createConeBRepObject(b_center, axis, xaxis, radiusR, radiusr, height, angle * M_PI,
                               label.toStdString());
    record.recordCone(getLastPersistentId(), b_center, axis, xaxis, radiusR, radiusr,
 height, angle,
        label.toStdString());
    update();

    objectFillOuts[OPERATE_BREP_CONE][0].def.str = label;
    objectFillOuts[OPERATE_BREP_CONE][1].def.point = b_center;
    objectFillOuts[OPERATE_BREP_CONE][2].def.vec = axis;
    objectFillOuts[OPERATE_BREP_CONE][3].def.vec = xaxis;
    objectFillOuts[OPERATE_BREP_CONE][4].def.d = radiusR;
    objectFillOuts[OPERATE_BREP_CONE][5].def.d = radiusr;
    objectFillOuts[OPERATE_BREP_CONE][6].def.d = height;
    objectFillOuts[OPERATE_BREP_CONE][7].def.d = angle;
}

void RenderViewer::slotCreateTorusBRep() {
    dialog->addElements(objectFillOuts[OPERATE_BREP_TORUS]);
    dialog->setWindowTitle(tr("Create a Torus BRep"));
    connect(dialog, &ParamDialog::dataReady, this, &RenderViewer::slotCreateTorusBRepAccepted);
    dialog->open();
}
void RenderViewer::slotCreateTorusBRepAccepted(const ParamDialog::Response& data) {
    QString label(data[0].second.str);
    AMCAX::Coord3 center = data[1].second.point;
    AMCAX::Coord3 axis = data[2].second.vec;
    AMCAX::Coord3 xaxis = data[3].second.vec;
    double radius0 = data[4].second.d;
    double radius1 = data[5].second.d;
    double angle = data[6].second.d;

    coreCommand_->createTorusBRepObject(center, axis, xaxis, radius0, radius1, angle * M_PI,
                                label.toStdString());

    record.recordTorus(getLastPersistentId(), center, axis, xaxis, radius0,
 radius1, angle,
        label.toStdString());
    update();

    objectFillOuts[OPERATE_BREP_TORUS][0].def.str = label;
    objectFillOuts[OPERATE_BREP_TORUS][1].def.point = center;
    objectFillOuts[OPERATE_BREP_TORUS][2].def.vec = axis;
    objectFillOuts[OPERATE_BREP_TORUS][3].def.vec = xaxis;
    objectFillOuts[OPERATE_BREP_TORUS][4].def.d = radius0;
    objectFillOuts[OPERATE_BREP_TORUS][5].def.d = radius1;
    objectFillOuts[OPERATE_BREP_TORUS][6].def.d = angle;
}

void RenderViewer::slotCreatePrismBRep() {
    dialog->addElements(objectFillOuts[OPERATE_BREP_PRISM]);
    dialog->setWindowTitle(tr("Create a Prism BRep"));
    connect(dialog, &ParamDialog::dataReady, this, &RenderViewer::slotCreatePrismBRepAccepted);
    dialog->open();
}
void RenderViewer::slotCreatePrismBRepAccepted(const ParamDialog::Response& data) {
    QString label(data[0].second.str);
    std::vector<AMCAX::Coord3d> pl = data[1].second.pointarr;
    AMCAX::Coord3d extaxis = AMCAX::Coord3d(data[2].second.vec.X(), data[2].second.vec.Y(), data[2].second.vec.Z());

    coreCommand_->createPrismBRepObject(pl, extaxis, label.toStdString());
    record.recordPrism(getLastPersistentId(), pl, extaxis, label.toStdString());
    update();

    objectFillOuts[OPERATE_BREP_PRISM][0].def.str = label;
    objectFillOuts[OPERATE_BREP_PRISM][1].def.point = pl[0];
    objectFillOuts[OPERATE_BREP_PRISM][2].def.vec = extaxis;
}

void RenderViewer::slotBRepExtrude()
{
    dialog->addElements(objectFillOuts[OPERATE_BREP_EXTRUDE]);
    dialog->setWindowTitle(tr("Set The Extrude Dir and Length"));
    connect(dialog, &ParamDialog::dataReady, this, &RenderViewer::slotBRepExtrudeAccepted);
    dialog->open();
}
void RenderViewer::slotBRepExtrudeAccepted(const ParamDialog::Response& data)
{
    AMCAX::Coord3d vec = data[0].second.vec;
    setOperationVector(vec);

    BRepObjectOperation(SelectModel::OBJECT_MODEL, OperationType::ENTER_OPERATION,
        MeshOperationType::MeshExtrudeFace);

    objectFillOuts[OPERATE_BREP_EXTRUDE][0].def.vec = vec;
}

void RenderViewer::slotCreatePolygonBRep() 
{
    dialog->addElements(objectFillOuts[OPERATE_BREP_POLYGON]);
    dialog->setWindowTitle(tr("Create a Polygon BRep"));
    connect(dialog, &ParamDialog::dataReady, this, &RenderViewer::slotCreatePolygonBRepAccepted);
    dialog->open();
}
void RenderViewer::slotCreatePolygonBRepAccepted(const ParamDialog::Response& data)
{
    QString label(data[0].second.str);
    std::vector<AMCAX::Coord3d> pl = data[1].second.pointarr;

    coreCommand_->createPolygonBRepObject(pl, label.toStdString());
    record.recordPolygon(getLastPersistentId(), pl, label.toStdString());
    update();

    objectFillOuts[OPERATE_BREP_POLYGON][0].def.str = label;
    objectFillOuts[OPERATE_BREP_POLYGON][1].def.point = pl[0];
}
