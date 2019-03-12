#ifndef GLSTRUCTUREVIEW_H
#define GLSTRUCTUREVIEW_H

#include<QObject>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

#include <QMouseEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QColor>
#include <QRgb>
#include <QPainter>
#include <QRect>

#include <core/structuralcontrol.h>
#include <glview/camera.h>
#include <glview/geometries.h>

#define EIGEN_DONT_VECTORIZE
#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT

#include <Eigen/Eigen>
#include <Eigen/StdVector>




class GLStructureView : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLStructureView(QWidget *parent = 0);
    ~GLStructureView();

    void setStructuralControl(StructuralControl * structuralControl);
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mouseDoubleClickEvent( QMouseEvent * event);
private:

    StructuralControl * pStructuralControl;

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

    Camera camera;

    QPoint lastMousePosition;
    QPoint pressMousePosition;
    QRect  selectingRect;
    bool   selecting;

    bool isHelpToShow;

public slots:
    void structuralControlChanged();
    void setFlagPerspective(bool flag);
    void setFlagAxes(bool flag);
    void setFlagCenter(bool flag);
    void setFlagLabel(bool flag);
    void setFlagCell(bool flag);
    void setFlagAtoms(bool flag);
    void setFlagBonds(bool flag);
    void setFlagWireframe(bool flag);
    void setFlagMeasureLines(bool flag);

    void setDefaultView();
    void setHelpMessage(bool flag);
signals:

};

#endif // GLSTRUCTUREVIEW_H
