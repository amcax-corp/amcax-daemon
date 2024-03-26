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
#include "ACAMWindow.h"
#include <QHBoxLayout>
#include <QToolBar>
#include <QTabBar>
#include <QFileDialog>
#include <QStandardPaths>
#include <fstream>

#include "RenderViewer.h"
#include "../Core/DataManager.h"
#include "../Core/ACAMCore.h"
#include "OperateToolbar.h"
#include "../Core/UndoRedoHandler.h"

using namespace acamcad;

ACAMWindow::ACAMWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    size_t icon_size = 36;

    this->setIconSize(QSize(icon_size, icon_size));
    this->setWindowIcon(QIcon(":images/images/logo.ico"));

    initCoreAndViewer();

    createAction();
    createMenus();
    createToolBars();

    OperateToolBar* tab_widget = new OperateToolBar(render_viewer_, this);
	render_viewer_->selectCheckChanged();
    
    QVBoxLayout* layout_main = new QVBoxLayout;

    size_t bar_height = tab_widget->tabBar()->height();

    tab_widget->setMaximumHeight(icon_size + bar_height + 2);

    layout_main->addWidget(tab_widget, 0, Qt::AlignTop);
    layout_main->addWidget(render_viewer_, 10);

    this->centralWidget()->setLayout(layout_main);

    this->resize(1280, 720);

}

ACAMWindow::~ACAMWindow()
{
    delete render_viewer_;
    delete dataManager_;
    delete undoRedoHandler_;
    delete core_;
}

void ACAMWindow::initCoreAndViewer()
{
    dataManager_ = new DataManager();

    undoRedoHandler_ = new UndoRedoHandler(dataManager_, 50); 

    core_ = new acamcad::AMCore();
    core_->setDataManager(dataManager_);
    core_->setUndoRedoHandler(undoRedoHandler_);

    render_viewer_ = new acamcad::RenderViewer(this);
    render_viewer_->setDataManger(dataManager_);
    render_viewer_->setCore(core_);
}

void ACAMWindow::createAction()
{
    // menu actions
    createAction_File();
    createAction_Edit();
    createAction_Help();
    
    // left side toolbar actions
    createAction_Tetransform();
    createAction_Topology();
}

void ACAMWindow::createAction_File()
{
    openAction_ = new QAction(tr("Open"));
    openAction_->setStatusTip(tr("Open a file"));
    connect(openAction_, &QAction::triggered, core_, &AMCore::loadObjects);

    saveAction_ = new QAction(tr("Save"));
    saveAction_->setStatusTip(tr("Save to file"));
    connect(saveAction_, &QAction::triggered, core_, QOverload<>::of(&AMCore::saveObjects));

    exportAction_ = new QAction(tr("Export Selected"));
    exportAction_->setStatusTip(tr("Export selected models"));
    connect(exportAction_, &QAction::triggered, core_, &AMCore::saveSelectedObjects);

    // TODO:
    exitAction_ = new QAction(tr("Exit"));
    exitAction_->setStatusTip(tr("Exit the program"));
    connect(exitAction_, &QAction::triggered, this, &ACAMWindow::close);
    // check if unsaved...
}

void ACAMWindow::createAction_Edit() 
{
    undoAction_ = new QAction(tr("Undo"));
    undoAction_->setStatusTip(tr("Undo"));
    connect(undoAction_, &QAction::triggered, render_viewer_, &RenderViewer::slotUndo);

    redoAction_ = new QAction(tr("Redo"));
    redoAction_->setStatusTip(tr("Redo"));
    connect(redoAction_, &QAction::triggered, render_viewer_, &RenderViewer::slotRedo);
}

void ACAMWindow::createAction_Help()
{
    aboutAction_ = new QAction(tr("About"));
    aboutAction_->setStatusTip("About us");
    // TODO connect
}

void ACAMWindow::createAction_Tetransform()
{
	bool_fuseAction_ = new QAction("Fuse Object");
	bool_fuseAction_->setStatusTip(tr("Fuse more than one Object to a one Object"));
	bool_fuseAction_->setIcon(QIcon(":images/images/BoolFuse.png"));
	connect(bool_fuseAction_, SIGNAL(triggered()), this, SLOT(slotBoolFuseObject()));

	bool_cutAction_ = new QAction("Cut Object");
	bool_cutAction_->setStatusTip(tr("one object Cut other more than one Object"));
	bool_cutAction_->setIcon(QIcon(":images/images/BoolCut.png"));
	connect(bool_cutAction_, SIGNAL(triggered()), this, SLOT(slotBoolCutObject()));

	bool_CommonAction_ = new QAction("Common Object");
	bool_CommonAction_->setStatusTip(tr("find more than one Object' common Object"));
	bool_CommonAction_->setIcon(QIcon(":images/images/BoolCommon.png"));
	connect(bool_CommonAction_, SIGNAL(triggered()), this, SLOT(slotBoolCommonObject()));


	duplicateAction_ = new QAction("duplicate");
}

