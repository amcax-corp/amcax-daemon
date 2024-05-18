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

#include "qglviewer.h"
#include "camera.h"
#include "manipulatedCameraFrame.h"

#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QGLContext>
#endif
#include <QImage>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPushButton>
#include <QTabWidget>
#include <QTextEdit>
#include <QTextStream>
#include <QTimer>
#include <QUrl>
#include <QtAlgorithms>

#include <GL/freeglut.h>

using namespace std;
using namespace qglviewer;

void QGLViewer::defaultConstructor() {

	setFocusPolicy(Qt::StrongFocus);

	camera_ = new Camera();
	setCamera(camera());

	setDefaultShortcuts();
	setDefaultMouseBindings();

	setSceneRadius(3.0);
	showEntireScene();

	// #CONNECTION# default values in initFromDOMElement()

	bufferTextureId_ = 0;
	bufferTextureMaxU_ = 0.0;
	bufferTextureMaxV_ = 0.0;
	bufferTextureWidth_ = 0;
	bufferTextureHeight_ = 0;
	previousBufferTextureFormat_ = 0;
	previousBufferTextureInternalFormat_ = 0;
	currentlyPressedKey_ = Qt::Key(0);

	helpWidget_ = nullptr;

	setAttribute(Qt::WA_NoSystemBackground);
}

