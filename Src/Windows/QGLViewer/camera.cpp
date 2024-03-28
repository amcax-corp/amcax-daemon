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
#include "camera.h"
#include "manipulatedCameraFrame.h"
#include "../../Utils/MathUtils.h"

using namespace std;
using namespace qglviewer;

/*! Default constructor.

 sceneCenter() is set to (0,0,0) and sceneRadius() is set to 1.0. type() is
 Camera::PERSPECTIVE, with a \c M_PI/4 fieldOfView().

 See IODistance(), physicalDistanceToScreen(), physicalScreenWidth() and
 focusDistance() documentations for default stereo parameter values. */
Camera::Camera()
	: frame_(nullptr), fieldOfView_(M_PI / 4.0), modelViewMatrixIsUpToDate_(false),
	projectionMatrixIsUpToDate_(false) {
	// Requires the interpolationKfi_
	setFrame(new ManipulatedCameraFrame());

	// #CONNECTION# All these default values identical in initFromDOMElement.

	// Requires fieldOfView() to define focusDistance()
	setSceneRadius(1.0);

	// Initial value (only scaled after this)
	orthoCoef_ = tan(fieldOfView() / 2.0);

	// Also defines the pivotPoint(), which changes orthoCoef_. Requires a
	// frame().
	setSceneCenter(Vec(0.0, 0.0, 0.0));

	// Requires fieldOfView() when called with ORTHOGRAPHIC. Attention to
	// projectionMatrix_ below.
	setType(PERSPECTIVE);

	// #CONNECTION# initFromDOMElement default values
	setZNearCoefficient(0.005);
	setZClippingCoefficient(5.0);

	// Dummy values
	setScreenWidthAndHeight(600, 400);

	// Stereo parameters
	setIODistance(0.062);
	setPhysicalScreenWidth(0.5);
	// focusDistance is set from setFieldOfView()

	// #CONNECTION# Camera copy constructor
	for (unsigned short j = 0; j < 16; ++j) {
		modelViewMatrix_[j] = ((j % 5 == 0) ? 1.0 : 0.0);
		// #CONNECTION# computeProjectionMatrix() is lazy and assumes 0.0 almost
		// everywhere.
		projectionMatrix_[j] = 0.0;
	}
	computeProjectionMatrix();
}

/*! Virtual destructor.

 The frame() is deleted, but the different keyFrameInterpolator() are \e not
 deleted (in case they are shared). */
Camera::~Camera() {
	delete frame_;
}

/*! Copy constructor. Performs a deep copy using operator=(). */
Camera::Camera(const Camera& camera) : QObject(), frame_(nullptr) {
	// Requires the interpolationKfi_
	setFrame(new ManipulatedCameraFrame(*camera.frame()));

	for (unsigned short j = 0; j < 16; ++j) {
		modelViewMatrix_[j] = ((j % 5 == 0) ? 1.0 : 0.0);
		// #CONNECTION# computeProjectionMatrix() is lazy and assumes 0.0 almost
		// everywhere.
		projectionMatrix_[j] = 0.0;
	}

	(*this) = camera;
}

/*! Equal operator.*/
Camera& Camera::operator=(const Camera& camera) {
	setScreenWidthAndHeight(camera.screenWidth(), camera.screenHeight());
	setFieldOfView(camera.fieldOfView());
	setSceneRadius(camera.sceneRadius());
	setSceneCenter(camera.sceneCenter());
	setZNearCoefficient(camera.zNearCoefficient());
	setZClippingCoefficient(camera.zClippingCoefficient());
	setType(camera.type());

	// Stereo parameters
	setIODistance(camera.IODistance());
	setFocusDistance(camera.focusDistance());
	setPhysicalScreenWidth(camera.physicalScreenWidth());

	orthoCoef_ = camera.orthoCoef_;
	projectionMatrixIsUpToDate_ = false;

	// frame_ and interpolationKfi_ pointers are not shared.
	frame_->setReferenceFrame(nullptr);
	frame_->setPosition(camera.position());
	frame_->setOrientation(camera.orientation());

	computeProjectionMatrix();
	computeModelViewMatrix();

	return *this;
}

/*! Sets Camera screenWidth() and screenHeight() (expressed in pixels).*/
void Camera::setScreenWidthAndHeight(int width, int height) {
	// Prevent negative and zero dimensions that would cause divisions by zero.
	screenWidth_ = width > 0 ? width : 1;
	screenHeight_ = height > 0 ? height : 1;
	projectionMatrixIsUpToDate_ = false;
}

/*! Returns the near clipping plane distance used by the Camera projection matrix. */
qreal Camera::zNear() const {
	const qreal zNearScene = zClippingCoefficient() * sceneRadius();
	qreal z = distanceToSceneCenter() - zNearScene; z = z / 2;

	// Prevents negative or null zNear values.
	const qreal zMin = zNearCoefficient() * zNearScene;
	if (z < zMin)
		switch (type()) {
		case Camera::PERSPECTIVE:
			z = zMin;
			break;
		case Camera::ORTHOGRAPHIC:
			z = 0.0;
			break;
		}
	return z;
}

/*! Returns the far clipping plane distance used by the Camera projection matrix.*/
qreal Camera::zFar() const {
	return distanceToSceneCenter() + zClippingCoefficient() * sceneRadius();
}

/*! Sets the vertical fieldOfView() of the Camera (in radians).*/
void Camera::setFieldOfView(qreal fov) {
	fieldOfView_ = fov;
	setFocusDistance(sceneRadius() / tan(fov / 2.0));
	projectionMatrixIsUpToDate_ = false;
}

