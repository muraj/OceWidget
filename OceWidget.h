#ifndef OCEWIDGET_H
#define OCEWIDGET_H

#include <QWidget>
#include <QSharedPointer>
#include <AIS_InteractiveContext.hxx>
#include <Handle_V3d_Viewer.hxx>
#include <Handle_V3d_View.hxx>

class OceViewerContext;

#ifdef WIN32
#include <WNT_Window.hxx>
typedef WNT_Window OceNativeWindow;
typedef Handle_WNT_Window Handle_OceNativeWindow;
#else
#include <Xw_Window.hxx>
typedef Xw_Window OceNativeWindow;
typedef Handle_Xw_Window Handle_OceNativeWindow;
#endif

class OceWidget : public QWidget {
  Q_OBJECT
  enum MouseNavMode {
    MOUSE_NOTHING = 0,
    MOUSE_ROTATE,
    MOUSE_PAN,
    MOUSE_ZOOM,
    NMOUSE_MODES
  };
public:
  OceWidget(QWidget* parent=0, Qt::WindowFlags f=0);
  virtual ~OceWidget() {}
  virtual QPaintEngine* paintEngine() const;
  virtual void paintEvent(QPaintEvent*);
  virtual void resizeEvent(QResizeEvent*);
  Handle_AIS_InteractiveContext getContext() { return context; }

protected:

  virtual void wheelEvent(QWheelEvent*);
  virtual void mousePressEvent(QMouseEvent*);
  virtual void mouseReleaseEvent(QMouseEvent*);
  virtual void mouseMoveEvent(QMouseEvent*);

private:
  void initialize();
  void paint();

  bool resizeNeeded;
  MouseNavMode mouseMode;
  QPoint mouseStart;

  Handle_V3d_Viewer viewer;
  Handle_OceNativeWindow window;
  Handle_V3d_View view;
  Handle_AIS_InteractiveContext context;
};

#endif // OCEWIDGET_H