/*! Constructor. All viewer parameters are set to their default values*/
QGLViewer::QGLViewer(QWidget* parent, Qt::WindowFlags flags)
	: QOpenGLWidget(parent, flags) {
	defaultConstructor();
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
/*! Same as QGLViewer(), but a \c QGLContext can be provided so that viewers share GL contexts,
even with \c QGLContext sub-classes (use \p shareWidget otherwise). */
QGLViewer::QGLViewer(QGLContext* context, QWidget* parent,
	const QGLWidget* shareWidget, Qt::WindowFlags flags)
	: QOpenGLWidget(parent, flags) {
	Q_UNUSED(context)
		Q_UNUSED(shareWidget)
		qWarning("The constructor with a QGLContext is deprecated, use the regular "
			"contructor instead.");
	defaultConstructor();
}

/*! Same as QGLViewer(), but a specific \c QGLFormat can be provided.*/
QGLViewer::QGLViewer(const QGLFormat& format, QWidget* parent,
	const QGLWidget* shareWidget, Qt::WindowFlags flags)
	: QOpenGLWidget(parent, flags) {
	Q_UNUSED(format)
		Q_UNUSED(shareWidget)
		qWarning("The constructor with a QGLFormat is deprecated, use the regular "
			"contructor instead.");
	defaultConstructor();
}
#endif // DOXYGEN

/*! Virtual destructor.

The viewer is replaced by \c nullptr in the QGLViewerPool() (in order to preserve
other viewer's indexes) and allocated memory is released. The camera() is
deleted and should be copied before if it is shared by an other viewer. */
QGLViewer::~QGLViewer() {
	// See closeEvent comment. Destructor is called (and not closeEvent) only when
	// the widget is embedded. Hence we saveToFile here. It is however a bad idea
	// if virtual domElement() has been overloaded ! if (parent())
	// saveStateToFileForAllViewers();

	delete camera();
	if (helpWidget()) {
		// Needed for Qt 4 which has no main widget.
		helpWidget()->close();
		delete helpWidget_;
	}
}

/*! Initializes the QGLViewer OpenGL context and then calls user-defined init().
This method is automatically called once, before the first call to paintGL().

Overload init() instead of this method to modify viewer specific OpenGL state or
to create display lists.

To make beginners' life easier and to simplify the examples, this method
slightly modifies the standard OpenGL state:
\code
glEnable(GL_LIGHT0);
glEnable(GL_LIGHTING);
glEnable(GL_DEPTH_TEST);
glEnable(GL_COLOR_MATERIAL);
\endcode

If you port an existing application to QGLViewer and your display changes, you
probably want to disable these flags in init() to get back to a standard OpenGL state. */
void QGLViewer::initializeGL() {
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);

	// Default colors
	setForegroundColor(QColor(25, 25, 25));
	setBackgroundColor(QColor(235, 235, 235));

	// Clear the buffer where we're going to draw
	if (format().stereo()) {
		glDrawBuffer(GL_BACK_RIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawBuffer(GL_BACK_LEFT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	else
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Calls user defined method. Default emits a signal.
	init();


	//init glut
	int x = 1;
	char* p = (char*)"exe";

	glutInit(&x, &p);
}

/*! Main paint method, inherited from \c QOpenGLWidget. */
void QGLViewer::paintGL()
{
	// Clears screen, set model view matrix...
	preDraw();
	// Used defined method. Default calls draw()
	if (camera()->frame()->isManipulated())
		fastDraw();
	else
		draw();
	// Add visual hints: axis, camera, grid...
	postDraw();


	//Q_EMIT drawFinished(true);
}

/*! Sets OpenGL state before draw().
Emits the drawNeeded() signal*/
void QGLViewer::preDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// GL_PROJECTION matrix
	camera()->loadProjectionMatrix();
	// GL_MODELVIEW matrix
	camera()->loadModelViewMatrix();

	///Q_EMIT drawNeeded();
}

/*! Called after draw() to draw viewer visual hints. */
void QGLViewer::postDraw() {
	// Reset model view matrix to world coordinates origin
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	camera()->loadModelViewMatrix();
	// TODO restore model loadProjectionMatrixStereo

	// Save OpenGL state
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	// Set neutral GL state
	glDisable(GL_TEXTURE_1D);
	glDisable(GL_TEXTURE_2D);
#ifdef GL_TEXTURE_3D // OpenGL 1.2 Only...
	glDisable(GL_TEXTURE_3D);
#endif

	glDisable(GL_TEXTURE_GEN_Q);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

#ifdef GL_RESCALE_NORMAL // OpenGL 1.2 Only...
	glEnable(GL_RESCALE_NORMAL);
#endif

	glDisable(GL_COLOR_MATERIAL);
	glColor4f(foregroundColor().redF(), foregroundColor().greenF(),
		foregroundColor().blueF(), foregroundColor().alphaF());

	//if (gridIsDrawn()) {
	//    glLineWidth(1.0);
	//    drawGrid(camera()->sceneRadius());
	//}

	// Restore foregroundColor
	float color[4];
	color[0] = foregroundColor().red() / 255.0f;
	color[1] = foregroundColor().green() / 255.0f;
	color[2] = foregroundColor().blue() / 255.0f;
	color[3] = 1.0f;
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	// Restore GL state
	glPopAttrib();
	glPopMatrix();
}

/*! Draws a simplified version of the scene to guarantee interactive camera displacements.
	see printGL */
void QGLViewer::fastDraw() { draw(); }

// Key bindings. 0 means not defined
void QGLViewer::setDefaultShortcuts() {
	// D e f a u l t   a c c e l e r a t o r s
	setShortcut(HELP, Qt::Key_H);
	setShortcut(MOVE_CAMERA_LEFT, Qt::Key_Left);
	setShortcut(MOVE_CAMERA_RIGHT, Qt::Key_Right);
	setShortcut(MOVE_CAMERA_UP, Qt::Key_Up);
	setShortcut(MOVE_CAMERA_DOWN, Qt::Key_Down);

	keyboardActionDescription_[HELP] =
		tr("Opens this help window", "HELP action description");
	keyboardActionDescription_[MOVE_CAMERA_LEFT] =
		tr("Moves camera left", "MOVE_CAMERA_LEFT action description");
	keyboardActionDescription_[MOVE_CAMERA_RIGHT] =
		tr("Moves camera right", "MOVE_CAMERA_RIGHT action description");
	keyboardActionDescription_[MOVE_CAMERA_UP] =
		tr("Moves camera up", "MOVE_CAMERA_UP action description");
	keyboardActionDescription_[MOVE_CAMERA_DOWN] =
		tr("Moves camera down", "MOVE_CAMERA_DOWN action description");
}

// M o u s e   b e h a v i o r
void QGLViewer::setDefaultMouseBindings() {
	//const Qt::KeyboardModifiers cameraKeyboardModifiers = Qt::NoModifier;
	//const Qt::KeyboardModifiers frameKeyboardModifiers = Qt::ControlModifier;

	//camera morse modele
	setMouseBinding(Qt::NoModifier, Qt::RightButton, CAMERA, ROTATE);
	//setMouseBinding(Qt::AltModifier, Qt::MidButton, CAMERA, ZOOM);
	setMouseBinding(Qt::ShiftModifier, Qt::RightButton, CAMERA, TRANSLATE);

	setMouseBinding(Qt::Key_R, Qt::NoModifier, Qt::RightButton, CAMERA, SCREEN_ROTATE);

	setWheelBinding(Qt::NoModifier, CAMERA, ZOOM);

	// scale
	setMouseBinding(Qt::AltModifier, Qt::LeftButton, SHOW_ENTIRE_SCENE, true);

	// D o u b l e   c l i c k
	setMouseBinding(Qt::NoModifier, Qt::LeftButton, ALIGN_CAMERA, true);
	setMouseBinding(Qt::NoModifier, Qt::RightButton, CENTER_SCENE, true);
}

/*! Associates a new qglviewer::Camera to the viewer. */
void QGLViewer::setCamera(Camera* const camera) {
	if (!camera)
		return;

	camera->setSceneRadius(sceneRadius());
	camera->setSceneCenter(sceneCenter());
	camera->setScreenWidthAndHeight(width(), height());

	// Disconnect current camera from this viewer.
	disconnect(this->camera()->frame(), SIGNAL(manipulated()), this, SLOT(update()));
	disconnect(this->camera()->frame(), SIGNAL(spun()), this, SLOT(update()));

	// Connect camera frame to this viewer.
	connect(camera->frame(), SIGNAL(manipulated()), SLOT(update()));
	connect(camera->frame(), SIGNAL(spun()), SLOT(update()));

	camera_ = camera;
}

/*! Draws a representation of \p light.

Called in draw(), this method is useful to debug or display your light setup.
Light drawing depends on the type of light (point, spot, directional).

The method retrieves the light setup using \c glGetLightfv. Position and define
your lights before calling this method.

Light is drawn using its diffuse color. Disabled lights are not displayed.

Drawing size is proportional to sceneRadius(). Use \p scale to rescale it.

See the <a href="../examples/drawLight.html">drawLight example</a> for an
illustration.

\attention You need to enable \c GL_COLOR_MATERIAL before calling this method.
\c glColor is set to the light diffuse color. */
void QGLViewer::drawLight(GLenum light, qreal scale) const {
	static GLUquadric* quadric = gluNewQuadric();

	const qreal length = sceneRadius() / 5.0 * scale;

	GLboolean lightIsOn;
	glGetBooleanv(light, &lightIsOn);

	if (lightIsOn) {
		// All light values are given in eye coordinates
		glPushMatrix();
		glLoadIdentity();

		float color[4];
		glGetLightfv(light, GL_DIFFUSE, color);
		glColor4fv(color);

		float pos[4];
		glGetLightfv(light, GL_POSITION, pos);

		if (static_cast<double>(pos[3]) != 0.0) {
			glTranslatef(pos[0] / pos[3], pos[1] / pos[3], pos[2] / pos[3]);

			GLfloat cutOff;
			glGetLightfv(light, GL_SPOT_CUTOFF, &cutOff);
			if (static_cast<double>(cutOff) != 180.0) {
				GLfloat dir[4];
				glGetLightfv(light, GL_SPOT_DIRECTION, dir);
				glMultMatrixd(Quaternion(Vec(0.0, 0.0, 1.0), Vec(dir[0], dir[1], dir[2])).matrix());
				gluCylinder(quadric, 0.0, 0.7 * length * sin(static_cast<double>(cutOff) * M_PI / 180.0),
					0.7 * length * cos(static_cast<double>(cutOff) * M_PI / 180.0), 12, 1);
			}
			else
				gluSphere(quadric, 0.2 * length, 10, 10);
		}
		else {
			// Directional light.
			Vec dir(static_cast<double>(pos[0]), static_cast<double>(pos[1]), static_cast<double>(pos[2]));
			dir.Normalize();
			Frame fr =
				Frame(camera()->cameraCoordinatesOf(
					4.0 * length * camera()->frame()->inverseTransformOf(dir)),
					Quaternion(Vec(0.0, 0.0, -1.0), dir));
			glMultMatrixd(fr.matrix());
		}

		glPopMatrix();
	}
}


/*! Modify the projection matrix so that drawing can be done directly with 2D
screen coordinates.

开始直接在2D绘制，具体参见qglViewer
*/
void QGLViewer::startScreenCoordinatesSystem(bool upward) const {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	if (upward)
		glOrtho(0, width(), 0, height(), 0.0, -1.0);
	else
		glOrtho(0, width(), height(), 0, 0.0, -1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

/*! Stops the pixel coordinate drawing block started by
startScreenCoordinatesSystem(). */
void QGLViewer::stopScreenCoordinatesSystem() const {
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}


static QString mouseButtonsString(Qt::MouseButtons b) {
	QString result("");
	bool addAmpersand = false;
	if (b & Qt::LeftButton) {
		result += QGLViewer::tr("Left", "left mouse button");
		addAmpersand = true;
	}
	if (b & Qt::MiddleButton) {
		if (addAmpersand)
			result += " & ";
		result += QGLViewer::tr("Middle", "middle mouse button");
		addAmpersand = true;
	}
	if (b & Qt::RightButton) {
		if (addAmpersand)
			result += " & ";
		result += QGLViewer::tr("Right", "right mouse button");
	}
	return result;
}

/*! 默认的鼠标点击事件 */
void QGLViewer::performClickAction(ClickAction ca, const QMouseEvent* const e) {
	// Note: action that need it should call update().
	switch (ca) {
		// # CONNECTION setMouseBinding prevents adding NO_CLICK_ACTION in
		// clickBinding_ This case should hence not be possible. Prevents unused case
		// warning.
	case NO_CLICK_ACTION:
		break;
	case RAP_FROM_PIXEL:
		if (!camera()->setPivotPointFromPixel(e->pos()))
			camera()->setPivotPoint(sceneCenter());
		update();
		break;
	case CENTER_SCENE:
		camera()->centerScene();
		break;
	case SHOW_ENTIRE_SCENE:
		camera()->showEntireScene();
		break;
	case ALIGN_CAMERA:
		Frame* frame = new Frame();
		frame->setTranslation(camera()->pivotPoint());
		camera()->frame()->alignWithFrame(frame, true);
		delete frame;
		break;
	}
}


void QGLViewer::mousePressEvent(QMouseEvent* e) {
	//#CONNECTION# mouseDoubleClickEvent has the same structure
	//#CONNECTION# mouseString() concatenates bindings description in inverse
	// order.
	ClickBindingPrivate cbp(e->modifiers(), e->button(), false,
		static_cast<Qt::MouseButtons>((e->buttons() & ~(e->button()))), currentlyPressedKey_);

	if (clickBinding_.contains(cbp)) {
		performClickAction(clickBinding_[cbp], e);
	}
	else {
		//#CONNECTION# wheelEvent has the same structure
		const MouseBindingPrivate mbp(e->modifiers(), e->button(), currentlyPressedKey_);

		if (mouseBinding_.contains(mbp)) {
			MouseActionPrivate map = mouseBinding_[mbp];
			switch (map.handler) {
			case CAMERA:
				camera()->frame()->startAction(map.action, map.withConstraint);
				camera()->frame()->mousePressEvent(e, camera());
				break;
			case FRAME:
				break;
			}
			if (map.action == SCREEN_ROTATE)
				// Display visual hint line
				update();
		}
		else
			e->ignore();
	}
}

void QGLViewer::mouseMoveEvent(QMouseEvent* e) {
	//#CONNECTION# mouseReleaseEvent has the same structure
	if (camera()->frame()->isManipulated())
	{
		camera()->frame()->mouseMoveEvent(e, camera());
	}
	else
	{
		if (hasMouseTracking()) {
			// do some thing
		}
	}
}

void QGLViewer::mouseReleaseEvent(QMouseEvent* e) {
	//#CONNECTION# mouseMoveEvent has the same structure
	if (camera()->frame()->isManipulated()) {
		camera()->frame()->mouseReleaseEvent(e, camera());
	}
	else
		e->ignore();

	// Not absolutely needed (see above commented code for the optimal version),
	update();
}

void QGLViewer::wheelEvent(QWheelEvent* e) {
	//#CONNECTION# mousePressEvent has the same structure
	WheelBindingPrivate wbp(e->modifiers(), currentlyPressedKey_);

	if (wheelBinding_.contains(wbp)) {
		MouseActionPrivate map = wheelBinding_[wbp];
		switch (map.handler) {
		case CAMERA:
			camera()->frame()->startAction(map.action, map.withConstraint);
			camera()->frame()->wheelEvent(e, camera());
			break;
		case FRAME:
			break;
		}
	}
	else
		e->ignore();
}

void QGLViewer::mouseDoubleClickEvent(QMouseEvent* e) {
	//#CONNECTION# mousePressEvent has the same structure
	ClickBindingPrivate cbp(e->modifiers(), e->button(), true,
		static_cast<Qt::MouseButtons>(e->buttons() & ~(e->button())),
		currentlyPressedKey_);
	if (clickBinding_.contains(cbp))
		performClickAction(clickBinding_[cbp], e);
	else
		e->ignore();
}

QString QGLViewer::clickActionString(QGLViewer::ClickAction ca) {
	switch (ca) {
	case QGLViewer::NO_CLICK_ACTION:
		return QString();
	case QGLViewer::RAP_FROM_PIXEL:
		return QGLViewer::tr("Sets pivot point", "RAP_FROM_PIXEL click action");
	case QGLViewer::CENTER_SCENE:
		return QGLViewer::tr("Centers scene", "CENTER_SCENE click action");
	case QGLViewer::SHOW_ENTIRE_SCENE:
		return QGLViewer::tr("Shows entire scene", "SHOW_ENTIRE_SCENE click action");
	case QGLViewer::ALIGN_CAMERA:
		return QGLViewer::tr("Aligns camera", "ALIGN_CAMERA click action");
	}
	return QString();
}

QString QGLViewer::mouseActionString(QGLViewer::MouseAction ma) {
	switch (ma) {
	case QGLViewer::NO_MOUSE_ACTION:
		return QString();
	case QGLViewer::ROTATE:
		return QGLViewer::tr("Rotates", "ROTATE mouse action");
	case QGLViewer::ZOOM:
		return QGLViewer::tr("Zooms", "ZOOM mouse action");
	case QGLViewer::TRANSLATE:
		return QGLViewer::tr("Translates", "TRANSLATE mouse action");
	case QGLViewer::SCREEN_ROTATE:
		return QGLViewer::tr("Rotates in screen plane", "SCREEN_ROTATE mouse action");
	case QGLViewer::SCREEN_TRANSLATE:
		return QGLViewer::tr("Horizontally/Vertically translates", "SCREEN_TRANSLATE mouse action");
	}
	return QString();
}

static QString keyString(unsigned int key) {
#if QT_VERSION >= 0x040100
	return QKeySequence(int(key)).toString(QKeySequence::NativeText);
#else
	return QString(QKeySequence(key));
#endif
}

QString QGLViewer::formatClickActionPrivate(ClickBindingPrivate cbp) {
	bool buttonsBefore = cbp.buttonsBefore != Qt::NoButton;
	QString keyModifierString = keyString(cbp.modifiers + cbp.key);
	if (!keyModifierString.isEmpty()) {
#ifdef Q_OS_MAC
		// modifiers never has a '+' sign. Add one space to clearly separate
		// modifiers (and possible key) from button
		keyModifierString += " ";
#else
		// modifiers might be of the form : 'S' or 'Ctrl+S' or 'Ctrl+'. For
		// consistency, add an other '+' if needed, no spaces
		if (!keyModifierString.endsWith('+'))
			keyModifierString += "+";
#endif
	}

	return tr("%1%2%3%4%5%6", "Modifier / button or wheel / double click / with "
		"/ button / pressed")
		.arg(keyModifierString)
		.arg(mouseButtonsString(cbp.button) +
			(cbp.button == Qt::NoButton ? tr("Wheel", "Mouse wheel") : ""))
		.arg(cbp.doubleClick ? tr(" double click", "Suffix after mouse button")
			: "")
		.arg(buttonsBefore ? tr(" with ", "As in : Left button with Ctrl pressed")
			: "")
		.arg(buttonsBefore ? mouseButtonsString(cbp.buttonsBefore) : "")
		.arg(buttonsBefore
			? tr(" pressed", "As in : Left button with Ctrl pressed")
			: "");
}


static QString tableLine(const QString& left, const QString& right) {
	static bool even = false;
	const QString tdtd("</b></td><td>");
	const QString tdtr("</td></tr>\n");

	QString res("<tr bgcolor=\"");

	if (even)
		res += "#eeeeff\">";
	else
		res += "#ffffff\">";
	res += "<td><b>" + left + tdtd + right + tdtr;
	even = !even;

	return res;
}

/*! Returns a QString that describes the application mouse bindings, displayed
in the help() window \c Mouse tab.

Result is a table that describes custom application mouse binding descriptions
defined using setMouseBindingDescription() as well as standard mouse bindings
(defined using setMouseBinding() and setWheelBinding()). See the <a
href="../mouse.html">mouse page</a> for details on mouse bindings.

See also helpString() and keyboardString(). */
QString QGLViewer::mouseString() const {
	QString text(
		"<center><table border=\"1\" cellspacing=\"0\" cellpadding=\"4\">\n");
	const QString trtd("<tr><td>");
	const QString tdtr("</td></tr>\n");
	const QString tdtd("</td><td>");

	text += QString("<tr bgcolor=\"#aaaacc\"><th align=\"center\">%1</th><th "
		"align=\"center\">%2</th></tr>\n")
		.arg(tr("Button(s)",
			"Buttons column header in help window mouse tab"))
		.arg(tr("Description",
			"Description column header in help window mouse tab"));

	QMap<ClickBindingPrivate, QString> mouseBinding;

	// User-defined mouse bindings come first.
	for (QMap<ClickBindingPrivate, QString>::ConstIterator
		itm = mouseDescription_.begin(),
		endm = mouseDescription_.end();
		itm != endm; ++itm)
		mouseBinding[itm.key()] = itm.value();

	for (QMap<ClickBindingPrivate, QString>::ConstIterator
		it = mouseBinding.begin(),
		end = mouseBinding.end();
		it != end; ++it) {
		// Should not be needed (see setMouseBindingDescription())
		if (it.value().isNull())
			continue;

		text += tableLine(formatClickActionPrivate(it.key()), it.value());
	}

	// Optional separator line
	if (!mouseBinding.isEmpty()) {
		mouseBinding.clear();
		text += QString("<tr bgcolor=\"#aaaacc\"><td colspan=2>%1</td></tr>\n")
			.arg(tr("Standard mouse bindings", "In help window mouse tab"));
	}

	// Then concatenates the descriptions of wheelBinding_, mouseBinding_ and
	// clickBinding_. The order is significant and corresponds to the priorities
	// set in mousePressEvent() (reverse priority order, last one overwrites
	// previous) #CONNECTION# mousePressEvent() order
	for (QMap<MouseBindingPrivate, MouseActionPrivate>::ConstIterator
		itmb = mouseBinding_.begin(),
		endmb = mouseBinding_.end();
		itmb != endmb; ++itmb) {
		ClickBindingPrivate cbp(itmb.key().modifiers, itmb.key().button, false,
			Qt::NoButton, itmb.key().key);

		QString text = mouseActionString(itmb.value().action);

		if (!text.isNull()) {
			switch (itmb.value().handler) {
			case CAMERA:
				text += " " + tr("camera", "Suffix after action");
				break;
			case FRAME:
				text += " " + tr("manipulated frame", "Suffix after action");
				break;
			}
			if (!(itmb.value().withConstraint))
				text += "*";
		}
		mouseBinding[cbp] = text;
	}

	for (QMap<WheelBindingPrivate, MouseActionPrivate>::ConstIterator
		itw = wheelBinding_.begin(),
		endw = wheelBinding_.end();
		itw != endw; ++itw) {
		ClickBindingPrivate cbp(itw.key().modifiers, Qt::NoButton, false,
			Qt::NoButton, itw.key().key);

		QString text = mouseActionString(itw.value().action);

		if (!text.isNull()) {
			switch (itw.value().handler) {
			case CAMERA:
				text += " " + tr("camera", "Suffix after action");
				break;
			case FRAME:
				text += " " + tr("manipulated frame", "Suffix after action");
				break;
			}
			if (!(itw.value().withConstraint))
				text += "*";
		}

		mouseBinding[cbp] = text;
	}

	for (QMap<ClickBindingPrivate, ClickAction>::ConstIterator
		itcb = clickBinding_.begin(),
		endcb = clickBinding_.end();
		itcb != endcb; ++itcb)
		mouseBinding[itcb.key()] = clickActionString(itcb.value());

	for (QMap<ClickBindingPrivate, QString>::ConstIterator
		it2 = mouseBinding.begin(),
		end2 = mouseBinding.end();
		it2 != end2; ++it2) {
		if (it2.value().isNull())
			continue;

		text += tableLine(formatClickActionPrivate(it2.key()), it2.value());
	}

	text += "</table></center>";

	return text;
}

/*! Returns a QString that describes the application keyboard shortcut bindings,
and that will be displayed in the help() window \c Keyboard tab.

Default value is a table that describes the custom shortcuts defined using
setKeyDescription() as well as the \e standard QGLViewer::KeyboardAction
shortcuts (defined using setShortcut()). See the <a
href="../keyboard.html">keyboard page</a> for details on key customization.

See also helpString() and mouseString(). */
QString QGLViewer::keyboardString() const {
	QString text(
		"<center><table border=\"1\" cellspacing=\"0\" cellpadding=\"4\">\n");
	text += QString("<tr bgcolor=\"#aaaacc\"><th align=\"center\">%1</th><th "
		"align=\"center\">%2</th></tr>\n")
		.arg(QGLViewer::tr("Key(s)",
			"Keys column header in help window mouse tab"))
		.arg(QGLViewer::tr(
			"Description",
			"Description column header in help window mouse tab"));

	QMap<unsigned int, QString> keyDescription;

	// 1 - User defined key descriptions
	for (QMap<unsigned int, QString>::ConstIterator kd = keyDescription_.begin(),
		kdend = keyDescription_.end();
		kd != kdend; ++kd)
		keyDescription[kd.key()] = kd.value();

	// Add to text in sorted order
	for (QMap<unsigned int, QString>::ConstIterator kb = keyDescription.begin(),
		endb = keyDescription.end();
		kb != endb; ++kb)
		text += tableLine(keyString(kb.key()), kb.value());

	// 2 - Optional separator line
	if (!keyDescription.isEmpty()) {
		keyDescription.clear();
		text += QString("<tr bgcolor=\"#aaaacc\"><td colspan=2>%1</td></tr>\n")
			.arg(QGLViewer::tr("Standard viewer keys",
				"In help window keys tab"));
	}

	// 3 - KeyboardAction bindings description
	for (QMap<KeyboardAction, unsigned int>::ConstIterator
		it = keyboardBinding_.begin(),
		end = keyboardBinding_.end();
		it != end; ++it)
		if ((it.value() != 0) && (!cameraIsInRotateMode()))
			keyDescription[it.value()] = keyboardActionDescription_[it.key()];

	// Add to text in sorted order
	for (QMap<unsigned int, QString>::ConstIterator kb2 = keyDescription.begin(),
		endb2 = keyDescription.end();
		kb2 != endb2; ++kb2)
		text += tableLine(keyString(kb2.key()), kb2.value());

	return text;
}

/*! Opens a modal help window that includes four tabs, respectively filled with
helpString(), keyboardString(), mouseString() and about libQGLViewer.

Rich html-like text can be used (see the QStyleSheet documentation). This method
is called when the user presses the QGLViewer::HELP key (default is 'H').

You can use helpWidget() to access to the help widget (to add/remove tabs,
change layout...).

The helpRequired() signal is emitted. */
void QGLViewer::help() {
	Q_EMIT helpRequired();

	bool resize = false;
	int width = 600;
	int height = 400;

	static QString label[] = { tr("&Help", "Help window tab title"),
							  tr("&Keyboard", "Help window tab title"),
							  tr("&Mouse", "Help window tab title"),
							  tr("&About", "Help window about title") };

	if (!helpWidget()) {
		// Qt4 requires a nullptr parent...
		helpWidget_ = new QTabWidget(nullptr);
		helpWidget()->setWindowTitle(tr("Help", "Help window title"));

		resize = true;
		for (int i = 0; i < 4; ++i) {
			QTextEdit* tab = new QTextEdit(nullptr);
			tab->setReadOnly(true);

			helpWidget()->insertTab(i, tab, label[i]);
		}
	}

	for (int i = 0; i < 4; ++i) {
		QString text;
		switch (i) {
		case 0:
			text = helpString();
			break;
		case 1:
			text = keyboardString();
			break;
		case 2:
			text = mouseString();
			break;
		case 3:
			text = QString("LiuHao Test");
			break;
		default:
			break;
		}

		QTextEdit* textEdit = static_cast<QTextEdit*>(helpWidget()->widget(i));
		textEdit->setHtml(text);
		textEdit->setText(text);

		if (resize && (textEdit->height() > height))
			height = textEdit->height();
	}

	if (resize)
		helpWidget()->resize(width, height + 40); // 40 pixels is ~ tabs' height
	helpWidget()->show();
	helpWidget()->raise();
}

bool QGLViewer::isValidShortcutKey(int key) {
	return (key >= Qt::Key_Any && key < Qt::Key_Escape) ||
		(key >= Qt::Key_F1 && key <= Qt::Key_F35);
}


void QGLViewer::keyPressEvent(QKeyEvent* e) {
	if (e->key() == 0) {
		e->ignore();
		return;
	}

	const Qt::Key key = Qt::Key(e->key());

	const Qt::KeyboardModifiers modifiers = e->modifiers();

	QMap<KeyboardAction, unsigned int>::ConstIterator
		it = keyboardBinding_.begin(),
		end = keyboardBinding_.end();
	const unsigned int target = key | modifiers;
	while ((it != end) && (it.value() != target))
		++it;

	if (it != end)
		handleKeyboardAction(it.key());
	else {
		if (isValidShortcutKey(key))
			currentlyPressedKey_ = key;
		e->ignore();
	}
}

void QGLViewer::keyReleaseEvent(QKeyEvent* e) {
	if (isValidShortcutKey(e->key()))
		currentlyPressedKey_ = Qt::Key(0);
}

void QGLViewer::handleKeyboardAction(KeyboardAction id) {
	switch (id) {
	case HELP:
		help();
		break;
	case MOVE_CAMERA_LEFT:
		camera()->frame()->translate(camera()->frame()->inverseTransformOf(
			Vec(-10.0 * camera()->flySpeed(), 0.0, 0.0)));
		update();
		break;
	case MOVE_CAMERA_RIGHT:
		camera()->frame()->translate(camera()->frame()->inverseTransformOf(
			Vec(10.0 * camera()->flySpeed(), 0.0, 0.0)));
		update();
		break;
	case MOVE_CAMERA_UP:
		camera()->frame()->translate(camera()->frame()->inverseTransformOf(
			Vec(0.0, 10.0 * camera()->flySpeed(), 0.0)));
		update();
		break;
	case MOVE_CAMERA_DOWN:
		camera()->frame()->translate(camera()->frame()->inverseTransformOf(
			Vec(0.0, -10.0 * camera()->flySpeed(), 0.0)));
		update();
		break;
	}
}

/*! Callback method used when the widget size is modified.

If you overload this method, first call the inherited method. Also called when
the widget is created, before its first display. */
void QGLViewer::resizeGL(int width, int height) {
	QOpenGLWidget::resizeGL(width, height);
	glViewport(0, 0, GLint(width), GLint(height));
	camera()->setScreenWidthAndHeight(this->width(), this->height());
}

//////////////////////////////////////////////////////////////////////////
//                         键 盘   快 捷 键                              //
//////////////////////////////////////////////////////////////////////////

/*! Defines the shortcut() that triggers a given QGLViewer::KeyboardAction.

Here are some examples:
\code
// Press 'Q' to exit application
setShortcut(EXIT_VIEWER, Qt::Key_Q);

// Alt+M toggles camera mode
setShortcut(CAMERA_MODE, Qt::ALT + Qt::Key_M);

// The DISPLAY_FPS action is disabled
setShortcut(DISPLAY_FPS, 0);
\endcode

Only one shortcut can be assigned to a given QGLViewer::KeyboardAction (new
bindings replace previous ones). If several KeyboardAction are binded to the
same shortcut, only one of them is active. */
void QGLViewer::setShortcut(KeyboardAction action, unsigned int key) {
	keyboardBinding_[action] = key;
}

/*! Returns the keyboard shortcut associated to a given
QGLViewer::KeyboardAction.

If you want to define keyboard shortcuts for custom actions (say, open a scene
file), overload keyPressEvent() and then setKeyDescription().
 */
unsigned int QGLViewer::shortcut(KeyboardAction action) const {
	if (keyboardBinding_.contains(action))
		return keyboardBinding_[action];
	else
		return 0;
}

////////////////////////////////////////////////////////////////////////////////
//              M o u s e   b e h a v i o r   s t a t e   k e y s             //
////////////////////////////////////////////////////////////////////////////////

// 将ClickAction关联到按钮和键盘键以及修饰符的组合。
void QGLViewer::setMouseBinding(Qt::Key key, Qt::KeyboardModifiers modifiers,
	Qt::MouseButton button, ClickAction action,
	bool doubleClick, Qt::MouseButtons buttonsBefore) {
	if ((buttonsBefore != Qt::NoButton) && !doubleClick) {
		qWarning("Buttons before is only meaningful when doubleClick is true in "
			"setMouseBinding().");
		return;
	}

	if (button == Qt::NoButton) {
		qWarning("No mouse button specified in setMouseBinding");
		return;
	}

	ClickBindingPrivate cbp(modifiers, button, doubleClick, buttonsBefore, key);

	// #CONNECTION performClickAction comment on NO_CLICK_ACTION
	if (action == NO_CLICK_ACTION)
		clickBinding_.remove(cbp);
	else
		clickBinding_.insert(cbp, action);

	if ((!doubleClick) && (buttonsBefore == Qt::NoButton)) {
		MouseBindingPrivate mbp(modifiers, button, key);
		mouseBinding_.remove(mbp);
	}
}

// 将MouseAction关联到按钮和键盘键以及修饰符的组合。同时标注鼠标事件接收者
// 鼠标事件的接收者是MouseHandler（CAMERA或FRAME）。
void QGLViewer::setMouseBinding(Qt::Key key, Qt::KeyboardModifiers modifiers,
	Qt::MouseButton button, MouseHandler handler,
	MouseAction action, bool withConstraint)
{
	if (button == Qt::NoButton) {
		qWarning("No mouse button specified in setMouseBinding");
		return;
	}

	MouseActionPrivate map;
	map.handler = handler;
	map.action = action;
	map.withConstraint = withConstraint;

	MouseBindingPrivate mbp(modifiers, button, key);
	if (action == NO_MOUSE_ACTION)
		mouseBinding_.remove(mbp);
	else
		mouseBinding_.insert(mbp, map);

	ClickBindingPrivate cbp(modifiers, button, false, Qt::NoButton, key);
	clickBinding_.remove(cbp);
}

/*! Defines a MouseAction binding. */
void QGLViewer::setMouseBinding(Qt::KeyboardModifiers modifiers,
	Qt::MouseButton button, MouseHandler handler,
	MouseAction action, bool withConstraint)
{
	setMouseBinding(Qt::Key(0), modifiers, button, handler, action,
		withConstraint);
}

/*! Defines a ClickAction binding. */
void QGLViewer::setMouseBinding(Qt::KeyboardModifiers modifiers,
	Qt::MouseButton button, ClickAction action,
	bool doubleClick, Qt::MouseButtons buttonsBefore)
{
	setMouseBinding(Qt::Key(0), modifiers, button, action, doubleClick, buttonsBefore);
}

// 将MouseAction和MouseHandler关联到鼠标滚轮事件。
void QGLViewer::setWheelBinding(Qt::Key key, Qt::KeyboardModifiers modifiers,
	MouseHandler handler, MouseAction action,
	bool withConstraint) {
	//#CONNECTION# ManipulatedFrame::wheelEvent and
	// ManipulatedCameraFrame::wheelEvent switches
	if ((action != ZOOM) && (action != NO_MOUSE_ACTION)) {
		qWarning("Cannot bind %s to wheel",
			mouseActionString(action).toLatin1().constData());
		return;
	}

	if ((handler == FRAME) && (action != ZOOM) && (action != NO_MOUSE_ACTION)) {
		qWarning("Cannot bind %s to FRAME wheel",
			mouseActionString(action).toLatin1().constData());
		return;
	}

	MouseActionPrivate map;
	map.handler = handler;
	map.action = action;
	map.withConstraint = withConstraint;

	WheelBindingPrivate wbp(modifiers, key);
	if (action == NO_MOUSE_ACTION)
		wheelBinding_.remove(wbp);
	else
		wheelBinding_[wbp] = map;
}

/*! Defines a mouse wheel binding.*/
void QGLViewer::setWheelBinding(Qt::KeyboardModifiers modifiers,
	MouseHandler handler, MouseAction action, bool withConstraint) {
	setWheelBinding(Qt::Key(0), modifiers, handler, action, withConstraint);
}


/*! Clears all the default mouse bindings. */
void QGLViewer::clearMouseBindings() {
	mouseBinding_.clear();
	clickBinding_.clear();
	wheelBinding_.clear();
}

/*! Returns the MouseAction the will be triggered when the mouse \p button is
pressed, while the keyboard \p modifiers and \p key are pressed. */
QGLViewer::MouseAction QGLViewer::mouseAction(Qt::Key key,
	Qt::KeyboardModifiers modifiers, Qt::MouseButton button) const
{
	MouseBindingPrivate mbp(modifiers, button, key);
	if (mouseBinding_.contains(mbp))
		return mouseBinding_[mbp].action;
	else
		return NO_MOUSE_ACTION;
}

/*! Returns the MouseHandler which will be activated when the mouse \p button is
pressed, while the \p modifiers and \p key are pressed.*/
int QGLViewer::mouseHandler(Qt::Key key, Qt::KeyboardModifiers modifiers,
	Qt::MouseButton button) const {
	MouseBindingPrivate mbp(modifiers, button, key);
	if (mouseBinding_.contains(mbp))
		return mouseBinding_[mbp].handler;
	else
		return -1;
}

/*! Returns the keyboard state that triggers \p action on \p handler \p
withConstraint using the mouse wheel.*/
void QGLViewer::getWheelActionBinding(MouseHandler handler, MouseAction action,
	bool withConstraint, Qt::Key& key, Qt::KeyboardModifiers& modifiers) const
{
	for (QMap<WheelBindingPrivate, MouseActionPrivate>::ConstIterator
		it = wheelBinding_.begin(),
		end = wheelBinding_.end();
		it != end; ++it)
		if ((it.value().handler == handler) && (it.value().action == action) &&
			(it.value().withConstraint == withConstraint)) {
			key = it.key().key;
			modifiers = it.key().modifiers;
			return;
		}

	key = Qt::Key(-1);
	modifiers = Qt::NoModifier;
}

/*! Returns the mouse and keyboard state that triggers \p action on \p handler
\p withConstraint.*/
void QGLViewer::getMouseActionBinding(MouseHandler handler, MouseAction action,
	bool withConstraint, Qt::Key& key,
	Qt::KeyboardModifiers& modifiers, Qt::MouseButton& button) const
{
	for (QMap<MouseBindingPrivate, MouseActionPrivate>::ConstIterator
		it = mouseBinding_.begin(),
		end = mouseBinding_.end();
		it != end; ++it) {
		if ((it.value().handler == handler) && (it.value().action == action) &&
			(it.value().withConstraint == withConstraint)) {
			key = it.key().key;
			modifiers = it.key().modifiers;
			button = it.key().button;
			return;
		}
	}

	key = Qt::Key(0);
	modifiers = Qt::NoModifier;
	button = Qt::NoButton;
}

/*! Returns the MouseAction (if any) that is performed when using the wheel,
when the \p modifiers and \p key keyboard keys are pressed.*/
QGLViewer::MouseAction QGLViewer::wheelAction(Qt::Key key,
	Qt::KeyboardModifiers modifiers) const
{
	WheelBindingPrivate wbp(modifiers, key);
	if (wheelBinding_.contains(wbp))
		return wheelBinding_[wbp].action;
	else
		return NO_MOUSE_ACTION;
}

/*! Returns the MouseHandler (if any) that receives wheel events when the \p
  modifiers and \p key keyboard keys are pressed.*/
int QGLViewer::wheelHandler(Qt::Key key, Qt::KeyboardModifiers modifiers) const {
	WheelBindingPrivate wbp(modifiers, key);
	if (wheelBinding_.contains(wbp))
		return wheelBinding_[wbp].handler;
	else
		return -1;
}

/*! Same as mouseAction(), but for the ClickAction set using setMouseBinding(). */
QGLViewer::ClickAction QGLViewer::clickAction(Qt::Key key,
	Qt::KeyboardModifiers modifiers, Qt::MouseButton button,
	bool doubleClick, Qt::MouseButtons buttonsBefore) const
{
	ClickBindingPrivate cbp(modifiers, button, doubleClick, buttonsBefore, key);
	if (clickBinding_.contains(cbp))
		return clickBinding_[cbp];
	else
		return NO_CLICK_ACTION;
}

/*! Returns the mouse and keyboard state that triggers \p action. */
void QGLViewer::getClickActionBinding(ClickAction action, Qt::Key& key,
	Qt::KeyboardModifiers& modifiers, Qt::MouseButton& button, bool& doubleClick,
	Qt::MouseButtons& buttonsBefore) const {
	for (QMap<ClickBindingPrivate, ClickAction>::ConstIterator
		it = clickBinding_.begin(),
		end = clickBinding_.end();
		it != end; ++it)
		if (it.value() == action) {
			modifiers = it.key().modifiers;
			button = it.key().button;
			doubleClick = it.key().doubleClick;
			buttonsBefore = it.key().buttonsBefore;
			key = it.key().key;
			return;
		}

	modifiers = Qt::NoModifier;
	button = Qt::NoButton;
	doubleClick = false;
	buttonsBefore = Qt::NoButton;
	key = Qt::Key(0);
}

/*! This function should be used in conjunction with toggleCameraMode(). It
returns \c true when at least one mouse button is binded to the \c ROTATE
mouseAction. This is crude way of determining which "mode" the camera is in. */
bool QGLViewer::cameraIsInRotateMode() const {
	//#CONNECTION# used in toggleCameraMode() and keyboardString()
	Qt::Key key;
	Qt::KeyboardModifiers modifiers;
	Qt::MouseButton button;
	getMouseActionBinding(CAMERA, ROTATE, true /*constraint*/, key, modifiers,
		button);
	return button != Qt::NoButton;
}


/*! Makes a copy of the current buffer into a texture.

Creates a texture (when needed) and uses glCopyTexSubImage2D() to directly copy
the buffer in it.

Use \p internalFormat and \p format to define the texture format and hence which
and how components of the buffer are copied into the texture. See the
glTexImage2D() documentation for details.

When \p format is c GL_NONE (default), its value is set to \p internalFormat,
which fits most cases. Typical \p internalFormat (and \p format) values are \c
GL_DEPTH_COMPONENT and \c GL_RGBA. Use \c GL_LUMINANCE as the \p internalFormat
and \c GL_RED, \c GL_GREEN or \c GL_BLUE as \p format to capture a single color
component as a luminance (grey scaled) value. Note that \c GL_STENCIL is not
supported as a format.

The texture has dimensions which are powers of two. It is as small as possible
while always being larger or equal to the current size of the widget. The buffer
image hence does not entirely fill the texture: it is stuck to the lower left
corner (corresponding to the (0,0) texture coordinates). Use bufferTextureMaxU()
and bufferTextureMaxV() to get the upper right corner maximum u and v texture
coordinates. Use bufferTextureId() to retrieve the id of the created texture.

Here is how to display a grey-level image of the z-buffer:
\code
copyBufferToTexture(GL_DEPTH_COMPONENT);

glMatrixMode(GL_TEXTURE);
glLoadIdentity();

glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
glEnable(GL_TEXTURE_2D);

startScreenCoordinatesSystem(true);

glBegin(GL_QUADS);
glTexCoord2f(0.0, 0.0);                                 glVertex2i(0, 0);
glTexCoord2f(bufferTextureMaxU(), 0.0);                 glVertex2i(width(), 0);
glTexCoord2f(bufferTextureMaxU(), bufferTextureMaxV()); glVertex2i(width(),
height()); glTexCoord2f(0.0, bufferTextureMaxV());                 glVertex2i(0,
height()); glEnd();

stopScreenCoordinatesSystem();

glDisable(GL_TEXTURE_2D);
\endcode

Use glReadBuffer() to select which buffer is copied into the texture. See also
\c glPixelTransfer(), \c glPixelZoom() and \c glCopyPixel() for pixel color
transformations during copy.

Call makeCurrent() before this method to make the OpenGL context active if
needed.

\note The \c GL_DEPTH_COMPONENT format may not be supported by all hardware. It
may sometimes be emulated in software, resulting in poor performances.

\note The bufferTextureId() texture is binded at the end of this method. */
void QGLViewer::copyBufferToTexture(GLint internalFormat, GLenum format) {
	int h = 16;
	int w = 16;
	// Todo compare performance with qt code.
	while (w < width())
		w <<= 1;
	while (h < height())
		h <<= 1;

	bool init = false;

	if ((w != bufferTextureWidth_) || (h != bufferTextureHeight_)) {
		bufferTextureWidth_ = w;
		bufferTextureHeight_ = h;
		bufferTextureMaxU_ = width() / qreal(bufferTextureWidth_);
		bufferTextureMaxV_ = height() / qreal(bufferTextureHeight_);
		init = true;
	}

	if (bufferTextureId() == 0) {
		glGenTextures(1, &bufferTextureId_);
		glBindTexture(GL_TEXTURE_2D, bufferTextureId_);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		init = true;
	}
	else
		glBindTexture(GL_TEXTURE_2D, bufferTextureId_);

	if ((format != previousBufferTextureFormat_) ||
		(internalFormat != previousBufferTextureInternalFormat_)) {
		previousBufferTextureFormat_ = format;
		previousBufferTextureInternalFormat_ = internalFormat;
		init = true;
	}

	if (init) {
		if (format == GL_NONE)
			format = GLenum(internalFormat);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, bufferTextureWidth_,
			bufferTextureHeight_, 0, format, GL_UNSIGNED_BYTE, nullptr);
	}

	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width(), height());
}

/*! Returns the texture id of the texture created by copyBufferToTexture().

Use glBindTexture() to use this texture. Note that this is already done by
copyBufferToTexture().

Returns \c 0 is copyBufferToTexture() was never called or if the texure was
deleted using glDeleteTextures() since then. */
GLuint QGLViewer::bufferTextureId() const {
	if (glIsTexture(bufferTextureId_))
		return bufferTextureId_;
	else
		return 0;
}
