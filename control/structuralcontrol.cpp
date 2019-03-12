#include "structuralcontrol.h"
#include <QTime>
#include <QCoreApplication>

FileData::FileData() : isSaved(true) {}

// NAME: ANE - Atomic Nanostructure Editor

MolData::MolData() : Molecule::Molecule(), m_defaultViewRequired(true) {}

bool MolData::isDefaultViewRequired() const {
  return m_defaultViewRequired;
}
void MolData::requireDefaultView(bool require) {
  m_defaultViewRequired = require;
}

void MolData::syncOpenGLAtoms(SphereData & openGLAtoms) {
  openGLAtoms.clear();

  for(int i = 0; i < m_positions.size(); i++) {
      const ElementData::ElementData::ElementInfo & ei = getElementInfo(i);
      float r = ei.vanDerWaalsRadio*0.2;
      QVector3D p(m_positions[i].x(),m_positions[i].y(),m_positions[i].z());
      QVector4D c(float(ei.cpkColor[0]),float(ei.cpkColor[1]),float(ei.cpkColor[2]),1.0);

      openGLAtoms.indecies << i;
      openGLAtoms.positions << p;
      openGLAtoms.colors << c;
      openGLAtoms.radius << r;

      openGLAtoms.dataColor << c << c << c << c;
      openGLAtoms.dataVertex << p << p << p << p;
      openGLAtoms.dataTexCoord << QVector2D(-r,-r) << QVector2D(-r, r) << QVector2D( r,-r) << QVector2D( r, r);

      GLuint index = 4 * i;
      openGLAtoms.dataIndex << index + 0 << index + 1 << index + 2 << index + 3 << index + 2 << index + 1;
    }
}
void MolData::syncOpenGLHalos(SphereData & openGLHalos) {
  openGLHalos.clear();

  int count = 0;
  for(int i = 0; i < m_positions.size(); i++) if(getGroup(i) == 1) {

      const ElementData::ElementData::ElementInfo & ei = getElementInfo(i);
      float r = ei.vanDerWaalsRadio*0.2*1.25;
      QVector3D p(m_positions[i].x(),m_positions[i].y(),m_positions[i].z());
      //QVector4D c(float(ei.cpkColor[0]),float(ei.cpkColor[1]),float(ei.cpkColor[2]),1.0);
      QVector4D c(0.6,0.0,0.6,0.5);

      openGLHalos.indecies << count;
      openGLHalos.positions << p;
      openGLHalos.colors << c;
      openGLHalos.radius << r;

      openGLHalos.dataColor << c << c << c << c;
      openGLHalos.dataVertex << p << p << p << p;
      openGLHalos.dataTexCoord << QVector2D(-r,-r) << QVector2D(-r, r) << QVector2D( r,-r) << QVector2D( r, r);

      GLuint index = 4 * count;
      openGLHalos.dataIndex << index + 0 << index + 1 << index + 2 << index + 3 << index + 2 << index + 1;
      count++;
    }
}
void MolData::syncOpenGLPicks(SphereData & openGLPicks){
  openGLPicks.clear();

  QVector4D pickcolors[4] = {QVector4D(1.0,0.0,0.0,1.0),
                             QVector4D(0.0,1.0,0.0,1.0),
                             QVector4D(0.0,0.0,1.0,1.0),
                             QVector4D(1.0,1.0,0.0,1.0)};
  int count = 0;
  for(int j = 0; j < m_picks.size(); j++) {

      int i = m_picks[j];
      const ElementData::ElementData::ElementInfo & ei = getElementInfo(i);
      float r = ei.vanDerWaalsRadio*0.2*1.3;
      QVector3D p(m_positions[i].x(),m_positions[i].y(),m_positions[i].z());
      //QVector4D c(float(ei.cpkColor[0]),float(ei.cpkColor[1]),float(ei.cpkColor[2]),1.0);
      QVector4D c = pickcolors[j%4];

      openGLPicks.indecies << count;
      openGLPicks.positions << p;
      openGLPicks.colors << c;
      openGLPicks.radius << r;

      openGLPicks.dataColor << c << c << c << c;
      openGLPicks.dataVertex << p << p << p << p;
      openGLPicks.dataTexCoord << QVector2D(-r,-r) << QVector2D(-r, r) << QVector2D( r,-r) << QVector2D( r, r);

      GLuint index = 4 * count;
      openGLPicks.dataIndex << index + 0 << index + 1 << index + 2 << index + 3 << index + 2 << index + 1;
      count++;
    }
}


