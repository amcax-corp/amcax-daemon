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

#include "QGLViewer/qglviewer.h"

#include "../Object/ObjectDefine.h"
#include "AxisOperate.h"
#include "../Core/CoreDefine.h"
#include "ParamDialog.h"
#include "../record/Record.h"

namespace acamcad
{
class DataManager;
class AMCore;
class SelectTool;

class RenderViewer : public QGLViewer 
{
	Q_OBJECT
private: // type def
	struct SelectAction {
        SelectModel model;
		QAction *action;
		SelectAction(const SelectModel &sm, QAction *&act): 
            model(sm), action(act) { }
        SelectAction(){}
		AMCAX::Coord3 mouseMoveTrans(const QPoint& e_point, const AMCAX::Coord3& o_center);
	};

    enum OperateType {

        OPERATE_BREP_PLANE,
		OPERATE_BREP_POLYGON,
        OPERATE_BREP_CUBE,
        OPERATE_BREP_SPHERE,
        OPERATE_BREP_CYLINDER,
        OPERATE_BREP_CONE,
        OPERATE_BREP_TORUS,
		OPERATE_BREP_PRISM,
        OPERATE_BREP_EXTRUDE,
    };

public:
	RenderViewer(QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

protected:
	virtual void draw();
	virtual void postDraw();
	
	virtual void init();
	virtual QString helpString() const;

	virtual void mousePressEvent(QMouseEvent* e);
	virtual void mouseMoveEvent(QMouseEvent* e);
	virtual void mouseReleaseEvent(QMouseEvent* e);
	virtual void mouseDoubleClickEvent(QMouseEvent* e);

	virtual void keyPressEvent(QKeyEvent*);
	virtual void keyReleaseEvent(QKeyEvent*);

private:
	void initTSpline();
	void initMesh();
	void initBRep();

private:
	void beginPickOperationSingle();
	void beginEnterOperation();

	/// do with the click operation, such as append face;
	void beginEnterClickOperation();

	void cancelOperation();

	AMCAX::Coord3 mouseMoveTrans(const QPoint& e_point, const  AMCAX::Coord3& o_center);

	/*! @name Select Method */
public:
	bool select(const QMouseEvent* ev, bool is_select_muti);
	void beginSelection(const QPoint& point);
	void endSelection(const QPoint& point, SelectModel type);

public:
	void selectFrame(const QPoint& point, PyramidInf& frustum);

public:
	bool selectOpAxis(const QMouseEvent* ev);
	void selectDrawAxis();
	void endSelectionAxis(const QPoint& point);

public:
	GLuint* selectBuffer() { return selectBuffer_; }
	/*! Default value is 400, 100 for 1 object with 4 size. */
	void setSelectBufferSize(int size);
	int selectBufferSize() const { return selectBufferSize_; }

	/*! Default value is 3 pixels. */
	int selectRegionWidth() const { return selectRegionWidth_; }
	/*! Default value is 3 pixels. */
	int selectRegionHeight() const { return selectRegionHeight_; }
	void setSelectRegionWidth(int width) { selectRegionWidth_ = width; }
	void setSelectRegionHeight(int height) { selectRegionHeight_ = height; }

	void setSelectRegionWidthDefault() { selectRegionWidth_ = 3; }
	void setSelectRegionHeightDefault() { selectRegionHeight_ = 3; }
private:
	int selectRegionWidth_, selectRegionHeight_;
	GLuint* selectBuffer_;
	int selectBufferSize_;
	int selectedId_;
	int selectedSubsetId_;

	std::vector< SelectInfoWithCamera > select_info_list_;

private:
	void setFrameSelection(bool is_frame_select) { is_frame_selection_ = is_frame_select; }
	bool is_frame_selection_;	//标识是否开始框选
	QRect frame_selection_rectangle_;
	std::vector<int> selectedFrame_id_;

	Qt::MouseButton click_buttom;

public:
	void setSelectModel(SelectModel model) { select_model_ = model; }
	SelectModel getSelectModel() { return select_model_; }

	void clearSelected();

private:
	SelectModel select_model_;

public:
	void addSelector(const SelectModel &sm, QAction *&act);

Q_SIGNALS:
	void selectedModelChanged();

private:
	void setSelectInfoCamera(const QPoint& pixel, acamcad::SelectInfoWithCamera& s_info);

	/*! @name Click Method */
public:
	void click(const QMouseEvent* ev);

	void clearClicked();
private:
	std::vector< ClickInfo > click_info_list_;

	/*! @name Draw Method */
public:
	void setDrawModel(DrawModel model) { draw_model_ = model; }
	DrawModel getDrawModel() { return draw_model_; }

private:
	DrawModel draw_model_;

public:
	void setScenesCenter();

	void setWorkPlane();


private:
	// 绘制选中的物体
	void drawSelectedElement();

	void drawSelectMovePoint() const;
	void drawClickedElement() const;
	void drawSelectionRectangle(QRect& rectangle_) const;

private:
	void drawGrid(double size, int nbSubdivisions);
	void drawWorkPlaneGrid(double size = 5.0, int nbSubdivisions = 50);	//size 是 grid 的总 size

	void setGridIsDrawn(bool draw = true) { is_draw_grid_ = draw; }
	void changeGridIsDrawn() { setGridIsDrawn(!is_draw_grid_); }

	void drawWorldAxis();

	bool createOperateAxis();
	AxisOperation* o_axis_;

	PlaneObject* work_plane_;
	bool is_draw_grid_; 

public:
	void setOperateType(OperationType op_type) { operate_type_ = op_type; }
	OperationType operateType() { return operate_type_; }
	void setOperationVector(const AMCAX::Coord3d & vector) { operation_vector_ = vector; }
	void setRecord(Record* record) { record = record; }
private:
	OperationType operate_type_;

    QVector<SelectAction> selectActions;
	QMap<OperateType, QVector<ParamDialog::RequestUnit>> objectFillOuts;

private:
	//相机起点，选择射线方向
	qglviewer::Vec view_ori_, view_dir_;
	//所有选中物体的中心的和，这个没有除以物体数目
	AMCAX::Coord3 select_object_center_;

	//prevPos_ 之前的鼠标位置，用于鼠标的迭代更新(鼠标移动）
	//pressPos_ 鼠标点击时的位置
	QPoint prevPos_, pressPos_;

	//operation_point_ 在交互对象上的点
	//operation_vector_ 约束交互的移动方向
	std::vector<AMCAX::Coord3> operation_point_;
    // std::vector<MVector3> operation_vector_;
    // MVector3 operation_point_;
	AMCAX::Coord3 operation_vector_;

	bool is_pick_move_select_; //标识是否可以开始移动

public:
	void setDataManger(DataManager* dataManager);
	void setCore(AMCore* core);

private:
	DataManager* dataManager_;
	AMCore* coreCommand_;
	SelectTool* selectTool_;
    ParamDialog *dialog;
	acamcad::Record record;

private:
	void selectCheckChanged();
    void BaseObjectOperation(const SelectModel& s_model, 
		const OperationType& op_type, const MeshOperationType mesh_operate_type);
    void BRepObjectOperation(const SelectModel& s_model,
        const OperationType& op_type, const MeshOperationType mesh_operate_type);

    void disconnectDialog();

public slots:
	void slotUndo();
	void slotRedo();


	friend class OperateToolBar;
    friend class ACAMWindow;
private slots: // base tab
    void slotRenderMode_Wire();
    void slotRenderMode_Rendering();
    void slotRenderMode_Shading();

    void slotSetScenesCenter();

    void slotSelectMode_Vert();
    void slotSelectMode_Edge();
    void slotSelectMode_Face();
    void slotSelectMode_Object();

    void slotCopyObject();
    void slotCombineObject();


private slots: // BRep tab
    void slotCreateSingleFaceBRep();

	int getLastPersistentId();

    void slotCreatPlaneBRep();
    void slotCreatPlaneBRepAccepted(const ParamDialog::Response& data);


    void slotCreatCubeBRep();
    void slotCreatCubeBRepAccepted(const ParamDialog::Response& data);

    void slotCreateSphereBRep();
    void slotCreateSphereBRepAccepted(const ParamDialog::Response& data);

    void slotCreateCylinderBRep();
    void slotCreateCylinderBRepAccepted(const ParamDialog::Response& data);

    void slotCreateConeBrep();
    void slotCreateConeBrepAccepted(const ParamDialog::Response& data);

    void slotCreateTorusBRep();
    void slotCreateTorusBRepAccepted(const ParamDialog::Response& data);

	void slotCreatePrismBRep();
    void slotCreatePrismBRepAccepted(const ParamDialog::Response& data);

    void slotBRepExtrude();
    void slotBRepExtrudeAccepted(const ParamDialog::Response& data);

	void slotCreatePolygonBRep();
	void slotCreatePolygonBRepAccepted(const ParamDialog::Response& data);

};

}
