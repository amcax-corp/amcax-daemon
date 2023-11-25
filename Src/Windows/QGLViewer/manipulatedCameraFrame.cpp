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
#include "manipulatedCameraFrame.h"

#include <QMouseEvent>

using namespace qglviewer;
using namespace std;

ManipulatedCameraFrame::ManipulatedCameraFrame()
    : rotatesAroundUpVector_(false), zoomsOnPivotPoint_(false) 
{
    setFlySpeed(0.0);
}

ManipulatedCameraFrame &ManipulatedCameraFrame:: operator=(const ManipulatedCameraFrame &mcf) 
{
  ManipulatedFrame::operator=(mcf);

  setFlySpeed(mcf.flySpeed());
  setRotatesAroundUpVector(mcf.rotatesAroundUpVector_);
  setZoomsOnPivotPoint(mcf.zoomsOnPivotPoint_);

  return *this;
}

ManipulatedCameraFrame::ManipulatedCameraFrame(const ManipulatedCameraFrame &mcf)
    : ManipulatedFrame(mcf) 
{
  (*this) = (mcf);
}

////////////////////////////////////////////////////////////////////////////////

/*! Overloading of ManipulatedFrame::spin().*/
void ManipulatedCameraFrame::spin() {
  rotateAroundPoint(spinningQuaternion(), pivotPoint());
}


void ManipulatedCameraFrame::zoom(qreal delta, const Camera *const camera) {
  const qreal sceneRadius = camera->sceneRadius();
  if (zoomsOnPivotPoint_) {
    Vec direction = position() - camera->pivotPoint();
    if (direction.Norm() > 0.02 * sceneRadius || delta > 0.0)
      translate(delta * direction);
  } else {
    const qreal coef =
        qMax(fabs((camera->frame()->coordinatesOf(camera->pivotPoint())).Z()), qreal(0.2) * sceneRadius);
    Vec trans(0.0, 0.0, -coef * delta);
    translate(inverseTransformOf(trans));
  }
}

////////////////////////////////////////////////////////////////////////////////
//                 M o u s e    h a n d l i n g                               //
////////////////////////////////////////////////////////////////////////////////

/*! Overloading of ManipulatedFrame::mouseMoveEvent().*/
void ManipulatedCameraFrame::mouseMoveEvent(QMouseEvent *const event, Camera *const camera) {
  // #CONNECTION# QGLViewer::mouseMoveEvent does the update().
  switch (action_) {
  case QGLViewer::TRANSLATE: {
    const QPoint delta = prevPos_ - event->pos();
    Vec trans((double)(delta.x()), (double)(-delta.y()), 0.0);

    // Scale to fit the screen mouse displacement
    switch (camera->type()) {
    case Camera::PERSPECTIVE:
      trans *= 2.0 * tan(camera->fieldOfView() / 2.0) *
               fabs((camera->frame()->coordinatesOf(pivotPoint())).Z()) /
               camera->screenHeight();
      break;
    case Camera::ORTHOGRAPHIC: {
      GLdouble w, h;
      camera->getOrthoWidthHeight(w, h);
      trans[0] *= 2.0 * w / camera->screenWidth();
      trans[1] *= 2.0 * h / camera->screenHeight();
      break;
    }
    }
    translate(inverseTransformOf(translationSensitivity() * trans));
    break;
  }

  case QGLViewer::ZOOM: {
    zoom(deltaWithPrevPos(event, camera), camera);
    break;
  }

  case QGLViewer::ROTATE: {
    Quaternion rot;
    if (rotatesAroundUpVector_) {
      // Multiply by 2.0 to get on average about the same speed as with the
      // deformed ball
      qreal dx = 2.0 * rotationSensitivity() * (prevPos_.x() - event->x()) /
                 camera->screenWidth();
      qreal dy = 2.0 * rotationSensitivity() * (prevPos_.y() - event->y()) /
                 camera->screenHeight();
      Vec verticalAxis = transformOf(sceneUpVector_);
      rot = Quaternion(verticalAxis, dx) * Quaternion(Vec(1.0, 0.0, 0.0), dy);
    } else {
      Vec trans = camera->projectedCoordinatesOf(pivotPoint());
      rot = deformedBallQuaternion(event->x(), event->y(), trans[0], trans[1], camera);
    }
    //#CONNECTION# These two methods should go together (spinning detection and
    // activation)
    computeMouseSpeed(event);
    setSpinningQuaternion(rot);
    spin();
    break;
  }

  case QGLViewer::SCREEN_ROTATE: {
    Vec trans = camera->projectedCoordinatesOf(pivotPoint());

    const qreal angle = atan2(event->y() - trans[1], event->x() - trans[0]) -
                        atan2(prevPos_.y() - trans[1], prevPos_.x() - trans[0]);

    Quaternion rot(Vec(0.0, 0.0, 1.0), angle);
    //#CONNECTION# These two methods should go together (spinning detection and
    // activation)
    computeMouseSpeed(event);
    setSpinningQuaternion(rot);
    spin();
    updateSceneUpVector();
    break;
  }

  case QGLViewer::SCREEN_TRANSLATE: {
    Vec trans;
    int dir = mouseOriginalDirection(event);
    if (dir == 1)
        trans = Vec((double)(prevPos_.x() - event->x()), 0.0, 0.0);
    else if (dir == -1)
        trans = Vec(0.0, (double)(event->y() - prevPos_.y()), 0.0); 

    switch (camera->type()) {
    case Camera::PERSPECTIVE:
      trans *= 2.0 * tan(camera->fieldOfView() / 2.0) *
               fabs((camera->frame()->coordinatesOf(pivotPoint())).Z()) /
               camera->screenHeight();
      break;
    case Camera::ORTHOGRAPHIC: {
      GLdouble w, h;
      camera->getOrthoWidthHeight(w, h);
      trans[0] *= 2.0 * w / camera->screenWidth();
      trans[1] *= 2.0 * h / camera->screenHeight();
      break;
    }
    }

    translate(inverseTransformOf(translationSensitivity() * trans));
    break;
  }

  case QGLViewer::NO_MOUSE_ACTION:
    break;
  }

  if (action_ != QGLViewer::NO_MOUSE_ACTION) {
    prevPos_ = event->pos();

    Q_EMIT manipulated();
  }
}

/*! This is an overload of ManipulatedFrame::mouseReleaseEvent(). The
  QGLViewer::MouseAction is terminated. */
void ManipulatedCameraFrame::mouseReleaseEvent(QMouseEvent *const event,
                                               Camera *const camera) 
{
  ManipulatedFrame::mouseReleaseEvent(event, camera);
}

/*! This is an overload of ManipulatedFrame::wheelEvent().*/
void ManipulatedCameraFrame::wheelEvent(QWheelEvent *const event,
                                        Camera *const camera) {
  //#CONNECTION# QGLViewer::setWheelBinding, ManipulatedFrame::wheelEvent.
  switch (action_) {
  case QGLViewer::ZOOM: {
    zoom(wheelDelta(event), camera);
    Q_EMIT manipulated();
    break;
  }
  default:
    break;
  }

  const int finalDrawAfterWheelEventDelay = 400;

  // This could also be done *before* manipulated is emitted, so that
  // isManipulated() returns false. But then fastDraw would not be used with
  // wheel. Detecting the last wheel event and forcing a final draw() is done
  // using the timer_.
  action_ = QGLViewer::NO_MOUSE_ACTION;
}

////////////////////////////////////////////////////////////////////////////////

void ManipulatedCameraFrame::updateSceneUpVector() {
    sceneUpVector_ = inverseTransformOf(Vec(0.0, 1.0, 0.0));
}