void MolData::syncOpenGLBonds(CylinderData & openGLBonds) {
  openGLBonds.clear();

  GLuint indexCounter = 0;
  for(int i = 0; i < m_links.size(); i++) {

      int a = std::get<0>(m_links[i]);
      int b = std::get<1>(m_links[i]);
      char o = m_orders[i];

      float r = 0.15;

      const ElementData::ElementData::ElementInfo & eia = getElementInfo(a);
      const ElementData::ElementData::ElementInfo & eib = getElementInfo(b);

      QVector3D va(m_positions[a].x(),m_positions[a].y(),m_positions[a].z());
      QVector3D vb(m_positions[b].x(),m_positions[b].y(),m_positions[b].z());
      QVector3D vm = 0.5*(va+vb);

      QVector4D ca(float(eia.cpkColor[0]),float(eia.cpkColor[1]),float(eia.cpkColor[2]),1.0);
      QVector4D cb(float(eib.cpkColor[0]),float(eib.cpkColor[1]),float(eib.cpkColor[2]),1.0);

      //QVector4D ca(1.0,0,0,1.0);
      //QVector4D cb(0,0,1,1.0);

      openGLBonds.indecies << i;
      openGLBonds.positions1 << va;
      openGLBonds.positions2 << vb;
      openGLBonds.colors1 << ca;
      openGLBonds.colors2 << cb;
      openGLBonds.radius << r;

      QMatrix4x4 mMatrix,nMatrix;

      //mMatrix.setToIdentity();
      //mMatrix.translate(va);
      //mMatrix.rotate(-std::acos((vm-va).normalized().z())/3.1415*180.0,QVector3D::normal(vm-va,{0.0,0.0,1.0}));
      //mMatrix.scale(r,r,(vm-va).length());
      //mMatrix.optimize();
      nMatrix.setToIdentity();
      nMatrix.rotate(-std::acos((vm-va).normalized().z())/3.1415*180.0,QVector3D::normal(vm-va,{0.0,0.0,1.0}));
      nMatrix.scale(r,r,(vm-va).length());
      nMatrix.optimize();

      for(int k = 0 ; k < openGLBonds.singleDataVertex.size(); k++) {
          openGLBonds.dataVertex << nMatrix*openGLBonds.singleDataVertex[k] + va;
          openGLBonds.dataNormal << nMatrix*openGLBonds.singleDataNormal[k];
          openGLBonds.dataColor << ca;
        }
      for(int k = 0 ; k < openGLBonds.singleDataIndex.size(); k++) {
          openGLBonds.dataIndex << openGLBonds.singleDataIndex[k] + indexCounter;
        }
      indexCounter+=openGLBonds.singleDataVertex.size();

      //mMatrix.setToIdentity();
      //mMatrix.translate(vm);
      //mMatrix.rotate(-std::acos((vb-vm).normalized().z())/3.1415*180.0,QVector3D::normal(vb-vm,{0.0,0.0,1.0}));
      //mMatrix.scale(r,r,(vb-vm).length());
      //mMatrix.optimize();
      nMatrix.setToIdentity();
      nMatrix.rotate(-std::acos((vb-vm).normalized().z())/3.1415*180.0,QVector3D::normal(vb-vm,{0.0,0.0,1.0}));
      nMatrix.scale(r,r,(vb-vm).length());
      nMatrix.optimize();


      for(int k = 0 ; k < openGLBonds.singleDataVertex.size(); k++) {
          openGLBonds.dataVertex << nMatrix*openGLBonds.singleDataVertex[k] + vm;
          openGLBonds.dataNormal << nMatrix*openGLBonds.singleDataNormal[k];
          openGLBonds.dataColor << cb;
        }
      for(int k = 0 ; k < openGLBonds.singleDataIndex.size(); k++) {
          openGLBonds.dataIndex << openGLBonds.singleDataIndex[k] + indexCounter;
        }
      indexCounter+=openGLBonds.singleDataVertex.size();

    }
}
void MolData::syncOpenGLWires(LineData & openGLLines) {
  openGLLines.clear();

  GLuint indexCounter = 0;
  for(int i = 0; i < m_links.size(); i++) {

      int a = std::get<0>(m_links[i]);
      int b = std::get<1>(m_links[i]);
      char o = m_orders[i];

      float r = 0.15;

      const ElementData::ElementData::ElementInfo & eia = getElementInfo(a);
      const ElementData::ElementData::ElementInfo & eib = getElementInfo(b);

      QVector3D va(m_positions[a].x(),m_positions[a].y(),m_positions[a].z());
      QVector3D vb(m_positions[b].x(),m_positions[b].y(),m_positions[b].z());
      QVector3D vm = 0.5*(va+vb);

      QVector4D ca(float(eia.cpkColor[0]),float(eia.cpkColor[1]),float(eia.cpkColor[2]),1.0);
      QVector4D cb(float(eib.cpkColor[0]),float(eib.cpkColor[1]),float(eib.cpkColor[2]),1.0);

      //QVector4D ca(1.0,0,0,1.0);
      //QVector4D cb(0,0,1,1.0);

      openGLLines.indecies << i;
      openGLLines.positions1 << va;
      openGLLines.positions2 << vb;
      openGLLines.colors << ca;
      openGLLines.width << 0.1;


      openGLLines.dataVertex << va << vm << vm << vb;
      openGLLines.dataColor << ca << ca << cb << cb;
      openGLLines.dataIndex << indexCounter << indexCounter+1 << indexCounter+2 << indexCounter+3;
      indexCounter += 4;
    }

}
void MolData::syncOpenGLMeasureLines(LineData & openGLLines,TextData & openGLTexts) {
  openGLLines.clear();
  openGLTexts.clear();

  GLuint indexCounter = 0;
  for(auto it = m_distances.begin(); it != m_distances.end(); ++it) {

      int a = std::get<0>(*it);
      int b = std::get<1>(*it);

      QVector3D va(m_positions[a].x(),m_positions[a].y(),m_positions[a].z());
      QVector3D vb(m_positions[b].x(),m_positions[b].y(),m_positions[b].z());

      QVector4D ca(1,1,1,1.0);

      //openGLLines.indecies << i;
      openGLLines.positions1 << va;
      openGLLines.positions2 << vb;
      openGLLines.colors << ca;
      openGLLines.width << 0.1;


      openGLLines.dataVertex << va << vb;
      openGLLines.dataColor << ca << ca;
      openGLLines.dataIndex << indexCounter << indexCounter+1;
      indexCounter += 2;

      int count = openGLTexts.charTextureAtlas.addVerexTexCoords(openGLTexts.dataVertexTexCoord,QString::number((va-vb).length(),'f',3),1,1);
      for(int k = 0; k < count;k++)
        openGLTexts.dataPosition << 0.5*(va+vb);
    }

}
void MolData::syncOpenGLUnitCell(LineData & openGLLines,TextData & openGLTexts) {
  openGLLines.clear();
  openGLTexts.clear();

  if(m_hasUnitCell == false) return;

  Eigen::Vector3d cmi,cma;
  getBBox(cmi,cma);

  QVector3D o(cmi.x(),cmi.y(),cmi.z());
  QVector3D a(m_a1.x(),m_a1.y(),m_a1.z());
  QVector3D b(m_a2.x(),m_a2.y(),m_a2.z());
  QVector3D c(m_a3.x(),m_a3.y(),m_a3.z());

  openGLLines.dataVertex << o << o+a << o+b << o+b+a << o+c << o+c+a << o+b+c << o+b+c+a;
  openGLLines.dataVertex << o << o+b << o+a << o+a+b << o+c << o+c+b << o+a+c << o+b+c+a;
  openGLLines.dataVertex << o << o+c << o+a << o+a+c << o+b << o+c+b << o+a+b << o+b+c+a;

  QVector4D ca(1,1,0,1.0);
  QVector4D cb(0,1,1,1.0);
  QVector4D cc(1,0,1,1.0);

  for(int k = 0; k < 8;k++) { openGLLines.dataColor << ca; openGLLines.dataIndex << k; }
  for(int k = 0; k < 8;k++) { openGLLines.dataColor << cb; openGLLines.dataIndex << k+8; }
  for(int k = 0; k < 8;k++) { openGLLines.dataColor << cc; openGLLines.dataIndex << k+16; }

  int count = openGLTexts.charTextureAtlas.addVerexTexCoords(openGLTexts.dataVertexTexCoord,QString::number(a.length(),'f',3),1,1);
  for(int k = 0; k < count;k++) openGLTexts.dataPosition << 0.5*(2.0*o+a);
  count = openGLTexts.charTextureAtlas.addVerexTexCoords(openGLTexts.dataVertexTexCoord,QString::number(b.length(),'f',3),1,1);
  for(int k = 0; k < count;k++) openGLTexts.dataPosition << 0.5*(2.0*o+b);
  count = openGLTexts.charTextureAtlas.addVerexTexCoords(openGLTexts.dataVertexTexCoord,QString::number(c.length(),'f',3),1,1);
  for(int k = 0; k < count;k++) openGLTexts.dataPosition << 0.5*(2.0*o+c);

}
void MolData::syncOpenGLCenterAxes(LineData & openGLLines) {
  openGLLines.clear();

  const Eigen::Vector3d & center = getCenter();
  QVector3D c(center.x(),center.y(),center.z());
  const Eigen::Vector3d & bbox = getBBox();
  QVector3D bb(bbox.x(),bbox.y(),bbox.z());

  //openGLLines.dataVertex << QVector3D(c.x()-0.5*bb.x(),0,0) << QVector3D(c.x()+0.5*bb.x(),0,0);
  //openGLLines.dataVertex << QVector3D(0,c.y()-0.5*bb.y(),0) << QVector3D(0,c.y()+0.5*bb.y(),0);
  //openGLLines.dataVertex << QVector3D(0,0,c.z()-0.5*bb.z()) << QVector3D(0,0,c.z()+0.5*bb.z());
  openGLLines.dataVertex << QVector3D(-0.5*bb.x(),0,0)+c << QVector3D(0.5*bb.x(),0,0)+c;
  openGLLines.dataVertex << QVector3D(0,-0.5*bb.y(),0)+c << QVector3D(0,0.5*bb.y(),0)+c;
  openGLLines.dataVertex << QVector3D(0,0,-0.5*bb.z())+c << QVector3D(0,0,0.5*bb.z())+c;
  openGLLines.dataColor << QVector4D(1.0,0.0,0.0,1.0) << QVector4D(1.0,0.0,0.0,1.0) << QVector4D(0.0,1.0,0.0,1.0) << QVector4D(0.0,1.0,0.0,1.0) << QVector4D(0.0,0.0,1.0,1.0) << QVector4D(0.0,0.0,1.0,1.0);
  openGLLines.dataIndex << 0 << 1 << 2 << 3 << 4 << 5;
}
void MolData::syncOpenGLLabels(TextData & openGLTexts) {

  openGLTexts.clear();

  for(int i = 0; i < m_positions.size(); i++) {
      const ElementData::ElementData::ElementInfo & ei = getElementInfo(i);
      float r = ei.vanDerWaalsRadio*0.2;
      QVector3D p(m_positions[i].x(),m_positions[i].y(),m_positions[i].z());
      QVector4D c(float(ei.cpkColor[0]),float(ei.cpkColor[1]),float(ei.cpkColor[2]),1.0);

      int count = 0;
      if(i == 0|| i == m_positions.size()-1)
       count = openGLTexts.charTextureAtlas.addVerexTexCoords(openGLTexts.dataVertexTexCoord,QString(ei.symbol)+QString::number(i+1),1,1);
      else count = openGLTexts.charTextureAtlas.addVerexTexCoords(openGLTexts.dataVertexTexCoord,QString(ei.symbol)+QString::number(i+1),1,1);
      //int count = openGLTexts.charTextureAtlas.addVerexTexCoords(openGLTexts.dataVertexTexCoord,QString::number(i+1),0.5,0.5);

      //qDebug() << count;
      for(int k = 0; k < count;k++)
        openGLTexts.dataPosition << p;

    }



}













