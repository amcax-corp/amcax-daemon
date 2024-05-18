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
#pragma once

#include "frame.h"
#include "qglviewer.h"

#include <QDateTime>
#include <QString>
#include <QElapsedTimer>

namespace qglviewer {
	/*! A ManipulatedFrame is a Frame that can be rotated and translated using the mouse. */

	class ManipulatedFrame : public qglviewer::Frame {
#ifndef DOXYGEN
		friend class Camera;
		friend class ::QGLViewer;
#endif

		Q_OBJECT

	public:
		ManipulatedFrame();
		/*! Virtual destructor. Empty. */
		virtual ~ManipulatedFrame() {}

		ManipulatedFrame(const ManipulatedFrame& mf);
		ManipulatedFrame& operator=(const ManipulatedFrame& mf);

	Q_SIGNALS:
		/*! This signal is emitted when ever the ManipulatedFrame is manipulated (i.e.rotated or translated) using the mouse.
		Connect this signal to any object that should be notified.

		This signal is automatically connected to the QGLViewer::update() slot */
		void manipulated();

		/*! This signal is emitted when the ManipulatedFrame isSpinning().
		This signal is automatically connected to the QGLViewer::update() slot */
		void spun();

		/*! @name Manipulation sensitivity */
		//@{
	public Q_SLOTS:
		/*! Defines the rotationSensitivity(). */
		void setRotationSensitivity(qreal sensitivity) {
			rotationSensitivity_ = sensitivity;
		}
		/*! Defines the translationSensitivity(). */
		void setTranslationSensitivity(qreal sensitivity) {
			translationSensitivity_ = sensitivity;
		}
		/*! Defines the spinningSensitivity(), in pixels per milliseconds. */
		void setSpinningSensitivity(qreal sensitivity) {
			spinningSensitivity_ = sensitivity;
		}
		/*! Defines the wheelSensitivity(). */
		void setWheelSensitivity(qreal sensitivity) {
			wheelSensitivity_ = sensitivity;
		}
		/*! Defines the zoomSensitivity(). */
		void setZoomSensitivity(qreal sensitivity) { zoomSensitivity_ = sensitivity; }

	public:
		/*! Returns the influence of a mouse displacement on the ManipulatedFrame rotation.
		Default value is 1.0. a higher value will generate a larger rotation */
		qreal rotationSensitivity() const { return rotationSensitivity_; }

		/*! Returns the influence of a mouse displacement on the ManipulatedFrame translation.
		Default value is 1.0. a higher value will generate a larger translation */
		qreal translationSensitivity() const { return translationSensitivity_; }

		/*! Returns the minimum mouse speed required (at button release) to make the ManipulatedFrame spin().
		Mouse speed is expressed in pixels per milliseconds. Default value is 0.3 (300 pixels per second) */
		qreal spinningSensitivity() const { return spinningSensitivity_; }

		/*! Returns the zoom sensitivity.
		Default value is 1.0. A higher value will make the zoom faster.
		Use a negative value to invert the zoom in and out directions.*/
		qreal zoomSensitivity() const { return zoomSensitivity_; }

		/*! Returns the mouse wheel sensitivity.
		Default value is 1.0. A higher value will make the wheel action more efficient
		Use a negative value to invert the zoom in and out directions.*/
		qreal wheelSensitivity() const { return wheelSensitivity_; }
		//@}

		/*! @name Spinning */
		//@{
	public:
		/*! Returns \c true when the ManipulatedFrame is spinning. */
		bool isSpinning() const { return isSpinning_; }
		/*! Returns the incremental rotation that is applied by spin() to the
		 ManipulatedFrame orientation when it isSpinning().*/
		Quaternion spinningQuaternion() const { return spinningQuaternion_; }

	public Q_SLOTS:
		/*! Defines the spinningQuaternion(). Its axis is defined in the
		ManipulatedFrame coordinate system. */
		void setSpinningQuaternion(const Quaternion& spinningQuaternion) {
			spinningQuaternion_ = spinningQuaternion;
		}
		virtual void startSpinning(int updateInterval);

		/*! Stops the spinning motion started using startSpinning().
		isSpinning() will return \c false after this call. */
		virtual void stopSpinning() {
			spinningTimer_.stop();
			isSpinning_ = false;
		}

	protected Q_SLOTS:
		virtual void spin();

	private Q_SLOTS:
		void spinUpdate();
		//@}

		/*! @name Mouse event handlers */
		//@{
	protected:
		virtual void mousePressEvent(QMouseEvent* const event, Camera* const camera);
		virtual void mouseMoveEvent(QMouseEvent* const event, Camera* const camera);
		virtual void mouseReleaseEvent(QMouseEvent* const event, Camera* const camera);
		virtual void mouseDoubleClickEvent(QMouseEvent* const event, Camera* const camera);
		virtual void wheelEvent(QWheelEvent* const event, Camera* const camera);
		//@}

	public:
		/*! @name Current state */
		//@{
		bool isManipulated() const;

		/*! Returns the \c MouseAction currently applied to this ManipulatedFrame.*/
		QGLViewer::MouseAction currentMouseAction() const { return action_; }
		//@}

	protected:
		Quaternion deformedBallQuaternion(int x, int y, qreal cx, qreal cy,
			const Camera* const camera);

		QGLViewer::MouseAction action_;

		virtual void startAction(int ma, bool withConstraint = true); // int is really a QGLViewer::MouseAction

		void computeMouseSpeed(const QMouseEvent* const e);
		int mouseOriginalDirection(const QMouseEvent* const e);

		/*! Returns a screen scaled delta from event's position to prevPos_, along the
				X or Y direction, whichever has the largest magnitude. */
		qreal deltaWithPrevPos(QMouseEvent* const event, Camera* const camera) const;
		/*! Returns a normalized wheel delta, proportionnal to wheelSensitivity(). */
		qreal wheelDelta(const QWheelEvent* event) const;

		// Previous mouse position (used for incremental updates) and mouse press position.
		QPoint prevPos_, pressPos_;

	private:
		void zoom(qreal delta, const Camera* const camera);

	private:
		// Sensitivity
		qreal rotationSensitivity_;
		qreal translationSensitivity_;
		qreal spinningSensitivity_;
		qreal wheelSensitivity_;
		qreal zoomSensitivity_;

		// Mouse speed and spinning
		QElapsedTimer last_move_time;
		qreal mouseSpeed_;
		int delay_;
		bool isSpinning_;
		QTimer spinningTimer_;
		Quaternion spinningQuaternion_;

		// Whether the SCREEN_TRANS direction (horizontal or vertical) is fixed or not.
		bool dirIsFixed_;
	};

} // namespace qglviewer
