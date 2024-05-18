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

#include <QClipboard>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QGL>
#include <QOpenGLWidget>
#else
#include <QtOpenGLWidgets/qopenglwidget.h>
#endif

#include <QMap>
#include <QTime>
#include <QTimer>
#include <QFileInfo>


#include "camera.h"

class QTabWidget;

namespace qglviewer {
	class ManipulatedFrame;
	class ManipulatedCameraFrame;
} // namespace qglviewer

class QGLViewer : public QOpenGLWidget {
	Q_OBJECT

public:
	explicit QGLViewer(QWidget* parent = 0,
		Qt::WindowFlags flags = Qt::WindowFlags());

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	explicit QGLViewer(QGLContext* context, QWidget* parent = 0,
		const QGLWidget* shareWidget = 0,
		Qt::WindowFlags flags = 0);
	explicit QGLViewer(const QGLFormat& format, QWidget* parent = 0,
		const QGLWidget* shareWidget = 0,
		Qt::WindowFlags flags = 0);
#endif

	virtual ~QGLViewer();

	/*! @name CoreViewer's colors */
	//@{
public:
	/*! Returns the background color of the viewer. */
	QColor backgroundColor() const { return backgroundColor_; }

	/*! Returns the foreground color used by the viewer. */
	QColor foregroundColor() const { return foregroundColor_; }
public Q_SLOTS:
	/*! Sets the backgroundColor() of the viewer and calls \c qglClearColor(). See
	   also setForegroundColor(). */
	void setBackgroundColor(const QColor& color) {
		backgroundColor_ = color;
		glClearColor(color.redF(), color.greenF(), color.blueF(), color.alphaF());
	}
	/*! Sets the foregroundColor() of the viewer, used to draw visual hints. See
	 * also setBackgroundColor(). */
	void setForegroundColor(const QColor& color) { foregroundColor_ = color; }
	//@}

	/*! @name Scene dimensions */
	//@{
public:
	/*! Returns the scene radius. */
	qreal sceneRadius() const { return camera()->sceneRadius(); }
	/*! Returns the scene center, defined in world coordinates. */
	qglviewer::Vec sceneCenter() const { return camera()->sceneCenter(); }

public Q_SLOTS:
	/*! Sets the sceneRadius(). */
	virtual void setSceneRadius(qreal radius) {
		camera()->setSceneRadius(radius);
	}

	/*! Sets the sceneCenter(), defined in world coordinates. */
	virtual void setSceneCenter(const qglviewer::Vec& getCenter) {
		camera()->setSceneCenter(getCenter);
	}

	/*! Convenient way to call setSceneCenter() and setSceneRadius() from a (world
	  axis aligned) bounding box of the scene.

	  This is equivalent to:
	  \code
	  setSceneCenter((min+max) / 2.0);
	  setSceneRadius((max-min).norm() / 2.0);
	  \endcode */
	void setSceneBoundingBox(const qglviewer::Vec& min,
		const qglviewer::Vec& max) {
		camera()->setSceneBoundingBox(min, max);
	}

	/*! Moves the camera so that the entire scene is visible.
	  Simple wrapper around qglviewer::Camera::showEntireScene(). */
	void showEntireScene() {
		camera()->showEntireScene();
		update();
	}
	//@}

	/*! @name Associated objects */
	//@{
public:
	/*! Returns the associated qglviewer::Camera, never \c nullptr. */
	qglviewer::Camera* camera() const { return camera_; }

public Q_SLOTS:
	void setCamera(qglviewer::Camera* const camera);
	//@}

	/*! @name State of the viewer */
	//@{
public:
	/*! Returns the aspect ratio of the viewer's widget (width() / height()). */
	qreal aspectRatio() const { return width() / static_cast<qreal>(height()); }

	/*! Returns the recommended size for the QGLViewer. Default value is 600x400 pixels. */
	virtual QSize sizeHint() const { return QSize(600, 400); }

private:
	bool cameraIsInRotateMode() const;


public:
	virtual void startScreenCoordinatesSystem(bool upward = false) const;
	virtual void stopScreenCoordinatesSystem() const;

protected:
	virtual void drawLight(GLenum light, qreal scale = 1.0) const;

private:
	/*! Vectorial rendering callback method. */
	void drawVectorial() { paintGL(); }
	//@}