/*! Defines the Camera type().*/
void Camera::setType(Type type) {
	if ((type == Camera::ORTHOGRAPHIC) && (type_ == Camera::PERSPECTIVE))
		orthoCoef_ = tan(fieldOfView() / 2.0);
	type_ = type;
	projectionMatrixIsUpToDate_ = false;
}

/*! Sets the Camera frame().*/
void Camera::setFrame(ManipulatedCameraFrame* const mcf) {
	if (!mcf)
		return;

	if (frame_) {
		disconnect(frame_, SIGNAL(modified()), this, SLOT(onFrameModified()));
	}

	frame_ = mcf;

	connect(frame_, SIGNAL(modified()), this, SLOT(onFrameModified()));
	onFrameModified();
}

/*! Returns the distance from the Camera center to sceneCenter(), projected
  along the Camera Z axis. Used by zNear() and zFar() to optimize the Z range.
*/
qreal Camera::distanceToSceneCenter() const {
	return fabs((frame()->coordinatesOf(sceneCenter())).Z());
}

/*! Returns the \p halfWidth and \p halfHeight of the Camera orthographic frustum.*/
void Camera::getOrthoWidthHeight(GLdouble& halfWidth,
	GLdouble& halfHeight) const {
	const qreal dist = orthoCoef_ * fabs(cameraCoordinatesOf(pivotPoint()).Z());
	//#CONNECTION# fitScreenRegion
	halfWidth = dist * ((aspectRatio() < 1.0) ? 1.0 : aspectRatio());
	halfHeight = dist * ((aspectRatio() < 1.0) ? 1.0 / aspectRatio() : 1.0);
}

/*! Computes the projection matrix associated with the Camera. */
void Camera::computeProjectionMatrix() const {
	if (projectionMatrixIsUpToDate_)
		return;

	const qreal ZNear = zNear();
	const qreal ZFar = zFar();

	switch (type()) {
	case Camera::PERSPECTIVE: {
		// #CONNECTION# all non null coefficients were set to 0.0 in constructor.
		const qreal f = 1.0 / tan(fieldOfView() / 2.0);
		projectionMatrix_[0] = f / aspectRatio();
		projectionMatrix_[5] = f;
		projectionMatrix_[10] = (ZNear + ZFar) / (ZNear - ZFar);
		projectionMatrix_[11] = -1.0;
		projectionMatrix_[14] = 2.0 * ZNear * ZFar / (ZNear - ZFar);
		projectionMatrix_[15] = 0.0;

		break;
	}
	case Camera::ORTHOGRAPHIC: {
		GLdouble w, h;
		getOrthoWidthHeight(w, h);
		projectionMatrix_[0] = 1.0 / w;
		projectionMatrix_[5] = 1.0 / h;
		projectionMatrix_[10] = -2.0 / (ZFar - ZNear);
		projectionMatrix_[11] = 0.0;
		projectionMatrix_[14] = -(ZFar + ZNear) / (ZFar - ZNear);
		projectionMatrix_[15] = 1.0;
		// same as glOrtho( -w, w, -h, h, zNear(), zFar() );
		break;
	}
	}

	projectionMatrixIsUpToDate_ = true;
}

/*! Computes the modelView matrix associated with the Camera's position() and orientation().*/
void Camera::computeModelViewMatrix() const {
	if (modelViewMatrixIsUpToDate_)
		return;

	const Quaternion q = frame()->orientation();

	const qreal q00 = 2.0 * q[0] * q[0];
	const qreal q11 = 2.0 * q[1] * q[1];
	const qreal q22 = 2.0 * q[2] * q[2];

	const qreal q01 = 2.0 * q[0] * q[1];
	const qreal q02 = 2.0 * q[0] * q[2];
	const qreal q03 = 2.0 * q[0] * q[3];

	const qreal q12 = 2.0 * q[1] * q[2];
	const qreal q13 = 2.0 * q[1] * q[3];

	const qreal q23 = 2.0 * q[2] * q[3];

	modelViewMatrix_[0] = 1.0 - q11 - q22;
	modelViewMatrix_[1] = q01 - q23;
	modelViewMatrix_[2] = q02 + q13;
	modelViewMatrix_[3] = 0.0;

	modelViewMatrix_[4] = q01 + q23;
	modelViewMatrix_[5] = 1.0 - q22 - q00;
	modelViewMatrix_[6] = q12 - q03;
	modelViewMatrix_[7] = 0.0;

	modelViewMatrix_[8] = q02 - q13;
	modelViewMatrix_[9] = q12 + q03;
	modelViewMatrix_[10] = 1.0 - q11 - q00;
	modelViewMatrix_[11] = 0.0;

	const Vec t = q.inverseRotate(frame()->position());

	modelViewMatrix_[12] = -t.X();
	modelViewMatrix_[13] = -t.Y();
	modelViewMatrix_[14] = -t.Z();
	modelViewMatrix_[15] = 1.0;

	modelViewMatrixIsUpToDate_ = true;
}

/*! Loads the OpenGL \c GL_PROJECTION matrix with the Camera projection matrix.

 The Camera projection matrix is computed using computeProjectionMatrix().*/
void Camera::loadProjectionMatrix(bool reset) const {
	// WARNING: makeCurrent must be called by every calling method
	//glViewport(0, 0, 800, 600);
	glMatrixMode(GL_PROJECTION);

	if (reset)
		glLoadIdentity();

	computeProjectionMatrix();

	glMultMatrixd(projectionMatrix_);
}

/*! Loads the OpenGL \c GL_MODELVIEW matrix with the modelView matrix
 corresponding to the Camera.

 Calls computeModelViewMatrix() to compute the Camera's modelView matrix.*/
