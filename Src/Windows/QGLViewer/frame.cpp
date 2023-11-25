/* ===================================================================
* This file is a modification of libQGLViewer.
*
* Quote from https://github.com/GillesDebunne/libQGLViewer
*
* "libQGLViewer uses dual licensing: it is freely available under the
* terms of the GNU-GPL license for open source software development,
* while commercial applications can apply for a commercial license.
*
* http://www.libqglviewer.com
* "
* ====================================================================
*/
#include "frame.h"
#include <math.h>
#include "../../Utils/MathUtils.h"

using namespace qglviewer;
using namespace acamcad;

Frame::Frame() : referenceFrame_(nullptr) {}

Frame::Frame(const Vec& position, const Quaternion& orientation)
    : t_(position), q_(orientation),  referenceFrame_(nullptr) {}

Frame& Frame::operator=(const Frame& frame) {
    setTranslationAndRotation(frame.translation(), frame.rotation());
    setReferenceFrame(frame.referenceFrame());
    return *this;
}

Frame::Frame(const Frame& frame) : QObject() { (*this) = frame; }

/////////////////////////////// MATRICES //////////////////////////////////////

const GLdouble* Frame::matrix() const {
    static GLdouble m[4][4];
    getMatrix(m);
    return (const GLdouble*)(m);
}

void Frame::getMatrix(GLdouble m[4][4]) const {
    
    q_.getMatrix(m); 

    m[3][0] = t_[0];
    m[3][1] = t_[1];
    m[3][2] = t_[2];
}

void Frame::getMatrix(GLdouble m[16]) const {
    q_.getMatrix(m); 

    m[12] = t_[0];
    m[13] = t_[1];
    m[14] = t_[2];
}

/*! Returns a Frame representing the inverse of the Frame space transformation.

  The rotation() of the new Frame is the Quaternion::inverse() of the original
  rotation. Its translation() is the negated inverse rotated image of the
  original translation.

  If a Frame is considered as a space rigid transformation (translation and
  rotation), the inverse() Frame performs the inverse transformation.

  Only the local Frame transformation (i.e. defined with respect to the
  referenceFrame()) is inverted. Use worldInverse() for a global inverse.

  The resulting Frame has the same referenceFrame() as the Frame and a \c nullptr
  constraint().

  \note The scaling factor of the 4x4 matrix is 1.0. */
Frame Frame::inverse() const {
    Frame fr(-(q_.inverseRotate(t_)), q_.inverse());
    fr.setReferenceFrame(referenceFrame());
    return fr;
}

const GLdouble* Frame::worldMatrix() const {
    if (referenceFrame()) {
        static Frame fr;
        fr.setTranslation(position());
        fr.setRotation(orientation());
        return fr.matrix();
    }
    else
        return matrix();
}

void Frame::getWorldMatrix(GLdouble m[4][4]) const {
    const GLdouble* mat = worldMatrix();
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            m[i][j] = mat[i * 4 + j];
}

void Frame::getWorldMatrix(GLdouble m[16]) const {
    const GLdouble* mat = worldMatrix();
    for (int i = 0; i < 16; ++i)
        m[i] = mat[i];
}


void Frame::setFromMatrix(const GLdouble m[4][4]) {
    if (fabs(m[3][3]) < 1E-8) {
        qWarning("Frame::setFromMatrix: Null homogeneous coefficient");
        return;
    }

    qreal rot[3][3];
    for (int i = 0; i < 3; ++i) {
        t_[i] = m[3][i] / m[3][3];
        for (int j = 0; j < 3; ++j)
            // Beware of the transposition (OpenGL to European math)
            rot[i][j] = m[j][i] / m[3][3];
    }
    q_.setFromRotationMatrix(rot);
    Q_EMIT modified();
}

void Frame::setFromMatrix(const GLdouble m[16]) {
    GLdouble mat[4][4];
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            mat[i][j] = m[i * 4 + j];
    setFromMatrix(mat);
}

//////////////////// SET AND GET LOCAL TRANSLATION AND ROTATION //////////////////////////////////

void Frame::setTranslation(qreal x, qreal y, qreal z) {
    setTranslation(Vec(x, y, z));
}

void Frame::getTranslation(qreal& x, qreal& y, qreal& z) const {
    const Vec t = translation();
    x = t[0];
    y = t[1];
    z = t[2];
}


