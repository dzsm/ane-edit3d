#include "glstructureview.h"


GLStructureView::GLStructureView(QWidget *parent) : QOpenGLWidget(parent), pStructuralControl(0) ,selecting(false)
{

}

GLStructureView::~GLStructureView()
{
  makeCurrent();
  labels.texture.destroy();
}


void GLStructureView::setStructuralControl(StructuralControl * structuralControl) {
  pStructuralControl = structuralControl;
  update();
}


void GLStructureView::initializeGL() {

  initializeOpenGLFunctions();

  glEnable(GL_DEPTH_TEST);


  //glEnable(GL_LIGHTING);
  //glEnable(GL_LIGHT0);
  glClearColor(0.1f, 0.50f, 0.50f, 1.0f);

  glEnable(GL_BLEND);
  //glBlendFunc(GL_ONE, GL_ONE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  //glEnable(GL_CULL_FACE);
  //glEnable(GL_DEPTH32F_STENCIL8_NV);
  //glClearColor(QColor(Qt::black));

  //sphere.addShaders();
  //sphere.init();

  atoms.initialize(":/shaders/spheres_vs.glsl",":/shaders/spheres_fs.glsl");

  halos.initialize(":/shaders/halospheres_vs.glsl",":/shaders/halospheres_fs.glsl");
  pickhalos.initialize(":/shaders/pickspheres_vs.glsl",":/shaders/pickspheres_fs.glsl");

  bonds.initialize();
  lines.initialize();

  axeslines.initialize();
  axeslines.selfData.dataVertex << QVector3D(-1.0,0.0,0.0) << QVector3D(5.0,0.0,0.0) << QVector3D(0.0,-1.0,0.0) << QVector3D(0.0,5.0,0.0) << QVector3D(0.0,0.0,-1.0) << QVector3D(0.0,0.0,5.0);
  axeslines.selfData.dataColor << QVector4D(1.0,0.0,0.0,1.0) << QVector4D(1.0,0.0,0.0,1.0) << QVector4D(0.0,1.0,0.0,1.0) << QVector4D(0.0,1.0,0.0,1.0) << QVector4D(0.0,0.0,1.0,1.0) << QVector4D(0.0,0.0,1.0,1.0);
  axeslines.selfData.dataIndex << 0 << 1 << 2 << 3 << 4 << 5;
  axeslines.updateData();

  centeraxeslines.initialize();
  measurelines.initialize();
  unitcell.initialize();

  measurelabels.initialize();
  unitcelllabels.initialize();

  labels.initialize();

}

