#ifndef FRAMESLIDER_H
#define FRAMESLIDER_H

#include <QObject>
#include <QWidget>
#include <QFrame>
#include <control/control.h>

namespace Ui {
  class MainWindow;
}

class FrameSlider : public QWidget
{
  Q_OBJECT
private:
  Ui::MainWindow * ui;
public:
  FrameSlider(QWidget *parent = 0);
  ~FrameSlider();

  void setupUi(Ui::MainWindow * ui);
  void setControl(Control * control);

private:
  Control * control;
public slots:
  void controlChanged();
  void slided(int value);

  //void forwardPlay();
  //void backwardPlay();
  //void fastForwardPlay();
  //void fastBackwardPlay() ;
};

#endif // FRAMESLIDER_H
