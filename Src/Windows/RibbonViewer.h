#pragma once
#include "SARibbonMainWindow.h"
class SARibbonCategory;
class SARibbonContextCategory;
class SARibbonCustomizeWidget;
class SARibbonActionsManager;
class SARibbonQuickAccessBar;
class SARibbonButtonGroupWidget;
class SARibbonPannel;
class QTextEdit;
class QComboBox;
class QCloseEvent;
class QLineEdit;
#include <qtoolbar.h>


#include <memory>
#include "../Core/CoreDefine.h"
#include "../Core/ACAMCore.h"
#include "../Core/DataManager.h"
#include "RenderViewer.h"


namespace acamcad {
	class RenderViewer;
	class DataManager;
	//class BackupManager;
	class AMCore;
	//class OperateToolBar;
	//class UndoRedoHandler;
}

class RibbonViewer :public SARibbonMainWindow, public RenderViewerListen, public AMCoreListener
{
	Q_OBJECT
public:
	RibbonViewer(QWidget* ptr = nullptr);
	~RibbonViewer();

	// RenderViewerListen
	virtual void refresh_view(acamcad::DrawModel model) override;
	virtual void refresh_select(acamcad::SelectModel model) override;

	//AMCoreListener
	virtual void refresh_title(QString title);
	virtual void refresh_file(QString title);


protected:
	QAction* createAction(const QString& text, const QString& iconurl, const QString& objName);
	QAction* createAction(const QString& text, const QString& iconurl);

	void initQuickBar();
	void initData();
	void initRender();
	void refreshQuickBar();
	void initFile();
	void initTMSpline();
	void initToolBar();
	void refreshToolBar();
	void initSubD();
	void initBRrep();
	void initAnalysis();
	void initHelper();


	void BaseObjectOperation(const acamcad::SelectModel& s_model,
		const acamcad::OperationType& op_type, const acamcad::MeshOperationType mesh_operate_type);
	void BaseObjectOperationSelect(const acamcad::SelectModel& s_model,
		const acamcad::OperationType& op_type, const acamcad::MeshOperationType mesh_operate_type);

	//QMainWindow;
	virtual void resizeEvent(QResizeEvent* event) override;

	void refreshFileRecently();
	void refreshSize();

private:
	acamcad::RenderViewer* render_viewer_;
	std::unique_ptr<acamcad::DataManager> dataManager_;
	std::unique_ptr<acamcad::AMCore> core_;

	//UI
	QAction* action_undo_;
	QAction* action_redo_;
	QToolBar* toolbar_;

	QToolButton* button_select_;
	QToolButton* button_view_;

	QMenu* menu_file_recently_;
	QAction* action_file_recently_;
};

