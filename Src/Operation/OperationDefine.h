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

#include "MOperate_Select.h"

#include "MOperate_getCenter.h"
#include "MOperate_getBoundingBox.h"
#include "MOperate_getDirection.h"
#include "MOperate_getPlane.h"

#include "SGLOperate_ChamferEdge.h"
#include "SGLOperate_FilletEdge.h"

#include "CreateOperate_Cone.h"
#include "CreateOperate_ConeTSpline.h"
#include "CreateOperate_Cube.h"
#include "CreateOperate_CubeMesh.h"
#include "CreateOperate_CubeTSpline.h"
#include "CreateOperate_Cylinder.h"
#include "CreateOperate_CylinderTSpline.h"
#include "CreateOperate_Plane.h"
#include "CreateOperate_PlaneMesh.h"
#include "CreateOperate_PlaneTSpline.h"
#include "CreateOperate_Sphere.h"
#include "CreateOperate_SphereTSpline.h"
#include "CreateOperate_Torus.h"
#include "CreateOperate_TorusTSpline.h"
#include "CreateOperate_Prism.h"
#include "CreateOperate_Polygon.h"
#include "CreateOperate_QuadballTSpline.h"
#include "CreateOperate_SingleFace.h"
#include "CreateOperate_SingleFaceMesh.h"
#include "CreateOperate_SingleFaceTSpline.h"
#include "CreateOperate_Circular.h"
#include "CreateOperate_CircularTSpline.h"
#include "CreateOperate_Ring.h"
#include "CreateOperate_RoundedPolygon.h"
#include "CreateOperate_RoundedPrism.h"
#include "CreateOperate_Wedge.h"

#include "CreateOperate_Builder.h"

#include "MultOperate_BoolCommon.h"
#include "MultOperate_BoolCut.h"
#include "MultOperate_BoolFuse.h"
#include "MultOperate_Combine.h"

#include "SGLOperate_ExtrudeShape.h"
#include "SGLOperate_TriangulateMesh.h"
#include "SGLOperate_EmbedFace.h"
#include "SGLOperate_DeleteElement.h"
#include "SGLOperate_Separate.h"
#include "SGLOperate_WeldEdge.h"
#include "SGLOperate_SplitLoop.h"
#include "MOperate_SelectCloset.h"
#include "SGLOperate_SplitFaceByEdge.h"
#include "SGLOperate_SplitFaceVertex.h"
#include "SGLOperate_Bridge.h"
#include "SGLOperate_Thicken.h"
#include "SGLOperate_ExtrudeEdge.h"
#include "SGLOperate_ReverseNormal.h"
#include "SGLOperate_CreaseRemove.h"
#include "SGLOperate_Crease.h"
#include "SGLOperate_Repair.h"
#include "SGLOperate_Uniform.h"
#include "SGLOperate_SplitFace.h"
#include "SGLOperate_ExtrudeFace.h"
#include "SGLOperate_FillHole.h"