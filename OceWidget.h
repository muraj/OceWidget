#ifndef OCEWIDGET_H
#define OCEWIDGET_H

#include <QWidget>
#include <QSharedPointer>
#include <AIS_InteractiveContext.hxx>
#include <Handle_V3d_Viewer.hxx>
#include <Handle_V3d_View.hxx>

class OceViewerContext;

class OceWidget : public QWidget {
  Q_OBJECT
  enum MouseNavMode {
    MOUSE_NOTHING = 0,
    MOUSE_ROTATE,
    MOUSE_PAN,
    MOUSE_ZOOM,
    MOUSE_PICK,
    NMOUSE_MODES
  };

public:

  OceWidget(QWidget* parent=0, Qt::WindowFlags f=0);
  virtual ~OceWidget() {}
  virtual QPaintEngine* paintEngine() const;
  virtual void paintEvent(QPaintEvent*);
  virtual void resizeEvent(QResizeEvent*);
  Handle_AIS_InteractiveContext getContext() { return context; }

signals:

  void selectionChanged();

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
  Handle_V3d_View view;
  Handle_AIS_InteractiveContext context;
};

#endif // OCEWIDGET_H