	/*! @name Buffer to texture */
	//@{
public:
	GLuint bufferTextureId() const;
	/*! Returns the texture coordinate corresponding to the u extremum of the bufferTexture.

	The bufferTexture is created by copyBufferToTexture(). The texture size has
	powers of two dimensions and the buffer image hence only fills a part of it.
	This value corresponds to the u coordinate of the extremum right side of the
	buffer image.

	Use (0,0) to (bufferTextureMaxU(), bufferTextureMaxV()) texture coordinates to
	map the entire texture on a quad. */
	qreal bufferTextureMaxU() const { return bufferTextureMaxU_; }
	/*! Same as bufferTextureMaxU(), but for the v texture coordinate. */
	qreal bufferTextureMaxV() const { return bufferTextureMaxV_; }

public Q_SLOTS:
	void copyBufferToTexture(GLint internalFormat, GLenum format = GL_NONE);
	//@}

public:
Q_SIGNALS:
	/*! Signal emitted by the default init() method.

	Connect this signal to the methods that need to be called to initialize your
	viewer or overload init(). */
	void viewerInitialized();

	/*! Signal emitted by the default draw() method.

	Connect this signal to your main drawing method or overload draw(). See the <a
	href="../examples/callback.html">callback example</a> for an illustration. */
	void drawNeeded();

	/*! Signal emitted at the end of the QGLViewer::paintGL() method, when frame
	is drawn.

	Can be used to notify an image grabbing process that the image is ready. A
	typical example is to connect this signal to the saveSnapshot() method, so
	that a (numbered) snapshot is generated after each new display, in order to
	create a movie: \code connect(viewer, SIGNAL(drawFinished(bool)),
	SLOT(saveSnapshot(bool))); \endcode

	The \p automatic bool variable is always \c true and has been added so that
	the signal can be connected to saveSnapshot() with an \c automatic value set
	to \c true. */
	void drawFinished(bool automatic);

	/*! Signal emitted by the default QGLViewer::help() method.

	Connect this signal to your own help method or overload help(). */
	void helpRequired();

	/*! @name Help window */
	//@{
public:
	/*! Returns the QString displayed in the help() window main tab. */
	virtual QString helpString() const { return tr("No help available."); }

	virtual QString mouseString() const;
	virtual QString keyboardString() const;

public Q_SLOTS:
	virtual void help();

protected:
	/*! Returns a pointer to the help widget.

	Use this only if you want to directly modify the help widget. Otherwise use
	helpString(), setKeyDescription() and setMouseBindingDescription() to
	customize the text displayed in the help window tabs. */
	QTabWidget* helpWidget() { return helpWidget_; }
	//@}

	/*! @name Drawing methods */
	//@{
protected:
	virtual void resizeGL(int width, int height);
	virtual void initializeGL();

	/*! Initializes the viewer OpenGL context.

	This method is called before the first drawing and should be overloaded to
	initialize some of the OpenGL flags. The default implementation is empty. See
	initializeGL().

	Typical usage include camera() initialization (showEntireScene()), previous
	viewer state restoration (restoreStateFromFile()), OpenGL state modification
	and display list creation.

	Note that initializeGL() modifies the standard OpenGL context. These values
	can be restored back in this method.

	\attention You should not call updateGL() (or any method that calls it) in
	this method, as it will result in an infinite loop. The different QGLViewer
	set methods (setAxisIsDrawn(), setFPSIsDisplayed()...) are protected against
	this problem and can safely be called.

	\note All the OpenGL specific initializations must be done in this method: the
	OpenGL context is not yet available in your viewer constructor. */
	virtual void init() { Q_EMIT viewerInitialized(); }

	virtual void paintGL();
	virtual void preDraw();

	/*! The core method of the viewer, that draws the scene.

	If you build a class that inherits from QGLViewer, this is the method you want
	to overload. See the <a href="../examples/simpleViewer.html">simpleViewer
	example</a> for an illustration.

	The camera modelView matrix set in preDraw() converts from the world to the
	camera coordinate systems. Vertices given in draw() can then be considered as
	being given in the world coordinate system. The camera is moved in this world
	using the mouse. This representation is much more intuitive than the default
	camera-centric OpenGL standard.

	\attention The \c GL_PROJECTION matrix should not be modified by this method,
	to correctly display visual hints (axis, grid, FPS...) in postDraw(). Use
	push/pop or call camera()->loadProjectionMatrix() at the end of draw() if you
	need to change the projection matrix (unlikely). On the other hand, the \c
	GL_MODELVIEW matrix can be modified and left in a arbitrary state. */
	virtual void draw() {}
	virtual void fastDraw();
	virtual void postDraw();
	//@}

