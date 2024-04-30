#include "RibbonViewer.h"
//#if !SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
#include "SAFramelessHelper.h"
//#endif
#include "SARibbonApplicationButton.h"
#include "SARibbonBar.h"
#include "SARibbonButtonGroupWidget.h"
#include "SARibbonCategory.h"
#include "SARibbonCheckBox.h"
#include "SARibbonColorToolButton.h"
#include "SARibbonComboBox.h"
#include "SARibbonCustomizeDialog.h"
#include "SARibbonCustomizeWidget.h"
#include "SARibbonGallery.h"
#include "SARibbonLineEdit.h"
#include "SARibbonMenu.h"
#include "SARibbonPannel.h"
#include "SARibbonQuickAccessBar.h"
#include "SARibbonToolButton.h"
#include "colorWidgets/SAColorGridWidget.h"
#include "colorWidgets/SAColorPaletteGridWidget.h"
#include "SARibbonSystemButtonBar.h"
#include <QAbstractButton>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QCalendarWidget>
#include <QDebug>
#include <QElapsedTimer>
#include <QFile>
#include <QFileDialog>
#include <QFontComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QStatusBar>
#include <QTextEdit>
#include <QTextStream>
#include <QXmlStreamWriter>
#include <QMessageBox>
#include <QShortcut>
#include <QLineEdit>
#include <QDialogButtonBox>
#include "RenderViewer.h"
#include "../Core/DataManager.h"
#include "../Core/ACAMCore.h"
#include <qtextcodec.h>
#include <functional>
#include <qtoolbar.h>
#include <QDateTime>

using namespace acamcad;

namespace
{


#define PRINT_COST_START()                                                                                             \
    QElapsedTimer __TMP_COST;                                                                                          \
    __TMP_COST.start();                                                                                                \
    int __TMP_LASTTIMES = 0

#define PRINT_COST(STR)                                                                                                \
    do {                                                                                                               \
        int ___TMP_INT = __TMP_COST.elapsed();                                                                         \
        qDebug() << STR << " cost " << ___TMP_INT - __TMP_LASTTIMES << " ms (" << ___TMP_INT << ")";                   \
        mTextedit->append(QString("%1 cost %2 ms(%3)").arg(STR).arg(___TMP_INT - __TMP_LASTTIMES).arg(___TMP_INT));    \
        __TMP_LASTTIMES = ___TMP_INT;                                                                                  \
    } while (0)

}

RibbonViewer::RibbonViewer(QWidget* ptr) : SARibbonMainWindow(ptr)
{

	//Window
	setMinimumSize(QSize(1280, 850));
	setWindowIcon(QIcon(":images/images/logo.ico"));
	setStatusBar(new QStatusBar);
	ribbonBar()->setContentsMargins(0, 0, 0, 0);
	//setWindowTitle("九韶精灵");
	refresh_title(QString());


	initData();
	initRender();

	initQuickBar();
	initFile();
	initTMSpline();
	initSubD();
	initBRrep();
	initAnalysis();
	initHelper();
	initToolBar();

	refreshSize();
	///show();
}

RibbonViewer::~RibbonViewer()
{
	acamcad::LocalDataManager::DataModelPosition data;

	data.array = saveGeometry();

	dataManager_->execStore(data);
}

void RibbonViewer::refresh_view(acamcad::DrawModel model)
{
	switch (model)
	{
	case acamcad::DrawModel::WIRE_MODE:
		button_view_->setIcon(QIcon(":/icon/icon/wireframe.svg"));
		button_view_->setToolTip(tr("线框"));
		break;
	case acamcad::DrawModel::SHADING_MODEL:
		button_view_->setIcon(QIcon(":/icon/icon/smooth.svg"));
		button_view_->setToolTip(tr("渲染"));
		break;
	case acamcad::DrawModel::RENDERING_MODEL:
		button_view_->setIcon(QIcon(":/icon/icon/render.svg"));
		button_view_->setToolTip(tr("平滑"));
		break;
	default:
		break;
	}
}
void RibbonViewer::refresh_select(acamcad::SelectModel model)
{
	switch (model)
	{
	case acamcad::SelectModel::VERTEX_MODEL:
		button_select_->setIcon(QIcon(":/icon/icon/vertex-select.svg"));
		button_select_->setToolTip(tr("点选择"));
		break;
	case acamcad::SelectModel::EDGE_MODEL:
		button_select_->setIcon(QIcon(":/icon/icon/line-select.svg"));
		button_select_->setToolTip(tr("线选择"));
		break;
	case acamcad::SelectModel::FACE_MODEL:
		button_select_->setIcon(QIcon(":/icon/icon/face-select.svg"));
		button_select_->setToolTip(tr("面选择"));
		break;
	case acamcad::SelectModel::OBJECT_MODEL:
		button_select_->setIcon(QIcon(":/icon/icon/object-select.svg"));
		button_select_->setToolTip(tr("体选择"));
		break;
	default:
		break;
	}
}