void Frame::setRotation(qreal q0, qreal q1, qreal q2, qreal q3) {
    setRotation(Quaternion(q0, q1, q2, q3));
}

void Frame::getRotation(qreal& q0, qreal& q1, qreal& q2, qreal& q3) const {
    const Quaternion q = rotation();
    q0 = q[0];
    q1 = q[1];
    q2 = q[2];
    q3 = q[3];
}

//////////////////////translate and rotation frame//////////////////////////

void Frame::translate(const Vec& t) {
    Vec tbis = t;
    translate(tbis);
}

void Frame::translate(Vec& t) {
    t_ += t;
    Q_EMIT modified();
}

void Frame::translate(qreal x, qreal y, qreal z) {
    Vec t(x, y, z);
    translate(t);
}

void Frame::translate(qreal& x, qreal& y, qreal& z) {
    Vec t(x, y, z);
    translate(t);
    x = t[0];
    y = t[1];
    z = t[2];
}


void Frame::rotate(const Quaternion& q) {
    Quaternion qbis = q;
    rotate(qbis);
}

void Frame::rotate(Quaternion& q) {
    q_ *= q;
    q_.normalize(); // Prevents numerical drift
    Q_EMIT modified();
}

void Frame::rotate(qreal& q0, qreal& q1, qreal& q2, qreal& q3) {
    Quaternion q(q0, q1, q2, q3);
    rotate(q);
    q0 = q[0];
    q1 = q[1];
    q2 = q[2];
    q3 = q[3];
}

void Frame::rotate(qreal q0, qreal q1, qreal q2, qreal q3) {
    Quaternion q(q0, q1, q2, q3);
    rotate(q);
}


void Frame::rotateAroundPoint(Quaternion& rotation, const Vec& point) {
    q_ *= rotation;
    q_.normalize(); // Prevents numerical drift

    Vec trans = point +
        Quaternion(inverseTransformOf(rotation.axis()), rotation.angle()).rotate(position() - point) - t_;
    t_ += trans;
    Q_EMIT modified();
}

void Frame::rotateAroundPoint(const Quaternion& rotation, const Vec& point) {
    Quaternion rot = rotation;
    rotateAroundPoint(rot, point);
}

//////////////////// SET AND GET WORLD POSITION AND ORIENTATION //////////////////////////////////

/*! Sets the position() of the Frame, defined in the world coordinate system.
Emits the modified() signal.

Use setTranslation() to define the \e local frame translation (with respect to
the referenceFrame()). The potential constraint() of the Frame is not taken into
account, use setPositionWithConstraint() instead. */
void Frame::setPosition(const Vec& position) {
    if (referenceFrame())
        setTranslation(referenceFrame()->coordinatesOf(position));
    else
        setTranslation(position);
}

void Frame::setPosition(qreal x, qreal y, qreal z) {
    setPosition(Vec(x, y, z));
}

void Frame::setPositionAndOrientation(const Vec& position,
    const Quaternion& orientation) {
    if (referenceFrame()) {
        t_ = referenceFrame()->coordinatesOf(position);
        q_ = referenceFrame()->orientation().inverse() * orientation;
    }
    else {
        t_ = position;
        q_ = orientation;
    }
    Q_EMIT modified();
}

void Frame::setTranslationAndRotation(const Vec& translation,
    const Quaternion& rotation) {
    t_ = translation;
    q_ = rotation;
    Q_EMIT modified();
}

void Frame::getPosition(qreal& x, qreal& y, qreal& z) const {
    Vec p = position();
    x = p[0];
    y = p[1];
    z = p[2];
}

void Frame::setOrientation(const Quaternion& orientation) {
    if (referenceFrame())
        setRotation(referenceFrame()->orientation().inverse() * orientation);
    else
        setRotation(orientation);
}

void Frame::setOrientation(qreal q0, qreal q1, qreal q2, qreal q3) {
    setOrientation(Quaternion(q0, q1, q2, q3));
}

void Frame::getOrientation(qreal& q0, qreal& q1, qreal& q2, qreal& q3) const {
    Quaternion o = orientation();
    q0 = o[0];
    q1 = o[1];
    q2 = o[2];
    q3 = o[3];
}


Vec Frame::position() const {
    if (referenceFrame_)
        return inverseCoordinatesOf(Vec(0.0, 0.0, 0.0));
    else
        return t_;
}

Quaternion Frame::orientation() const {
    Quaternion res = rotation();
    const Frame* fr = referenceFrame();
    while (fr != nullptr) {
        res = fr->rotation() * res;
        fr = fr->referenceFrame();
    }
    return res;
}


