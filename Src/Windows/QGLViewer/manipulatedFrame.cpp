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
#include "manipulatedFrame.h"
#include "manipulatedCameraFrame.h"
#include "camera.h"

#include <cstdlib>
#include "../../Utils/MathUtils.h"
#include <QMouseEvent>

using namespace qglviewer;
using namespace std;
using namespace acamcad;

ManipulatedFrame::ManipulatedFrame() : action_(QGLViewer::NO_MOUSE_ACTION) 
{
  // #CONNECTION# initFromDOMElement and accessor docs
  setRotationSensitivity(1.0);
  setTranslationSensitivity(1.0);
  setSpinningSensitivity(0.5);
  setWheelSensitivity(1.0);
  setZoomSensitivity(1.0);

  isSpinning_ = false;

  connect(&spinningTimer_, SIGNAL(timeout()), SLOT(spinUpdate()));
}

/*! Equal operator. Calls Frame::operator=() and then copy attributes. */
ManipulatedFrame &ManipulatedFrame::operator=(const ManipulatedFrame &mf) {
  Frame::operator=(mf);

  setRotationSensitivity(mf.rotationSensitivity());
  setTranslationSensitivity(mf.translationSensitivity());
  setSpinningSensitivity(mf.spinningSensitivity());
  setWheelSensitivity(mf.wheelSensitivity());
  setZoomSensitivity(mf.zoomSensitivity());

  mouseSpeed_ = 0.0;
  dirIsFixed_ = false;
  action_ = QGLViewer::NO_MOUSE_ACTION;

  return *this;
}

/*! Copy constructor. Performs a deep copy of all attributes using operator=().*/
ManipulatedFrame::ManipulatedFrame(const ManipulatedFrame &mf) : Frame(mf) 
{
  (*this) = mf;
}

////////////////////////////////////////////////////////////////////////////////
//                 M o u s e    h a n d l i n g                               //
////////////////////////////////////////////////////////////////////////////////

/*! Returns \c true when the ManipulatedFrame is being manipulated with the mouse.*/
bool ManipulatedFrame::isManipulated() const {
  return action_ != QGLViewer::NO_MOUSE_ACTION;
}

/*! Starts the spinning of the ManipulatedFrame. */
void ManipulatedFrame::startSpinning(int updateInterval) {
  isSpinning_ = true;
  spinningTimer_.start(updateInterval);
}

////////////////////////////////////////////////////////////////////////////////
//                               S P I N                                      //
////////////////////////////////////////////////////////////////////////////////


/*! Rotates the ManipulatedFrame by its spinningQuaternion(). Called by a timer
  when the ManipulatedFrame isSpinning(). */
void ManipulatedFrame::spin() { rotate(spinningQuaternion()); }

/* spin() and spinUpdate() differ since spin can be used by itself (for instance
   by QGLViewer::SCREEN_ROTATE) without a spun emission. Much nicer to use the
   spinningQuaternion() and hence spin() for these incremental updates. Nothing
   special to be done for continuous spinning with this design. */
void ManipulatedFrame::spinUpdate() {
  spin();
  Q_EMIT spun();
}

/*! Updates mouse speed, measured in pixels/milliseconds. */
void ManipulatedFrame::computeMouseSpeed(const QMouseEvent* const e) {
    const QPoint delta = (e->pos() - prevPos_);
    const qreal dist = std::sqrt(qreal(delta.x() * delta.x() + delta.y() * delta.y()));
    delay_ = last_move_time.restart();
    if (delay_ == 0)
        // Less than a millisecond: assume delay = 1ms
        mouseSpeed_ = dist;
    else
        mouseSpeed_ = dist / delay_;
}


/*! Protected internal method used to handle mouse events. */
void ManipulatedFrame::startAction(int ma, bool withConstraint) {
  action_ = static_cast<QGLViewer::MouseAction>(ma);

  // #CONNECTION# manipulatedFrame::wheelEvent,
  // manipulatedCameraFrame::wheelEvent and mouseReleaseEvent() restore previous
  // constraint

  switch (action_) {
  case QGLViewer::ROTATE:
  case QGLViewer::SCREEN_ROTATE:
    mouseSpeed_ = 0.0;
    stopSpinning();
    break;

  case QGLViewer::SCREEN_TRANSLATE:
    dirIsFixed_ = false;
    break;

  default:
    break;
  }
}


