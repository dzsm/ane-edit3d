#include "camera.h"
#include <cmath>

Camera::Camera() {
    m_isPerspective = true;
}

void Camera::setView(QVector3D center, QVector3D bbox) {
    m_center = center;
    m_distance = 0.5*bbox.length()/std::tan(15.0/180.0*3.1415);
    vMatrix.setToIdentity();
    vMatrix.lookAt({0,0,-m_distance}, m_center, {0,1,0});
}
void Camera::setCenter(QVector3D center) {

    vMatrix.translate(m_center-center);
    m_center = center;

    /*m_center = center;
    m_distance = 0.5*bbox.length()/std::tan(15.0/180.0*3.1415);
    vMatrix.setToIdentity();
    vMatrix.lookAt({0,0,-m_distance}, m_center, {0,1,0});*/
}
void Camera::setSize(int w,int h) {
    m_width = w;
    m_height = h;
}


void Camera::setProjection() {

    float zNear = 0.01;
    float zFar = 2000.0;
    if(m_isPerspective ) {
        //qDebug() << "m_isPerspective";
        int m = std::min(m_width,m_height);
        pMatrix.setToIdentity();
        float fieldOfView  = 30.0;
        float f  = 1.0f / std::tan(fieldOfView * float(3.1415) / 360.0f);
        pMatrix(0, 0) = f / (static_cast<float>(m)/static_cast<float>(m_height))  ;
        pMatrix(1, 1) = f / (static_cast<float>(m)/static_cast<float>(m_width))  ;
        pMatrix(2, 2) = (zNear + zFar) / (zNear - zFar);
        pMatrix(2, 3) = (2.0f * zFar * zNear) / (zNear - zFar);
        pMatrix(3, 2) = -1;
        pMatrix(3, 3) = 0;

    }else  {

        //qDebug() << "m_isPerspective NEVEREVER???";

        float f =  2.0*std::tan(30.0/180.0*3.1415/2.0) * m_scale;
        //float m = std::max(m_width,m_height); // or /f here
        //pMatrix.ortho(-m_width/m/2.0,m_width/m/2.0,-m_height/m/2.0,m_height/m/2.0,-1000,1000);
        //pMatrix.scale(1/f,1/f,1.0);

        int m = std::min(m_width,m_height);
        pMatrix.setToIdentity();
        pMatrix(0, 0) = 1.0f / (static_cast<float>(m)/static_cast<float>(m_height));
        pMatrix(1, 1) = 1.0f / (static_cast<float>(m)/static_cast<float>(m_width));
        pMatrix(2, 2) = -2.0f / (zFar - zNear);
        pMatrix(2, 3) = -(zFar + zNear) / (zFar - zNear);
        pMatrix(3, 3) = 1.0f;
        pMatrix.scale(1/f,1/f,1.0);

    }

}

void Camera::mouseRotation(const QPoint &delta) {
    const float ROTATION_SPEED = 0.02f;

    QVector3D xAxis = QVector3D(vMatrix.row(0)).normalized();
    QVector3D yAxis = QVector3D(vMatrix.row(1)).normalized();
    QVector3D center = m_center;

    vMatrix.translate(center);
    vMatrix.rotate(180.0/3.1415*static_cast<float>(delta.y()) * ROTATION_SPEED, xAxis);
    vMatrix.rotate(180.0/3.1415*static_cast<float>(delta.x()) * ROTATION_SPEED, yAxis);
    vMatrix.translate(-center);
}
void Camera::mousePan(const QPoint &fromPoint,const QPoint &toPoint) {

    QVector3D center = m_center;
    QVector3D from(unProject(fromPoint, center));
    QVector3D to(unProject(toPoint, center));

    vMatrix.translate(to-from);
}
void Camera::mouseWheel(int delta,QPoint pos) {
    const float ZOOM_SPEED = 0.02f;

    QVector3D zAxis = QVector3D(vMatrix.row(2)).normalized();
    vMatrix.translate(zAxis * static_cast<float>(delta) * ZOOM_SPEED);
    if(delta>0)
        m_scale *= 1.2;
    else m_scale /= 1.2;

}
void Camera::changeProjectionMode(bool isPerspective) {
    m_isPerspective = isPerspective;
    setProjection();
}