void GLStructureView::paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  axeslines.render(camera);

  if (!pStructuralControl->sequence.isEmpty()) {

      centeraxeslines.render(camera);
      unitcell.render(camera);
      unitcelllabels.render(camera);
      measurelines.render(camera);
      measurelabels.render(camera);

      bonds.render(camera);
      lines.render(camera);

      atoms.render(camera);
      halos.render(camera);
      pickhalos.render(camera);

      labels.render(camera);

    }

  if(selecting) {

      QPainter painter;
      painter.begin(this);
      painter.setRenderHint(QPainter::Antialiasing);
      //QBrush background = QBrush(QColor(64, 32, 64,140));
      QBrush background = QBrush(QColor(100,10,100,50));
      //QPen circlePen = QPen(QColor(255,10,255));
      QPen circlePen =  QPen(Qt::red);//QPen(QColor(255,10,255));
      circlePen.setWidth(1);
      QPen textPen = QPen(Qt::white);
      //textFont.setPixelSize(50);
      painter.setPen(circlePen);
      painter.setBrush(background);
      selectingRect.setCoords(pressMousePosition.x(),pressMousePosition.y(),lastMousePosition.x(),lastMousePosition.y());
      painter.drawRect(selectingRect);
      painter.end();

      glEnable(GL_DEPTH_TEST);
      glEnable(GL_BLEND);
     // glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    }
  if(isHelpToShow)
  {
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    //QBrush background = QBrush(QColor(64, 32, 64,140));
    //QBrush background = QBrush(QColor(100,10,100,50));
    //QPen circlePen = QPen(QColor(255,10,255));
    //circlePen.setWidth(1);
    QPen textPen = QPen(Qt::darkRed);
    QFont font("Serif",10);
    painter.setFont(font);

    int hh = painter.fontMetrics().height();
    //textFont.setPixelSize(50);
    painter.setPen(textPen);
    //painter.setBrush(background);
    int x = 6;
    int y = 15;
    painter.drawText(x,y,"Turn off this help : F1");y+=hh;
    painter.drawText(x,y,"Mouse Left Button : Rotate");y+=hh;
    painter.drawText(x,y,"Mouse Right Button : Pan");y+=hh;
    painter.drawText(x,y,"Mouse Middle Button : Select");y+=hh;
    painter.drawText(x,y,"Mouse Wheel : Zoom");y+=hh;
    painter.drawText(x,y,"Mouse Left Dbl Click : Pick");y+=hh;
    painter.end();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
}

void GLStructureView::resizeGL(int width, int height)
{
  camera.setSize(width,height);
  camera.setProjection();
  glViewport(0,0,width,height);
}


void GLStructureView::mousePressEvent(QMouseEvent *event)
{

  lastMousePosition = event->pos();
  pressMousePosition = event->pos();

  if (event->buttons() & (Qt::MiddleButton)) {
      selecting = true;
    }

  if (event->buttons() & (Qt::LeftButton)) {

      std::multimap<float, int> h = atoms.hits(event->pos(),camera);
      //qDebug() << h.size();
      if(h.size()>0) {
          //qDebug() << (*(h.begin())).first;
          //qDebug() << (*(h.begin())).second;
        }
    }

  event->accept();
}

void GLStructureView::mouseMoveEvent(QMouseEvent *event)
{
  if (event->buttons() & (Qt::LeftButton )) {
      QPoint delta = event->pos() - lastMousePosition;
      camera.mouseRotation(delta);
      lastMousePosition = event->pos();
      event->accept();
      update();
    }

  if (event->buttons() & (Qt::RightButton )) {
      camera.mousePan(lastMousePosition,event->pos());
      lastMousePosition = event->pos();
      event->accept();
      update();
    }
  if (event->buttons() & (Qt::MiddleButton )) {
      lastMousePosition = event->pos();
      event->accept();
      update();
    }

}

void GLStructureView::mouseReleaseEvent(QMouseEvent *event)
{

  if(selecting) {
      selecting = false;
      if (!pStructuralControl->sequence.isEmpty()) {

          MolData & mol = pStructuralControl->sequence.currentFrame();

          for(int i = 0; i <  mol.size(); i++) {

              float x = mol.positions()[i].x();
              float y = mol.positions()[i].y();
              float z = mol.positions()[i].z();

              QVector3D q = camera.project({x,y,z});
              QPoint p(q.x(),q.y());
              QVector3D q2 = camera.DeviceFromWorld({x,y,z});
              if(q2.z()<1.0) if(selectingRect.contains(p)) {

                  if(mol.getGroup(i) == 1) mol.setGroup(i,0);
                  else if(mol.getGroup(i) == 0) mol.setGroup(i,1);
                }

            }
          emit pStructuralControl->changed();
        }

      selectingRect.setHeight(0);
      selectingRect.setWidth(0);

      update();
      event->accept();
    }

}

void GLStructureView::wheelEvent(QWheelEvent *event)
{
  int delta = event->delta();
  if (event->orientation() == Qt::Vertical) {
      camera.mouseWheel(delta,event->pos());
      event->accept();
      update();
    }

}

void GLStructureView::mouseDoubleClickEvent(QMouseEvent * event)
{
    if ( event->button() == Qt::LeftButton )
    {
        if(pStructuralControl->sequence.isEmpty()) return;
        MolData & mol = pStructuralControl->sequence.currentFrame();

        std::multimap<float, int> h = atoms.hits(event->pos(),camera);
        if(h.size()>0) {
            //qDebug() << (*(h.begin())).first;
            int atomIndex = (*(h.begin())).second;
            if(mol.picks().size()>0) {
                int atomIndexPrevious = mol.picks()[mol.picks().size()-1];
                if(atomIndexPrevious != atomIndex)
                    mol.addRemoveDistance(atomIndexPrevious,atomIndex);

              }
            mol.addRemovePicks(atomIndex);
            if(mol.picks().size()>4) mol.picks().erase(mol.picks().begin());
          } else {
            mol.clearPicks();
          }
        emit pStructuralControl->changed();

    }
}

void GLStructureView::keyPressEvent(QKeyEvent *event)
{
  if(pStructuralControl->sequence.isEmpty()) return;

  if (event->key() == Qt::Key_Left) {
      if(pStructuralControl->mutex.tryLock()) {
          pStructuralControl->sequence.moveBackward();
          pStructuralControl->mutex.unlock();  }

      emit pStructuralControl->changed();
    }
  if (event->key() == Qt::Key_Right) {
      if(pStructuralControl->mutex.tryLock()) {
          pStructuralControl->sequence.moveForward();
          pStructuralControl->mutex.unlock();  }
      emit pStructuralControl->changed();
    }
  if (event->key() == Qt::Key_Up) {
      if(pStructuralControl->mutex.tryLock()) {
          pStructuralControl->sequence.moveNextColumn();
          pStructuralControl->mutex.unlock();  }
      emit pStructuralControl->changed();
    }
  if (event->key() == Qt::Key_Down) {
      if(pStructuralControl->mutex.tryLock()) {
          pStructuralControl->sequence.movePreviousColumn();
          pStructuralControl->mutex.unlock();  }
      emit pStructuralControl->changed();
    }
  if (event->key() == Qt::Key_Home) {
      if(pStructuralControl->mutex.tryLock()) {
          pStructuralControl->sequence.moveFirst();
          pStructuralControl->mutex.unlock();  }
      emit pStructuralControl->changed();
    }
  if (event->key() == Qt::Key_End) {
      if(pStructuralControl->mutex.tryLock()) {
          pStructuralControl->sequence.moveLast();
          pStructuralControl->mutex.unlock();  }
      emit pStructuralControl->changed();
    }
}

void GLStructureView::structuralControlChanged() {

  if (pStructuralControl->sequence.isEmpty()) { camera.setView({0,0,0},{5,0,0}); return; }

  MolData & mol = pStructuralControl->sequence.currentFrame();

  if(mol.isDefaultViewRequired()) {
      camera.setView({float(mol.getCenter().x()),
                      float(mol.getCenter().y()),
                      float(mol.getCenter().z())},{float(mol.getBBox().norm()),0.0f,0.0f});
      mol.requireDefaultView(false);
    }

  mol.syncOpenGLAtoms(atoms.selfData);
  atoms.updateData();

  mol.syncOpenGLHalos(halos.selfData);
  halos.updateData();

  mol.syncOpenGLPicks(pickhalos.selfData);
  pickhalos.updateData();

  mol.syncOpenGLBonds(bonds.selfData);
  bonds.updateData();

  mol.syncOpenGLWires(lines.selfData);
  lines.updateData();

  mol.syncOpenGLCenterAxes(centeraxeslines.selfData);
  centeraxeslines.updateData();

  mol.syncOpenGLMeasureLines(measurelines.selfData,measurelabels.selfData);
  measurelines.updateData();
  measurelabels.updateData();

  mol.syncOpenGLUnitCell(unitcell.selfData,unitcelllabels.selfData);
  unitcell.updateData();
  unitcelllabels.updateData();

  mol.syncOpenGLLabels(labels.selfData);
  labels.updateData();

  update();
}


void GLStructureView::setFlagPerspective(bool flag) {

  camera.changeProjectionMode(flag);
  update();

  // hack forcing to call resizeGL
  this->resize(this->width()+1,this->height());
  this->resize(this->width()-1,this->height());

}
void GLStructureView::setFlagAxes(bool flag) {    axeslines.isRenderRequired = flag;  update(); }
void GLStructureView::setFlagCenter(bool flag) {    centeraxeslines.isRenderRequired = flag;  update(); }
void GLStructureView::setFlagLabel(bool flag) {    labels.isRenderRequired = flag;  update(); }
void GLStructureView::setFlagCell(bool flag) {    unitcell.isRenderRequired = flag;   update(); }
void GLStructureView::setFlagAtoms(bool flag) {   atoms.isRenderRequired = flag; halos.isRenderRequired = flag; pickhalos.isRenderRequired = flag;
 update(); }
void GLStructureView::setFlagBonds(bool flag) {   bonds.isRenderRequired = flag; if(flag) lines.isRenderRequired = false; update(); }
void GLStructureView::setFlagWireframe(bool flag) {     lines.isRenderRequired = flag; if(flag) bonds.isRenderRequired = false;  update(); }
void GLStructureView::setFlagMeasureLines(bool flag) {     measurelines.isRenderRequired = flag; measurelabels.isRenderRequired = flag; unitcelllabels.isRenderRequired = flag&&unitcell.isRenderRequired; update(); }
void GLStructureView::setDefaultView() {
  pStructuralControl->sequence.currentFrame().requireDefaultView(true);
  emit pStructuralControl->changed();
}

void      GLStructureView::setHelpMessage(bool flag)  {
  isHelpToShow = flag;
  update();
}


