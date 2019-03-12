#ifndef GLVIEW_H
#define GLVIEW_H

#include <QObject>
#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

#include <QMouseEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QColor>
#include <QRgb>
#include <QPainter>
#include <QRect>

#include <control/control.h>
#include <glview/camera.h>
#include <glview/geometries.h>

#define EIGEN_DONT_VECTORIZE
#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT

//#include <Eigen/Eigen>
//#include <Eigen/StdVector>

class GLView : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLView(QWidget *parent = 0);
    ~GLView();

    void setControl(Control * control);
protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;

    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent( QMouseEvent * event) Q_DECL_OVERRIDE;
private:

    Control * control;

    SmoothSphereGeometry   atoms;
    SmoothSphereGeometry   halos;
    SmoothSphereGeometry   pickhalos;
    SmoothCylinderGeometry bonds;
    LineGeometry           lines;
    LineGeometry           axeslines;
    LineGeometry           centeraxeslines;
    LineGeometry           measurelines;
    LineGeometry           unitcell;
    TextGeometry           labels;

    TextGeometry           measurelabels;
    TextGeometry           unitcelllabels;


    QPoint lastMousePosition;
    QPoint pressMousePosition;
    QRect  selectingRect;
    bool   selecting;

    bool isHelpToShow;
public:
    Camera camera;

public slots:
    void controlChanged();

signals:

};

#endif // GLVIEW_H