/*! Return 1 if mouse motion was started horizontally and -1 if it was more vertical. 
Returns 0 if this could not be determined yet (perfect diagonal motion, rare). */
int ManipulatedFrame::mouseOriginalDirection(const QMouseEvent *const e) {
  static bool horiz = true; // Two simultaneous manipulatedFrame require two mice !

  if (!dirIsFixed_) {
    const QPoint delta = e->pos() - pressPos_;
    dirIsFixed_ = std::abs(delta.x()) != std::abs(delta.y());
    horiz = std::abs(delta.x()) > std::abs(delta.y());
  }

  if (dirIsFixed_)
    if (horiz)
      return 1;
    else
      return -1;
  else
    return 0;
}

qreal ManipulatedFrame::deltaWithPrevPos(QMouseEvent *const event, Camera *const camera) const 
{
  qreal dx = qreal(event->x() - prevPos_.x()) / camera->screenWidth();
  qreal dy = qreal(event->y() - prevPos_.y()) / camera->screenHeight();

  qreal value = fabs(dx) > fabs(dy) ? dx : dy;
  return value * zoomSensitivity();
}

qreal ManipulatedFrame::wheelDelta(const QWheelEvent *event) const {
  static const qreal WHEEL_SENSITIVITY_COEF = 8E-4;
  return event->delta() * wheelSensitivity() * WHEEL_SENSITIVITY_COEF;
}

void ManipulatedFrame::zoom(qreal delta, const Camera *const camera) {
  Vec trans(0.0, 0.0, (camera->position() - position()).Norm() * delta);

  trans = camera->frame()->orientation().rotate(trans);
  if (referenceFrame())
    trans = referenceFrame()->transformOf(trans);
  translate(trans);
}



void ManipulatedFrame::mousePressEvent(QMouseEvent *const event, Camera *const camera) 
{
  Q_UNUSED(camera)

  prevPos_ = pressPos_ = event->pos();
}