///////////////////////////// REFERENCE FRAMES//////////////////////////////////////////

/*! Sets the referenceFrame() of the Frame.*/
void Frame::setReferenceFrame(const Frame* const refFrame) {
    if (settingAsReferenceFrameWillCreateALoop(refFrame))
        qWarning("Frame::setReferenceFrame would create a loop in Frame hierarchy");
    else {
        bool identical = (referenceFrame_ == refFrame);
        referenceFrame_ = refFrame;
        if (!identical)
            Q_EMIT modified();
    }
}

/*! Returns \c true if setting \p frame as the Frame's referenceFrame() would
  create a loop in the Frame hierarchy. */
bool Frame::settingAsReferenceFrameWillCreateALoop(const Frame* const frame) {
    const Frame* f = frame;
    while (f != nullptr) {
        if (f == this)
            return true;
        f = f->referenceFrame();
    }
    return false;
}

///////////////////////// FRAME TRANSFORMATIONS OF 3D POINTS /////////////////////////////////

/*! Returns the Frame coordinates of a point \p src defined in the world
 coordinate system (converts from world to Frame).*/
Vec Frame::coordinatesOf(const Vec& src) const {
    if (referenceFrame())
        return localCoordinatesOf(referenceFrame()->coordinatesOf(src));
    else
        return localCoordinatesOf(src);
}

/*! Returns the world coordinates of the point whose position in the Frame
  coordinate system is \p src (converts from Frame to world).*/
Vec Frame::inverseCoordinatesOf(const Vec& src) const {
    const Frame* fr = this;
    Vec res = src;
    while (fr != nullptr) {
        res = fr->localInverseCoordinatesOf(res);
        fr = fr->referenceFrame();
    }
    return res;
}

/*! Returns the Frame coordinates of a point \p src defined in the
  referenceFrame() coordinate system (converts from referenceFrame() to Frame).*/
Vec Frame::localCoordinatesOf(const Vec& src) const {
    return rotation().inverseRotate(src - translation());
}

/*! Returns the referenceFrame() coordinates of a point \p src defined in the
 Frame coordinate system (converts from Frame to referenceFrame()).*/
Vec Frame::localInverseCoordinatesOf(const Vec& src) const {
    return rotation().rotate(src) + translation();
}

/*! Returns the Frame coordinates of the point whose position in the \p from
  coordinate system is \p src (converts from \p from to Frame).*/
Vec Frame::coordinatesOfFrom(const Vec& src, const Frame* const from) const {
    if (this == from)
        return src;
    else if (referenceFrame())
        return localCoordinatesOf(referenceFrame()->coordinatesOfFrom(src, from));
    else
        return localCoordinatesOf(from->inverseCoordinatesOf(src));
}

/*! Returns the \p in coordinates of the point whose position in the Frame
  coordinate system is \p src (converts from Frame to \p in).*/
Vec Frame::coordinatesOfIn(const Vec& src, const Frame* const in) const {
    const Frame* fr = this;
    Vec res = src;
    while ((fr != nullptr) && (fr != in)) {
        res = fr->localInverseCoordinatesOf(res);
        fr = fr->referenceFrame();
    }

    if (fr != in)
        // in was not found in the branch of this, res is now expressed in the world
        // coordinate system. Simply convert to in coordinate system.
        res = in->coordinatesOf(res);

    return res;
}

////// qreal[3] versions

/*! Same as coordinatesOf(), but with \c qreal parameters. */
void Frame::getCoordinatesOf(const qreal src[3], qreal res[3]) const {
    const Vec r = coordinatesOf(Vec(src[0], src[1], src[2]));
    for (int i = 0; i < 3; ++i)
        res[i] = r[i];
}

/*! Same as inverseCoordinatesOf(), but with \c qreal parameters. */
void Frame::getInverseCoordinatesOf(const qreal src[3], qreal res[3]) const {
    const Vec r = inverseCoordinatesOf(Vec(src[0], src[1], src[2]));
    for (int i = 0; i < 3; ++i)
        res[i] = r[i];
}

/*! Same as localCoordinatesOf(), but with \c qreal parameters. */
void Frame::getLocalCoordinatesOf(const qreal src[3], qreal res[3]) const {
    const Vec r = localCoordinatesOf(Vec(src[0], src[1], src[2]));
    for (int i = 0; i < 3; ++i)
        res[i] = r[i];
}

