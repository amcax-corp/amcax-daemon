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
#include <common/CoordT.hpp>
#include <common/QuaternionT.hpp>
#include <vector>

namespace acamcad {
	class MathUtils {
	public:
		static inline AMCAX::Coord3 cross(const AMCAX::Coord3& a, const AMCAX::Coord3& b)
		{
			return AMCAX::Coord3(a.Y() * b.Z() - b.Y() * a.Z(),
				-(a.X() * b.Z() - b.X() * a.Z()),
				a.X() * b.Y() - b.X() * a.Y());
		};

		static int signTwoVector(const AMCAX::Coord3& a, const AMCAX::Coord3& b);
		static void GramSchmidtAlign(AMCAX::Coord3& v1, AMCAX::Coord3& v2, AMCAX::Coord3& v3);

		static bool intersection_Line_Plane(const AMCAX::Coord3& v, const AMCAX::Coord3& t, const AMCAX::Coord3& o, const AMCAX::Coord3& n, AMCAX::Coord3& insertP);
		static AMCAX::Coord3 projectPointPlane(const AMCAX::Coord3& center, const AMCAX::Coord3& normal, const AMCAX::Coord3& point);
		static double distPointLineSquared(const AMCAX::Coord3& p, const AMCAX::Coord3& v0, const AMCAX::Coord3& v1, AMCAX::Coord3& nearestPoint); 
		static double distLine_SLineSquared(const AMCAX::Coord3& _v00, const AMCAX::Coord3& _v01,
			const AMCAX::Coord3& _v1, const AMCAX::Coord3& _v11,
			AMCAX::Coord3& _min_v0, AMCAX::Coord3& _min_v1); 

		static bool isOnSamePlane(const std::vector<AMCAX::Coord3>& points); 

	};
	
}