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
#include <ostream>
#include "Quaternion.h"
#include "../Utils/MathUtils.h"
using namespace acamcad;

void Quaternion::setAxisAngle(const AMCAX::Coord3& axis, double angle) {
    const double norm = axis.Norm();
    if (norm < 1E-8) {
        // Null rotation
        q[0] = 0.0; q[1] = 0.0; q[2] = 0.0; q[3] = 1.0;
    }
    else {
        const double sin_half_angle = sin(angle / 2.0);
        q[0] = sin_half_angle * axis[0] / norm;
        q[1] = sin_half_angle * axis[1] / norm;
        q[2] = sin_half_angle * axis[2] / norm;
        q[3] = cos(angle / 2.0);
    }
}

/*! Constructs a Quaternion that will rotate from the \p from direction to the
\p to direction.

Note that this rotation is not uniquely defined. The selected axis is usually
orthogonal to \p from and \p to, minimizing the rotation angle. This method is
robust and can handle small or almost identical vectors. */
Quaternion::Quaternion(const AMCAX::Coord3& from, const AMCAX::Coord3& to) {
    const double epsilon = 1E-10;

    const double fromSqNorm = from.SquaredNorm();
    const double toSqNorm = to.SquaredNorm();
    // Identity Quaternion when one vector is null
    if ((fromSqNorm < epsilon) || (toSqNorm < epsilon)) {
        q[0] = q[1] = q[2] = 0.0; q[3] = 1.0;
    }
    else {
        AMCAX::Coord3 axis = MathUtils::cross(from, to);
        const double axisSqNorm = axis.SquaredNorm();

        // Aligned vectors, pick any axis, not aligned with from or to
        if (axisSqNorm < epsilon)
        {
            if (from.Dot(to) < 0.0)
            {
                if (std::abs(from[0]) > epsilon)
                {
                    axis = AMCAX::Coord3(-from[1], from[0], 0.0);
                }
                else if (std::abs(from[1]) > epsilon)
                {
                    axis = AMCAX::Coord3(0.0, -from[2], from[1]);
                }
                else if (std::abs(from[2]) > epsilon)
                {
                    axis = AMCAX::Coord3(from[2], 0.0, -from[0]);
                }
                axis.Normalize();
                q[0] = axis[0]; q[1] = axis[1]; q[2] = axis[2]; q[3] = 0.0;
            }
            else
            {
                q[0] = q[1] = q[2] = 0.0; q[3] = 1.0;
            }
        }
        else
        {
            double angle = asin(std::sqrt(axisSqNorm / (fromSqNorm * toSqNorm)));

            if (from.Dot(to) < 0.0)
                angle = M_PI - angle;

            setAxisAngle(axis, angle);
        }
    }
}


AMCAX::Coord3 Quaternion::inverseRotate(const AMCAX::Coord3& v) const {
    return inverse().rotate(v);
}

AMCAX::Coord3 Quaternion::rotate(const AMCAX::Coord3& v) const {
    const double q00 = 2.0 * q[0] * q[0];
    const double q11 = 2.0 * q[1] * q[1];
    const double q22 = 2.0 * q[2] * q[2];

    const double q01 = 2.0 * q[0] * q[1];
    const double q02 = 2.0 * q[0] * q[2];
    const double q03 = 2.0 * q[0] * q[3];

    const double q12 = 2.0 * q[1] * q[2];
    const double q13 = 2.0 * q[1] * q[3];

    const double q23 = 2.0 * q[2] * q[3];

    return AMCAX::Coord3(
        (1.0 - q11 - q22) * v[0] + (q01 - q23) * v[1] + (q02 + q13) * v[2],
        (q01 + q23) * v[0] + (1.0 - q22 - q00) * v[1] + (q12 - q03) * v[2],
        (q02 - q13) * v[0] + (q12 + q03) * v[1] + (1.0 - q11 - q00) * v[2]
    );
}


