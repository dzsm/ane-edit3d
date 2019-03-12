#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QWidget>
#include <QStatusBar>
#include <control/control.h>

namespace Ui {
  class MainWindow;
}
class StatusBar : public QStatusBar
{
  Q_OBJECT
private:
  Ui::MainWindow * ui;
public:
  //explicit StatusBar(QWidget *parent = 0); what is explicit?
  StatusBar(QWidget *parent = 0);
  ~StatusBar();

  void setupUi(Ui::MainWindow * ui);
  void setControl(Control * control);

private:
  Control * control;
public slots:
  void controlChanged();

};

#endif // STATUSBAR_H