StructuralControl::StructuralControl(QObject *parent) : QObject(parent)
{
}

bool StructuralControl::open(const std::string & filePath, std::string ext) {
  QMutexLocker locker(&mutex);

  //if(!QFile::exists(filePath)) return false;

  if(ext == "") {
      size_t i = filePath.rfind('.', filePath.length());
      if (i != std::string::npos)  ext = filePath.substr(i+1, filePath.length() - i);
    }

  if (ext == "edi") {
      std::ifstream is(filePath.c_str());
      if(!is.is_open()) return false;

      if(Molecule::loadEDI(sequence.createRowWithColumn(),is)) {
          sequence.currentColumn().data().fileInfo.setFile(filePath.c_str());
          emit changed();
          while (Molecule::loadEDI(sequence.createRow(),is)) { emit changed(); QCoreApplication::processEvents( QEventLoop::AllEvents, 100 ); }
          sequence.removeCurrentRow();
      } else {
          sequence.removeCurrentRow();
          return false;
      }
      is.close();

    } else if (ext == "xyz" || ext == "ani") {
      std::ifstream is(filePath.c_str());
      if(!is.is_open()) return false;

      if(Molecule::loadxyz(sequence.createRowWithColumn(),is)) {
          sequence.currentColumn().data().fileInfo.setFile(filePath.c_str());
          emit changed();
          int loadcount = 0;
          while (Molecule::loadxyz(sequence.createRow(),is)) { emit changed(); QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
              // if (!((++loadcount)%10)) { emit changed(); { QTime dieTime = QTime::currentTime().addMSecs( 1 ); while( QTime::currentTime() < dieTime ) { QCoreApplication::processEvents( QEventLoop::AllEvents, 100 ); } } }
          }

          sequence.removeCurrentRow();
      } else {
          sequence.removeCurrentRow();
          return false;
      }
      is.close();
    } else {
      openBabel.SetIOFormatFromExtension(filePath,filePath);
      if(openBabel.ReadFile(&sequence.createRowWithColumn(),filePath.c_str())) {
          sequence.currentColumn().data().fileInfo.setFile(filePath.c_str());
          while (openBabel.Read(&sequence.createRow())) {}
          sequence.removeCurrentRow();
        } else {
          sequence.removeCurrentRow();
          return false;
        }
    }

    if(sequence.isEmpty()) return false;
    sequence.currentColumn().data().isSaved = true;

    emit changed();
    return true;
}
bool StructuralControl::save(const std::string & filePath, std::string ext) {

  if(sequence.isEmpty()) return false;

  if(ext == "") {
      size_t i = filePath.rfind('.', filePath.length());
      if (i != std::string::npos)  ext = filePath.substr(i+1, filePath.length() - i);
    }

  if (ext == "edi") {
      std::ofstream os(filePath.c_str());

      if(!os.is_open()) return false;

      auto & column = sequence.currentColumn();
      for(int r = 0; r < column.numRows(); r++)
        Molecule::writeEDI(column.at(r),os);
      os.close();

    } else if (ext == "xyz" || ext == "ani") {
      std::ofstream os(filePath.c_str());

      if(!os.is_open()) return false;

      auto & column = sequence.currentColumn();
      for(int r = 0; r < column.numRows(); r++)
        Molecule::writexyz(column.at(r),os);
      os.close();
    } else {

      openBabel.SetIOFormatFromExtension(filePath,filePath);

      std::ofstream os(filePath.c_str());

      if(!os.is_open()) return false;

      auto & column = sequence.currentColumn();
      for(int r = 0; r < column.numRows(); r++) {
        openBabel.Write(&column.at(r),&os);
        }
      os.close();

    }

    sequence.currentColumn().data().fileInfo.setFile(filePath.c_str());
    sequence.currentColumn().data().isSaved = true;

    emit changed();
    return true;
}

