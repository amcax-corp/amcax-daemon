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
#ifndef CREATEOPERATE_WEDGE_H
#define CREATEOPERATE_WEDGE_H

#include "MOperate.h"
#include <common/CoordT.hpp>

namespace acamcad
{
	class CreateOperate_Wedge : public MOperation
	{
	public:
		CreateOperate_Wedge();
		CreateOperate_Wedge(const double dx, const double dy, const double dz, const double ltx);
		CreateOperate_Wedge(const AMCAX::Coord3& center,
			const double dx, const double dy, const double dz, const double ltx);
		CreateOperate_Wedge(const AMCAX::Coord3& center, const AMCAX::Coord3& axis,
			const double dx, const double dy, const double dz, const double ltx);
		CreateOperate_Wedge(const double dx, const double dy, const double dz,
			const double xmin, const double zmin, const double xmax, const double zmax);
		CreateOperate_Wedge(const AMCAX::Coord3& center,
			const double dx, const double dy, const double dz,
			const double xmin, const double zmin, const double xmax, const double zmax);
		CreateOperate_Wedge(const AMCAX::Coord3& center, const AMCAX::Coord3& axis,
			const double dx, const double dy, const double dz,
			const double xmin, const double zmin, const double xmax, const double zmax);
		~CreateOperate_Wedge();

	public:
		virtual bool DoOperate(AdapterObject* brep_object) override;

	private:
		enum TYPE
		{
			TYPE_NONE,
			TYPE_ARG_DX_DY_DZ_LTX,
			TYPE_ARG_C_DX_DY_DZ_LTX,
			TYPE_ARG_C_A_DX_DY_DZ_LTX,
			TYPE_ARG_DX_DY_DZ_XI_ZI_XA_ZA,
			TYPE_ARG_C_DX_DY_DZ_XI_ZI_XA_ZA,
			TYPE_ARG_C_A_DX_DY_DZ_XI_ZI_XA_ZA,
		} type_;

		double dx_, dy_, dz_;
		double ltx_;
		double xmin_, xmax_, zmin_, zmax_;
		AMCAX::Coord3 center_;
		AMCAX::Coord3 axis_;
	};
}

#endif // CREATEOPERATE_WEDGE_H
