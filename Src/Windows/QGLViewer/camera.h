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

#include <QMap>
#include "frame.h"
class QGLViewer;

using namespace acamcad; 

namespace qglviewer {

class ManipulatedCameraFrame;

class Camera : public QObject {

    friend class ::QGLViewer;

    Q_OBJECT

public:
    Camera();
    virtual ~Camera();

    Camera(const Camera& camera);
    Camera& operator=(const Camera& camera);

    /*! Enumerates the two possible types of Camera.*/
    //透视投影，正交投影
    enum Type { PERSPECTIVE, ORTHOGRAPHIC };

public:
    Vec position() const;
    Vec upVector() const;
    Vec viewDirection() const;
    Vec rightVector() const;
    Quaternion orientation() const;

    void setFromModelViewMatrix(const GLdouble* const modelViewMatrix);
    void setFromProjectionMatrix(const qreal matrix[12]);

public Q_SLOTS:
    void setPosition(const Vec& pos);
    void setOrientation(const Quaternion& q);
    void setOrientation(qreal theta, qreal phi);
    void setUpVector(const Vec& up, bool noMove = true);
    void setViewDirection(const Vec& direction);

public Q_SLOTS:
    void lookAt(const Vec& target);
    void showEntireScene();
    void fitSphere(const Vec& getCenter, qreal radius);
    void fitBoundingBox(const Vec& min, const Vec& max);
    void fitScreenRegion(const QRect& rectangle);
    void centerScene();

public:

    Type type() const { return type_; }

    /*! Returns the vertical field of view of the Camera (in radians). */
    qreal fieldOfView() const { return fieldOfView_; }

    /*! Returns the horizontal field of view of the Camera (in radians).*/
    qreal horizontalFieldOfView() const {
        return 2.0 * atan(tan(fieldOfView() / 2.0) * aspectRatio());
    }

    /*! Returns the Camera aspect ratio defined by screenWidth() / screenHeight(). */
    qreal aspectRatio() const {
        return screenWidth_ / static_cast<qreal>(screenHeight_);
    }
    /*! Returns the width (in pixels) of the Camera screen. */
    int screenWidth() const { return screenWidth_; }

    /*! Returns the height (in pixels) of the Camera screen. */
    int screenHeight() const { return screenHeight_; }

    void getViewport(GLint viewport[4]) const;
    qreal pixelGLRatio(const Vec& position) const;

    //透视投影 z坐标
    qreal zNearCoefficient() const { return zNearCoef_; }

    qreal zClippingCoefficient() const { return zClippingCoef_ * 2; }

    virtual qreal zNear() const;
    virtual qreal zFar() const;
    virtual void getOrthoWidthHeight(GLdouble& halfWidth, GLdouble& halfHeight) const;
    void getFrustumPlanesCoefficients(GLdouble coef[6][4]) const;

public Q_SLOTS:
    void setType(Type type);

    void setFieldOfView(qreal fov);

    void setHorizontalFieldOfView(qreal hfov) {
        setFieldOfView(2.0 * atan(tan(hfov / 2.0) / aspectRatio()));
    }

    void setFOVToFitScene();

    void setAspectRatio(qreal aspect) {
        setScreenWidthAndHeight(int(100.0 * aspect), 100);
    }

    void setScreenWidthAndHeight(int width, int height);
    /*! Sets the zNearCoefficient() value. */
    void setZNearCoefficient(qreal coef) {
        zNearCoef_ = coef;
        projectionMatrixIsUpToDate_ = false;
    }
    /*! Sets the zClippingCoefficient() value. */
    void setZClippingCoefficient(qreal coef) {
        zClippingCoef_ = coef;
        projectionMatrixIsUpToDate_ = false;
    }

public:
    qreal sceneRadius() const { return sceneRadius_; }

    Vec sceneCenter() const { return sceneCenter_; }
    qreal distanceToSceneCenter() const;

public Q_SLOTS:
    void setSceneRadius(qreal radius);
    void setSceneCenter(const Vec& getCenter);
    bool setSceneCenterFromPixel(const QPoint& pixel);
    void setSceneBoundingBox(const Vec& min, const Vec& max);
    //@}

    /*! @name Pivot Point */
    //@{
public Q_SLOTS:
    void setPivotPoint(const Vec& point);
    bool setPivotPointFromPixel(const QPoint& pixel);

public:
    Vec pivotPoint() const;

    //@}