void RibbonViewer::refresh_title(QString title)
{
	if (title.isEmpty())
		setWindowTitle(tr("九韶精灵"));
	else {
		QFileInfo info(title);
		setWindowTitle(QString("%1 - 九韶精灵").arg(info.fileName()));
	}
}

void RibbonViewer::refresh_file(QString title)
{
	qint64 time = QDateTime::currentDateTime().toMSecsSinceEpoch();
	std::vector<acamcad::LocalDataManager::DataModelRecent> vec;

	vec.push_back({ title,time });

	dataManager_->execStore(vec);

	refreshFileRecently();
}

QAction* RibbonViewer::createAction(const QString& text, const QString& iconurl, const QString& objName)
{
	QAction* act = new QAction(this);
	act->setText(text);
	act->setIcon(QIcon(iconurl));
	act->setObjectName(objName);
	return act;
}

QAction* RibbonViewer::createAction(const QString& text, const QString& iconurl)
{
	QAction* act = new QAction(this);
	act->setText(text);
	act->setIcon(QIcon(iconurl));
	act->setObjectName(text);

	return act;
}

void RibbonViewer::initQuickBar()
{
	SARibbonQuickAccessBar* quickAccessBar = ribbonBar()->quickAccessBar();
	ribbonBar()->showMinimumModeButton(true);
	//ribbonBar()->activeRightButtonGroup();

	QAction* p = createAction(tr("保存"), ":/icon/icon/save-small.svg", "save-quickbar");
	connect(p, &QAction::triggered, core_.get(), QOverload<>::of(&AMCore::saveObjects));
	quickAccessBar->addAction(p);
	quickAccessBar->addSeparator();

	action_undo_ = createAction(tr("撤销"), ":/icon/icon/undo.svg");
	action_undo_->setShortcut(QKeySequence("Ctrl+z"));
	action_undo_->setShortcutContext(Qt::ApplicationShortcut);
	connect(action_undo_, &QAction::triggered, render_viewer_, &RenderViewer::slotUndo);
	quickAccessBar->addAction(action_undo_);

	action_redo_ = createAction(tr("重做"), ":/icon/icon/redo.svg");
	action_redo_->setShortcut(QKeySequence("Ctrl+y"));
	action_redo_->setShortcutContext(Qt::ApplicationShortcut);
	connect(action_redo_, &QAction::triggered, render_viewer_, &RenderViewer::slotRedo);
	quickAccessBar->addAction(action_redo_);

	refreshQuickBar();

}

void RibbonViewer::initData()
{
	//render_viewer_ = new acamcad::RenderViewer(this);
	dataManager_ = std::make_unique<DataManager>();
	dataManager_->setFunc(std::bind(&RibbonViewer::refreshQuickBar, this));

	core_ = std::make_unique<AMCore>();
	core_->setDataManager(dataManager_.get());
	core_->setListener(this);

}

void RibbonViewer::initRender()
{
	render_viewer_ = new acamcad::RenderViewer(this);
	render_viewer_->setListen(this);
	render_viewer_->setDataManger(dataManager_.get());
	render_viewer_->setCore(core_.get());


	setCentralWidget(render_viewer_);
}

void RibbonViewer::refreshQuickBar()
{
	action_undo_->setEnabled(dataManager_->canUndo());
	action_redo_->setEnabled(dataManager_->canRedo());
}