void Camera::loadModelViewMatrix(bool reset) const {
	// WARNING: makeCurrent must be called by every calling method
	glMatrixMode(GL_MODELVIEW);
	computeModelViewMatrix();
	if (reset)
		glLoadMatrixd(modelViewMatrix_);
	else
		glMultMatrixd(modelViewMatrix_);
}

/*! Same as loadProjectionMatrix() but for a stereo setup.
 Only the Camera::PERSPECTIVE type() is supported for stereo mode. See QGLViewer::setStereoDisplay().
 \attention glMatrixMode is set to \c GL_PROJECTION. */
void Camera::loadProjectionMatrixStereo(bool leftBuffer) const {
	qreal left, right, bottom, top;
	qreal screenHalfWidth, halfWidth, side, shift, delta;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	switch (type()) {
	case Camera::PERSPECTIVE:
		// compute half width of screen,
		// corresponding to zero parallax plane to deduce decay of cameras
		screenHalfWidth = focusDistance() * tan(horizontalFieldOfView() / 2.0);
		shift = screenHalfWidth * IODistance() / physicalScreenWidth();
		// should be * current y  / y total
		// to take into account that the window doesn't cover the entire screen

		// compute half width of "view" at znear and the delta corresponding to
		// the shifted camera to deduce what to set for asymmetric frustums
		halfWidth = zNear() * tan(horizontalFieldOfView() / 2.0);
		delta = shift * zNear() / focusDistance();
		side = leftBuffer ? -1.0 : 1.0;

		left = -halfWidth + side * delta;
		right = halfWidth + side * delta;
		top = halfWidth / aspectRatio();
		bottom = -top;
		glFrustum(left, right, bottom, top, zNear(), zFar());
		break;

	case Camera::ORTHOGRAPHIC:
		qWarning("Camera::setProjectionMatrixStereo: Stereo not available with "
			"Ortho mode");
		break;
	}
}

/*! Same as loadModelViewMatrix() but for a stereo setup.
 Only the Camera::PERSPECTIVE type() is supported for stereo mode. See QGLViewer::setStereoDisplay().
 \attention glMatrixMode is set to \c GL_MODELVIEW. */
void Camera::loadModelViewMatrixStereo(bool leftBuffer) const {
	// WARNING: makeCurrent must be called by every calling method
	glMatrixMode(GL_MODELVIEW);

	qreal halfWidth = focusDistance() * tan(horizontalFieldOfView() / 2.0);
	qreal shift =
		halfWidth * IODistance() /
		physicalScreenWidth(); // * current window width / full screen width

	computeModelViewMatrix();
	if (leftBuffer)
		modelViewMatrix_[12] -= shift;
	else
		modelViewMatrix_[12] += shift;
	glLoadMatrixd(modelViewMatrix_);
}

/*! Fills \p m with the Camera projection matrix values.*/
void Camera::getProjectionMatrix(GLdouble m[16]) const {
	computeProjectionMatrix();
	for (unsigned short i = 0; i < 16; ++i)
		m[i] = projectionMatrix_[i];
}

void Camera::getProjectionMatrix(GLfloat m[16]) const {
	static GLdouble mat[16];
	getProjectionMatrix(mat);
	for (unsigned short i = 0; i < 16; ++i)
		m[i] = float(mat[i]);
}

/*! Fills \p m with the Camera modelView matrix values. */
void Camera::getModelViewMatrix(GLdouble m[16]) const {
	// May not be needed, but easier like this.
	// Prevents from retrieving matrix in stereo mode -> overwrites shifted value.
	computeModelViewMatrix();
	for (unsigned short i = 0; i < 16; ++i)
		m[i] = modelViewMatrix_[i];
}

void Camera::getModelViewMatrix(GLfloat m[16]) const {
	static GLdouble mat[16];
	getModelViewMatrix(mat);
	for (unsigned short i = 0; i < 16; ++i)
		m[i] = float(mat[i]);
}

/*! Fills \p m with the product of the ModelView and Projection matrices. */
void Camera::getModelViewProjectionMatrix(GLdouble m[16]) const {
	GLdouble mv[16];
	GLdouble proj[16];
	getModelViewMatrix(mv);
	getProjectionMatrix(proj);

	for (unsigned short i = 0; i < 4; ++i) {
		for (unsigned short j = 0; j < 4; ++j) {
			qreal sum = 0.0;
			for (unsigned short k = 0; k < 4; ++k)
				sum += proj[i + 4 * k] * mv[k + 4 * j];
			m[i + 4 * j] = sum;
		}
	}
}

void Camera::getModelViewProjectionMatrix(GLfloat m[16]) const {
	static GLdouble mat[16];
	getModelViewProjectionMatrix(mat);
	for (unsigned short i = 0; i < 16; ++i)
		m[i] = float(mat[i]);
}

/*! Sets the sceneRadius() value. Negative values are ignored. */
void Camera::setSceneRadius(qreal radius) {
	if (radius <= 0.0) {
		qWarning("Scene radius must be positive - Ignoring value");
		return;
	}

	sceneRadius_ = radius;
	projectionMatrixIsUpToDate_ = false;

	setFocusDistance(sceneRadius() / tan(fieldOfView() / 2.0));

	frame()->setFlySpeed(0.01 * sceneRadius());
}

void Camera::setSceneBoundingBox(const Vec& min, const Vec& max) {
	setSceneCenter((min + max) / 2.0);
	setSceneRadius(0.5 * (max - min).Norm());
}