bool StructuralControl::write(const std::string & filePath) {
  if(sequence.isEmpty()) return false;

  std::ofstream os(filePath.c_str());
  if(!os.is_open()) return false;

  MolData & mol = sequence.currentFrame();
  bool result = Molecule::writeEDI(mol,os);

  os.close();
  return result;
}
bool StructuralControl::writexyz(const std::string & filePath) {
  if(sequence.isEmpty()) return false;

  std::ofstream os(filePath.c_str());
  if(!os.is_open()) return false;

  MolData & mol = sequence.currentFrame();
  bool result = Molecule::writexyz(mol,os);

  os.close();
  return result;
}

bool StructuralControl::readxyz(const std::string & filePath) {
  if(sequence.isEmpty()) return false;

  std::ifstream is(filePath.c_str());
  if(!is.is_open()) return false;

  MolData & mol = sequence.currentFrame();
  bool result = Molecule::readxyz(mol,is);

  is.close();
  emit changed();
  return result;
}
bool StructuralControl::loadxyz(const std::string & filePath) {
  if(sequence.isEmpty()) return false;

  std::ifstream is(filePath.c_str());
  if(!is.is_open()) return false;

  MolData & mol = sequence.currentFrame();
  bool result = Molecule::loadxyz(mol,is);

  is.close();
  emit changed();
  return result;
}
bool StructuralControl::load(const std::string & filePath) {
  if(sequence.isEmpty()) return false;

  std::ifstream is(filePath.c_str());
  if(!is.is_open()) return false;

  MolData & mol = sequence.currentFrame();
  bool result = Molecule::loadEDI(mol,is);

  is.close();
  emit changed();
  return result;
}
bool StructuralControl::autolink(double factor,double maxd,double mind) {

  if(sequence.isEmpty()) return false;
  MolData & mol = sequence.currentFrame();
  mol.autoLink(factor,maxd,mind);
  emit changed();

  return true;

}

