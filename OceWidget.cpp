#include <QWheelEvent>
#include <QMouseEvent>
#include <QtCore/qmath.h>
#include <QDebug>

#include "OceWidget.h"

#include <V3d_Viewer.hxx>
#include <AIS_InteractiveContext.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <V3d_View.hxx>
#ifdef WIN32
#include <WNT_Window.hxx>
#else
#include <Xw_Window.hxx>
#endif

static Handle(V3d_Viewer) createViewer(
  const Standard_ExtString name,
  const Standard_CString domain,
  const Standard_Real size,
  const V3d_TypeOfOrientation viewProj,
  const Standard_Boolean computedMode,
  const Standard_Boolean defaultComputedMode)
{
  Handle(Graphic3d_GraphicDriver) driver = new OpenGl_GraphicDriver(
      new Aspect_DisplayConnection());
  return new V3d_Viewer(driver, name, domain, size, viewProj,
      Quantity_NOC_BLACK, V3d_ZBUFFER, V3d_GOURAUD, V3d_WAIT,
      computedMode, defaultComputedMode, V3d_TEX_NONE);
}

OceWidget::OceWidget(QWidget* parent, Qt::WindowFlags f) :
    QWidget(parent, f | Qt::MSWindowsOwnDC),
    resizeNeeded(false), mouseMode(MOUSE_NOTHING) {
  this->setMouseTracking(true);
  this->setBackgroundRole(QPalette::NoRole);
  this->setFocusPolicy(Qt::StrongFocus);
  // Double buffering off
  this->setAttribute(Qt::WA_PaintOnScreen);
  // Speed up resizing
  this->setAttribute(Qt::WA_NoSystemBackground, false);
  this->setAutoFillBackground(false);
}

QPaintEngine* OceWidget::paintEngine() const {
  return NULL;
}

void OceWidget::paintEvent(QPaintEvent*) {
  if(window.IsNull() && winId())
    initialize();  // Initialize the view
  if(!window.IsNull())
    paint();	   // Paint the view
}

void OceWidget::resizeEvent(QResizeEvent*) {
  resizeNeeded = true;
}

void OceWidget::wheelEvent(QWheelEvent* event) {
  if(view.IsNull()) {
    event->ignore();
    return;
  }
  Standard_Real scale = view->Scale();
  qreal delta = event->delta() / 120.0f;
  scale *= qPow(1.10f, delta);
  view->SetScale(scale);
}

void OceWidget::mousePressEvent(QMouseEvent* e) {
  QWidget::mousePressEvent(e);
  qDebug() << "MousePress:" << e->buttons() << e->modifiers();
  if(e->buttons() & Qt::LeftButton) {
    if(e->modifiers() == Qt::NoModifier) { // rotate
      mouseMode = MOUSE_ROTATE;
      view->StartRotation(e->pos().x(), e->pos().y());
    }
    else if(e->modifiers() & Qt::ShiftModifier) // pan
      mouseMode = MOUSE_PAN;
    else if(e->modifiers() & Qt::ControlModifier) // zoom
      mouseMode = MOUSE_ZOOM;
    mouseStart = e->pos();
  }
}

void OceWidget::mouseReleaseEvent(QMouseEvent* e) {
  QWidget::mouseReleaseEvent(e);
  mouseMode = MOUSE_NOTHING;
}

void OceWidget::mouseMoveEvent(QMouseEvent* e) {
  QWidget::mouseMoveEvent(e);
  qDebug() << "MouseMove:" << e->buttons() << mouseMode;
  if(e->buttons() == Qt::NoButton)
  {
    context->MoveTo(e->pos().x(), e->pos().y(), view);
    return;
  }
  switch(mouseMode) {
  case MOUSE_ROTATE:
    qDebug() << "Rotating...";
    view->Rotation(e->pos().x(), e->pos().y());
    break;
  case MOUSE_ZOOM:
    view->Zoom(mouseStart.x(), mouseStart.y(),
        e->pos().x(), e->pos().y());
    mouseStart = e->pos();
    break;
  case MOUSE_PAN:
    view->Pan(e->pos().x() - mouseStart.x(),
        e->pos().y() - mouseStart.y());
    mouseStart = e->pos();
    break;
  default: break;
  }
}

void OceWidget::initialize() {
  TCollection_ExtendedString name(this->windowTitle().toUtf8().constData());
  viewer = ::createViewer(name.ToExtString(), "", 3000.0, V3d_XposYnegZpos,
      Standard_True, Standard_True);
  if(viewer.IsNull()) return;
  viewer->SetDefaultTypeOfView(V3d_ORTHOGRAPHIC);
  viewer->SetDefaultLights();
  viewer->SetLightOn();
  view = viewer->CreateView();
  view->SetBackgroundColor(Quantity_NOC_BLACK);
  context = new AIS_InteractiveContext(viewer);
  Aspect_Handle hwd = (Aspect_Handle)winId();
  window = new OceNativeWindow(viewer->Driver()->GetDisplayConnection(), hwd);
#if WIN32
  window->SetFlags(WDF_NOERASEBKGRND);
#endif
  view->SetWindow(window);
  if(!window->IsMapped())
    window->Map();
  resizeNeeded = true;	// Resize when able.
}

void OceWidget::paint() {
  if(!this->view.IsNull()) {
    if(resizeNeeded)
      this->view->MustBeResized();
    else
      this->view->Redraw();
  }
  resizeNeeded = false;
}
