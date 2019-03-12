#ifndef CONTROL_H
#define CONTROL_H

#include <core/core.h>
#include <control/moleculedata.h>

#include <QObject>
#include <QFileInfo>
#include <QProcess>
#include <QMessageBox>
//#include <QtScript/QtScript>

struct FileData {
  QFileInfo    fileInfo;
  bool         isSaved;
  FileData() {};
};

class Control : public QObject
{
    Q_OBJECT
public:
    explicit Control(QObject *parent = 0);

    ///

    void broadcastChange() ;

    void wait() ;
    void errorMsg(std::string) ;
    void showMsg(std::string msg) ;
    std::mutex                                  mutex;
    FrameNavigation::Sequence<MoleculeData,FileData> sequence;

    std::map<std::string,std::string> settings;

    std::vector<MoleculeData> stack;

    //QScriptEngine engine;

    Q_INVOKABLE QVariant script_center(unsigned char bit);
    Q_INVOKABLE QVariant script_boundingbox(unsigned char bit);
    Q_INVOKABLE QVariant script_cell();
    Q_INVOKABLE QVariant script_coord(int id);
    Q_INVOKABLE QVariant script_picks();
    Q_INVOKABLE QVariant script_atom(int id);
    Q_INVOKABLE QVariant script_count(unsigned char bit);

    Q_INVOKABLE void script_add(int atomicNumber,double x, double y,double z,unsigned char bit);

    Q_INVOKABLE void script_write(unsigned char mask,QString qfilePath);
    Q_INVOKABLE void script_read(unsigned char mask, unsigned char flip,QString qfilePath);

    Q_INVOKABLE void script_filecreate(QString qfilename);
    Q_INVOKABLE void script_fileduplicate(QString qfilename);
    Q_INVOKABLE void script_fileclose();
    Q_INVOKABLE void script_filesave();
    Q_INVOKABLE void script_filesave(QString qfilePath);
    Q_INVOKABLE void script_fileopen(QString qfilePath);

    Q_INVOKABLE void script_group(unsigned char maskWrite, unsigned char flip, const std::vector<int> & atomIds); /// does vector works with script, maybe QVector??
    Q_INVOKABLE void script_group(unsigned char maskWrite, unsigned char flip, unsigned char maskRead) ;
    Q_INVOKABLE void script_pick(int atomId);
    //bool pick(std::string smart);
    Q_INVOKABLE void script_pick();
    Q_INVOKABLE void script_distance(int atomId1,int atomId2);
    Q_INVOKABLE void script_distance();
    Q_INVOKABLE void script_translate(unsigned char mask,double x,double y,double z);
    Q_INVOKABLE void script_rotate(unsigned char mask,double ax,double ay,double az,double angleRad, double ox,double oy,double oz);
    Q_INVOKABLE void script_rotateto(unsigned char mask, double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3);
    Q_INVOKABLE void script_alignalong(unsigned char mask, double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3);
    Q_INVOKABLE void script_autoalign(unsigned char mask, QString qxyz);
    Q_INVOKABLE void script_erase(unsigned char mask);
    Q_INVOKABLE void script_copy(unsigned char maskRead, unsigned char maskWrite, unsigned char flip);
    Q_INVOKABLE void script_repeatcell(int n1,int n2, int n3);
    Q_INVOKABLE void script_sort(unsigned char mask,double x,double y,double z);
    Q_INVOKABLE void script_gather(unsigned char mask);
    Q_INVOKABLE void script_cell(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3);

    Q_INVOKABLE void script_neighbourByGroup(unsigned char maskRead, unsigned char maskWrite, unsigned char flip);
    Q_INVOKABLE void script_neighbourByPick(int id, unsigned char maskWrite, unsigned char flip);

    Q_INVOKABLE void script_pick(unsigned char mask,int ith);
    Q_INVOKABLE void script_autolink(unsigned char mask,double factor,double maxd,double mind);
    Q_INVOKABLE void script_nearby(unsigned char mask, double x, double y, double z,double maxd,double mind, unsigned char maskWrite, unsigned char flip);
    Q_INVOKABLE void script_find(unsigned char mask,QString qsym, unsigned char maskWrite, unsigned char flip);
    Q_INVOKABLE void script_create();
    Q_INVOKABLE void script_duplicate();
    Q_INVOKABLE void script_remove();
    //bool move();
    Q_INVOKABLE void script_next();
    Q_INVOKABLE void script_previous();
    //bool first();
    //bool last();

    Q_INVOKABLE void script_push(unsigned char mask);
    Q_INVOKABLE void script_pop(unsigned char maskWrite, unsigned char flip);


    Q_INVOKABLE void script_settings(QString key,QString value);

    Q_INVOKABLE void script_execute(QString command);
    Q_INVOKABLE void script_launch(QString command);

    ///

signals:
    void changed();
    void error(QString);
    void message(QString);

public slots:
};

#endif // CONTROL_H