QVector3D Camera::DeviceFromView(const QVector3D &point) const
{
    QVector4D tPoint(point.x(), point.y(), point.z(), 1.0f);
    tPoint = pMatrix * tPoint;
    tPoint /= tPoint.w();
    return QVector3D(tPoint);
}
QVector3D Camera::ViewFromDevice(const QVector3D &point) const
{
    QVector4D result(point.x(),point.y(),point.z(),1.0);
    result = pMatrix.inverted() * result;
    result = result * result.w();
    result.setW(1.0);
    return QVector3D(result);
}
QPoint Camera::WinFromView(const QVector3D &point) const {
    QVector3D result = DeviceFromView(point);
    float d;
    return WinFromDevice(result,d);
}
QVector3D Camera::ViewFromWin(const QPoint &point, const QVector3D &reference) const
{
    //return unProject(WinToDevice(point,project(reference).z()));

    return ViewFromDevice(DeviceFromWin(point,DeviceFromWorld(reference).z()));
    //return ViewFromDevice(DeviceFromWin(point,-1.0));
}

QVector3D Camera::project(const   QVector3D &point) const
{
    QMatrix4x4 mvp = pMatrix * vMatrix;
    QVector4D tPoint(point.x(), point.y(), point.z(), 1.0f);
    tPoint = mvp * tPoint;
    QVector3D result(static_cast<float>(m_width)
                     * (tPoint.x() / tPoint.w() + 1.0f) / 2.0f,
                     static_cast<float>(m_height)
                     * (-tPoint.y() / tPoint.w() + 1.0f) / 2.0f,
                     (tPoint.z() / tPoint.w() + 1.0f) / 2.0f);
    return result;
}
QVector3D Camera::unProject(const   QVector3D &point) const
{
    QMatrix4x4 mvp = pMatrix * vMatrix;
    QVector4D result(2.0f * point.x() / static_cast<float>(m_width) - 1.0f,
                     2.0f * (static_cast<float>(m_height) - point.y()) /
                     static_cast<float>(m_height) - 1.0f,
                     2.0f * point.z() - 1.0f,
                     1.0f);
    result = mvp.inverted() * result;
    return   QVector3D(result.x() / result.w(), result.y() / result.w(),
                       result.z() / result.w());
}
QVector3D Camera::unProject(const QPoint &point,
                            const   QVector3D &reference) const
{
    return unProject(QVector3D(point.x(), point.y(), project(reference).z()));
}

QVector3D Camera::ligth() const  {
    return m_rotationMatrix *  QVector3D(0, 1, 1);
}
QVector3D Camera::eye() const  {
    return m_rotationMatrix *  QVector3D(0, 0, 1);
}


QVector3D Camera::DeviceFromWin(const QPoint & point, float depth = 0.0) const {
    int m = m_width;
    QVector3D result(
                2.0*(static_cast<float>(point.x()) - static_cast<float>(m_width)*0.5)/static_cast<float>(m_width),
                -2.0*(static_cast<float>(point.y()) - static_cast<float>(m_height)*0.5)/static_cast<float>(m_height),depth);
    return result;
}
QPoint Camera::WinFromDevice(QVector3D & point, float & depth) const {
    int m = m_width;
    depth = point.z();
    return QPoint(static_cast<int>(0.5*(static_cast<float>(m_width)*point.x() + static_cast<float>(m_width))),
                  static_cast<int>(0.5*(-static_cast<float>(m_height)*point.y() + static_cast<float>(m_height))));
}
QVector3D Camera::DeviceFromWorld(const QVector3D &point) const
{

    QMatrix4x4 mvp = pMatrix*vMatrix;
    QVector4D tPoint(point.x(), point.y(), point.z(), 1.0f);
    tPoint = vMatrix * tPoint;
    tPoint.setW(1.0);
    tPoint = pMatrix * tPoint;
    tPoint /= tPoint.w();
    return QVector3D(tPoint);

}
QVector3D Camera::WorldFromDevice(const QVector3D &point) const
{
    QMatrix4x4 mvp = pMatrix*vMatrix;
    QVector4D result(point.x(),point.y(),point.z(),1.0);
    result = pMatrix.inverted() * result;
    result = result * result.w();
    result.setW(1.0);
    result = vMatrix.inverted() * result;
    return QVector3D(result);
}
QPoint Camera::WinFromWorld(const QVector3D &point) const {
    QVector3D result = DeviceFromWorld(point);
    float d;
    return WinFromDevice(result,d);
}
QVector3D Camera::WorldFromWin(const QPoint &point, const QVector3D &reference) const
{
    return WorldFromDevice(DeviceFromWin(point,DeviceFromWorld(reference).z()));
}

