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
#pragma once

#include <QObject>
#include <QString>

//#include <QOpenGLWidget>
#include "../../Utils/Quaternion.h"
#include <common/VectorT.hpp>

namespace qglviewer
{
	typedef AMCAX::Coord3 Vec;
	typedef acamcad::Quaternion Quaternion;
}


#if _WIN32 || _WIN64
#include <Windows.h>
#endif
#include <GL/glu.h>

namespace qglviewer {
	/*
	* The Frame class represents a coordinate system, defined by a position and an orientation.
	*
	* The position and the orientation of a Frame are actually defined with respect to a referenceFrame().
	* The default referenceFrame() is the world coordinate system (represented by a nullptr referenceFrame()).
	*
	* the local translation() and rotation(), defined with respect to the referenceFrame(),
	* the global position() and orientation(), always defined with respect to the world coordinate system.
	*/

	class Frame : public QObject {
		Q_OBJECT

	public:
		Frame();

		/*! Virtual destructor. Empty. */
		virtual ~Frame() {}

		Frame(const Frame& frame);
		Frame& operator=(const Frame& frame);

	Q_SIGNALS:
		void modified();

	public:
		Frame(const Vec& position, const Quaternion& orientation);

		void setPosition(const Vec& position);
		void setPosition(qreal x, qreal y, qreal z);

		void setOrientation(const Quaternion& orientation);
		void setOrientation(qreal q0, qreal q1, qreal q2, qreal q3);

		void setPositionAndOrientation(const Vec& position,
			const Quaternion& orientation);

		Vec position() const;
		Quaternion orientation() const;

		void getPosition(qreal& x, qreal& y, qreal& z) const;
		void getOrientation(qreal& q0, qreal& q1, qreal& q2, qreal& q3) const;

	public:
		void setTranslation(const Vec& translation) {
			t_ = translation;
			Q_EMIT modified();
		}
		void setTranslation(qreal x, qreal y, qreal z);

		void setRotation(const Quaternion& rotation) {
			q_ = rotation;
			Q_EMIT modified();
		}
		void setRotation(qreal q0, qreal q1, qreal q2, qreal q3);

		void setTranslationAndRotation(const Vec& translation,
			const Quaternion& rotation);

		Vec translation() const { return t_; }
		Quaternion rotation() const { return q_; }

		void getTranslation(qreal& x, qreal& y, qreal& z) const;
		void getRotation(qreal& q0, qreal& q1, qreal& q2, qreal& q3) const;

	public:
		const Frame* referenceFrame() const { return referenceFrame_; }
		void setReferenceFrame(const Frame* const refFrame);
		bool settingAsReferenceFrameWillCreateALoop(const Frame* const frame);

		/*! Frame modification */
		void translate(Vec& t);
		void translate(const Vec& t);
		void translate(qreal x, qreal y, qreal z);
		void translate(qreal& x, qreal& y, qreal& z);

		void rotate(Quaternion& q);
		void rotate(const Quaternion& q);
		void rotate(qreal q0, qreal q1, qreal q2, qreal q3);
		void rotate(qreal& q0, qreal& q1, qreal& q2, qreal& q3);

		void rotateAroundPoint(Quaternion& rotation, const Vec& point);
		void rotateAroundPoint(const Quaternion& rotation, const Vec& point);

		void alignWithFrame(const Frame* const frame, bool move = false, qreal threshold = 0.0);
		void projectOnLine(const Vec& origin, const Vec& direction);

		/*! Coordinate system transformation of position */
		Vec coordinatesOf(const Vec& src) const;
		Vec inverseCoordinatesOf(const Vec& src) const;
		Vec localCoordinatesOf(const Vec& src) const;
		Vec localInverseCoordinatesOf(const Vec& src) const;
		Vec coordinatesOfIn(const Vec& src, const Frame* const in) const;
		Vec coordinatesOfFrom(const Vec& src, const Frame* const from) const;

		void getCoordinatesOf(const qreal src[3], qreal res[3]) const;
		void getInverseCoordinatesOf(const qreal src[3], qreal res[3]) const;
		void getLocalCoordinatesOf(const qreal src[3], qreal res[3]) const;
		void getLocalInverseCoordinatesOf(const qreal src[3], qreal res[3]) const;
		void getCoordinatesOfIn(const qreal src[3], qreal res[3],
			const Frame* const in) const;
		void getCoordinatesOfFrom(const qreal src[3], qreal res[3],
			const Frame* const from) const;

		/*! Coordinate system transformation of vectors */
		Vec transformOf(const Vec& src) const;
		Vec inverseTransformOf(const Vec& src) const;
		Vec localTransformOf(const Vec& src) const;
		Vec localInverseTransformOf(const Vec& src) const;
		Vec transformOfIn(const Vec& src, const Frame* const in) const;
		Vec transformOfFrom(const Vec& src, const Frame* const from) const;

		void getTransformOf(const qreal src[3], qreal res[3]) const;
		void getInverseTransformOf(const qreal src[3], qreal res[3]) const;
		void getLocalTransformOf(const qreal src[3], qreal res[3]) const;
		void getLocalInverseTransformOf(const qreal src[3], qreal res[3]) const;
		void getTransformOfIn(const qreal src[3], qreal res[3],
			const Frame* const in) const;
		void getTransformOfFrom(const qreal src[3], qreal res[3],
			const Frame* const from) const;

	public:
		const GLdouble* matrix() const;
		void getMatrix(GLdouble m[4][4]) const;
		void getMatrix(GLdouble m[16]) const;

		const GLdouble* worldMatrix() const;
		void getWorldMatrix(GLdouble m[4][4]) const;
		void getWorldMatrix(GLdouble m[16]) const;

		void setFromMatrix(const GLdouble m[4][4]);
		void setFromMatrix(const GLdouble m[16]);

		/*! Returns the inverse() of the Frame Local.*/
		Frame inverse() const;
		/*! Returns the inverse() of the Frame world transformation.*/
		Frame worldInverse() const {
			return Frame(-(orientation().inverseRotate(position())), orientation().inverse());
		}

	private:
		// Position   and   orientation
		Vec t_;
		Quaternion q_;

		// Frame   composition
		const Frame* referenceFrame_;
	};

} // namespace qglviewer