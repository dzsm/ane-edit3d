#include "glview.h"

GLView::GLView(QWidget *parent) : QOpenGLWidget(parent), control(0), selecting(false)
{
    isHelpToShow = true;
    axeslines.isRenderRequired = true;
    atoms.isRenderRequired = true;
    measurelines.isRenderRequired = true;
    pickhalos.isRenderRequired = true;
    halos.isRenderRequired = true;
    labels.isRenderRequired = true;
    measurelabels.isRenderRequired = true;
    bonds.isRenderRequired = true;
    lines.isRenderRequired = true;
    centeraxeslines.isRenderRequired = true;
    unitcell.isRenderRequired = true;
    unitcelllabels.isRenderRequired = true;

    camera.setView({0.0f,0.0f,0.0f},{5.0f,0.0f,0.0f});

}

GLView::~GLView()
{
    makeCurrent();
    labels.texture.destroy();
}


void GLView::setControl(Control * control) {
    this->control = control;
    update();
}


void GLView::initializeGL() {

    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);

    glClearColor(0.1f, 0.55f, 0.55f, 1.0f);
    ///glClearColor(0.0f, 0.0f, 0.0f,0.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

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

void GLView::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    axeslines.render(camera);

    if (!control->sequence.isEmpty()) {
        bonds.render(camera);
        lines.render(camera);

        atoms.render(camera);
        halos.render(camera);
        pickhalos.render(camera);

        labels.render(camera);

        centeraxeslines.render(camera);
        unitcell.render(camera);
        unitcelllabels.render(camera);
        measurelines.render(camera);
        measurelabels.render(camera);
    }
    if(selecting) {
        QPainter painter;
        painter.begin(this);
        painter.setRenderHint(QPainter::Antialiasing);
        QBrush background = QBrush(QColor(100,10,100,50));
        QPen circlePen =  QPen(Qt::red);
        circlePen.setWidth(1);
        QPen textPen = QPen(Qt::white);
        painter.setPen(circlePen);
        painter.setBrush(background);
        selectingRect.setCoords(pressMousePosition.x(),pressMousePosition.y(),lastMousePosition.x(),lastMousePosition.y());
        painter.drawRect(selectingRect);
        painter.end();

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    }
    if(isHelpToShow)  {
        QPainter painter;
        painter.begin(this);
        painter.setRenderHint(QPainter::Antialiasing);
        QPen textPen = QPen(Qt::darkRed);
        QFont font("Serif",10);
        painter.setFont(font);
        int hh = painter.fontMetrics().height();
        painter.setPen(textPen);
        int x = 6;
        int y = 15;

        painter.drawText(x,y,"Turn off this help : F1");y+=hh;
        painter.drawText(x,y,"Mouse Left Button : Rotate");y+=hh;
        painter.drawText(x,y,"Mouse Right Button : Pan");y+=hh;
        painter.drawText(x,y,"Mouse Middle Button : Select");y+=hh;
        painter.drawText(x,y,"Mouse Wheel : Zoom");y+=hh;
        painter.drawText(x,y,"Mouse Left Dbl Click : Pick");y+=hh;
        painter.drawText(x,y,"read: group & bit == bit");y+=hh;
        painter.drawText(x,y,"write: group = (group & ~mask) ^ flip");y+=hh;

        /*painter.drawText(x,y,"file create    <file>;     file open <file>;     file close;");y+=hh;
        painter.drawText(x,y,"file duplicate <file>;     file save <file>;     file save;");y+=hh;
        painter.drawText(x,y,"frame create; frame duplicate;   frame remove;");y+=hh;
        painter.drawText(x,y,"frame next;   frame previous;");y+=hh;

        painter.drawText(x,y,"erase <bit>;     gather <bit>;  sort <bit> <x> <y> <z>; repeatcell <n1> <n2> <n3>; copy <bit> <mask> <flip>;");y+=hh;
        painter.drawText(x,y,"group <mask> <flip> list <list>;  group <mask> <flip> <bit>;");y+=hh;
        painter.drawText(x,y,"pick <bit> <ith>;   pick <id>;    pick; distance <id1> <id2>; distance;");y+=hh;
        painter.drawText(x,y,"nb pick <pick-id> <mask> <flip>;  nb <bit> <mask> <flip>;");y+=hh;

        painter.drawText(x,y,"translate <bit> <x> <y> <z>;");y+=hh;
        painter.drawText(x,y,"rotate <bit> <ax> <ay> <az> <angle> <ox> <oy> <oz>;");y+=hh;
        painter.drawText(x,y,"rotateto <bit> <x1> <y1> <z1> <x2> <y2> <z2> <ox> <oy> <oz>;");y+=hh;
        painter.drawText(x,y,"alignalong <bit> <x1> <y1> <z1> <x2> <y2> <z2> <x3> <y3> <z3>;");y+=hh;
        painter.drawText(x,y,"autoalign <bit> <xyz>;");y+=hh;
        painter.drawText(x,y,"autolink <bit> <factor> <maxd> <mind>;");y+=hh;
        painter.drawText(x,y,"near <bit> <x> <y> <z> <maxd> <mind> <mask> <flip>;");y+=hh;
        painter.drawText(x,y,"find <bit> <label> <mask> <flip>;");y+=hh;
        painter.drawText(x,y,"cell <x1> <y1> <z1> <x2> <y2> <z2> <x3> <y3> <z3>;  cell;");y+=hh;
        painter.drawText(x,y,"add <atomicnumber> <x> <y> <z> <write-bit>;");y+=hh;
        painter.drawText(x,y,"get cell <&x1> <&y1> <&z1> <&x2> <&y2> <&z2> <&x3> <&y3> <&z3>;");y+=hh;
        painter.drawText(x,y,"get boundingbox <bit>  <&xm> <&ym> <&zm> <&xM> <&yM> <&zM>;");y+=hh;
        painter.drawText(x,y,"get center <bit> <&x> <&y> <&z>; get coord <id>  <&x> <&y> <&z>;");y+=hh;
        painter.drawText(x,y,"get pick  <ith>  <&x> <&y> <&z>; get count <bit> <&count>;  get atom pick <ith> <&atom>;");y+=hh;
        painter.drawText(x,y,"let <expr>;                      show <expr>;");y+=hh;
        painter.drawText(x,y,"read  <mask> <flip> <file>;      write <bit> <file>;");y+=hh;
        painter.drawText(x,y,"pop <mask> <flip>;               push <bit>;");y+=hh;
        painter.drawText(x,y,"setting <setting> <value>;       run <file/ane>;");y+=hh;
        painter.drawText(x,y,"jump <expr>;  (jump if((x=x+1)<5,-3,0);let x=0)");y+=hh;*/

        painter.end();

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    }
}