void Quaternion::setFromRotationMatrix(const double m[3][3]) {
    // Compute one plus the trace of the matrix
    const double onePlusTrace = 1.0 + m[0][0] + m[1][1] + m[2][2];

    if (onePlusTrace > 1E-5) {
        // Direct computation
        const double s = std::sqrt(onePlusTrace) * 2.0;
        q[0] = (m[2][1] - m[1][2]) / s;
        q[1] = (m[0][2] - m[2][0]) / s;
        q[2] = (m[1][0] - m[0][1]) / s;
        q[3] = 0.25 * s;
    }
    else {
        // Computation depends on major diagonal term
        if ((m[0][0] > m[1][1]) & (m[0][0] > m[2][2])) {
            const double s = std::sqrt(1.0 + m[0][0] - m[1][1] - m[2][2]) * 2.0;
            q[0] = 0.25 * s;
            q[1] = (m[0][1] + m[1][0]) / s;
            q[2] = (m[0][2] + m[2][0]) / s;
            q[3] = (m[1][2] - m[2][1]) / s;
        }
        else if (m[1][1] > m[2][2]) {
            const double s = std::sqrt(1.0 + m[1][1] - m[0][0] - m[2][2]) * 2.0;
            q[0] = (m[0][1] + m[1][0]) / s;
            q[1] = 0.25 * s;
            q[2] = (m[1][2] + m[2][1]) / s;
            q[3] = (m[0][2] - m[2][0]) / s;
        }
        else {
            const double s = std::sqrt(1.0 + m[2][2] - m[0][0] - m[1][1]) * 2.0;
            q[0] = (m[0][2] + m[2][0]) / s;
            q[1] = (m[1][2] + m[2][1]) / s;
            q[2] = 0.25 * s;
            q[3] = (m[0][1] - m[1][0]) / s;
        }
    }
    normalize();
}

/*! Sets the Quaternion from the three rotated vectors of an orthogonal basis.

  The three vectors do not have to be normalized but must be orthogonal and
  direct (X^Y=k*Z, with k>0).

  \code
  Quaternion q;
  q.setFromRotatedBasis(X, Y, Z);
  // Now q.rotate(AMCAX::Coord3(1,0,0)) == X and q.inverseRotate(X) == AMCAX::Coord3(1,0,0)
  // Same goes for Y and Z with AMCAX::Coord3(0,1,0) and AMCAX::Coord3(0,0,1).
  \endcode

  See also setFromRotationMatrix() and Quaternion(const AMCAX::Coord3&, const AMCAX::Coord3&). */
void Quaternion::setFromRotatedBasis(const AMCAX::Coord3& X, const AMCAX::Coord3& Y, const AMCAX::Coord3& Z) {
    double m[3][3];
    double normX = X.Norm();
    double normY = Y.Norm();
    double normZ = Z.Norm();

    for (int i = 0; i < 3; ++i) {
        m[i][0] = X[i] / normX;
        m[i][1] = Y[i] / normY;
        m[i][2] = Z[i] / normZ;
    }

    setFromRotationMatrix(m);
}


void Quaternion::getAxisAngle(AMCAX::Coord3& axis, double& angle) const {
    angle = 2.0 * acos(q[3]);
    axis = AMCAX::Coord3(q[0], q[1], q[2]);
    const double sinus = axis.Norm();
    if (sinus > 1E-8)
        axis /= sinus;

    if (angle > M_PI) {
        angle = 2.0 * double(M_PI) - angle;
        axis = -axis;
    }
}

AMCAX::Coord3 Quaternion::axis() const {
    AMCAX::Coord3 res = AMCAX::Coord3(q[0], q[1], q[2]);
    const double sinus = res.Norm();
    if (sinus > 1E-8)
        res /= sinus;
    return (acos(q[3]) <= M_PI / 2.0) ? res : -res;
}

double Quaternion::angle() const {
    const double angle = 2.0 * acos(q[3]);
    return (angle <= M_PI) ? angle : 2.0 * M_PI - angle;
}


double acamcad::Quaternion::Length() const
{
    return std::sqrt(LengthSquared());
}