	/*! @name Mouse, keyboard and event handlers */
	//@{
protected:
	virtual void mousePressEvent(QMouseEvent*);
	virtual void mouseMoveEvent(QMouseEvent*);
	virtual void mouseReleaseEvent(QMouseEvent*);
	virtual void mouseDoubleClickEvent(QMouseEvent*);
	virtual void wheelEvent(QWheelEvent*);
	virtual void keyPressEvent(QKeyEvent*);
	virtual void keyReleaseEvent(QKeyEvent*);
	//@}

	/*! @name Keyboard customization */
	//@{
public:
	/*! Defines the different actions that can be associated with a keyboard
	shortcut using setShortcut().

	See the <a href="../keyboard.html">keyboard page</a> for details. */
	enum KeyboardAction {
		HELP,
		MOVE_CAMERA_LEFT,
		MOVE_CAMERA_RIGHT,
		MOVE_CAMERA_UP,
		MOVE_CAMERA_DOWN
	};

	unsigned int shortcut(KeyboardAction action) const;

public Q_SLOTS:
	void setShortcut(KeyboardAction action, unsigned int key);


public:
	/*! @name Mouse customization */
	//@{
	/*! Defines the different mouse handlers: camera() or manipulatedFrame().*/
	enum MouseHandler { CAMERA, FRAME };

	/*! Defines the possible actions that can be binded to a mouse click using
	setMouseBinding(Qt::KeyboardModifiers, Qt::MouseButtons, ClickAction, bool,
	int).

	See the <a href="../mouse.html">mouse page</a> for details. */
	enum ClickAction {
		NO_CLICK_ACTION,
		RAP_FROM_PIXEL,
		CENTER_SCENE,
		SHOW_ENTIRE_SCENE,
		ALIGN_CAMERA
	};

	/*! Defines the possible actions that can be binded to a mouse action (a
	click, followed by a mouse displacement).

	These actions may be binded to the camera() or to the manipulatedFrame() (see
	QGLViewer::MouseHandler) using setMouseBinding(). */
	enum MouseAction {
		NO_MOUSE_ACTION,
		ROTATE,
		ZOOM,
		TRANSLATE,
		SCREEN_ROTATE,
		SCREEN_TRANSLATE,
	};

	MouseAction mouseAction(Qt::Key key, Qt::KeyboardModifiers modifiers, Qt::MouseButton button) const;

	int mouseHandler(Qt::Key key, Qt::KeyboardModifiers modifiers,
		Qt::MouseButton button) const;

	void getMouseActionBinding(MouseHandler handler, MouseAction action,
		bool withConstraint, Qt::Key& key,
		Qt::KeyboardModifiers& modifiers, Qt::MouseButton& button) const;

	ClickAction clickAction(Qt::Key key, Qt::KeyboardModifiers modifiers,
		Qt::MouseButton button, bool doubleClick = false,
		Qt::MouseButtons buttonsBefore = Qt::NoButton) const;

	void getClickActionBinding(ClickAction action, Qt::Key& key,
		Qt::KeyboardModifiers& modifiers,
		Qt::MouseButton& button, bool& doubleClick,
		Qt::MouseButtons& buttonsBefore) const;

	MouseAction wheelAction(Qt::Key key, Qt::KeyboardModifiers modifiers) const;
	int wheelHandler(Qt::Key key, Qt::KeyboardModifiers modifiers) const;

	void getWheelActionBinding(MouseHandler handler, MouseAction action,
		bool withConstraint, Qt::Key& key,
		Qt::KeyboardModifiers& modifiers) const;

public Q_SLOTS:
	void setMouseBinding(Qt::KeyboardModifiers modifiers, Qt::MouseButton buttons,
		MouseHandler handler, MouseAction action,
		bool withConstraint = true);
	void setMouseBinding(Qt::KeyboardModifiers modifiers, Qt::MouseButton button,
		ClickAction action, bool doubleClick = false,
		Qt::MouseButtons buttonsBefore = Qt::NoButton);
	void setWheelBinding(Qt::KeyboardModifiers modifiers, MouseHandler handler,
		MouseAction action, bool withConstraint = true);

	void setMouseBinding(Qt::Key key, Qt::KeyboardModifiers modifiers,
		Qt::MouseButton buttons, MouseHandler handler,
		MouseAction action, bool withConstraint = true);
	void setMouseBinding(Qt::Key key, Qt::KeyboardModifiers modifiers,
		Qt::MouseButton button, ClickAction action,
		bool doubleClick = false,
		Qt::MouseButtons buttonsBefore = Qt::NoButton);
	void setWheelBinding(Qt::Key key, Qt::KeyboardModifiers modifiers,
		MouseHandler handler, MouseAction action,
		bool withConstraint = true);