void RibbonViewer::initFile()
{
	SARibbonBar* ribbon = ribbonBar();

	auto btn = ribbon->applicationButton();
	SARibbonApplicationButton* appBtn = qobject_cast<SARibbonApplicationButton*>(btn);

	appBtn->setText("  文件  ");
	auto mMenuApplicationBtn = new SARibbonMenu(this);

	QAction* p = createAction("新建", ":/icon/icon/new.svg");
	connect(p, &QAction::triggered, this, [this](bool) {
		if (core_->newObjects())
		{
			dataManager_->clearSelected();
			dataManager_->clearHistory();
			dataManager_->deleteAllObject();
		}
		});
	mMenuApplicationBtn->addAction(p);

	p = createAction("打开", ":/icon/icon/open.svg");
	connect(p, &QAction::triggered, core_.get(), &AMCore::loadObjects);
	mMenuApplicationBtn->addAction(p);

	p = action_file_recently_ = createAction("最近打开文件", ":/icon/icon/open-recent.svg");
	//connect(p, &QAction::triggered, core_.get(), &AMCore::loadObjects);
	mMenuApplicationBtn->addAction(p);


	QMenu* pM = menu_file_recently_ = new QMenu(this);
	p->setMenu(pM);

	mMenuApplicationBtn->addSeparator();

	p = createAction("保存", ":/icon/icon/save.svg");
	connect(p, &QAction::triggered, core_.get(), QOverload<>::of(&AMCore::saveObjects));
	mMenuApplicationBtn->addAction(p);

	p = createAction("另存为", ":/icon/icon/save-as.svg");
	connect(p, &QAction::triggered, core_.get(), QOverload<>::of(&AMCore::saveAsObjects));
	mMenuApplicationBtn->addAction(p);

	p = createAction("保存选中", ":/icon/icon/export-selected.svg");
	connect(p, &QAction::triggered, core_.get(), &AMCore::saveSelectedObjects);
	mMenuApplicationBtn->addAction(p);


	mMenuApplicationBtn->addSeparator();
	p = createAction("退出", ":/icon/icon/exit.svg");
	connect(p, &QAction::triggered, this, &RibbonViewer::close);
	mMenuApplicationBtn->addAction(p);
	appBtn->setMenu(mMenuApplicationBtn);

	mMenuApplicationBtn->setMinimumWidth(150);
	//ribbon->setApplicationButton(btn);

	refreshFileRecently();
}

void RibbonViewer::refreshFileRecently()
{
	std::vector<acamcad::LocalDataManager::DataModelRecent> data;

	dataManager_->execGet(data);

	menu_file_recently_->clear();

	for (int i = 0; i < data.size(); i++)
	{
		auto& iter = data[i];
		QAction* p = new QAction(this);
		p->setText(QString("%1 | %2").arg(i + 1).arg(iter.file));
		connect(p, &QAction::triggered, this, [this, iter](bool) {
			core_->loadObjectsFromFile(iter.file);
			});
		menu_file_recently_->addAction(p);
	}

	action_file_recently_->setEnabled(!data.empty());

}

