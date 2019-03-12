#ifndef CAMERA_H
#define CAMERA_H

#include<QMatrix4x4>
#include<QMatrix3x3>

struct Camera {
    int m_width;
    int m_height;

    bool m_isPerspective;

    float     m_distance;
    QVector3D  m_center;
    QVector3D  m_pan;
    QMatrix4x4 m_rotationMatrix;
    float m_scale;

    QMatrix4x4 vMatrix;
    QMatrix4x4 pMatrix;

    Camera();

    void setSize(int w,int h);
    void setView(QVector3D center,QVector3D bbox);
    void setCenter(QVector3D center);
    //void setDistance(float distance);


    void setProjection();
    //void computeView() ;
    void mouseRotation(const QPoint &delta);
    void mousePan(const QPoint &fromPoint,const QPoint &toPoint);
    void mouseWheel(int delta,QPoint pos);
    void changeProjectionMode(bool isPerspective);
    QVector3D DeviceFromView(const QVector3D &point) const;
    QVector3D ViewFromDevice(const QVector3D &point) const;
    QPoint    WinFromView(const QVector3D &point) const;
    QVector3D ViewFromWin(const QPoint &point, const QVector3D &reference) const;
    QVector3D DeviceFromWin(const QPoint & point, float depth) const;
    QPoint    WinFromDevice(QVector3D & point, float & depth) const;

    QVector3D DeviceFromWorld(const QVector3D &point) const;
    QVector3D WorldFromDevice(const QVector3D &point) const;
    QPoint    WinFromWorld(const QVector3D &point) const;
    QVector3D WorldFromWin(const QPoint &point, const QVector3D &reference) const;

    QVector3D project(const   QVector3D &point) const;

    QVector3D unProject(const   QVector3D &point) const;

    QVector3D unProject(const QPoint &point,
                        const   QVector3D &reference) const;


    QVector3D ligth() const;
    QVector3D eye() const;

};

#endif // CAMERA_H