	void clearMouseBindings();

private:
	static QString mouseActionString(QGLViewer::MouseAction ma);
	static QString clickActionString(QGLViewer::ClickAction ca);
	//@}

private:
	// Copy constructor and operator= are declared private and undefined
	// Prevents everyone from trying to use them
	QGLViewer(const QGLViewer& v);
	QGLViewer& operator=(const QGLViewer& v);

	// Set parameters to their default values. Called by the constructors.
	void defaultConstructor();

	// get keyboard Action
	void handleKeyboardAction(KeyboardAction id);

	// S h o r t c u t   k e y s
	void setDefaultShortcuts();

private:
	// C a m e r a
	qglviewer::Camera* camera_;

	// C o l o r s
	QColor backgroundColor_, foregroundColor_;

	QMap<KeyboardAction, QString> keyboardActionDescription_;
	QMap<KeyboardAction, unsigned int> keyboardBinding_;
	QMap<unsigned int, QString> keyDescription_;

	// B u f f e r   T e x t u r e
	GLuint bufferTextureId_;
	qreal bufferTextureMaxU_, bufferTextureMaxV_;
	int bufferTextureWidth_, bufferTextureHeight_;
	unsigned int previousBufferTextureFormat_;
	int previousBufferTextureInternalFormat_;

	// M o u s e   a c t i o n s
	struct MouseActionPrivate {
		MouseHandler handler;
		MouseAction action;
		bool withConstraint;
	};

	// M o u s e   b i n d i n g s
	struct MouseBindingPrivate {
		const Qt::KeyboardModifiers modifiers;
		const Qt::MouseButton button;
		const Qt::Key key;

		MouseBindingPrivate(Qt::KeyboardModifiers m, Qt::MouseButton b, Qt::Key k)
			: modifiers(m), button(b), key(k) {}

		// This sort order is used in mouseString() to display sorted mouse bindings
		bool operator<(const MouseBindingPrivate& mbp) const {
			if (key != mbp.key)
				return key < mbp.key;
			if (modifiers != mbp.modifiers)
				return modifiers < mbp.modifiers;
			return button < mbp.button;
		}
	};

	// W h e e l   b i n d i n g s
	struct WheelBindingPrivate {
		const Qt::KeyboardModifiers modifiers;
		const Qt::Key key;

		WheelBindingPrivate(Qt::KeyboardModifiers m, Qt::Key k)
			: modifiers(m), key(k) {}

		// This sort order is used in mouseString() to display sorted wheel bindings
		bool operator<(const WheelBindingPrivate& wbp) const {
			if (key != wbp.key)
				return key < wbp.key;
			return modifiers < wbp.modifiers;
		}
	};

	// C l i c k   b i n d i n g s
	struct ClickBindingPrivate {
		const Qt::KeyboardModifiers modifiers;
		const Qt::MouseButton button;
		const bool doubleClick;
		const Qt::MouseButtons
			buttonsBefore; // only defined when doubleClick is true
		const Qt::Key key;

		ClickBindingPrivate(Qt::KeyboardModifiers m, Qt::MouseButton b, bool dc,
			Qt::MouseButtons bb, Qt::Key k)
			: modifiers(m), button(b), doubleClick(dc), buttonsBefore(bb), key(k) {}

		// This sort order is used in mouseString() to display sorted mouse bindings
		bool operator<(const ClickBindingPrivate& cbp) const {
			if (key != cbp.key)
				return key < cbp.key;
			if (buttonsBefore != cbp.buttonsBefore)
				return buttonsBefore < cbp.buttonsBefore;
			if (modifiers != cbp.modifiers)
				return modifiers < cbp.modifiers;
			if (button != cbp.button)
				return button < cbp.button;
			return doubleClick != cbp.doubleClick;
		}
	};

	static QString formatClickActionPrivate(ClickBindingPrivate cbp);
	static bool isValidShortcutKey(int key);

	QMap<ClickBindingPrivate, QString> mouseDescription_;

	void setDefaultMouseBindings();
	void performClickAction(ClickAction ca, const QMouseEvent* const e);
	QMap<MouseBindingPrivate, MouseActionPrivate> mouseBinding_;
	QMap<WheelBindingPrivate, MouseActionPrivate> wheelBinding_;
	QMap<ClickBindingPrivate, ClickAction> clickBinding_;
	Qt::Key currentlyPressedKey_;

	// H e l p   w i n d o w
	QTabWidget* helpWidget_;
};
