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

#include <math.h>
#include <common/CoordT.hpp>

namespace acamcad {

/// see https://en.wikipedia.org/wiki/Quaternion
/// only for rotation

class Quaternion {
private:
    double q[4];    //(x,y,z,w)

public:
    Quaternion() {
        q[0] = q[1] = q[2] = 0.0; q[3] = 1.0;
    }

    Quaternion(const AMCAX::Coord3& axis, double angle) { setAxisAngle(axis, angle); }

    Quaternion(const AMCAX::Coord3& from, const AMCAX::Coord3& to);

    Quaternion(double q0, double q1, double q2, double q3)
    {
        q[0] = q0; q[1] = q1; q[2] = q2; q[3] = q3;
    }

    Quaternion(const Quaternion& Q) {
        q[0] = Q.q[0]; q[1] = Q.q[1]; q[2] = Q.q[2]; q[3] = Q.q[3];
    }

    Quaternion& operator=(const Quaternion& Q) {
        q[0] = Q.q[0]; q[1] = Q.q[1]; q[2] = Q.q[2]; q[3] = Q.q[3];
        return (*this);
    }
    void setValue(double q0, double q1, double q2, double q3) {
        q[0] = q0; q[1] = q1; q[2] = q2; q[3] = q3;
    }

    void setAxisAngle(const AMCAX::Coord3& axis, double angle);

    double operator[](int i) const { return q[i]; }

    double& operator[](int i) { return q[i]; }

    void setFromRotationMatrix(const double m[3][3]);
    void setFromRotatedBasis(const AMCAX::Coord3& X, const AMCAX::Coord3& Y, const AMCAX::Coord3& Z);

    AMCAX::Coord3 axis() const;
    double angle() const;
    void getAxisAngle(AMCAX::Coord3& axis, double& angle) const;

    /*! Returns the composition of the \p a and \p b rotations. */
    /// Note that a*b usually differs from b*a.
    friend Quaternion operator*(const Quaternion& a, const Quaternion& b) {
        return Quaternion(
            a.q[3] * b.q[0] + b.q[3] * a.q[0] + a.q[1] * b.q[2] - a.q[2] * b.q[1],
            a.q[3] * b.q[1] + b.q[3] * a.q[1] + a.q[2] * b.q[0] - a.q[0] * b.q[2],
            a.q[3] * b.q[2] + b.q[3] * a.q[2] + a.q[0] * b.q[1] - a.q[1] * b.q[0],
            a.q[3] * b.q[3] - b.q[0] * a.q[0] - a.q[1] * b.q[1] - a.q[2] * b.q[2]);
    }

    /*! Quaternion rotation is composed with \p q. it is Right multiplication*/
    Quaternion& operator*=(const Quaternion& q) {
        *this = (*this) * q;
        return *this;
    }

    /*! Returns the image of \p v by the rotation \p q. */
    friend AMCAX::Coord3 operator*(const Quaternion& q, const AMCAX::Coord3& v) {
        return q.rotate(v);
    }

    /*! rotation qPq^-1 */
    AMCAX::Coord3 rotate(const AMCAX::Coord3& v) const;
    AMCAX::Coord3 inverseRotate(const AMCAX::Coord3& v) const;

    /*! Returns the inverse Quaternion (inverse rotation). same rotation angle(), but negated axis()*/
    Quaternion inverse() const { return Quaternion(-q[0], -q[1], -q[2], q[3]); }

    void invert()
    {
        q[0] = -q[0]; q[1] = -q[1];  q[2] = -q[2];
    }

    void negate()
    {
        q[0] = -q[0]; q[1] = -q[1];  q[2] = -q[2]; q[3] = -q[3];
    }

    double normalize() {
        const double norm = std::sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
        for (int i = 0; i < 4; ++i)
            q[i] /= norm;
        return norm;
    }

    Quaternion normalized() const {
        double Q[4];
        const double norm = std::sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
        for (int i = 0; i < 4; ++i)
            Q[i] = q[i] / norm;
        return Quaternion(Q[0], Q[1], Q[2], Q[3]);
    }

    double Length() const;
    double LengthSquared() const;

    const double* matrix() const;
    void getMatrix(double m[4][4]) const;
    void getMatrix(double m[16]) const;

    void getRotationMatrix(double m[3][3]) const;

    const double* inverseMatrix() const;
    void getInverseMatrix(double m[4][4]) const;
    void getInverseMatrix(double m[16]) const;

    void getInverseRotationMatrix(double m[3][3]) const;

    /// slerp(a,b,t) = sin((1-t)w)/sin(w)a + sin(tw)/sin(w)b
    static Quaternion slerp(const Quaternion& a, const Quaternion& b, double t, bool allowFlip = true);
    static Quaternion squad(const Quaternion& a, const Quaternion& tgA,
        const Quaternion& tgB, const Quaternion& b, double t);

    //!!!default |q| = 1.0, q[3] = 0.0(a = 0.0) ;
    Quaternion log();      //log(q) = log(|q|) + V/|V|*acos(a/|q|), where V = b*i + c*j + d*k.
    Quaternion exp();      //exp(q) = e ^ a * (cos(| V | ) + V/|V | *sin(| V | )), where V = b * i + c * j + d * k.

    /// The geodesic distance dg(p, q)
    static Quaternion lnDif(const Quaternion& a, const Quaternion& b);
};

} // namespace acamcad

std::ostream& operator<<(std::ostream& o, const acamcad::Quaternion&);
