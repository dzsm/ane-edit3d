#include <slider/frameslider.h>
#include "ui_mainwindow.h"


FrameSlider::FrameSlider(QWidget *parent) : QWidget(parent)
{

}

FrameSlider::~FrameSlider()
{

}

void FrameSlider::setupUi(Ui::MainWindow * ui) {

  this->ui = ui;
  ui->horizontalSliderFrame->setMinimum(1);
  ui->horizontalSliderFrame->setMaximum(0);
  //ui->widgetSliderFrame->setStyleSheet("background-color: rgb(0, 85, 127);");
  connect(ui->horizontalSliderFrame,SIGNAL(valueChanged(int)),this,SLOT(slided(int)));

  //connect(ui->toolButtonLeftPlay,SIGNAL(pressed()),this,SLOT(backwardPlay()));
  //connect(ui->toolButtonRightPlay,SIGNAL(pressed()),this,SLOT(forwardPlay()));
  //connect(ui->toolButtonLeftFast,SIGNAL(pressed()),this,SLOT(fastBackwardPlay()));
  //connect(ui->toolButtonRightFast,SIGNAL(pressed()),this,SLOT(fastForwardPlay()));
}

void FrameSlider::setControl(Control * control) {
  this->control = control;
}

void FrameSlider::controlChanged() {

  //this->set
  ui->horizontalSliderFrame->blockSignals(true);
  ui->horizontalSliderFrame->setMinimum(1);
  ui->horizontalSliderFrame->setMaximum(control->sequence.size());
  ui->horizontalSliderFrame->setPageStep(10);
  ui->horizontalSliderFrame->setTickInterval(std::max(control->sequence.size()/100,1));
  ui->horizontalSliderFrame->setValue(control->sequence.currentFrameIndex()+1);
  ui->horizontalSliderFrame->blockSignals(false);
}

void FrameSlider::slided(int value) {

  if(control->mutex.try_lock()) {
      control->sequence.moveFrameIndex(value-1);
      control->mutex.unlock();
    }

 control->broadcastChange();
}

/*
void FrameSlider::forwardPlay() {
  if(pStructuralControl->mutex.tryLock()) {
      pStructuralControl->sequence.moveForward();
      pStructuralControl->mutex.unlock();
    }

  emit pStructuralControl->changed();
}

void FrameSlider::backwardPlay() {
  if(pStructuralControl->mutex.tryLock()) {
      pStructuralControl->sequence.moveBackward();
      pStructuralControl->mutex.unlock();
    }

  emit pStructuralControl->changed();
}

void FrameSlider::fastForwardPlay() {
  if(pStructuralControl->mutex.tryLock()) {
      pStructuralControl->sequence.moveNextColumn();
      pStructuralControl->mutex.unlock();
    }

  emit pStructuralControl->changed();
}

void FrameSlider::fastBackwardPlay() {
  if(pStructuralControl->mutex.tryLock()) {
      pStructuralControl->sequence.movePreviousColumn();
      pStructuralControl->mutex.unlock();
    }

  emit pStructuralControl->changed();
}
*/