bool StructuralControl::stepforward() {
  sequence.moveForward();
  emit changed();
  return true;
}
bool StructuralControl::stepbackward() {
  sequence.moveBackward();
  emit changed();
  return true;
}
bool StructuralControl::newframe() {
  sequence.createRow();
  emit changed();
  return true;
}
bool StructuralControl::newfile() {
  sequence.createRowWithColumn();
  emit changed();
  return true;
}
bool StructuralControl::duplicateframe() {
  if(sequence.isEmpty()) return false;

  MolData & mol = sequence.currentFrame();
  sequence.createRow() = mol;
  emit changed();
  return true;
}
bool StructuralControl::duplicateframeasfile() {
  if(sequence.isEmpty()) return false;

  MolData & mol = sequence.currentFrame();
  sequence.createRowWithColumn() = mol;
  emit changed();
  return true;
}
bool StructuralControl::translatealltocenter() {
  if(sequence.isEmpty()) return false;

  MolData & mol = sequence.currentFrame();
  bool result = mol.translateAllToCenter();
  emit changed();
  return result;
}

bool StructuralControl::translateselected(double x,double y,double z) {
  if(sequence.isEmpty()) return false;

  MolData & mol = sequence.currentFrame();
  bool result = mol.translate(Eigen::Vector3d(x,y,z),1);
  emit changed();
  return result;
}

