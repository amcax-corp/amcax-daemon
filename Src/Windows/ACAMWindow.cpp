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
#include <QStatusBar>

#include "RenderViewer.h"
#include "../Core/DataManager.h"
#include "../Core/ACAMCore.h"
#include "OperateToolbar.h"
#include "../Core/UndoRedoHandler.h"

using namespace acamcad;


ACAMWindow::ACAMWindow(QWidget* parent)
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
	connect(tab_widget, &OperateToolBar::currentChanged, this, &ACAMWindow::slotCurrentChanged);

	render_viewer_->selectCheckChanged();

	QVBoxLayout* layout_main = new QVBoxLayout;

	size_t bar_height = tab_widget->tabBar()->height();

	tab_widget->setMaximumHeight(icon_size + bar_height + 2);

	layout_main->addWidget(tab_widget, 0, Qt::AlignTop);
	layout_main->addWidget(render_viewer_, 10);

	this->centralWidget()->setLayout(layout_main);

	this->resize(1280, 750);

	slotCurrentChanged(1);

	//instance = this;

	//ShowMessge("Selet dd", acamcad::DataType::BREP_TYPE | acamcad::DataType::TSPLINEU_TYPE);

	///statusBar()->showMessage("XXXXXXXXXXXXXXXXXXX!!!");

}

ACAMWindow::~ACAMWindow()
{
	delete render_viewer_;
	delete dataManager_;
	//delete undoRedoHandler_;
	delete core_;
}

void ACAMWindow::initCoreAndViewer()
{
	dataManager_ = new DataManager();

	//undoRedoHandler_ = new UndoRedoHandler(dataManager_, 50); 

	core_ = new acamcad::AMCore();
	core_->setDataManager(dataManager_);
	//core_->setUndoRedoHandler(undoRedoHandler_);

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
	createAction_LeftTools();
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


	//undoAction_->setEnabled(false);
}

void ACAMWindow::createAction_Help()
{
	aboutAction_ = new QAction(tr("About"));
	aboutAction_->setStatusTip("About us");
	// TODO connect
}


