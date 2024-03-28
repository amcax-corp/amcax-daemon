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
#include "MOperate_SelectInfo.h"

namespace acamcad
{

	class SGLOperate_TriangulateMesh : public MOperate_SingleObject
	{
	public:
		SGLOperate_TriangulateMesh(const SelectModel& smodle);
		virtual ~SGLOperate_TriangulateMesh() {};

	public:
		virtual bool DoOperate(AdapterObject* brep_object) override;

	public:
		virtual MeshOperationType OperationType() { return MeshOperationType::MeshTriangulate; }
	};

} //namespace acamcad