void ACAMWindow::createAction_Topology()
{
    chamferAction_ = new QAction("Chamfer Edges");
    chamferAction_->setStatusTip(tr("select an edge to chamfer"));
    chamferAction_->setIcon(QIcon(":images/images/Chamfer.png"));
    connect(chamferAction_, &QAction::triggered, this, &ACAMWindow::slotChamfer);

    filletAction_ = new QAction("Fillet Edges");
    filletAction_->setStatusTip(tr("select an edge to fillet"));
    filletAction_->setIcon(QIcon(":images/images/fillet.png"));
    connect(filletAction_, &QAction::triggered, this, &ACAMWindow::slotFillet);

    extrudAction_ = new QAction("Extrude Face");
    extrudAction_->setStatusTip(tr("select a face to extrude"));
    extrudAction_->setIcon(QIcon(":images/images/ExtrudeFace.png"));
    connect(extrudAction_, &QAction::triggered, render_viewer_, &RenderViewer::slotBRepExtrude);
}

//================================================================

void ACAMWindow::createMenus()
{
    fileMenu_ = menuBar()->addMenu(tr("&File"));
    fileMenu_->addAction(openAction_);
    fileMenu_->addAction(saveAction_);
    fileMenu_->addAction(exportAction_); 
    fileMenu_->addSeparator();
    fileMenu_->addAction(exitAction_);

    editMenu_ = menuBar()->addMenu(tr("Edit"));
    editMenu_->addAction(undoAction_);
    editMenu_->addAction(redoAction_);

    helpMenu_ = menuBar()->addMenu(tr("Help"));
    helpMenu_->addAction(aboutAction_);
}

    void ACAMWindow::createToolBars()
{
//=============================================================
	QToolBar* editToolBar_ = new QToolBar(this);
    editToolBar_->setObjectName(QString::fromUtf8("Edit"));
    this->addToolBar(Qt::LeftToolBarArea, editToolBar_);

    editToolBar_->addAction(chamferAction_);
    editToolBar_->addAction(filletAction_);
    editToolBar_->addAction(extrudAction_); 

    editToolBar_->addSeparator();
    editToolBar_->addAction(bool_fuseAction_);
    editToolBar_->addAction(bool_cutAction_);
    editToolBar_->addAction(bool_CommonAction_);
}

//================================================================
#include <QStandardPaths>
#include <QDir>
#include <QPixmap>
#include <QMessageBox>
#include <QString>
#include <QTextCodec>    

//================================================================

void ACAMWindow::slotBoolFuseObject()
{
	BaseObjectOperation(SelectModel::OBJECT_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshFuse);
}

void ACAMWindow::slotBoolCutObject()
{
	BaseObjectOperation(SelectModel::OBJECT_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshCut);
}

void ACAMWindow::slotBoolCommonObject()
{
	BaseObjectOperation(SelectModel::OBJECT_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshCommon);
}

void ACAMWindow::slotChamfer()
{
	BaseObjectOperation(SelectModel::EDGE_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshChamferEdge);
}

void ACAMWindow::slotFillet()
{
    BaseObjectOperation(SelectModel::EDGE_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshFilletEdge);
}

//==========================================================================


void ACAMWindow::BaseObjectOperation(const SelectModel& s_model, const OperationType& op_type, const MeshOperationType mesh_operate_type)
{
    render_viewer_->clearSelected();
    render_viewer_->setSelectModel(s_model);
    render_viewer_->setOperateType(op_type);

	core_->setMeshOperationType(mesh_operate_type);
    render_viewer_->selectCheckChanged();
    render_viewer_->update();
}

void ACAMWindow::BaseObjectOperationSelect(const SelectModel& s_model, const OperationType& op_type, const MeshOperationType mesh_operate_type)
{
    render_viewer_->setSelectModel(s_model);
    render_viewer_->setOperateType(op_type);

	core_->setMeshOperationType(mesh_operate_type);

    render_viewer_->selectCheckChanged();
    render_viewer_->update();
}
