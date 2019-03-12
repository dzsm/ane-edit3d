#ifndef COMMANDCONTROL_H
#define COMMANDCONTROL_H
#include <control/control.h>

#include <QWidget>
#include <QTemporaryFile>
#include <QProcess>
#include <QProcessEnvironment>
#include <QMap>
#include <QErrorMessage>
#include <QMessageBox>
#include <QStringList>
#include <QAction>

namespace Ui {
  class MainWindow;
}


class CommandControl : public QWidget
{
  Q_OBJECT
private:
  Ui::MainWindow * ui;
public:
  CommandControl(QWidget *parent = 0);
  ~CommandControl();

  void setupUi(Ui::MainWindow * ui);
  void setControl(Control * control);

private:
  Control * control;

  QStringList commandHistory;

public slots:
  void controlChanged();
  void run();
};

#endif // COMMANDCONTROL_H