    /*! @name Associated frame */
    //@{
public:
    /*! Returns the ManipulatedCameraFrame attached to the Camera.

    This ManipulatedCameraFrame defines its position() and orientation() and can
    translate mouse events into Camera displacement. Set using setFrame(). */
    ManipulatedCameraFrame* frame() const { return frame_; }
public Q_SLOTS:
    void setFrame(ManipulatedCameraFrame* const mcf);

public:
    virtual void loadProjectionMatrix(bool reset = true) const;
    virtual void loadModelViewMatrix(bool reset = true) const;
    void computeProjectionMatrix() const;
    void computeModelViewMatrix() const;

    virtual void loadProjectionMatrixStereo(bool leftBuffer = true) const;
    virtual void loadModelViewMatrixStereo(bool leftBuffer = true) const;

    void getProjectionMatrix(GLfloat m[16]) const;
    void getProjectionMatrix(GLdouble m[16]) const;

    void getModelViewMatrix(GLfloat m[16]) const;
    void getModelViewMatrix(GLdouble m[16]) const;

    void getModelViewProjectionMatrix(GLfloat m[16]) const;
    void getModelViewProjectionMatrix(GLdouble m[16]) const;

    virtual void draw(bool drawFarPlane = true, qreal scale = 1.0) const;
    //@}

    /*! @name World to Camera coordinate systems conversions */
    //@{
public:
    Vec cameraCoordinatesOf(const Vec& src) const;
    Vec worldCoordinatesOf(const Vec& src) const;
    void getCameraCoordinatesOf(const qreal src[3], qreal res[3]) const;
    void getWorldCoordinatesOf(const qreal src[3], qreal res[3]) const;
    //@}

    /*! @name 2D screen to 3D world coordinate systems conversions */
    //@{
public:
    Vec projectedCoordinatesOf(const Vec& src, const Frame* frame = nullptr) const;
    Vec unprojectedCoordinatesOf(const Vec& src, const Frame* frame = nullptr) const;
    void getProjectedCoordinatesOf(const qreal src[3], qreal res[3],
        const Frame* frame = nullptr) const;
    void getUnprojectedCoordinatesOf(const qreal src[3], qreal res[3],
        const Frame* frame = nullptr) const;
    void convertClickToLine(const QPoint& pixel, Vec& orig, Vec& dir) const;
    Vec pointUnderPixel(const QPoint& pixel, bool& found) const;

public:
    qreal flySpeed() const;
public Q_SLOTS:
    void setFlySpeed(qreal speed);

public:
    /*! Returns the user's inter-ocular distance (in meters). Default value is
    0.062m, which fits most people. */
    qreal IODistance() const { return IODistance_; }

    /*! Returns the physical distance between the user's eyes and the screen (in meters). */
    qreal physicalDistanceToScreen() const {
        return physicalScreenWidth() / 2.0 / tan(horizontalFieldOfView() / 2.0);
    }

    /*! Returns the physical screen width, in meters. Default value is 0.5m */
    qreal physicalScreenWidth() const { return physicalScreenWidth_; }

    /*! Returns the focus distance used by stereo display, expressed in OpenGL units. */
    qreal focusDistance() const { return focusDistance_; }
public Q_SLOTS:
    /*! Sets the IODistance(). */
    void setIODistance(qreal distance) { IODistance_ = distance; }

    /*! Sets the physical screen (monitor or projected wall) width (in meters). */
    void setPhysicalScreenWidth(qreal width) { physicalScreenWidth_ = width; }

    /*! Sets the focusDistance(), in OpenGL scene units. */
    void setFocusDistance(qreal distance) { focusDistance_ = distance; }
    //@}

private Q_SLOTS:
    void onFrameModified();

private:
    // F r a m e
    ManipulatedCameraFrame* frame_;

    // C a m e r a   p a r a m e t e r s
    int screenWidth_, screenHeight_; // size of the window, in pixels
    qreal fieldOfView_;              // in radians
    Vec sceneCenter_;
    qreal sceneRadius_; // OpenGL units
    qreal zNearCoef_;
    qreal zClippingCoef_;
    qreal orthoCoef_;
    Type type_;                            // PERSPECTIVE or ORTHOGRAPHIC
    mutable GLdouble modelViewMatrix_[16]; // Buffered model view matrix.
    mutable bool modelViewMatrixIsUpToDate_;
    mutable GLdouble projectionMatrix_[16]; // Buffered projection matrix.
    mutable bool projectionMatrixIsUpToDate_;

    // S t e r e o   p a r a m e t e r s
    qreal IODistance_;          // inter-ocular distance, in meters
    qreal focusDistance_;       // in scene units
    qreal physicalScreenWidth_; // in meters
};

} // namespace qglviewer