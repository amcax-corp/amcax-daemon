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

#include "SelectInfo.h"
#include <iostream>

namespace acamcad
{

	enum class DrawModel
	{
		WIRE_MODE, SHADING_MODEL, RENDERING_MODEL,
		ZEBRA_MODE, GAUSS_MODE, MEAN_MODE, C_GRAPH_MODE
	};

	//===================================================================

	//大的交互操作的类型，用于鼠标和键盘
	enum class OperationType
	{
		NO_OPERATION,				//执行两个功能，1是基础选择，2是选择后立刻移动
		MOVE_OPERATION,				//显示移动操纵轴，开始移动
		ROTATE_OPERATION,			//显示移动操纵轴，开始旋转
		SCALE_OPERATION,			//显示移动操纵轴，开始缩放

		ENTER_OPERATION,			//选择后一定数目后，键盘按下enter键之后，包含物体层和单元层执行操作，例如 meshtriangle，split face，collapse edge
		PICK_OPERATION_SINGLE,		//选择一个单元后，立刻进行操作，例如edge swap
		PICK_OPERATION_MOVE,		//选择一个单元后，按住鼠标移动，松开后执行操作。
		PICK_OPERATION_MOVE_MULTI,	//选择单元，移动，松开确认，在继续选择，回车执行操作。
		CTRL_OPERATION,				//挤出用的选择操作
		ALT_OPERATION,				//挤出壳用的选择操作

		CLICK_OPERATION_SELECT,		//连续单击，获取选择线与工作平面的交点, used in add single face
		CLICK_OPERATION				//not used now
	};

	//网格的交互操作类型
	enum class MeshOperationType
	{
		NO_operotion,
		MeshTriangulate, MeshQuadrilate, MeshReverseNormal,
		MeshSubdiceCC, MeshSubdiveLoop,
		MeshCopy, MeshDelete, MeshCombine, MeshMirror,
		MeshDeleteElement, MeshEmbedFace,
		MeshSplitLoop, MeshSplitEdge, MeshSplitFaceByEdge,
		MeshCollapseEdge, MeshSplitFace, MeshSplitFaceVertex,
		MeshSwapEdge, MeshCombineFace, MeshBridgeEdge,
		MeshExtrudeFace, MeshExtrudeEdgeVertical, MeshExtrudeEdgeHorizontal,
		MeshExtrudeFaceNew,

		MeshWeldEdge, MeshSeparate, MeshProject,

		MeshFuse, MeshCommon, MeshCut, MeshCut21,

		MeshFaceAppend,		//添加自由面或者向网格中追加面
		MeshChamferEdge, MeshFilletEdge, MeshThicken,
		MeshAddCreaseEdge, MeshRemoveCreaseEdge, MeshSetFrepFix, MeshSetFrepForce,
		MeshFillHole,
		TSplineUniform, TSplineRepair,

		TSplineRing, TSplineFillLine,	//temp test shape

		TSplineCurvatureGraph
	};

	inline std::ostream& operator<<(std::ostream& o, const MeshOperationType& m)
	{
		switch (m)
		{
		case MeshOperationType::NO_operotion:
			return o << "NO operation";
		case MeshOperationType::MeshTriangulate:
			return o << "Triangulate";
		case MeshOperationType::MeshReverseNormal:
			return o << "Reverse Normal";
		case MeshOperationType::MeshSubdiceCC:
			return o << "Subdivide Catmull-Clark";
		case MeshOperationType::MeshSubdiveLoop:
			return o << "Subdivide Loop";
		case MeshOperationType::MeshCopy:
			return o << "Copy";
		case MeshOperationType::MeshDelete:
			return o << "Delete";
		case MeshOperationType::MeshCombine:
			return o << "Combine object";
		case MeshOperationType::MeshMirror:
			return o << "Mirror mesh";
		case MeshOperationType::MeshDeleteElement:
			return o << "Delete face";
		case MeshOperationType::MeshEmbedFace:
			return o << "Embed Face";
		case MeshOperationType::MeshSplitLoop:
			return o << "Split Loop";
		case MeshOperationType::MeshSplitEdge:
			return o << "Split Edge";
		case MeshOperationType::MeshSplitFaceVertex:
			return o << "Split Face by Vertex";
		case MeshOperationType::MeshSplitFaceByEdge:
			return o << "Split Face With Edge";
		case MeshOperationType::MeshCollapseEdge:
			return o << "Combine two Vertives";
		case MeshOperationType::MeshSplitFace:
			return o << "Split Face with vert";
		case MeshOperationType::MeshExtrudeFaceNew:
			return o << "Extrude face new";
		case MeshOperationType::MeshSwapEdge:
			return o << "Swap Edge";
		case MeshOperationType::MeshCombineFace:
			return o << "Combine two Face";
		case MeshOperationType::MeshBridgeEdge:
			return o << "Bridge Edge";
		case MeshOperationType::MeshExtrudeFace:
			return o << "Extrude Face";
		case MeshOperationType::MeshThicken:
			return o << "Shell Face";
		case MeshOperationType::MeshExtrudeEdgeVertical:
			return o << "Ectrude Edge Vertical";
		case MeshOperationType::MeshExtrudeEdgeHorizontal:
			return o << "Extrude Edge Horizontal";
		case MeshOperationType::MeshFaceAppend:
			return o << "Append New Face";
		case MeshOperationType::MeshWeldEdge:
			return o << "Weld Edge";
		case MeshOperationType::MeshSeparate:
			return o << "Separate Edge";
		case MeshOperationType::MeshChamferEdge:
			return o << "Bevel Edge";
		case MeshOperationType::MeshAddCreaseEdge:
			return o << "Make Edge Crease";
		case MeshOperationType::MeshRemoveCreaseEdge:
			return o << "Remove Edge Crease";
		case MeshOperationType::MeshFillHole:
			return o << "Fill hole";
		case MeshOperationType::TSplineUniform:
			return o << "Param Uniform";
		default:
			break;
		}
		return o << " Error operation ";
	}

} //namespace acamcad