/*! Sets the sceneCenter(). */
void Camera::setSceneCenter(const Vec& getCenter) {
	sceneCenter_ = getCenter;
	setPivotPoint(sceneCenter());
	projectionMatrixIsUpToDate_ = false;
}

/*! setSceneCenter() to the result of pointUnderPixel(\p pixel).*/
bool Camera::setSceneCenterFromPixel(const QPoint& pixel) {
	bool found;
	Vec point = pointUnderPixel(pixel, found);
	if (found)
		setSceneCenter(point);
	return found;
}

/*! Changes the pivotPoint() to \p point (defined in the world coordinate system). */
void Camera::setPivotPoint(const Vec& point) {
	const qreal prevDist = fabs(cameraCoordinatesOf(pivotPoint()).Z());

	// If frame's RAP is set directly, projectionMatrixIsUpToDate_ should also be
	// set to false to ensure proper recomputation of the ORTHO projection matrix.
	frame()->setPivotPoint(point);

	// orthoCoef_ is used to compensate for changes of the pivotPoint, so that the
	// image does not change when the pivotPoint is changed in ORTHOGRAPHIC mode.
	const qreal newDist = fabs(cameraCoordinatesOf(pivotPoint()).Z());
	// Prevents division by zero when rap is set to camera position
	if ((prevDist > 1E-9) && (newDist > 1E-9))
		orthoCoef_ *= prevDist / newDist;
	projectionMatrixIsUpToDate_ = false;
}

/*! The pivotPoint() is set to the point located under \p pixel on screen. */
bool Camera::setPivotPointFromPixel(const QPoint& pixel) {
	bool found;
	Vec point = pointUnderPixel(pixel, found);
	if (found)
		setPivotPoint(point);
	return found;
}

/*! Returns the ratio between pixel and OpenGL units at \p position. */
qreal Camera::pixelGLRatio(const Vec& position) const {
	switch (type()) {
	case Camera::PERSPECTIVE:
		return 2.0 * fabs((frame()->coordinatesOf(position)).Z()) *
			tan(fieldOfView() / 2.0) / screenHeight();
	case Camera::ORTHOGRAPHIC: {
		GLdouble w, h;
		getOrthoWidthHeight(w, h);
		return 2.0 * h / screenHeight();
	}
	}
	// Bad compilers complain
	return 1.0;
}

/*! Changes the Camera fieldOfView() so that the entire scene (defined by
 QGLViewer::sceneCenter() and QGLViewer::sceneRadius()) is visible from the
 Camera position().*/
void Camera::setFOVToFitScene() {
	if (distanceToSceneCenter() > std::sqrt(2.0) * sceneRadius())
		setFieldOfView(2.0 * asin(sceneRadius() / distanceToSceneCenter()));
	else
		setFieldOfView(M_PI / 2.0);
}