double acamcad::Quaternion::LengthSquared() const
{
    return (q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
}

const double* Quaternion::matrix() const {
    static double m[4][4];
    getMatrix(m);
    return (const double*)(m);
}

void Quaternion::getMatrix(double m[4][4]) const {
    const double q00 = 2.0 * q[0] * q[0];
    const double q11 = 2.0 * q[1] * q[1];
    const double q22 = 2.0 * q[2] * q[2];

    const double q01 = 2.0 * q[0] * q[1];
    const double q02 = 2.0 * q[0] * q[2];
    const double q03 = 2.0 * q[0] * q[3];

    const double q12 = 2.0 * q[1] * q[2];
    const double q13 = 2.0 * q[1] * q[3];

    const double q23 = 2.0 * q[2] * q[3];

    m[0][0] = 1.0 - q11 - q22;
    m[1][0] = q01 - q23;
    m[2][0] = q02 + q13;

    m[0][1] = q01 + q23;
    m[1][1] = 1.0 - q22 - q00;
    m[2][1] = q12 - q03;

    m[0][2] = q02 - q13;
    m[1][2] = q12 + q03;
    m[2][2] = 1.0 - q11 - q00;

    m[0][3] = 0.0; m[1][3] = 0.0; m[2][3] = 0.0;
    m[3][0] = 0.0; m[3][1] = 0.0; m[3][2] = 0.0;
    m[3][3] = 1.0;
}

void Quaternion::getMatrix(double m[16]) const {
    static double mat[4][4];
    getMatrix(mat);
    int count = 0;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            m[count++] = mat[i][j];
}

void Quaternion::getRotationMatrix(double m[3][3]) const {
    static double mat[4][4];
    getMatrix(mat);
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            // Beware of transposition
            m[i][j] = double(mat[j][i]);
}

const double* Quaternion::inverseMatrix() const {
    static double m[4][4];
    getInverseMatrix(m);
    return (const double*)(m);
}

void Quaternion::getInverseMatrix(double m[4][4]) const {
    inverse().getMatrix(m);
}

void Quaternion::getInverseMatrix(double m[16]) const {
    inverse().getMatrix(m);
}

void Quaternion::getInverseRotationMatrix(double m[3][3]) const {
    static double mat[4][4];
    getInverseMatrix(mat);
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            // Beware of transposition
            m[i][j] = double(mat[j][i]);
}


Quaternion Quaternion::slerp(const Quaternion& a, const Quaternion& b, double t, bool allowFlip)
{
    //a,b is unit
    double cosAngle = a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];

    double c1, c2;
    // Linear interpolation for close orientations
    if ((1.0 - fabs(cosAngle)) < 0.01) {
        c1 = 1.0 - t;
        c2 = t;
    }
    else {
        // Spherical interpolation
        double angle = acos(fabs(cosAngle));
        double sinAngle = sin(angle);
        c1 = sin(angle * (1.0 - t)) / sinAngle;
        c2 = sin(angle * t) / sinAngle;
    }

    // Use the shortest path
    if (allowFlip && (cosAngle < 0.0))
        c1 = -c1;

    return Quaternion(c1 * a[0] + c2 * b[0], c1 * a[1] + c2 * b[1],
        c1 * a[2] + c2 * b[2], c1 * a[3] + c2 * b[3]);
}

/*! Returns the slerp interpolation of the two Quaternions \p a and \p b, at
  time \p t, using tangents \p tgA and \p tgB.

  The resulting Quaternion is "between" \p a and \p b (result is \p a when \p
  t=0 and \p b for \p t=1).

  Use squadTangent() to define the Quaternion tangents \p tgA and \p tgB. */
Quaternion Quaternion::squad(const Quaternion& a, const Quaternion& tgA,
    const Quaternion& tgB, const Quaternion& b,
    double t) {
    Quaternion ab = Quaternion::slerp(a, b, t);
    Quaternion tg = Quaternion::slerp(tgA, tgB, t, false);
    return Quaternion::slerp(ab, tg, 2.0 * t * (1.0 - t), false);
}


Quaternion Quaternion::log() {
    double vlen = std::sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2]);
    // qlen = 1;

    if (vlen < 1E-6)
        return Quaternion(q[0], q[1], q[2], 0.0);
    else {
        double coef = acos(q[3]) / vlen;
        return Quaternion(q[0] * coef, q[1] * coef, q[2] * coef, 0.0);
    }
}

Quaternion Quaternion::exp() {
    double vlen = std::sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2]);
    // qlen = 1;

    if (vlen < 1E-6)
        return Quaternion(q[0], q[1], q[2], cos(vlen));
    else {
        double coef = sin(vlen) / vlen;
        return Quaternion(q[0] * coef, q[1] * coef, q[2] * coef, cos(vlen));
    }
}

Quaternion Quaternion::lnDif(const Quaternion& a, const Quaternion& b) {
    Quaternion dif = a.inverse() * b;
    dif.normalize();
    return dif.log();
}

std::ostream& operator<<(std::ostream& o, const Quaternion& Q) {
    return o << Q[0] << '\t' << Q[1] << '\t' << Q[2] << '\t' << Q[3];
}