bool StructuralControl::sortselected(double x,double y,double z) {
  if(sequence.isEmpty()) return false;

  MolData & mol = sequence.currentFrame();
  bool result = mol.sort(Eigen::Vector3d(x,y,z),1);
  emit changed();
  return result;
}
bool StructuralControl::groupselectedtobeginning() {
  if(sequence.isEmpty()) return false;

  MolData & mol = sequence.currentFrame();
  bool result = mol.groupToBegining(1);
  emit changed();
  return result;
}

bool StructuralControl::autoalignselected(char aa[3]) {
  if(sequence.isEmpty()) return false;

  MolData & mol = sequence.currentFrame();
  bool result = mol.autoAlign(aa,1);
  emit changed();
  return result;
}

bool StructuralControl::rotateselected(double x,double y,double z,double angle,double xo,double yo,double zo) {
  if(sequence.isEmpty()) return false;

  MolData & mol = sequence.currentFrame();
  bool result = mol.rotateAround(Eigen::Vector3d(x,y,z),angle,Eigen::Vector3d(xo,yo,zo),1);
  emit changed();
  return result;
}
bool StructuralControl::setunitcell(double x1,double y1,double z1,double x2,double y2,double z2,double x3,double y3,double z3) {
  if(sequence.isEmpty()) return false;

  MolData & mol = sequence.currentFrame();
  bool result = mol.setUnitCell(Eigen::Vector3d(x1,y1,z1),Eigen::Vector3d(x2,y2,z2),Eigen::Vector3d(x3,y3,z3));
  emit changed();
  return result;
}
bool StructuralControl::guessunitcell(double x,double y,double z) {
  if(sequence.isEmpty()) return false;

  MolData & mol = sequence.currentFrame();
  bool result = mol.setUnitCellGuess(x,y,z);
  emit changed();
  return result;
}
bool StructuralControl::repeatcell(int x,int y,int z) {
  if(sequence.isEmpty()) return false;

  MolData & mol = sequence.currentFrame();
  bool result = mol.repeateLattice(x,y,z);
  emit changed();
  return result;
}






