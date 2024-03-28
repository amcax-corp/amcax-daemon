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

namespace acamcad
{

	class CreateOperate_Circular : public MOperation
	{
	public:
		CreateOperate_Circular();
		CreateOperate_Circular(double radius, double angle, size_t seg = 4);

	public:

		virtual bool DoOperate(AdapterObject* brep_object) override;

	private:
		double radius_;
		double angle_;
		size_t seg_;

	};

}