void ACAMWindow::createAction_LeftTools()
{
	chamferAction_ = new QAction("Chamfer Edges");
	chamferAction_->setStatusTip(tr("(Support Brep)select an edge to chamfer"));
	chamferAction_->setIcon(QIcon(":images/images/Chamfer.png"));
	connect(chamferAction_, &QAction::triggered, this, &ACAMWindow::slotChamfer);

	deleteFaceAction_ = new QAction("Delete Face");
	deleteFaceAction_->setStatusTip(tr("(Support Mesh)Delete a face from mesh, the define is face, Enter sure"));
	deleteFaceAction_->setIcon(QIcon(":images/images/DeleteFace.png"));
	connect(deleteFaceAction_, &QAction::triggered, this, &ACAMWindow::slotDeleteElement);

	embedAction_ = new QAction("Embed face");
	embedAction_->setStatusTip(
		tr("(Support Mesh)Embed a face to mesh, select connected boundary vert or connected boundary edge, enter sure"));
	embedAction_->setIcon(QIcon(":images/images/EmbedFace.png"));
	connect(embedAction_, &QAction::triggered, this, &ACAMWindow::slotEmbedFace);


	separateAction_ = new QAction("Separate");
	deleteFaceAction_->setStatusTip(tr("(Support Mesh)Sperate a face from mesh , select vertex or edge, Enter sure"));
	separateAction_->setIcon(QIcon(":images/images/Separate.png"));
	connect(separateAction_, &QAction::triggered, this, &ACAMWindow::slotSeparateEdge);

	weldAction_ = new QAction("Weld");
	deleteFaceAction_->setStatusTip(tr("(Support Mesh)Weld a edge to mesh , select edge, Enter sure"));
	weldAction_->setIcon(QIcon(":images/images/Weld.png"));
	connect(weldAction_, &QAction::triggered, this, &ACAMWindow::slotWeldEdge);



	splitLoopAction_ = new QAction("Split loop");
	splitLoopAction_->setStatusTip(tr("Split a mesh loop"));
	splitLoopAction_->setIcon(QIcon(":images/images/SplitLoop.png"));
	connect(splitLoopAction_, &QAction::triggered, this, &ACAMWindow::slotSplitLoop);

	//splitEdgeAction_ = new QAction("Split edge");
	//splitEdgeAction_->setStatusTip(tr("Split a edge"));
	//splitEdgeAction_->setIcon(QIcon(":images/images/SplitEdge.png"));
	//connect(splitEdgeAction_, &QAction::triggered, this, &ACAMWindow::slotSplitEdge);
	splitFaceAction_ = new QAction("split face");
	splitFaceAction_->setStatusTip(tr("split face cross"));
	splitFaceAction_->setIcon(QIcon(":images/images/SplitFace1.png"));
	connect(splitFaceAction_, &QAction::triggered, this, &ACAMWindow::slotSplitFace);


	splitFaceEdgeAction_ = new QAction("Split Face by Edge");
	splitFaceEdgeAction_->setStatusTip(tr("Split Face by select some edge"));
	splitFaceEdgeAction_->setIcon(QIcon(":images/images/SplitEdgeOnly.png"));
	connect(splitFaceEdgeAction_, &QAction::triggered, this, &ACAMWindow::slotSplitFaceByEdge);


	splitFaceVertAction_ = new QAction("Cut face");
	splitFaceVertAction_->setStatusTip(tr("Cat the face by select vert"));
	splitFaceVertAction_->setIcon(QIcon(":images/images/SplitFace.png"));
	connect(splitFaceVertAction_, &QAction::triggered, this, &ACAMWindow::slotSplitFaceByVert);

	//collapseEdgeAction_ = new QAction("combine Vert");
	//collapseEdgeAction_->setStatusTip(tr("Combine ywo Vertex"));
	//collapseEdgeAction_->setIcon(QIcon(":images/images/CollapseEdge.png"));
	//connect(collapseEdgeAction_, &QAction::triggered, this, &ACAMWindow::slotCollapseEdge);

	//swapEdgeAction_ = new QAction("Swap Edge");
	//swapEdgeAction_->setStatusTip(tr("Swap a mesh edge"));
	//swapEdgeAction_->setIcon(QIcon(":images/images/SwapEdge.png"));
	//connect(swapEdgeAction_, &QAction::triggered, this, &ACAMWindow::slotSwapMeshEdge);


	bridgeEdgeAction_ = new QAction("Bridge edge");
	bridgeEdgeAction_->setStatusTip(tr("Bridge to edge, select two not connected boudnary edge, enter sure"));
	bridgeEdgeAction_->setIcon(QIcon(":images/images/BridgeEdge.png"));
	connect(bridgeEdgeAction_, &QAction::triggered, this, &ACAMWindow::slotBridgeEdge);

	extrudeEdgeVAction_ = new QAction("Extrude edge Vertical");
	extrudeEdgeVAction_->setStatusTip(tr("Select some edge, use Ctrl+left to extrude edge, direction vartical face"));
	extrudeEdgeVAction_->setIcon(QIcon(":images/images/ExtrudeEdgeVertical.png"));
	connect(extrudeEdgeVAction_, &QAction::triggered, this, &ACAMWindow::slotExtrudeEdgeV);

	extrudeEdgeHAction_ = new QAction("Extrude edge Horizontal");
	extrudeEdgeHAction_->setStatusTip(tr("Select some edge, use Ctrl+left to extrude edge, direction horizontal face"));
	extrudeEdgeHAction_->setIcon(QIcon(":images/images/ExtrudeEdgeHorizontal.png"));
	connect(extrudeEdgeHAction_, &QAction::triggered, this, &ACAMWindow::slotExtrudeEdgeH);

	//chamferAction_->setVisible(false);

	thickenAction_ = new QAction("Thicken Mesh");
	thickenAction_->setStatusTip(tr("Thicken a mesh"));
	thickenAction_->setIcon(QIcon(":images/images/Thicken.png"));
	connect(thickenAction_, &QAction::triggered, this, &ACAMWindow::slotThicken);

	normalAction_ = new QAction("Reverse Mesh Normal");
	normalAction_->setStatusTip(tr("Reverse Mesh Normal"));
	normalAction_->setIcon(QIcon(":images/images/ReverseNormal.png"));
	connect(normalAction_, &QAction::triggered, this, &ACAMWindow::slotMeshReverse);

	fillHoleAction_ = new QAction("FillHole");
	fillHoleAction_->setStatusTip(tr("Try to FillHole"));
	fillHoleAction_->setIcon(QIcon(":images/images/FillHole.png"));
	connect(fillHoleAction_, &QAction::triggered, this, &ACAMWindow::slotFillHole);

	filletAction_ = new QAction("Fillet Edges");
	filletAction_->setStatusTip(tr("(Support Brep)select an edge to fillet"));
	filletAction_->setIcon(QIcon(":images/images/fillet.png"));
	connect(filletAction_, &QAction::triggered, this, &ACAMWindow::slotFillet);

	extrudAction_ = new QAction("Extrude Face");
	extrudAction_->setStatusTip(tr("(Support Brep)select a face to extrude(Support Mesh)"));
	extrudAction_->setIcon(QIcon(":images/images/ExtrudeFace.png"));
	connect(extrudAction_, &QAction::triggered, render_viewer_, &RenderViewer::slotBRepExtrude);

	extrudActionMesh_ = new QAction("Extrude Face Mesh");
	extrudActionMesh_->setStatusTip(tr("(Support Brep)select a face to extrude(Support Mesh)"));
	extrudActionMesh_->setIcon(QIcon(":images/images/ExtrudeFace.png"));
	connect(extrudActionMesh_, &QAction::triggered, this, &ACAMWindow::slotExtrudeFace);

	bool_fuseAction_ = new QAction("Fuse Object");
	bool_fuseAction_->setStatusTip(tr("(Support Brep)Fuse more than one Object to a one Object"));
	bool_fuseAction_->setIcon(QIcon(":images/images/BoolFuse.png"));
	connect(bool_fuseAction_, SIGNAL(triggered()), this, SLOT(slotBoolFuseObject()));

	bool_cutAction_ = new QAction("Cut Object");
	bool_cutAction_->setStatusTip(tr("(Support Brep)one object Cut other more than one Object"));
	bool_cutAction_->setIcon(QIcon(":images/images/BoolCut.png"));
	connect(bool_cutAction_, SIGNAL(triggered()), this, SLOT(slotBoolCutObject()));

	bool_CommonAction_ = new QAction("Common Object");
	bool_CommonAction_->setStatusTip(tr("(Support Brep)find more than one Object' common Object"));
	bool_CommonAction_->setIcon(QIcon(":images/images/BoolCommon.png"));
	connect(bool_CommonAction_, SIGNAL(triggered()), this, SLOT(slotBoolCommonObject()));


	duplicateAction_ = new QAction("duplicate");
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



	editToolBar_->addAction(deleteFaceAction_);
	list_actions_.push_back(deleteFaceAction_);
	editToolBar_->addAction(embedAction_);
	list_actions_.push_back(embedAction_);
	editToolBar_->addAction(separateAction_);
	list_actions_.push_back(separateAction_);
	editToolBar_->addAction(weldAction_);
	list_actions_.push_back(weldAction_);

	editToolBar_->addSeparator();

	//=====================================
	editToolBar_->addAction(splitLoopAction_);
	list_actions_.push_back(splitLoopAction_);
	editToolBar_->addAction(splitFaceAction_);
	list_actions_.push_back(splitFaceAction_);
	//editToolBar_->addAction(splitEdgeAction_);
	editToolBar_->addAction(splitFaceEdgeAction_);
	list_actions_.push_back(splitFaceEdgeAction_);
	editToolBar_->addAction(splitFaceVertAction_);
	list_actions_.push_back(splitFaceVertAction_);
	//editToolBar_->addAction(collapseEdgeAction_);
	//editToolBar_->addAction(swapEdgeAction_);

	editToolBar_->addAction(chamferAction_);
	list_actions_.push_back(chamferAction_);
	editToolBar_->addAction(filletAction_);
	list_actions_.push_back(filletAction_);
	editToolBar_->addAction(bridgeEdgeAction_);
	list_actions_.push_back(bridgeEdgeAction_);
	editToolBar_->addAction(extrudeEdgeVAction_);
	list_actions_.push_back(extrudeEdgeVAction_);
	editToolBar_->addAction(extrudeEdgeHAction_);
	list_actions_.push_back(extrudeEdgeHAction_);
	editToolBar_->addAction(extrudAction_);
	list_actions_.push_back(extrudAction_);
	editToolBar_->addAction(extrudActionMesh_);
	list_actions_.push_back(extrudActionMesh_);

	editToolBar_->addSeparator();

	editToolBar_->addAction(thickenAction_);
	list_actions_.push_back(thickenAction_);
	editToolBar_->addAction(normalAction_);
	list_actions_.push_back(normalAction_);
	editToolBar_->addAction(fillHoleAction_);
	list_actions_.push_back(fillHoleAction_);


	editToolBar_->addAction(bool_fuseAction_);
	list_actions_.push_back(bool_fuseAction_);
	editToolBar_->addAction(bool_cutAction_);
	list_actions_.push_back(bool_cutAction_);
	editToolBar_->addAction(bool_CommonAction_);
	list_actions_.push_back(bool_CommonAction_);
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

void ACAMWindow::slotEmbedFace()
{
	BaseObjectOperation(SelectModel::VERTEX_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshEmbedFace);
}

void ACAMWindow::slotDeleteElement()
{
	BaseObjectOperation(SelectModel::FACE_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshDeleteElement);
}

void ACAMWindow::slotSeparateEdge()
{
	BaseObjectOperation(SelectModel::EDGE_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshSeparate);
}

void ACAMWindow::slotWeldEdge()
{
	BaseObjectOperation(SelectModel::EDGE_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshWeldEdge);
}

void ACAMWindow::slotFillet()
{
	BaseObjectOperation(SelectModel::EDGE_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshFilletEdge);
}


void ACAMWindow::slotSplitEdge()
{
	BaseObjectOperation(SelectModel::EDGE_MODEL, OperationType::PICK_OPERATION_MOVE, MeshOperationType::MeshSplitEdge);
}

void ACAMWindow::slotSplitLoop()
{
	BaseObjectOperation(SelectModel::EDGE_MODEL, OperationType::PICK_OPERATION_MOVE, MeshOperationType::MeshSplitLoop);
}

void ACAMWindow::slotSplitFace()
{
	BaseObjectOperation(SelectModel::FACE_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshSplitFace);
}

void ACAMWindow::slotSplitFaceByEdge()
{
	BaseObjectOperation(SelectModel::EDGE_MODEL, OperationType::PICK_OPERATION_MOVE_MULTI, MeshOperationType::MeshSplitFaceByEdge);
}


void ACAMWindow::slotSplitFaceByVert()
{
	BaseObjectOperation(SelectModel::VERTEX_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshSplitFaceVertex);
}


void ACAMWindow::slotSwapMeshEdge()
{
	BaseObjectOperation(SelectModel::EDGE_MODEL, OperationType::PICK_OPERATION_SINGLE, MeshOperationType::MeshSwapEdge);
}

void ACAMWindow::slotCombineTwoFace()
{
	BaseObjectOperation(SelectModel::EDGE_MODEL, OperationType::PICK_OPERATION_SINGLE, MeshOperationType::MeshCombineFace);
}

void ACAMWindow::slotCollapseEdge()
{
	BaseObjectOperation(SelectModel::VERTEX_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshCollapseEdge);
}

void ACAMWindow::slotBridgeEdge()
{
	BaseObjectOperation(SelectModel::EDGE_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshBridgeEdge);
}

void ACAMWindow::slotExtrudeEdgeV()
{
	BaseObjectOperationSelect(SelectModel::EDGE_MODEL, OperationType::CTRL_OPERATION, MeshOperationType::MeshExtrudeEdgeVertical);
}

void ACAMWindow::slotExtrudeEdgeH()
{
	BaseObjectOperationSelect(SelectModel::EDGE_MODEL, OperationType::CTRL_OPERATION, MeshOperationType::MeshExtrudeEdgeHorizontal);
}


void ACAMWindow::slotThicken()
{
	BaseObjectOperation(SelectModel::OBJECT_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshThicken);
}

void ACAMWindow::slotMeshReverse()
{
	BaseObjectOperation(SelectModel::OBJECT_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshReverseNormal);
}

void ACAMWindow::slotFillHole()
{
	BaseObjectOperation(SelectModel::EDGE_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshFillHole);
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
	if (render_viewer_->getSelectModel() != s_model)
		render_viewer_->clearSelected();

	render_viewer_->setSelectModel(s_model);
	render_viewer_->setOperateType(op_type);

	core_->setMeshOperationType(mesh_operate_type);

	render_viewer_->selectCheckChanged();
	render_viewer_->update();
}
void ACAMWindow::slotExtrudeFace()
{
	BaseObjectOperationSelect(SelectModel::FACE_MODEL, OperationType::CTRL_OPERATION, MeshOperationType::MeshExtrudeFaceNew);
}

void ACAMWindow::slotCurrentChanged(int index)
{
	std::vector<std::vector<int>> array = {
		{
			//true,true,true,true,true,
			//true,true,true,true,true,
			//true,true,true,true,true,
			//true,true,true,true,true,
			//true
		},
		//BRep
		{
		false,false,false,false,false,
		false,false,false,true,true,
		false,false,false,true,false,
		false,false,false,true,true,
		true
		},
		//Mesh
		{
		true,true,true,true,true ,
		false,true,true,false,false,
		true,true,true,false,true,
		true,true,true,false,false,
		false
		},
		//TSpline
		{
		true,true,true,true ,true,
		true,true,true,true,false,
		true,true,true,false,true,
		true,true,true,false,false,
		false
		},
	};

	for (int i = 0; i < array[index].size(); i++)
	{
		list_actions_[i]->setVisible(array[index][i]);
	}


	///std::cout << "ACAMWindow::slotCurrentChanged!!" << list_actions_.size() << std::endl;
}