void ManipulatedFrame::mouseMoveEvent(QMouseEvent *const event, Camera *const camera) 
{
  switch (action_) {
  case QGLViewer::TRANSLATE: {
    const QPoint delta = event->pos() - prevPos_;
    Vec trans((double)(delta.x()), (double)(-delta.y()), 0.0);
    // Scale to fit the screen mouse displacement
    switch (camera->type()) {
    case Camera::PERSPECTIVE:
      trans *= 2.0 * tan(camera->fieldOfView() / 2.0) *
               fabs((camera->frame()->coordinatesOf(position())).Z()) /
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
    // Transform to world coordinate system.
    trans =
        camera->frame()->orientation().rotate(translationSensitivity() * trans);
    // And then down to frame
    if (referenceFrame())
      trans = referenceFrame()->transformOf(trans);
    translate(trans);
    break;
  }

  case QGLViewer::ZOOM: {
    zoom(deltaWithPrevPos(event, camera), camera);
    break;
  }

  case QGLViewer::SCREEN_ROTATE: {
    Vec trans = camera->projectedCoordinatesOf(position());

    const qreal prev_angle =
        atan2(prevPos_.y() - trans[1], prevPos_.x() - trans[0]);
    const qreal angle = atan2(event->y() - trans[1], event->x() - trans[0]);

    const Vec axis =
        transformOf(camera->frame()->inverseTransformOf(Vec(0.0, 0.0, -1.0)));
    Quaternion rot(axis, angle - prev_angle);
    //#CONNECTION# These two methods should go together (spinning detection and
    // activation)
    computeMouseSpeed(event);
    setSpinningQuaternion(rot);
    spin();
    break;
  }

  case QGLViewer::SCREEN_TRANSLATE: {
    Vec trans;
    int dir = mouseOriginalDirection(event);
    if (dir == 1)
        trans = Vec((double)(event->x() - prevPos_.x()), 0.0, 0.0);
    else if (dir == -1)
        trans = Vec(0.0, (double)(prevPos_.y() - event->y()), 0.0);

    switch (camera->type()) {
    case Camera::PERSPECTIVE:
      trans *= 2.0 * tan(camera->fieldOfView() / 2.0) *
               fabs((camera->frame()->coordinatesOf(position())).Z()) /
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
    // Transform to world coordinate system.
    trans =
        camera->frame()->orientation().rotate(translationSensitivity() * trans);
    // And then down to frame
    if (referenceFrame())
      trans = referenceFrame()->transformOf(trans);

    translate(trans);
    break;
  }

  case QGLViewer::ROTATE: {
    Vec trans = camera->projectedCoordinatesOf(position());
    Quaternion rot = deformedBallQuaternion(event->x(), event->y(), trans[0],
                                            trans[1], camera);
    trans = Vec(-rot[0], -rot[1], -rot[2]);
    //trans = camera->frame()->orientation().rotate(trans);
    trans = transformOf(trans);

    rot[0] = trans[0];
    rot[1] = trans[1];
    rot[2] = trans[2];

    computeMouseSpeed(event);
    setSpinningQuaternion(rot);
    spin();
    break;
  }

  case QGLViewer::NO_MOUSE_ACTION:
    // Possible when the ManipulatedFrame is a MouseGrabber. This method is then
    // called without startAction because of mouseTracking.
    break;
  }

  if (action_ != QGLViewer::NO_MOUSE_ACTION) {
    prevPos_ = event->pos();
    Q_EMIT manipulated();
  }
}


void ManipulatedFrame::mouseReleaseEvent(QMouseEvent *const event, Camera *const camera) 
{
  Q_UNUSED(event)
  Q_UNUSED(camera)

  if ( (action_ == QGLViewer::SCREEN_ROTATE) && (mouseSpeed_ >= spinningSensitivity()) )
    startSpinning(delay_);

  action_ = QGLViewer::NO_MOUSE_ACTION;
}

void ManipulatedFrame::mouseDoubleClickEvent(QMouseEvent *const event, Camera *const camera) 
{
  if (event->modifiers() == Qt::NoModifier)
    switch (event->button()) {
    case Qt::LeftButton:
      alignWithFrame(camera->frame());
      break;
    case Qt::RightButton:
      projectOnLine(camera->position(), camera->viewDirection());
      break;
    default:
      break;
    }
}

void ManipulatedFrame::wheelEvent(QWheelEvent *const event, Camera *const camera) {
  //#CONNECTION# QGLViewer::setWheelBinding
  if (action_ == QGLViewer::ZOOM) {
    zoom(wheelDelta(event), camera);
    Q_EMIT manipulated();
  }

  action_ = QGLViewer::NO_MOUSE_ACTION;
}

////////////////////////////////////////////////////////////////////////////////

/*! Returns "pseudo-distance" from (x,y) to ball of radius size.
\arg for a point inside the ball, it is proportional to the euclidean distance
to the ball \arg for a point outside the ball, it is proportional to the inverse
of this distance (tends to zero) on the ball, the function is continuous. */
static qreal projectOnBall(qreal x, qreal y) {
  // If you change the size value, change angle computation in
  // deformedBallQuaternion().
  const qreal size = 1.0;
  const qreal size2 = size * size;
  const qreal size_limit = size2 * 0.5;

  const qreal d = x * x + y * y;
  return d < size_limit ? std::sqrt(size2 - d) : size_limit / sqrt(d);
}

#ifndef DOXYGEN
/*! Returns a quaternion computed according to the mouse motion. Mouse positions
are projected on a deformed ball, centered on (\p cx,\p cy). */
Quaternion ManipulatedFrame::deformedBallQuaternion(int x, int y, qreal cx, qreal cy, const Camera *const camera) {
  // Points on the deformed ball
  qreal px =
      rotationSensitivity() * (prevPos_.x() - cx) / camera->screenWidth();
  qreal py =
      rotationSensitivity() * (cy - prevPos_.y()) / camera->screenHeight();
  qreal dx = rotationSensitivity() * (x - cx) / camera->screenWidth();
  qreal dy = rotationSensitivity() * (cy - y) / camera->screenHeight();

  const Vec p1(px, py, projectOnBall(px, py));
  const Vec p2(dx, dy, projectOnBall(dx, dy));
  // Approximation of rotation angle
  // Should be divided by the projectOnBall size, but it is 1.0
  const Vec axis = MathUtils::cross(p2, p1);
  const qreal angle =
      5.0 *
      asin(std::sqrt(axis.SquaredNorm() / p1.SquaredNorm() / p2.SquaredNorm()));
  return Quaternion(axis, angle);
}
#endif // DOXYGEN
