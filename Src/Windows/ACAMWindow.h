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
	ACAMWindow(QWidget *parent = Q_NULLPTR);
	~ACAMWindow();

private:
	void initCoreAndViewer();
	
	void createMenus();
	void createToolBars();

	void createAction();

	void createAction_File();
	void createAction_Edit();
	void createAction_Help();

	void createAction_Tetransform();	
	void createAction_Topology();

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
	QAction* filletAction_;
	QAction* extrudAction_; 


	QAction* bool_fuseAction_;
	QAction* bool_CommonAction_;
	QAction* bool_cutAction_;

private:
	QToolBar* editToolBar_;


private slots:
	void slotChamfer();
	void slotFillet();

private slots:
	void slotBoolFuseObject();
	void slotBoolCutObject();
	void slotBoolCommonObject();

private:
	void BaseObjectOperation(const acamcad::SelectModel& s_model, 
		const acamcad::OperationType& op_type, const acamcad::MeshOperationType mesh_operate_type);
	void BaseObjectOperationSelect(const acamcad::SelectModel& s_model,
		const acamcad::OperationType& op_type, const acamcad::MeshOperationType mesh_operate_type);

private:
	Ui::ACAMWindow ui;
    
    acamcad::RenderViewer* render_viewer_;

	acamcad::DataManager* dataManager_;
	acamcad::AMCore* core_; 
	acamcad::UndoRedoHandler* undoRedoHandler_; 
};