/*! Returns the coordinates of the 3D point located at pixel (x,y) on screen.*/
Vec Camera::pointUnderPixel(const QPoint& pixel, bool& found) const {
	float depth;
	// Qt uses upper corner for its origin while GL uses the lower corner.
	glReadPixels(pixel.x(), screenHeight() - 1 - pixel.y(), 1, 1,
		GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
	found = static_cast<double>(depth) < 1.0;
	Vec point(static_cast<double>(pixel.x()), static_cast<double>(pixel.y()), static_cast<double>(depth));
	point = unprojectedCoordinatesOf(point);
	return point;
}

/*! Moves the Camera so that the entire scene is visible. */
void Camera::showEntireScene() { fitSphere(sceneCenter(), sceneRadius()); }

/*! Moves the Camera so that its sceneCenter() is projected on the center of the
 window. The orientation() and fieldOfView() are unchanged. */
void Camera::centerScene() {
	frame()->projectOnLine(sceneCenter(), viewDirection());
}

/*! Sets the Camera orientation(), so that it looks at point \p target (defined
 in the world coordinate system).*/
void Camera::lookAt(const Vec& target) {
	setViewDirection(target - position());
}

/*! Moves the Camera so that the sphere defined by (\p center, \p radius) is
 visible and fits in the frustum.*/
void Camera::fitSphere(const Vec& getCenter, qreal radius) {
	qreal distance = 0.0;
	switch (type()) {
	case Camera::PERSPECTIVE: {
		const qreal yview = radius / sin(fieldOfView() / 2.0);
		const qreal xview = radius / sin(horizontalFieldOfView() / 2.0);
		distance = qMax(xview, yview);
		break;
	}
	case Camera::ORTHOGRAPHIC: {
		distance =
			((getCenter - pivotPoint()) | viewDirection()) + (radius / orthoCoef_);
		break;
	}
	}
	Vec newPos(getCenter - distance * viewDirection());
	frame()->setPosition(newPos);
}

/*! Moves the Camera so that the (world axis aligned) bounding box (\p min, \p
  max) is entirely visible, using fitSphere(). */
void Camera::fitBoundingBox(const Vec& min, const Vec& max) {
	qreal diameter = qMax(fabs(max[1] - min[1]), fabs(max[0] - min[0]));
	diameter = qMax(fabs(max[2] - min[2]), diameter);
	fitSphere(0.5 * (min + max), 0.5 * diameter);
}

/*! Moves the Camera so that the rectangular screen region defined by \p
  rectangle (pixel units, with origin in the upper left corner) fits the screen.*/
void Camera::fitScreenRegion(const QRect& rectangle) {
	const Vec vd = viewDirection();
	const qreal distToPlane = distanceToSceneCenter();
	const QPoint getCenter = rectangle.center();

	Vec orig, dir;
	convertClickToLine(getCenter, orig, dir);
	Vec newCenter = orig + distToPlane / (dir | vd) * dir;

	convertClickToLine(QPoint(rectangle.x(), getCenter.y()), orig, dir);
	const Vec pointX = orig + distToPlane / (dir | vd) * dir;

	convertClickToLine(QPoint(getCenter.x(), rectangle.y()), orig, dir);
	const Vec pointY = orig + distToPlane / (dir | vd) * dir;

	qreal distance = 0.0;
	switch (type()) {
	case Camera::PERSPECTIVE: {
		const qreal distX =
			(pointX - newCenter).Norm() / sin(horizontalFieldOfView() / 2.0);
		const qreal distY = (pointY - newCenter).Norm() / sin(fieldOfView() / 2.0);
		distance = qMax(distX, distY);
		break;
	}
	case Camera::ORTHOGRAPHIC: {
		const qreal dist = ((newCenter - pivotPoint()) | vd);
		//#CONNECTION# getOrthoWidthHeight
		const qreal distX = (pointX - newCenter).Norm() / orthoCoef_ /
			((aspectRatio() < 1.0) ? 1.0 : aspectRatio());
		const qreal distY = (pointY - newCenter).Norm() / orthoCoef_ /
			((aspectRatio() < 1.0) ? 1.0 / aspectRatio() : 1.0);
		distance = dist + qMax(distX, distY);
		break;
	}
	}

	Vec newPos(newCenter - distance * vd);
	frame()->setPosition(newPos);
}

/*! Rotates the Camera so that its upVector() becomes \p up (defined in the
 world coordinate system).*/
void Camera::setUpVector(const Vec& up, bool noMove) {
	Quaternion q(Vec(0.0, 1.0, 0.0), frame()->transformOf(up));

	if (!noMove)
		frame()->setPosition(pivotPoint() -
			(frame()->orientation() * q)
			.rotate(frame()->coordinatesOf(pivotPoint())));

	frame()->rotate(q);

	// Useful in fly mode to keep the horizontal direction.
	frame()->updateSceneUpVector();
}

/*! Sets the orientation() of the Camera using polar coordinates.*/
void Camera::setOrientation(qreal theta, qreal phi) {
	Vec axis(0.0, 1.0, 0.0);
	const Quaternion rot1(axis, theta);
	axis = Vec(-cos(theta), 0.0, sin(theta));
	const Quaternion rot2(axis, phi);
	setOrientation(rot1 * rot2);
}

/*! Sets the Camera orientation(), defined in the world coordinate system. */
void Camera::setOrientation(const Quaternion& q) {
	frame()->setOrientation(q);
	frame()->updateSceneUpVector();
}

/*! Rotates the Camera so that its viewDirection() is \p direction (defined in
 the world coordinate system).*/
void Camera::setViewDirection(const Vec& direction) {
	if (direction.SquaredNorm() < 1E-10)
		return;

	Vec xAxis = acamcad::MathUtils::cross(direction, upVector());
	if (xAxis.SquaredNorm() < 1E-10) {
		// target is aligned with upVector, this means a rotation around X axis
		// X axis is then unchanged, let's keep it !
		xAxis = frame()->inverseTransformOf(Vec(1.0, 0.0, 0.0));
	}

	Quaternion q;
	q.setFromRotatedBasis(xAxis, acamcad::MathUtils::cross(xAxis, direction), -direction);
	frame()->setOrientation(q);
}

// Compute a 3 by 3 determinant.
static qreal det(qreal m00, qreal m01, qreal m02, qreal m10, qreal m11,
	qreal m12, qreal m20, qreal m21, qreal m22) {
	return m00 * m11 * m22 + m01 * m12 * m20 + m02 * m10 * m21 - m20 * m11 * m02 -
		m10 * m01 * m22 - m00 * m21 * m12;
}

// Computes the index of element [i][j] in a \c qreal matrix[3][4].
static inline unsigned int ind(unsigned int i, unsigned int j) {
	return (i * 4 + j);
}

/*! Returns the Camera position (the eye), defined in the world coordinate system.*/
Vec Camera::position() const { return frame()->position(); }

/*! Returns the normalized up vector of the Camera, defined in the world coordinate system.*/
Vec Camera::upVector() const {
	return frame()->inverseTransformOf(Vec(0.0, 1.0, 0.0));
}
/*! Returns the normalized view direction of the Camera, defined in the world coordinate system.*/
Vec Camera::viewDirection() const {
	return frame()->inverseTransformOf(Vec(0.0, 0.0, -1.0));
}

/*! Returns the normalized right vector of the Camera, defined in the world coordinate system.*/
Vec Camera::rightVector() const {
	return frame()->inverseTransformOf(Vec(1.0, 0.0, 0.0));
}

/*! Returns the Camera orientation, defined in the world coordinate system. */
Quaternion Camera::orientation() const { return frame()->orientation(); }

/*! Sets the Camera position() (the eye), defined in the world coordinate system. */
void Camera::setPosition(const Vec& pos) { frame()->setPosition(pos); }

/*! Returns the Camera frame coordinates of a point \p src defined in world coordinates.*/
Vec Camera::cameraCoordinatesOf(const Vec& src) const {
	return frame()->coordinatesOf(src);
}

/*! Returns the world coordinates of the point whose position \p src is defined
in the Camera coordinate system. */
Vec Camera::worldCoordinatesOf(const Vec& src) const {
	return frame()->inverseCoordinatesOf(src);
}

qreal Camera::flySpeed() const { return frame()->flySpeed(); }

void Camera::setFlySpeed(qreal speed) { frame()->setFlySpeed(speed); }

/*! The point the Camera pivots around with the QGLViewer::ROTATE mouse binding.
Defined in world coordinate system.

Default value is the sceneCenter().

\attention setSceneCenter() changes this value. */
Vec Camera::pivotPoint() const { return frame()->pivotPoint(); }

/*! Sets the Camera's position() and orientation() from an OpenGL ModelView matrix.*/
void Camera::setFromModelViewMatrix(const GLdouble* const modelViewMatrix) {
	// Get upper left (rotation) matrix
	qreal upperLeft[3][3];
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			upperLeft[i][j] = modelViewMatrix[i * 4 + j];

	// Transform upperLeft into the associated Quaternion
	Quaternion q;
	q.setFromRotationMatrix(upperLeft);

	setOrientation(q);
	setPosition(-q.rotate(
		Vec(modelViewMatrix[12], modelViewMatrix[13], modelViewMatrix[14])));
}

/*! Defines the Camera position(), orientation() and fieldOfView() from a projection matrix.*/
void Camera::setFromProjectionMatrix(const qreal matrix[12]) {
	// The 3 lines of the matrix are the normals to the planes x=0, y=0, z=0
	// in the camera CS. As we normalize them, we do not need the 4th coordinate.
	Vec line_0(matrix[ind(0, 0)], matrix[ind(0, 1)], matrix[ind(0, 2)]);
	Vec line_1(matrix[ind(1, 0)], matrix[ind(1, 1)], matrix[ind(1, 2)]);
	Vec line_2(matrix[ind(2, 0)], matrix[ind(2, 1)], matrix[ind(2, 2)]);

	line_0.Normalize();
	line_1.Normalize();
	line_2.Normalize();

	// The camera position is at (0,0,0) in the camera CS so it is the
	// intersection of the 3 planes. It can be seen as the kernel
	// of the 3x4 projection matrix. We calculate it through 4 dimensional
	// vectorial product. We go directly into 3D that is to say we directly
	// divide the first 3 coordinates by the 4th one.

	// We derive the 4 dimensional vectorial product formula from the
	// computation of a 4x4 determinant that is developped according to
	// its 4th column. This implies some 3x3 determinants.
	const Vec cam_pos =
		Vec(det(matrix[ind(0, 1)], matrix[ind(0, 2)], matrix[ind(0, 3)],
			matrix[ind(1, 1)], matrix[ind(1, 2)], matrix[ind(1, 3)],
			matrix[ind(2, 1)], matrix[ind(2, 2)], matrix[ind(2, 3)]),

			-det(matrix[ind(0, 0)], matrix[ind(0, 2)], matrix[ind(0, 3)],
				matrix[ind(1, 0)], matrix[ind(1, 2)], matrix[ind(1, 3)],
				matrix[ind(2, 0)], matrix[ind(2, 2)], matrix[ind(2, 3)]),

			det(matrix[ind(0, 0)], matrix[ind(0, 1)], matrix[ind(0, 3)],
				matrix[ind(1, 0)], matrix[ind(1, 1)], matrix[ind(1, 3)],
				matrix[ind(2, 0)], matrix[ind(2, 1)], matrix[ind(2, 3)])) /

		(-det(matrix[ind(0, 0)], matrix[ind(0, 1)], matrix[ind(0, 2)],
			matrix[ind(1, 0)], matrix[ind(1, 1)], matrix[ind(1, 2)],
			matrix[ind(2, 0)], matrix[ind(2, 1)], matrix[ind(2, 2)]));

	// We compute the rotation matrix column by column.

	// GL Z axis is front facing.
	Vec column_2 = -line_2;

	// X-axis is almost like line_0 but should be orthogonal to the Z axis.
	Vec column_0 = acamcad::MathUtils::cross(acamcad::MathUtils::cross(column_2, line_0), column_2);
	column_0.Normalize();

	// Y-axis is almost like line_1 but should be orthogonal to the Z axis.
	// Moreover line_1 is downward oriented as the screen CS.
	Vec column_1 = -acamcad::MathUtils::cross(acamcad::MathUtils::cross(column_2, line_1), column_2);
	column_1.Normalize();

	qreal rot[3][3];
	rot[0][0] = column_0[0];
	rot[1][0] = column_0[1];
	rot[2][0] = column_0[2];

	rot[0][1] = column_1[0];
	rot[1][1] = column_1[1];
	rot[2][1] = column_1[2];

	rot[0][2] = column_2[0];
	rot[1][2] = column_2[1];
	rot[2][2] = column_2[2];

	// We compute the field of view

	// line_1^column_0 -> vector of intersection line between
	// y_screen=0 and x_camera=0 plane.
	// column_2*(...)  -> cos of the angle between Z vector et y_screen=0 plane
	// * 2 -> field of view = 2 * half angle

	// We need some intermediate values.
	Vec dummy = acamcad::MathUtils::cross(line_1, column_0);
	dummy.Normalize();
	qreal fov = acos(column_2 | dummy) * 2.0;

	// We set the camera.
	Quaternion q;
	q.setFromRotationMatrix(rot);
	setOrientation(q);
	setPosition(cam_pos);
	setFieldOfView(fov);
}

///////////////////////// Camera to world transform ///////////////////////

/*! Same as cameraCoordinatesOf(), but with \c qreal[3] parameters (\p src and
 * \p res may be identical pointers). */
void Camera::getCameraCoordinatesOf(const qreal src[3], qreal res[3]) const {
	Vec r = cameraCoordinatesOf(Vec(src[0], src[1], src[2]));
	for (int i = 0; i < 3; ++i)
		res[i] = r[i];
}

/*! Same as worldCoordinatesOf(), but with \c qreal[3] parameters (\p src and \p
 * res may be identical pointers). */
void Camera::getWorldCoordinatesOf(const qreal src[3], qreal res[3]) const {
	Vec r = worldCoordinatesOf(Vec(src[0], src[1], src[2]));
	for (int i = 0; i < 3; ++i)
		res[i] = r[i];
}

/*! Fills \p viewport with the Camera OpenGL viewport. */
void Camera::getViewport(GLint viewport[4]) const {
	viewport[0] = 0;
	viewport[1] = screenHeight();
	viewport[2] = screenWidth();
	viewport[3] = -screenHeight();
}

/*! Returns the screen projected coordinates of a point \p src defined in the \p
 frame coordinate system.

 When \p frame in \c nullptr (default), \p src is expressed in the world coordinate
 system.*/
Vec Camera::projectedCoordinatesOf(const Vec& src, const Frame* frame) const {
	GLdouble x, y, z;
	static GLint viewport[4];
	getViewport(viewport);

	if (frame) {
		const Vec tmp = frame->inverseCoordinatesOf(src);
		gluProject(tmp.X(), tmp.Y(), tmp.Z(), modelViewMatrix_, projectionMatrix_,
			viewport, &x, &y, &z);
	}
	else
		gluProject(src.X(), src.Y(), src.Z(), modelViewMatrix_, projectionMatrix_,
			viewport, &x, &y, &z);

	return Vec(x, y, z);
}

/*! Returns the world unprojected coordinates of a point \p src defined in the
 screen coordinate system.*/
Vec Camera::unprojectedCoordinatesOf(const Vec& src, const Frame* frame) const {
	GLdouble x, y, z;
	static GLint viewport[4];
	getViewport(viewport);
	gluUnProject(src.X(), src.Y(), src.Z(), modelViewMatrix_, projectionMatrix_,
		viewport, &x, &y, &z);
	if (frame)
		return frame->coordinatesOf(Vec(x, y, z));
	else
		return Vec(x, y, z);
}

/*! Same as projectedCoordinatesOf(), but with \c qreal parameters (\p src and
 * \p res can be identical pointers). */
void Camera::getProjectedCoordinatesOf(const qreal src[3], qreal res[3],
	const Frame* frame) const {
	Vec r = projectedCoordinatesOf(Vec(src[0], src[1], src[2]), frame);
	for (int i = 0; i < 3; ++i)
		res[i] = r[i];
}

/*! Same as unprojectedCoordinatesOf(), but with \c qreal parameters (\p src and
 * \p res can be identical pointers). */
void Camera::getUnprojectedCoordinatesOf(const qreal src[3], qreal res[3],
	const Frame* frame) const {
	Vec r = unprojectedCoordinatesOf(Vec(src[0], src[1], src[2]), frame);
	for (int i = 0; i < 3; ++i)
		res[i] = r[i];
}


////////////////////////////////////////////////////////////////////////////////

/*! Gives the coefficients of a 3D half-line passing through the Camera eye and
 pixel (x,y).*/
void Camera::convertClickToLine(const QPoint& pixel, Vec& orig,
	Vec& dir) const {
	switch (type()) {
	case Camera::PERSPECTIVE:
		orig = position();
		dir = Vec(((2.0 * pixel.x() / screenWidth()) - 1.0) *
			tan(fieldOfView() / 2.0) * aspectRatio(),
			((2.0 * (screenHeight() - pixel.y()) / screenHeight()) - 1.0) *
			tan(fieldOfView() / 2.0),
			-1.0);
		dir = worldCoordinatesOf(dir) - orig;
		dir.Normalize();
		break;

	case Camera::ORTHOGRAPHIC: {
		GLdouble w, h;
		getOrthoWidthHeight(w, h);
		orig = Vec((2.0 * pixel.x() / screenWidth() - 1.0) * w,
			-(2.0 * pixel.y() / screenHeight() - 1.0) * h, 0.0);
		orig = worldCoordinatesOf(orig);
		dir = viewDirection();
		break;
	}
	}
}

/*! Draws a representation of the Camera in the 3D world.*/
void Camera::draw(bool drawFarPlane, qreal scale) const {
	glPushMatrix();
	glMultMatrixd(frame()->worldMatrix());

	// 0 is the upper left coordinates of the near corner, 1 for the far one
	Vec points[2];

	points[0].SetZ(scale * zNear());
	points[1].SetZ(scale * zFar());

	switch (type()) {
	case Camera::PERSPECTIVE: {
		points[0].SetY(points[0].Z() * tan(fieldOfView() / 2.0));
		points[0].SetX(points[0].Y() * aspectRatio());

		const qreal ratio = points[1].Z() / points[0].Z();

		points[1].SetY(ratio * points[0].Y());
		points[1].SetX(ratio * points[0].X());
		break;
	}
	case Camera::ORTHOGRAPHIC: {
		GLdouble hw, hh;
		getOrthoWidthHeight(hw, hh);
		points[0].SetX(scale * qreal(hw));
		points[0].SetY(scale * qreal(hh));

		points[1] = points[0];
		break;
	}
	}

	const int farIndex = drawFarPlane ? 1 : 0;

	// Near and (optionally) far plane(s)
	glBegin(GL_QUADS);
	for (int i = farIndex; i >= 0; --i) {
		glNormal3d(0.0f, 0.0f, (i == 0) ? 1.0f : -1.0f);
		glVertex3d(points[i].X(), points[i].Y(), -points[i].Z());
		glVertex3d(-points[i].X(), points[i].Y(), -points[i].Z());
		glVertex3d(-points[i].X(), -points[i].Y(), -points[i].Z());
		glVertex3d(points[i].X(), -points[i].Y(), -points[i].Z());
	}
	glEnd();

	// Up arrow
	const qreal arrowHeight = 1.5 * points[0].Y();
	const qreal baseHeight = 1.2 * points[0].Y();
	const qreal arrowHalfWidth = 0.5 * points[0].X();
	const qreal baseHalfWidth = 0.3 * points[0].X();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// Base
	glBegin(GL_QUADS);
	glVertex3d(-baseHalfWidth, points[0].Y(), -points[0].Z());
	glVertex3d(baseHalfWidth, points[0].Y(), -points[0].Z());
	glVertex3d(baseHalfWidth, baseHeight, -points[0].Z());
	glVertex3d(-baseHalfWidth, baseHeight, -points[0].Z());
	glEnd();

	// Arrow
	glBegin(GL_TRIANGLES);
	glVertex3d(0.0, arrowHeight, -points[0].Z());
	glVertex3d(-arrowHalfWidth, baseHeight, -points[0].Z());
	glVertex3d(arrowHalfWidth, baseHeight, -points[0].Z());
	glEnd();

	// Frustum lines
	switch (type()) {
	case Camera::PERSPECTIVE:
		glBegin(GL_LINES);
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(points[farIndex].X(), points[farIndex].Y(), -points[farIndex].Z());
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(-points[farIndex].X(), points[farIndex].Y(), -points[farIndex].Z());
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(-points[farIndex].X(), -points[farIndex].Y(), -points[farIndex].Z());
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(points[farIndex].X(), -points[farIndex].Y(), -points[farIndex].Z());
		glEnd();
		break;
	case Camera::ORTHOGRAPHIC:
		if (drawFarPlane) {
			glBegin(GL_LINES);
			glVertex3d(points[0].X(), points[0].Y(), -points[0].Z());
			glVertex3d(points[1].X(), points[1].Y(), -points[1].Z());
			glVertex3d(-points[0].X(), points[0].Y(), -points[0].Z());
			glVertex3d(-points[1].X(), points[1].Y(), -points[1].Z());
			glVertex3d(-points[0].X(), -points[0].Y(), -points[0].Z());
			glVertex3d(-points[1].X(), -points[1].Y(), -points[1].Z());
			glVertex3d(points[0].X(), -points[0].Y(), -points[0].Z());
			glVertex3d(points[1].X(), -points[1].Y(), -points[1].Z());
			glEnd();
		}
	}

	glPopMatrix();
}

/*! Returns the 6 plane equations of the Camera frustum. */
void Camera::getFrustumPlanesCoefficients(GLdouble coef[6][4]) const {
	// Computed once and for all
	const Vec pos = position();
	const Vec viewDir = viewDirection();
	const Vec up = upVector();
	const Vec right = rightVector();
	const qreal posViewDir = pos | viewDir;

	static Vec normal[6];
	static GLdouble dist[6];

	switch (type()) {
	case Camera::PERSPECTIVE: {
		const qreal hhfov = horizontalFieldOfView() / 2.0;
		const qreal chhfov = cos(hhfov);
		const qreal shhfov = sin(hhfov);
		normal[0] = -shhfov * viewDir;
		normal[1] = normal[0] + chhfov * right;
		normal[0] = normal[0] - chhfov * right;

		normal[2] = -viewDir;
		normal[3] = viewDir;

		const qreal hfov = fieldOfView() / 2.0;
		const qreal chfov = cos(hfov);
		const qreal shfov = sin(hfov);
		normal[4] = -shfov * viewDir;
		normal[5] = normal[4] - chfov * up;
		normal[4] = normal[4] + chfov * up;

		for (int i = 0; i < 2; ++i)
			dist[i] = pos | normal[i];
		for (int j = 4; j < 6; ++j)
			dist[j] = pos | normal[j];

		// Natural equations are:
		// dist[0,1,4,5] = pos * normal[0,1,4,5];
		// dist[2] = (pos + zNear() * viewDir) * normal[2];
		// dist[3] = (pos + zFar()  * viewDir) * normal[3];

		// 2 times less computations using expanded/merged equations. Dir vectors
		// are normalized.
		const qreal posRightCosHH = chhfov * pos | right;
		dist[0] = -shhfov * posViewDir;
		dist[1] = dist[0] + posRightCosHH;
		dist[0] = dist[0] - posRightCosHH;
		const qreal posUpCosH = chfov * pos | up;
		dist[4] = -shfov * posViewDir;
		dist[5] = dist[4] - posUpCosH;
		dist[4] = dist[4] + posUpCosH;

		break;
	}
	case Camera::ORTHOGRAPHIC:
		normal[0] = -right;
		normal[1] = right;
		normal[4] = up;
		normal[5] = -up;

		GLdouble hw, hh;
		getOrthoWidthHeight(hw, hh);
		dist[0] = (pos - hw * right) | normal[0];
		dist[1] = (pos + hw * right) | normal[1];
		dist[4] = (pos + hh * up) | normal[4];
		dist[5] = (pos - hh * up) | normal[5];
		break;
	}

	// Front and far planes are identical for both camera types.
	normal[2] = -viewDir;
	normal[3] = viewDir;
	dist[2] = -posViewDir - zNear();
	dist[3] = posViewDir + zFar();

	for (int i = 0; i < 6; ++i) {
		coef[i][0] = GLdouble(normal[i].X());
		coef[i][1] = GLdouble(normal[i].Y());
		coef[i][2] = GLdouble(normal[i].Z());
		coef[i][3] = dist[i];
	}
}

void Camera::onFrameModified() {
	projectionMatrixIsUpToDate_ = false;
	modelViewMatrixIsUpToDate_ = false;
}
