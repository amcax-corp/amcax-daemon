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
#include "MathUtils.h"

#ifndef FLT_MIN
#define FLT_MIN          1.175494351e-38F        // min normalized positive value
#endif

namespace acamcad
{
	int MathUtils::signTwoVector(const AMCAX::Coord3& a, const AMCAX::Coord3& b) {
		if (a.Dot(b) > 1e-8)
			return 1;
		else if (a.Dot(b) < -1e-8)
			return -1;
		else
			return 0;
	}

	void MathUtils::GramSchmidtAlign(AMCAX::Coord3& v1, AMCAX::Coord3& v2, AMCAX::Coord3& v3)
	{
		if (std::abs(v1[0]) > std::abs(v1[1]) && std::abs(v1[0]) > std::abs(v1[2]))
		{
			v2[0] = 0.0; v2[1] = 1.0; v2[2] = 0.0;
			v3[0] = 0.0; v3[1] = 0.0; v3[2] = 1.0;
		}
		else if (std::abs(v1[1]) > std::abs(v1[0]) && std::abs(v1[1]) > std::abs(v1[2])) {
			v2[0] = 1.0; v2[1] = 0.0; v2[2] = 0.0;
			v3[0] = 0.0; v3[1] = 0.0; v3[2] = 1.0;
		}
		else {
			v2[0] = 1.0; v2[1] = 0.0; v2[2] = 0.0;
			v3[0] = 0.0; v3[1] = 1.0; v3[2] = 0.0;
		}

		AMCAX::Coord3 tmp;
		v1.Normalize();
		tmp = cross(v3, v1);
		tmp.Normalize();
		v2[0] = tmp[0]; v2[1] = tmp[1]; v2[2] = tmp[2];
		v3 = cross(v1, v2);
		v3.Normalize();
	}

	bool MathUtils::intersection_Line_Plane(const AMCAX::Coord3& v, const AMCAX::Coord3& t, const AMCAX::Coord3& o, const AMCAX::Coord3& n, AMCAX::Coord3& insertP)
	{
		double tn = t.Dot(n);
		if (std::abs(tn) < 0.1)
		{
			return false;
		}

		AMCAX::Coord3 vp = v - o;
		double opn = vp.Dot(n);
		double s = -1 * opn / tn;
		insertP = (v + s * t);
		return true;
	}

	AMCAX::Coord3 MathUtils::projectPointPlane(const AMCAX::Coord3& center, const AMCAX::Coord3& normal, const AMCAX::Coord3& point)
	{
		AMCAX::Coord3 vp = point - center;
		double dis = vp.Dot(normal);
		AMCAX::Coord3 vp_new = vp - (dis * normal);
		return center + vp_new;
	}

	double MathUtils::distPointLineSquared(const AMCAX::Coord3& p, const AMCAX::Coord3& v0, const AMCAX::Coord3& v1, AMCAX::Coord3& nearestPoint)
	{
		AMCAX::Coord3 d1(p - v0), d2(v1 - v0);

		double t = d1.Dot(d2) / d2.SquaredNorm(); 

		if (t > 1.0)
		{
			nearestPoint = v1;
		}
		else if (t > 0.0)
		{
			nearestPoint = v0 + d2 * t;
		}
		else
		{
			nearestPoint = v0;
		}

		d1 = p - nearestPoint;
		return d1.SquaredNorm(); 
	}

	double MathUtils::distLine_SLineSquared(const AMCAX::Coord3& _v00, const AMCAX::Coord3& _v01,
		const AMCAX::Coord3& _v1, const AMCAX::Coord3& _v11,
		AMCAX::Coord3& _min_v0, AMCAX::Coord3& _min_v1)
	{
		AMCAX::Coord3 kDiff = _v00 - _v1;
		AMCAX::Coord3 d0 = _v01 - _v00;
		AMCAX::Coord3 d1 = _v11 - _v1;

		double fA00 = d0.SquaredNorm();
		double fA01 = -(d0 | d1);
		double fA11 = d1.SquaredNorm();
		double fB0 = (kDiff | d0);
		double fC = kDiff.SquaredNorm();
		double fDet = fabs(fA00 * fA11 - fA01 * fA01);
		double fB1, fS, fT, fSqrDist;

		if (fDet > FLT_MIN)
		{
			// line segments are not parallel
			fB1 = -(kDiff | d1);
			fS = fA01 * fB1 - fA11 * fB0;
			fT = fA01 * fB0 - fA00 * fB1;

			if (fS >= 0.0)
			{
				if (fS <= fDet)
				{
					double fInvDet = 1.0 / fDet;
					fS *= fInvDet;
					fT *= fInvDet;
				}
				else
				{
					double fInvDet = 1.0 / fDet;
					fT *= fInvDet;
					fS = 1.0;
				}
			}
			else
			{
				double fInvDet = 1.0 / fDet;
				fT *= fInvDet;
				fS = 0.0;
			}
		}
		else
		{
			fB1 = (kDiff | d1);

			fS = 0;
			fT = fB1 / fA11;

		}

		_min_v0 = _v00 + fS * d0;
		_min_v1 = _v1 + fT * d1;
		fSqrDist = (_min_v1 - _min_v0).SquaredNorm();
		return fabs(fSqrDist);
	}

	bool MathUtils::isOnSamePlane(const std::vector<AMCAX::Coord3>& points)
	{
		if (points.size() <= 3) 
		{
			return true; 
		}

		const AMCAX::Coord3 zero = AMCAX::Coord3(0.0, 0.0, 0.0);

		AMCAX::Coord3 p0 = points[0]; 
		AMCAX::Coord3 p1 = points[1];
		AMCAX::Coord3 v1 = p1 - p0;
		AMCAX::Coord3 ref_n = zero; 

		for (int i = 2; i < points.size(); i++) {
			AMCAX::Coord3 v_i = points[i] - p0; 
			AMCAX::Coord3 n_i = v_i.Cross(v1).Normalized(); 

			if (n_i.IsEqual(zero, 1e-6)) {
				continue; 
			}

			if (ref_n.IsEqual(zero, 1e-6)) {	//refence n is still empty. 
				ref_n = n_i; 
			}
			else {
				if (!n_i.IsEqual(ref_n, 1e-6) && !n_i.IsEqual(-ref_n, 1e-6)) {		//reference is not empty now, check
					return false; 
				}
			}
		}

		return true; 
	}


}
