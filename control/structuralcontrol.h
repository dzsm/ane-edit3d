#ifndef STRUCTURALCONTROL_H
#define STRUCTURALCONTROL_H

#include <QObject>
#include <QDebug>
#include <QFileInfo>
#include <QMutex>
#include <QMutexLocker>

#include <iomanip>

#include <core/framenavigation.h>
#include <core/molecule.h>
#include <core/settings.h>
#include <ob/ob.h>
#include <glview/geometries.h>

struct FileData {
  QFileInfo    fileInfo;
  bool         isSaved;
  FileData();
};

struct MolData : public Molecule::Molecule {
  bool m_defaultViewRequired;

  MolData() ;

  bool isDefaultViewRequired() const;
  void requireDefaultView(bool require);

  //SphereData   openGLAtoms;
  //SphereData   openGLHalos;
  //CylinderData openGLBonds;

  void syncOpenGLAtoms(SphereData & openGLAtoms);
  void syncOpenGLHalos(SphereData & openGLHalos);
  void syncOpenGLPicks(SphereData & openGLPicks);
  void syncOpenGLBonds(CylinderData & openGLBonds);
  void syncOpenGLWires(LineData & openGLLines);
  void syncOpenGLMeasureLines(LineData & openGLLines,TextData & openGLTexts);
  void syncOpenGLUnitCell(LineData & openGLLines,TextData & openGLTexts);
  void syncOpenGLCenterAxes(LineData & openGLLines);
  void syncOpenGLLabels(TextData & openGLTexts);

};

class StructuralControl : public QObject
{
  Q_OBJECT
public:
  explicit StructuralControl(QObject *parent = 0);

  QMutex mutex;

  OpenBabelWrapper                               openBabel;
  FrameNavigation::Sequence<MolData,FileData>    sequence;
  Settings                                       settings;

  bool open(const std::string & filePath, std::string ext = "");
  bool save(const std::string & filePath, std::string ext = "");

  bool write(const std::string & filePath);
  bool writexyz(const std::string & filePath);

  bool readxyz(const std::string & filePath);
  bool loadxyz(const std::string & filePath);
  bool load(const std::string & filePath);
  bool autolink(double factor = 1.4,double maxd = 4.0,double mind = 0.2);

  bool stepforward();
  bool stepbackward();
  bool newframe();
  bool newfile();
  bool duplicateframe();
  bool duplicateframeasfile();

  bool translatealltocenter();
  bool translateselected(double x,double y,double z);
  bool sortselected(double x,double y,double z) ;
  bool groupselectedtobeginning();
  bool autoalignselected(char aa[3]);
  bool rotateselected(double x,double y,double z,double angle,double ox,double yo,double zo);
  bool setunitcell(double x1,double y1,double z1,double x2,double y2,double z2,double x3,double y3,double z3);
  bool guessunitcell(double x,double y,double z);
  bool repeatcell(int x,int y,int z);

signals:
  void changed();
public slots:
};

#endif // STRUCTURALCONTROL_H