void GLView::resizeGL(int width, int height)
{
    camera.setSize(width,height);
    camera.setProjection();
    glViewport(0,0,width,height);
}
void GLView::mousePressEvent(QMouseEvent *event)
{

    lastMousePosition = event->pos();
    pressMousePosition = event->pos();

    if(((event->buttons() & (Qt::MiddleButton))&&(event->modifiers() & (Qt::NoModifier)) )||
       ((event->buttons() & (Qt::LeftButton))&&(event->modifiers() & (Qt::ShiftModifier)) ) ){
        selecting = true;
    }

    if ((event->buttons() & (Qt::LeftButton))&&(event->modifiers() & (Qt::NoModifier)) ) {

        std::multimap<float, int> h = atoms.hits(event->pos(),camera);
        //qDebug() << h.size();
        if(h.size()>0) {
            //qDebug() << (*(h.begin())).first;
            //qDebug() << (*(h.begin())).second;
        }
    }

    event->accept();
}
void GLView::mouseMoveEvent(QMouseEvent *event)
{

    if ((event->buttons() == Qt::LeftButton)&&(event->modifiers() == Qt::NoModifier))
    {
        QPoint delta = event->pos() - lastMousePosition;
        camera.mouseRotation(delta);
        lastMousePosition = event->pos();
        event->accept();
        update();
    }

    if ((event->buttons() == Qt::RightButton)&&(event->modifiers() == Qt::NoModifier))
    {
        camera.mousePan(lastMousePosition,event->pos());
        lastMousePosition = event->pos();
        event->accept();
        update();
    }
    if(((event->buttons() == (Qt::MiddleButton))&&(event->modifiers() == (Qt::NoModifier))
        )||(
        (event->buttons() == (Qt::LeftButton))&&(event->modifiers() == (Qt::ShiftModifier)) ) )
    {
        lastMousePosition = event->pos();
        event->accept();
        update();
    }

}
void GLView::mouseReleaseEvent(QMouseEvent *event)
{
    if(selecting) {
        selecting = false;

        if (!control->sequence.isEmpty()) {

            MoleculeData & mol = control->sequence.currentFrame();

            std::vector<int> selection;
            selection.reserve(mol.size());
            for(int i = 0; i <  mol.size(); i++) {
                float x = mol.positions()[i].x();
                float y = mol.positions()[i].y();
                float z = mol.positions()[i].z();

                QVector3D q = camera.project({x,y,z});
                QPoint p(q.x(),q.y());
                QVector3D q2 = camera.DeviceFromWorld({x,y,z});

                if(q2.z()<=1.0) if(selectingRect.contains(p)) selection.push_back(i);
            }
            if(selection.size()>0) {
                control->script_group(1, 1, selection);
            } else {
                control->script_group(1, 0, 0);
            }
        }

        selectingRect.setHeight(0);
        selectingRect.setWidth(0);

        update();
    }
    event->accept();
}
void GLView::wheelEvent(QWheelEvent *event)
{
    int delta = event->delta();
    if (event->orientation() == Qt::Vertical) {
        camera.mouseWheel(delta,event->pos());
        event->accept();
        update();
    }
}
void GLView::mouseDoubleClickEvent(QMouseEvent * event)
{
    if ( event->button() == Qt::LeftButton )
    {
        if(control->sequence.isEmpty()) return;
        MoleculeData & mol = control->sequence.currentFrame();

        std::multimap<float, int> h = atoms.hits(event->pos(),camera);
        if(h.size()>0) {
            int atomIndex = (*(h.begin())).second;
            if(mol.picks().size()>0) {
                int atomIndexPrevious = mol.picks()[mol.picks().size()-1];
                if(atomIndexPrevious != atomIndex)
                    control->script_distance(atomIndexPrevious,atomIndex);

            }
            control->script_pick(atomIndex);
        } else {

            control->script_pick();
            control->script_distance();
        }


    }
    event->accept();
}
void GLView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F1) {
        isHelpToShow =!isHelpToShow;
        if(isHelpToShow) control->settings["help"] = "ON"; else control->settings["help"] = "OFF";
        update();
    }

    /*if (event->key() == Qt::Key_Left) {
        control->previous();
    }
    if (event->key() == Qt::Key_Right) {
        control->next();
    }
    if (event->key() == Qt::Key_Space) {

        if (control->sequence.isEmpty()) { camera.setView({0,0,0},{5,0,0}); } else {

            double x,y,z,mx,my,mz,Mx,My,Mz;

            if(control->getPick(0,x,y,z)) ; else control->getCenter(255,x,y,z);
            control->getBoundingBox(255,mx,my,mz,Mx,My,Mz);
            camera.setView({float(x),float(y),float(z)},{float(Mx-mx),float(My-my),float(Mz-mz)});
        }
        update();
    }
    if (event->key() == Qt::Key_C) {

        if (control->sequence.isEmpty()) { camera.setView({0,0,0},{5,0,0}); } else {

            double x,y,z,mx,my,mz,Mx,My,Mz;

            if(control->getPick(0,x,y,z)) {
                camera.setCenter({float(x),float(y),float(z)});
            }

        }
        update();
    }
    event->accept();*/
}
void GLView::controlChanged() {

    atoms.isRenderRequired = (control->settings["atoms"] == "ON");
    axeslines.isRenderRequired = (control->settings["axeslines"] == "ON");
    measurelines.isRenderRequired = (control->settings["measurelines"] == "ON");
    pickhalos.isRenderRequired =  (control->settings["pickhalos"] == "ON");
    halos.isRenderRequired =  (control->settings["halos"] == "ON");
    measurelabels.isRenderRequired = (control->settings["measurelabels"] == "ON");
    bonds.isRenderRequired = (control->settings["bonds"] == "ON");
    lines.isRenderRequired = (control->settings["lines"] == "ON");
    centeraxeslines.isRenderRequired = (control->settings["halos"] == "ON");
    unitcell.isRenderRequired = (control->settings["cell"] == "ON");
    unitcelllabels.isRenderRequired = (control->settings["celllabels"] == "ON");
    isHelpToShow = (control->settings["help"] == "ON");


    if (control->sequence.isEmpty()) { camera.setView({0,0,0},{5,0,0}); update(); return; }

    MoleculeData & mol = control->sequence.currentFrame();

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

    int mode = 0;

    //SYMBOL|ID|GROUP|GROUPBIT|NONE
    if(control->settings["labels"] == "SYMBOL") mode = 0;
    if(control->settings["labels"] == "ID") mode = 1;
    if(control->settings["labels"] == "GROUP") mode = 2;
    if(control->settings["labels"] == "GROUPBIT") mode = 3;
    labels.isRenderRequired = (control->settings["labels"] != "NONE");
    mol.syncOpenGLLabels(labels.selfData,mode);
    labels.updateData();

    update();
}