/*! Same as localInverseCoordinatesOf(), but with \c qreal parameters. */
void Frame::getLocalInverseCoordinatesOf(const qreal src[3],
    qreal res[3]) const {
    const Vec r = localInverseCoordinatesOf(Vec(src[0], src[1], src[2]));
    for (int i = 0; i < 3; ++i)
        res[i] = r[i];
}

/*! Same as coordinatesOfIn(), but with \c qreal parameters. */
void Frame::getCoordinatesOfIn(const qreal src[3], qreal res[3],
    const Frame* const in) const {
    const Vec r = coordinatesOfIn(Vec(src[0], src[1], src[2]), in);
    for (int i = 0; i < 3; ++i)
        res[i] = r[i];
}

/*! Same as coordinatesOfFrom(), but with \c qreal parameters. */
void Frame::getCoordinatesOfFrom(const qreal src[3], qreal res[3],
    const Frame* const from) const {
    const Vec r = coordinatesOfFrom(Vec(src[0], src[1], src[2]), from);
    for (int i = 0; i < 3; ++i)
        res[i] = r[i];
}

///////////////////////// FRAME TRANSFORMATIONS OF VECTORS/////////////////////////////////

/*! Returns the Frame transform of a vector \p src defined in the world
 coordinate system (converts vectors from world to Frame).*/
Vec Frame::transformOf(const Vec& src) const {
    if (referenceFrame())
        return localTransformOf(referenceFrame()->transformOf(src));
    else
        return localTransformOf(src);
}

/*! Returns the world transform of the vector whose coordinates in the Frame
  coordinate system is \p src (converts vectors from Frame to world).*/
Vec Frame::inverseTransformOf(const Vec& src) const {
    const Frame* fr = this;
    Vec res = src;
    while (fr != nullptr) {
        res = fr->localInverseTransformOf(res);
        fr = fr->referenceFrame();
    }
    return res;
}

/*! Returns the Frame transform of a vector \p src defined in the referenceFrame() 
  coordinate system (converts vectors from referenceFrame() toFrame).*/
Vec Frame::localTransformOf(const Vec& src) const {
    return rotation().inverseRotate(src);
}

/*! Returns the referenceFrame() transform of a vector \p src defined in the
 Frame coordinate system (converts vectors from Frame to referenceFrame()).*/
Vec Frame::localInverseTransformOf(const Vec& src) const {
    return rotation().rotate(src);
}

/*! Returns the Frame transform of the vector whose coordinates in the \p from
  coordinate system is \p src (converts vectors from \p from to Frame).*/
Vec Frame::transformOfFrom(const Vec& src, const Frame* const from) const {
    if (this == from)
        return src;
    else if (referenceFrame())
        return localTransformOf(referenceFrame()->transformOfFrom(src, from));
    else
        return localTransformOf(from->inverseTransformOf(src));
}

/*! Returns the \p in transform of the vector whose coordinates in the Frame
  coordinate system is \p src (converts vectors from Frame to \p in).*/
Vec Frame::transformOfIn(const Vec& src, const Frame* const in) const {
    const Frame* fr = this;
    Vec res = src;
    while ((fr != nullptr) && (fr != in)) {
        res = fr->localInverseTransformOf(res);
        fr = fr->referenceFrame();
    }

    if (fr != in)
        // in was not found in the branch of this, res is now expressed in the world
        // coordinate system. Simply convert to in coordinate system.
        res = in->transformOf(res);

    return res;
}

/////////////////  qreal[3] versions  //////////////////////

/*! Same as transformOf(), but with \c qreal parameters. */
void Frame::getTransformOf(const qreal src[3], qreal res[3]) const {
    Vec r = transformOf(Vec(src[0], src[1], src[2]));
    for (int i = 0; i < 3; ++i)
        res[i] = r[i];
}

/*! Same as inverseTransformOf(), but with \c qreal parameters. */
void Frame::getInverseTransformOf(const qreal src[3], qreal res[3]) const {
    Vec r = inverseTransformOf(Vec(src[0], src[1], src[2]));
    for (int i = 0; i < 3; ++i)
        res[i] = r[i];
}

/*! Same as localTransformOf(), but with \c qreal parameters. */
void Frame::getLocalTransformOf(const qreal src[3], qreal res[3]) const {
    Vec r = localTransformOf(Vec(src[0], src[1], src[2]));
    for (int i = 0; i < 3; ++i)
        res[i] = r[i];
}

