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
#include "MOperate.h"
#include <common/CoordT.hpp>

namespace acamcad
{
	class SGLOperate_ExtrudeShape : public MOperation
	{
	public:
		SGLOperate_ExtrudeShape(const SelectModel& smodle);
		virtual ~SGLOperate_ExtrudeShape() {};

	public:
		virtual bool DoOperate(AdapterObject* brep_object) override;

	public:
		virtual MeshOperationType OperationType() { return MeshOperationType::MeshExtrudeFace; }

		virtual void setDirection(const AMCAX::Coord3& vec3) { dire_ = vec3; }
		virtual AMCAX::Coord3 getDirection() { return dire_; }

	private:
		AMCAX::Coord3 dire_;
	};

} //n
