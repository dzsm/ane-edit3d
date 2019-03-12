#ifndef ACTIONRUN_H
#define ACTIONRUN_H

#include <QTemporaryFile>
#include <QProcess>
#include <QProcessEnvironment>
#include <QMap>
#include <QErrorMessage>
#include <QMessageBox>
#include <QStringList>
#include <QAction>


class ActionRun : public QAction {
  Q_OBJECT
public:
  QString command;

  ActionRun(QWidget * parent  = 0);
public slots:
  void triggerCommand();
signals:
  void triggeredCommand(const QString & command);
};


#endif // ACTIONRUN_H
