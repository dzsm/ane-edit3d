#include "moleculedata.h"
#include <bitset>
MoleculeData::MoleculeData() : Molecule::Molecule()
{

}

void MoleculeData::syncOpenGLAtoms(SphereData & openGLAtoms) {
  openGLAtoms.clear();

  for(int i = 0; i < m_positions.size(); i++) {
      float r = Element::vdwRadius[m_atoms[i]]*0.2;
      QVector3D p(m_positions[i].x(),m_positions[i].y(),m_positions[i].z());
      const float *c4 = Element::color[m_atoms[i]];
      QVector4D c(c4[0],c4[1],c4[2],c4[3]);

      QVector4D c2(1.0f-c4[0],1.0-c4[1],1.0-c4[2],c4[3]);

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
void MoleculeData::syncOpenGLHalos(SphereData & openGLHalos) {
  openGLHalos.clear();

  int count = 0;
  for(int i = 0; i < m_positions.size(); i++) if(m_group[i]&static_cast<unsigned char>(1)==static_cast<unsigned char>(1)) {
      float r = Element::vdwRadius[m_atoms[i]]*0.2;
      QVector3D p(m_positions[i].x(),m_positions[i].y(),m_positions[i].z());

      const float *c4 = Element::color[m_atoms[i]];
      QVector4D c;
      if (c4[0]+c4[1]+c4[2]<1.5) c = QVector4D(0.8,0.8,0.8,0.7); else c = QVector4D(0.2,0.2,0.2,0.7);

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
void MoleculeData::syncOpenGLPicks(SphereData & openGLPicks){
  openGLPicks.clear();

  QVector4D pickcolors[4] = {QVector4D(1.0,0.0,0.0,1.0),
                             QVector4D(0.0,1.0,0.0,1.0),
                             QVector4D(0.0,0.0,1.0,1.0),
                             QVector4D(1.0,1.0,0.0,1.0)};
  int count = 0;
  for(int j = 0; j < m_picks.size(); j++) {

      int i = m_picks[j];
      float r = Element::vdwRadius[m_atoms[i]]*0.2*1.3;

      QVector3D p(m_positions[i].x(),m_positions[i].y(),m_positions[i].z());
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


void MoleculeData::syncOpenGLBonds(CylinderData & openGLBonds) {
  openGLBonds.clear();

  GLuint indexCounter = 0;
  for(int i = 0; i < m_links.size(); i++) {

      int a = std::get<0>(m_links[i]);
      int b = std::get<1>(m_links[i]);
      char o = m_orders[i];

      float r = 0.15;

      QVector3D va(m_positions[a].x(),m_positions[a].y(),m_positions[a].z());
      QVector3D vb(m_positions[b].x(),m_positions[b].y(),m_positions[b].z());
      QVector3D vm = 0.5*(va+vb);

      const float *ca4 = Element::color[m_atoms[a]];
      QVector4D ca(ca4[0],ca4[1],ca4[2],ca4[3]);
      const float *cb4 = Element::color[m_atoms[b]];
      QVector4D cb(cb4[0],cb4[1],cb4[2],cb4[3]);

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
void MoleculeData::syncOpenGLWires(LineData & openGLLines) {
  openGLLines.clear();

  GLuint indexCounter = 0;
  for(int i = 0; i < m_links.size(); i++) {

      int a = std::get<0>(m_links[i]);
      int b = std::get<1>(m_links[i]);
      char o = m_orders[i];

      float r = 0.15;

      QVector3D va(m_positions[a].x(),m_positions[a].y(),m_positions[a].z());
      QVector3D vb(m_positions[b].x(),m_positions[b].y(),m_positions[b].z());
      QVector3D vm = 0.5*(va+vb);

      const float *ca4 = Element::color[m_atoms[a]];
      QVector4D ca(ca4[0],ca4[1],ca4[2],ca4[3]);
      const float *cb4 = Element::color[m_atoms[b]];
      QVector4D cb(cb4[0],cb4[1],cb4[2],cb4[3]);

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
void MoleculeData::syncOpenGLMeasureLines(LineData & openGLLines,TextData & openGLTexts) {
  openGLLines.clear();
  openGLTexts.clear();

  GLuint indexCounter = 0;
  for(auto it = m_distances.begin(); it != m_distances.end(); ++it) {

      int a = std::get<0>(*it);
      int b = std::get<1>(*it);

      QVector3D va(m_positions[a].x(),m_positions[a].y(),m_positions[a].z());
      QVector3D vb(m_positions[b].x(),m_positions[b].y(),m_positions[b].z());

      QVector4D ca(0,0,0,1.0);

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
void MoleculeData::syncOpenGLUnitCell(LineData & openGLLines,TextData & openGLTexts) {
  openGLLines.clear();
  openGLTexts.clear();

  if(m_hasCell == false) return;

  Eigen::Vector3d cmi,cma;
  getBoundingBox(255,cmi,cma);

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
void MoleculeData::syncOpenGLCenterAxes(LineData & openGLLines) {
  openGLLines.clear();

  const Eigen::Vector3d & center = getCenter(255);
  QVector3D c(center.x(),center.y(),center.z());
  Eigen::Vector3d cmi,cma;
  const Eigen::Vector3d & bbox = getBoundingBox(255,cmi,cma);
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
void MoleculeData::syncOpenGLLabels(TextData & openGLTexts, int mode) {

  openGLTexts.clear();

  for(int i = 0; i < m_positions.size(); i++) {
      float r = Element::vdwRadius[m_atoms[i]]*0.2*1.25;
      QVector3D p(m_positions[i].x(),m_positions[i].y(),m_positions[i].z());
      const float *c4 = Element::color[m_atoms[i]];
      QVector4D c(c4[0],c4[1],c4[2],c4[3]);

      QString sym = Element::symbol[m_atoms[i]];

      std::bitset<8> bs = m_group[i];
      int count = 0;
      if(mode == 0) count = openGLTexts.charTextureAtlas.addVerexTexCoords(openGLTexts.dataVertexTexCoord,sym,1,1);
      else if(mode == 1) count = openGLTexts.charTextureAtlas.addVerexTexCoords(openGLTexts.dataVertexTexCoord,QString::number(i+1),1,1);
      else if(mode == 2) count = openGLTexts.charTextureAtlas.addVerexTexCoords(openGLTexts.dataVertexTexCoord,QString::number(m_group[i]),1,1);
      else if(mode == 3) count = openGLTexts.charTextureAtlas.addVerexTexCoords(openGLTexts.dataVertexTexCoord,QString(bs.to_string().c_str()),1,1);
      //else count = openGLTexts.charTextureAtlas.addVerexTexCoords(openGLTexts.dataVertexTexCoord,QString::number(i+1),1,1);

      for(int k = 0; k < count;k++) openGLTexts.dataPosition << p;
    }

}