/*! Same as localInverseTransformOf(), but with \c qreal parameters. */
void Frame::getLocalInverseTransformOf(const qreal src[3], qreal res[3]) const {
    Vec r = localInverseTransformOf(Vec(src[0], src[1], src[2]));
    for (int i = 0; i < 3; ++i)
        res[i] = r[i];
}

/*! Same as transformOfIn(), but with \c qreal parameters. */
void Frame::getTransformOfIn(const qreal src[3], qreal res[3],
    const Frame* const in) const {
    Vec r = transformOfIn(Vec(src[0], src[1], src[2]), in);
    for (int i = 0; i < 3; ++i)
        res[i] = r[i];
}

/*! Same as transformOfFrom(), but with \c qreal parameters. */
void Frame::getTransformOfFrom(const qreal src[3], qreal res[3],
    const Frame* const from) const {
    Vec r = transformOfFrom(Vec(src[0], src[1], src[2]), from);
    for (int i = 0; i < 3; ++i)
        res[i] = r[i];
}

/////////////////////////////////   ALIGN   /////////////////////////////////

/*! Aligns the Frame with \p frame, so that two of their axis are parallel. */
void Frame::alignWithFrame(const Frame* const frame, bool move,
    qreal threshold) {
    Vec directions[2][3];
    for (unsigned short d = 0; d < 3; ++d) {
        Vec dir((d == 0) ? 1.0 : 0.0, (d == 1) ? 1.0 : 0.0, (d == 2) ? 1.0 : 0.0);
        if (frame)
            directions[0][d] = frame->inverseTransformOf(dir);
        else
            directions[0][d] = dir;
        directions[1][d] = inverseTransformOf(dir);
    }

    qreal maxProj = 0.0;
    qreal proj;
    unsigned short index[2];
    index[0] = index[1] = 0;
    for (unsigned short i = 0; i < 3; ++i)
        for (unsigned short j = 0; j < 3; ++j)
            if ((proj = fabs(directions[0][i] | directions[1][j])) >= maxProj) {
                index[0] = i;
                index[1] = j;
                maxProj = proj;
            }

    Frame old;
    old = *this;

    qreal coef = directions[0][index[0]] | directions[1][index[1]];
    if (fabs(coef) >= threshold) {
        const Vec axis = MathUtils::cross(directions[0][index[0]], directions[1][index[1]]);
        qreal angle = asin(axis.Norm());
        if (coef >= 0.0)
            angle = -angle;
        rotate(rotation().inverse() * Quaternion(axis, angle) * orientation());

        // Try to align an other axis direction
        unsigned short d = (index[1] + 1) % 3;
        Vec dir((d == 0) ? 1.0 : 0.0, (d == 1) ? 1.0 : 0.0, (d == 2) ? 1.0 : 0.0);
        dir = inverseTransformOf(dir);

        qreal max = 0.0;
        for (unsigned short i = 0; i < 3; ++i) {
            qreal proj = fabs(directions[0][i] | dir);
            if (proj > max) {
                index[0] = i;
                max = proj;
            }
        }

        if (max >= threshold) {
            const Vec axis = MathUtils::cross(directions[0][index[0]], dir);
            qreal angle = asin(axis.Norm());
            if ((directions[0][index[0]] | dir) >= 0.0)
                angle = -angle;
            rotate(rotation().inverse() * Quaternion(axis, angle) * orientation());
        }
    }

    if (move) {
        Vec getCenter;
        if (frame)
            getCenter = frame->position();

        translate(getCenter - orientation().rotate(old.coordinatesOf(getCenter)) -
            translation());
    }
}

/*! Translates the Frame so that its position() lies on the line defined by \p
origin and \p direction (defined in the world coordinate system).*/
void Frame::projectOnLine(const Vec& origin, const Vec& direction) {
    // If you are trying to find a bug here, because of memory problems, you waste
    // your time. This is a bug in the gcc 3.3 compiler. Compile the library in
    // debug mode and test. Uncommenting this line also seems to solve the
    // problem. Horrible. cout << "position = " << position() << endl; If you
    // found a problem or are using a different compiler, please let me know.
 /*   const Vec shift = origin - position();
    Vec proj = shift;
    AMCAX::Vector3 vec; 
    
    proj.projectOnAxis(direction);
    translate(shift - proj); */
}
