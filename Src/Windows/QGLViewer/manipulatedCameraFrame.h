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

#include "manipulatedFrame.h"

namespace qglviewer {
/*! The ManipulatedCameraFrame class represents a ManipulatedFrame with Camera specific mouse bindings. */
class ManipulatedCameraFrame : public qglviewer::ManipulatedFrame {
#ifndef DOXYGEN
  friend class Camera;
  friend class ::QGLViewer;
#endif

  Q_OBJECT

public:
  ManipulatedCameraFrame();
  /*! Virtual destructor. Empty. */
  virtual ~ManipulatedCameraFrame() {}

  ManipulatedCameraFrame(const ManipulatedCameraFrame &mcf);
  ManipulatedCameraFrame &operator=(const ManipulatedCameraFrame &mcf);

  /*! @name Pivot point */
  //@{
public:
  /*! Returns the point the ManipulatedCameraFrame pivot point, around which the camera rotates.
  It is defined in the world coordinate system. Default value is (0,0,0). */
  Vec pivotPoint() const { return pivotPoint_; }
  /*! Sets the pivotPoint(), defined in the world coordinate system. */
  void setPivotPoint(const Vec &point) { pivotPoint_ = point; }
  //@}

  /*! @name Camera manipulation */
  //@{
public:
  /*! Returns \c true when the frame's rotation is constrained around the sceneUpVector(), 
     and \c false otherwise, when the rotation is completely free (default). 
     沿 UpVector 旋转， 沿 UpVector 上下移动*/
  bool rotatesAroundUpVector() const { return rotatesAroundUpVector_; }

  /*! Sets the value of rotatesAroundUpVector(). Default value is false (free rotation). */
  void setRotatesAroundUpVector(bool constrained) {
    rotatesAroundUpVector_ = constrained;
  }


  /*! Returns whether or not the QGLViewer::ZOOM action zooms on the pivot point. 
     沿 PivotPoint 缩放*/
  bool zoomsOnPivotPoint() const { return zoomsOnPivotPoint_; }

  /*! Sets the value of zoomsOnPivotPoint(). Default value is false. */
  void setZoomsOnPivotPoint(bool enabled) { zoomsOnPivotPoint_ = enabled; }

private:
  void zoom(qreal delta, const Camera *const camera);
  //@}

public Q_SLOTS:
  void setFlySpeed(qreal speed) { flySpeed_ = speed; }
  void setSceneUpVector(const Vec& up) { sceneUpVector_ = up; }

public:
  qreal flySpeed() const { return flySpeed_; }

  Vec sceneUpVector() const { return sceneUpVector_; }

  /*! @name Mouse event handlers */
  //@{
protected:
  virtual void mouseReleaseEvent(QMouseEvent *const event, Camera *const camera);
  virtual void mouseMoveEvent(QMouseEvent *const event, Camera *const camera);
  virtual void wheelEvent(QWheelEvent *const event, Camera *const camera);
  //@}

protected Q_SLOTS:
  virtual void spin();

private:
  void updateSceneUpVector();

private:
  qreal flySpeed_;

  Vec sceneUpVector_;
  bool rotatesAroundUpVector_;

  Vec pivotPoint_; 
  bool zoomsOnPivotPoint_;
};

} // namespace qglviewer
