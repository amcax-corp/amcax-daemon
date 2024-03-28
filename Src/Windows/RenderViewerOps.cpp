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

#include "../Core/CoreDefine.h"
#include "../Core/ACAMCore.h"
#include "RenderViewer.h"
#include "ACAMWindow.h"

#include <cmath>
#include <algorithm>
#include <utility>
#include <tuple>

using namespace acamcad;


void RenderViewer::addSelector(const SelectModel& sm, QAction*& act) {
	selectActions.push_back(SelectAction{ sm, act });
}

void RenderViewer::selectCheckChanged() {
	auto sm = getSelectModel();
	std::for_each(selectActions.begin(), selectActions.end(), [&](SelectAction& sa) {
		sa.action->setChecked(sm == sa.model);
		});
}

void RenderViewer::BaseObjectOperation(const SelectModel& s_model,
	const OperationType& op_type,
	const MeshOperationType mesh_operate_type) {
	clearSelected();
	setSelectModel(s_model);
	setOperateType(op_type);

	coreCommand_->setMeshOperationType(mesh_operate_type);

	selectCheckChanged();
	update();
}

void RenderViewer::MeshObjectOperation(const SelectModel& s_model,
	const OperationType& op_type,
	const MeshOperationType mesh_operate_type)
{
	coreCommand_->setOperationDataType(OperationDataType::MESH_TYPE);
	BaseObjectOperation(s_model, op_type, mesh_operate_type);
}


void RenderViewer::BRepObjectOperation(const SelectModel& s_model,
	const OperationType& op_type,
	const MeshOperationType mesh_operate_type)
{
	coreCommand_->setOperationDataType(OperationDataType::BREP_TYPE);
	BaseObjectOperation(s_model, op_type, mesh_operate_type);
}


void RenderViewer::disconnectDialog() {
	disconnect(dialog, &ParamDialog::dataReady, this, nullptr);
}