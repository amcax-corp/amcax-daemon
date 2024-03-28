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
#pragma once

#include <QMainWindow>
#include "ui_ACAMWindow.h"
#include <list>

#include "../Core/CoreDefine.h"


namespace acamcad {
	class RenderViewer;
	class DataManager;
	class BackupManager;
	class AMCore;
	class OperateToolBar;
	class UndoRedoHandler;
}

class ACAMWindow : public QMainWindow
{
	Q_OBJECT

public:
	ACAMWindow(QWidget* parent = Q_NULLPTR);
	~ACAMWindow();

	//static void ShowMessge(const QString& text, int type);

private:
	void initCoreAndViewer();

	void createMenus();
	void createToolBars();

	void createAction();

	void createAction_File();
	void createAction_Edit();
	void createAction_Help();

	void createAction_LeftTools();
	//void createAction_Topology();

	//void refresh_LeftTools();

private: // menu entries
	// file menu
	QAction* openAction_;
	QAction* saveAction_;
	QAction* exportAction_;
	QAction* exitAction_;
	// file menu

	// edit menu
	QAction* undoAction_;
	QAction* redoAction_;
	// edit menu

	// help menu
	QAction* aboutAction_;
	// help menu

	// menu bar
	QMenu* fileMenu_;
	QMenu* editMenu_;
	QMenu* helpMenu_;
	// menu bar

private:

	QAction* duplicateAction_;

private:


private:

	QAction* chamferAction_;
	QAction* embedAction_;
	QAction* separateAction_;
	QAction* weldAction_;
	QAction* deleteFaceAction_;
	QAction* filletAction_;
	QAction* extrudAction_;
	QAction* extrudActionMesh_;

	QAction* splitFaceEdgeAction_;
	QAction* splitEdgeAction_;
	QAction* splitLoopAction_;
	QAction* splitFaceAction_;
	QAction* splitFaceVertAction_;
	QAction* collapseEdgeAction_;
	QAction* swapEdgeAction_;

	QAction* bridgeEdgeAction_;
	QAction* extrudeEdgeVAction_;
	QAction* extrudeEdgeHAction_;
	QAction* thickenAction_;
	QAction* normalAction_;
	QAction* fillHoleAction_;

	QAction* bool_fuseAction_;
	QAction* bool_CommonAction_;
	QAction* bool_cutAction_;

private:
	QToolBar* editToolBar_;


private slots:
	void slotChamfer();
	void slotFillet();
	void slotEmbedFace();
	void slotDeleteElement();
	void slotSeparateEdge();
	void slotWeldEdge();


	void slotSplitEdge();
	void slotSplitLoop();
	void slotSplitFace();
	void slotSplitFaceByEdge();


	void slotBoolFuseObject();
	void slotBoolCutObject();
	void slotBoolCommonObject();
	void slotSplitFaceByVert();
	void slotSwapMeshEdge();
	void slotCombineTwoFace();
	void slotCollapseEdge();

	void slotBridgeEdge();
	void slotExtrudeEdgeV();
	void slotExtrudeEdgeH();
	void slotThicken();
	void slotMeshReverse();
	void slotFillHole();
	void slotExtrudeFace();

	//
	void slotCurrentChanged(int index);


private:
	void BaseObjectOperation(const acamcad::SelectModel& s_model,
		const acamcad::OperationType& op_type, const acamcad::MeshOperationType mesh_operate_type);
	void BaseObjectOperationSelect(const acamcad::SelectModel& s_model,
		const acamcad::OperationType& op_type, const acamcad::MeshOperationType mesh_operate_type);

	std::vector<QAction*> list_actions_;

private:
	Ui::ACAMWindow ui;

	acamcad::RenderViewer* render_viewer_;

	acamcad::DataManager* dataManager_;
	acamcad::AMCore* core_;
	//acamcad::UndoRedoHandler* undoRedoHandler_; 
};