void RibbonViewer::initTMSpline()
{
	SARibbonCategory* category = new SARibbonCategory(this);
	category->setCategoryName("  TMSpline  ");
	category->setObjectName("categoryTMSpline");
	ribbonBar()->addCategoryPage(category);

	SARibbonPannel* pannel1 = new SARibbonPannel(tr("创建"), this);
	category->addPannel(pannel1);

	auto pannel1Button = pannel1->pannelLayout()->pannelTitleLabel();
	pannel1Button->setPopupMode(QToolButton::InstantPopup);

	auto pMenu = new SARibbonMenu(this);
	pannel1Button->setMenu(pMenu);
	pMenu->setMinimumWidth(250);

	QAction* action = createAction(tr("单一面"), ":/TMSpline/TMSpline/single-face.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotCreateSingleFaceTSpline);
	pannel1->addAction(action);
	pMenu->addAction(action);


	action = createAction(tr("平面"), ":/TMSpline/TMSpline/plane.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotCreatePlaneTSpline);
	pannel1->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("正方体"), ":/TMSpline/TMSpline/cube.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotCreateCubeTSpline);
	pannel1->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("球体"), ":/TMSpline/TMSpline/sphere.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotCreateUVSphereTSpline);
	pannel1->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("四分球"), ":/TMSpline/TMSpline/quad-sphere.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotCreateQuadSphereTSpline);
	pannel1->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("圆柱体"), ":/TMSpline/TMSpline/cylinder.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotCreateCylinderTSpline);
	pannel1->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("圆锥"), ":/TMSpline/TMSpline/cone.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotCreateConeTSpline);
	pannel1->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("圆环"), ":/TMSpline/TMSpline/torus.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotCreateTorusTSpline);
	pannel1->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("扇形"), ":/TMSpline/TMSpline/circular-arc.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotCreateCircularTSpline);
	pannel1->addAction(action);
	pMenu->addAction(action);

	SARibbonPannel* pannel2 = new SARibbonPannel("编辑", this);
	category->addPannel(pannel2);

	auto pannel2Button = pannel2->pannelLayout()->pannelTitleLabel();
	pannel2Button->setPopupMode(QToolButton::InstantPopup);

	pMenu = new SARibbonMenu(this);
	pannel2Button->setMenu(pMenu);
	pMenu->setMinimumWidth(600);

	action = createAction(tr("锐化"), ":/TMSpline/TMSpline/add-crease-edge.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotAddCreaseEdge);
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("取消锐化"), ":/TMSpline/TMSpline/remove-crease-edge.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotRemoveCreaseEdge);
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("均匀分布"), ":/TMSpline/TMSpline/make-uniform.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotTsplineUniform);
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("修复实体"), ":/TMSpline/TMSpline/repair.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotTsplineRepair);
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("倒角"), ":/TMSpline/TMSpline/chamfer.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperation(SelectModel::EDGE_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshChamferEdge);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("桥接边"), ":/TMSpline/TMSpline/brige-edge.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperation(SelectModel::EDGE_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshBridgeEdge);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("垂直\n挤出边"), ":/TMSpline/TMSpline/extrude-edge-v.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperationSelect(SelectModel::EDGE_MODEL, OperationType::CTRL_OPERATION, MeshOperationType::MeshExtrudeEdgeVertical);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("水平\n挤出边"), ":/TMSpline/TMSpline/extrude-edge-h.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperationSelect(SelectModel::EDGE_MODEL, OperationType::CTRL_OPERATION, MeshOperationType::MeshExtrudeEdgeHorizontal);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("挤出面"), ":/TMSpline/TMSpline/extrude-face.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperationSelect(SelectModel::FACE_MODEL, OperationType::CTRL_OPERATION, MeshOperationType::MeshExtrudeFaceNew);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("翻转\n法向量"), ":/TMSpline/TMSpline/reverse-normal.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperation(SelectModel::OBJECT_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshReverseNormal);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("填充洞口"), ":/TMSpline/TMSpline/fill-hole.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperation(SelectModel::EDGE_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshFillHole);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("加厚"), ":/TMSpline/TMSpline/thicken.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperation(SelectModel::OBJECT_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshThicken);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("删除面"), ":/TMSpline/TMSpline/delete-face.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperation(SelectModel::FACE_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshDeleteElement);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("嵌入面"), ":/TMSpline/TMSpline/embed-face.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperation(SelectModel::VERTEX_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshEmbedFace);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("分离"), ":/TMSpline/TMSpline/separate.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperation(SelectModel::EDGE_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshSeparate);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("缝合"), ":/TMSpline/TMSpline/weld.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperation(SelectModel::EDGE_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshWeldEdge);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("循环分割"), ":/TMSpline/TMSpline/split-loop.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperation(SelectModel::EDGE_MODEL, OperationType::PICK_OPERATION_MOVE, MeshOperationType::MeshSplitLoop);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("十字分割"), ":/TMSpline/TMSpline/split-face-cross.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperation(SelectModel::FACE_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshSplitFace);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("分割面"), ":/TMSpline/TMSpline/split-face.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperation(SelectModel::EDGE_MODEL, OperationType::PICK_OPERATION_MOVE_MULTI, MeshOperationType::MeshSplitFaceByEdge);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("切割面"), ":/TMSpline/TMSpline/cut-face.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperation(SelectModel::VERTEX_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshSplitFaceVertex);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("复制面"), ":/TMSpline/TMSpline/copy-face.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperation(SelectModel::FACE_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshCopy);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

}

void RibbonViewer::BaseObjectOperation(const SelectModel& s_model, const OperationType& op_type, const MeshOperationType mesh_operate_type)
{
	render_viewer_->clearSelected();
	render_viewer_->setSelectModel(s_model);
	render_viewer_->setOperateType(op_type);

	core_->setMeshOperationType(mesh_operate_type);
	render_viewer_->selectCheckChanged();
	render_viewer_->update();
}

void RibbonViewer::BaseObjectOperationSelect(const SelectModel& s_model, const OperationType& op_type, const MeshOperationType mesh_operate_type)
{
	if (render_viewer_->getSelectModel() != s_model)
		render_viewer_->clearSelected();

	render_viewer_->setSelectModel(s_model);
	render_viewer_->setOperateType(op_type);

	core_->setMeshOperationType(mesh_operate_type);

	render_viewer_->selectCheckChanged();
	render_viewer_->update();
}

void RibbonViewer::initToolBar()
{
	//return;
	toolbar_ = new QToolBar(this);

	addToolBar(Qt::ToolBarArea_Mask, toolbar_);

	toolbar_->setStyleSheet("QToolBar{border: none; background-color: transparent;} ");

	//bar->move(100, 300);
	toolbar_->setMovable(false);
	toolbar_->setFixedSize(QSize(405, 40));
	//toolbar_->setFixedHeight(40);
	toolbar_->setIconSize(QSize(28, 28));

	//QPushButton* btn = new QPushButton("PushBuuton", this);
	auto action = createAction(tr("移动"), ":/icon/icon/move.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		render_viewer_->setOperateType(OperationType::MOVE_OPERATION);
		});
	toolbar_->addAction(action);

	action = createAction(tr("旋转"), ":/icon/icon/rotate.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		render_viewer_->setOperateType(OperationType::ROTATE_OPERATION);
		});
	toolbar_->addAction(action);

	action = createAction(tr("缩放"), ":/icon/icon/scale.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		render_viewer_->setOperateType(OperationType::SCALE_OPERATION);
		});
	toolbar_->addAction(action);

	action = createAction(tr("复制"), ":/icon/icon/copy-object.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotCopyObject);
	toolbar_->addAction(action);

	action = createAction(tr("镜像"), ":/icon/icon/mirror.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotMirrorObject);
	toolbar_->addAction(action);

	action = createAction(tr("合并对象"), ":/icon/icon/create_component.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotCombineObject);
	toolbar_->addAction(action);

	action = createAction(tr("删除"), ":/icon/icon/delete.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotDeleteObject);
	toolbar_->addAction(action);

	toolbar_->addSeparator();

	{
		QToolButton* pButton = button_view_ = new QToolButton(this);
		pButton->setText(tr("显示模式"));
		pButton->setIcon(QIcon(":/icon/icon/render.svg"));
		pButton->setPopupMode(QToolButton::InstantPopup);
		toolbar_->addWidget(pButton);

		auto pMenu = new SARibbonMenu(this);

		QAction* p = createAction("线框", ":/icon/icon/wireframe.svg");
		connect(p, &QAction::triggered, this, [this](bool) {
			render_viewer_->slotRenderMode_Wire();
			//pButton->setIcon(QIcon(":/icon/icon/wireframe.svg"));
			});;
		pMenu->addAction(p);


		p = createAction("渲染", ":/icon/icon/render.svg");
		connect(p, &QAction::triggered, this, [this](bool) {
			render_viewer_->slotRenderMode_Rendering();
			//pButton->setIcon(QIcon(":/icon/icon/render.svg"));
			});;
		pMenu->addAction(p);

		p = createAction("平滑", ":/icon/icon/smooth.svg");
		connect(p, &QAction::triggered, this, [this](bool) {
			render_viewer_->slotRenderMode_Shading();
			//pButton->setIcon(QIcon(":/icon/icon/smooth.svg"));
			});
		pMenu->addAction(p);

		pButton->setMenu(pMenu);

	}

	{
		QToolButton* pButton = button_select_ = new QToolButton(this);
		pButton->setText(tr("选择模式"));
		pButton->setIcon(QIcon(":/icon/icon/object-select.svg"));
		pButton->setPopupMode(QToolButton::InstantPopup);
		toolbar_->addWidget(pButton);

		auto pMenu = new SARibbonMenu(this);
		pButton->setMenu(pMenu);

		QAction* p = createAction("点选择", ":/icon/icon/vertex-select.svg");
		connect(p, &QAction::triggered, this, [this](bool) {
			render_viewer_->slotSelectMode_Vert();
			//pButton->setIcon(QIcon(":/icon/icon/vertex-select.svg"));
			});;
		pMenu->addAction(p);


		p = createAction("线选择", ":/icon/icon/line-select.svg");
		connect(p, &QAction::triggered, this, [this](bool) {
			render_viewer_->slotSelectMode_Edge();
			//pButton->setIcon(QIcon(":/icon/icon/line-select.svg"));
			});;
		pMenu->addAction(p);

		p = createAction("面选择", ":/icon/icon/face-select.svg");
		connect(p, &QAction::triggered, this, [this](bool) {
			render_viewer_->slotSelectMode_Face();
			//pButton->setIcon(QIcon(":/icon/icon/face-select.svg"));
			});
		pMenu->addAction(p);

		p = createAction("体选择", ":/icon/icon/object-select.svg");
		connect(p, &QAction::triggered, this, [this](bool) {
			render_viewer_->slotSelectMode_Object();
			//pButton->setIcon(QIcon(":/icon/icon/object-select.svg"));
			});
		pMenu->addAction(p);

	}

	toolbar_->addSeparator();

	action = createAction(tr("适应窗口"), ":/icon/icon/fit-screen.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotSetScenesCenter);
	toolbar_->addAction(action);

	action = createAction(tr("工作平面"), ":/icon/icon/workplane.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotSetWorkPlane);
	toolbar_->addAction(action);


	refreshToolBar();

}

void RibbonViewer::refreshToolBar()
{
	//toolbar_->setGeometry
	auto w = width() - toolbar_->width();
	toolbar_->move(w / 2, 180);

}

void RibbonViewer::resizeEvent(QResizeEvent* event)
{
	SARibbonMainWindow::resizeEvent(event);
	refreshToolBar();
}

void RibbonViewer::initSubD()
{
	SARibbonCategory* category = new SARibbonCategory(this);
	category->setCategoryName("  SubD  ");
	category->setObjectName("categorySubD");
	ribbonBar()->addCategoryPage(category);

	SARibbonPannel* pannel1 = new SARibbonPannel(tr("创建"), this);
	category->addPannel(pannel1);

	auto pannel1Button = pannel1->pannelLayout()->pannelTitleLabel();
	pannel1Button->setPopupMode(QToolButton::InstantPopup);

	auto pMenu = new SARibbonMenu(this);
	pannel1Button->setMenu(pMenu);
	pMenu->setMinimumWidth(80);

	QAction* action = createAction(tr("单一面"), ":/TMSpline/TMSpline/single-face.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotCreateSingleFaceMesh);
	pannel1->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("平面"), ":/TMSpline/TMSpline/plane.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotCreatPlaneMesh);
	pannel1->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("立方体"), ":/TMSpline/TMSpline/cube.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotCreatCubeMesh);
	pannel1->addAction(action);
	pMenu->addAction(action);

	SARibbonPannel* pannel2 = new SARibbonPannel("编辑", this);
	category->addPannel(pannel2);

	auto pannel2Button = pannel2->pannelLayout()->pannelTitleLabel();
	pannel2Button->setPopupMode(QToolButton::InstantPopup);

	pMenu = new SARibbonMenu(this);
	pannel2Button->setMenu(pMenu);
	pMenu->setMinimumWidth(500);

	action = createAction(tr("细分方法\nCatmull Clark"), ":/TMSpline/TMSpline/subd-catmull-clark.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotMeshSubdiveCatmullClark);
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("细分方法\nLoop"), ":/TMSpline/TMSpline/subd-loop.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotMeshSubdiveLoop);
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("三角化\n网格"), ":/TMSpline/TMSpline/triangulated-mesh.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotMeshTriangulation);
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("桥接边"), ":/TMSpline/TMSpline/brige-edge.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		BaseObjectOperation(SelectModel::EDGE_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshBridgeEdge);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("垂直\n挤出边"), ":/TMSpline/TMSpline/extrude-edge-v.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperationSelect(SelectModel::EDGE_MODEL, OperationType::CTRL_OPERATION, MeshOperationType::MeshExtrudeEdgeVertical);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("水平\n挤出边"), ":/TMSpline/TMSpline/extrude-edge-h.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperationSelect(SelectModel::EDGE_MODEL, OperationType::CTRL_OPERATION, MeshOperationType::MeshExtrudeEdgeHorizontal);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("挤出面"), ":/TMSpline/TMSpline/extrude-face.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperationSelect(SelectModel::FACE_MODEL, OperationType::CTRL_OPERATION, MeshOperationType::MeshExtrudeFaceNew);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("翻转\n法向量"), ":/TMSpline/TMSpline/reverse-normal.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperation(SelectModel::OBJECT_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshReverseNormal);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);


	action = createAction(tr("填充洞口"), ":/TMSpline/TMSpline/fill-hole.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperation(SelectModel::EDGE_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshFillHole);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("加厚"), ":/TMSpline/TMSpline/thicken.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperation(SelectModel::OBJECT_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshThicken);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("删除面"), ":/TMSpline/TMSpline/delete-face.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperation(SelectModel::FACE_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshDeleteElement);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("嵌入面"), ":/TMSpline/TMSpline/embed-face.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperation(SelectModel::VERTEX_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshEmbedFace);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("分离"), ":/TMSpline/TMSpline/separate.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperation(SelectModel::EDGE_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshSeparate);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("缝合"), ":/TMSpline/TMSpline/weld.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperation(SelectModel::EDGE_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshWeldEdge);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("循环分割"), ":/TMSpline/TMSpline/split-loop.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperation(SelectModel::EDGE_MODEL, OperationType::PICK_OPERATION_MOVE, MeshOperationType::MeshSplitLoop);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("分割面"), ":/TMSpline/TMSpline/split-face.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperation(SelectModel::EDGE_MODEL, OperationType::PICK_OPERATION_MOVE_MULTI, MeshOperationType::MeshSplitFaceByEdge);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("切割面"), ":/TMSpline/TMSpline/cut-face.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperation(SelectModel::VERTEX_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshSplitFaceVertex);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("复制面"), ":/TMSpline/TMSpline/copy-face.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperation(SelectModel::FACE_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshCopy);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);
}

void RibbonViewer::initBRrep()
{
	SARibbonCategory* category = new SARibbonCategory(this);
	category->setCategoryName("  BRep  ");
	category->setObjectName("categoryBRep");
	ribbonBar()->addCategoryPage(category);

	SARibbonPannel* pannel1 = new SARibbonPannel(tr("创建"), this);
	category->addPannel(pannel1);

	auto pannel1Button = pannel1->pannelLayout()->pannelTitleLabel();
	pannel1Button->setPopupMode(QToolButton::InstantPopup);

	auto pMenu = new SARibbonMenu(this);
	pannel1Button->setMenu(pMenu);
	pMenu->setMinimumWidth(250);

	QAction* action = createAction(tr("单一面"), ":/TMSpline/TMSpline/single-face.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotCreateSingleFaceBRep);
	pannel1->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("平面"), ":/TMSpline/TMSpline/plane.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotCreatPlaneBRep);
	pannel1->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("多边形"), ":/TMSpline/TMSpline/polygon.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotCreatePolygonBRep);
	pannel1->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("立方体"), ":/TMSpline/TMSpline/cube.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotCreatCubeBRep);
	pannel1->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("球体"), ":/TMSpline/TMSpline/sphere.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotCreateSphereBRep);
	pannel1->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("圆柱"), ":/TMSpline/TMSpline/cylinder.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotCreateCylinderBRep);
	pannel1->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("圆锥"), ":/TMSpline/TMSpline/cone.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotCreateConeBrep);
	pannel1->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("圆环"), ":/TMSpline/TMSpline/torus.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotCreateTorusBRep);
	pannel1->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("棱柱"), ":/TMSpline/TMSpline/prism.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotCreatePrismBRep);
	pannel1->addAction(action);
	pMenu->addAction(action);

	SARibbonPannel* pannel2 = new SARibbonPannel("编辑", this);
	category->addPannel(pannel2);

	auto pannel2Button = pannel2->pannelLayout()->pannelTitleLabel();
	pannel2Button->setPopupMode(QToolButton::InstantPopup);

	pMenu = new SARibbonMenu(this);
	pannel2Button->setMenu(pMenu);
	pMenu->setMinimumWidth(180);


	action = createAction(tr("倒角"), ":/TMSpline/TMSpline/chamfer.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		this->BaseObjectOperation(SelectModel::EDGE_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshChamferEdge);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("圆角"), ":/TMSpline/TMSpline/fillet.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		BaseObjectOperation(SelectModel::EDGE_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshFilletEdge);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("合并"), ":/TMSpline/TMSpline/union.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		BaseObjectOperation(SelectModel::OBJECT_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshFuse);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("减去"), ":/TMSpline/TMSpline/union-1.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		BaseObjectOperation(SelectModel::OBJECT_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshCut);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("相交"), ":/TMSpline/TMSpline/union-2.svg");
	connect(action, &QAction::triggered, this, [this](bool) {
		BaseObjectOperation(SelectModel::OBJECT_MODEL, OperationType::ENTER_OPERATION, MeshOperationType::MeshCommon);
		});
	pannel2->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("挤出面"), ":/TMSpline/TMSpline/extrude-face.svg");
	connect(action, &QAction::triggered, render_viewer_, &RenderViewer::slotBRepExtrude);
	pannel2->addAction(action);
	pMenu->addAction(action);

}

void RibbonViewer::initAnalysis()
{
	SARibbonCategory* category = new SARibbonCategory(this);
	category->setCategoryName("  分析  ");
	category->setObjectName("categoryAnalysis");
	ribbonBar()->addCategoryPage(category);

	SARibbonPannel* pannel1 = new SARibbonPannel(tr("分析"), this);
	category->addPannel(pannel1);
	//pannel1->setEnableShowTitle(false);

	auto pannel1Button = pannel1->pannelLayout()->pannelTitleLabel();
	pannel1Button->setPopupMode(QToolButton::InstantPopup);

	auto pMenu = new SARibbonMenu(this);
	pannel1Button->setMenu(pMenu);
	//pMenu->setMinimumWidth(120);

	QAction* action = createAction(tr("斑纹\n分析"), ":/TMSpline/TMSpline/zebra.svg");
	pannel1->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("曲率映射\n分析"), ":/TMSpline/TMSpline/curvature.svg");
	pannel1->addAction(action);
	pMenu->addAction(action);

	action = createAction(tr("曲率梳\n分析"), ":/TMSpline/TMSpline/curvature-comb.svg");
	pannel1->addAction(action);
	pMenu->addAction(action);
}

void RibbonViewer::initHelper()
{
	SARibbonCategory* category = new SARibbonCategory(this);
	category->setCategoryName("  帮助  ");
	category->setObjectName("categoryAnalysis");
	ribbonBar()->addCategoryPage(category);

	SARibbonPannel* pannel1 = new SARibbonPannel(tr("关于"), this);
	category->addPannel(pannel1);
	//pannel1->setEnableShowTitle(false);

	auto pannel1Button = pannel1->pannelLayout()->pannelTitleLabel();
	pannel1Button->setPopupMode(QToolButton::InstantPopup);

	auto pMenu = new SARibbonMenu(this);
	pannel1Button->setMenu(pMenu);
	//pMenu->setMinimumWidth(250);

	QAction* action = createAction(tr("帮助"), ":/icon/icon/about.svg");
	pannel1->addAction(action);
	pMenu->addAction(action);
}

void RibbonViewer::refreshSize()
{
	acamcad::LocalDataManager::DataModelPosition data;

	dataManager_->execGet(data);

	//QByteArray array = data.array;
	show();
	restoreGeometry(data.array);

}