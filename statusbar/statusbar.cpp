#include "statusbar/statusbar.h"
#include "ui_mainwindow.h"

#include <QDir>
StatusBar::StatusBar(QWidget *parent) : QStatusBar(parent)
{

}


StatusBar::~StatusBar()
{

}

void StatusBar::setupUi(Ui::MainWindow * ui) {

  this->ui = ui;
  ui->statusBar->addPermanentWidget(ui->statusBarInfo_FileLocation,1);

  ui->statusBar->addPermanentWidget(ui->widgetStatus,0);
  ui->statusBar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
  ui->statusBarInfo_FileLocation->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

  QPalette p = palette();
  p.setBrush(ui->statusBarInfo_FileLocation->backgroundRole(), QBrush(Qt::transparent));
  ui->statusBarInfo_FileLocation->setPalette(p);
  ui->statusBarInfo_AtomNumbers->setPalette(p);
  ui->statusBarInfo_FrameInfo->setPalette(p);

  ui->statusBarInfo_FileLocation->setText(QDir::currentPath());
  ui->statusBarInfo_AtomNumbers->setText("0");
  ui->statusBarInfo_FrameInfo->setText("0:0/0:0/0(0)");

}

void StatusBar::setControl(Control * control) {
  this->control = control;
}

void StatusBar::controlChanged() {
  if(control->sequence.isEmpty()) {
      ui->statusBarInfo_FileLocation->setText(QDir::currentPath());
      ui->statusBarInfo_AtomNumbers->setText("0");
      ui->statusBarInfo_FrameInfo->setText("0:0/0:0/0("+QString::number(control->stack.size())+")");
    } else {
      ui->statusBarInfo_FileLocation->setText(control->sequence.currentColumn().data().fileInfo.absoluteDir().path());
      ui->statusBarInfo_AtomNumbers->setText(QString::number(control->sequence.currentFrame().size()));
      QString n1 = QString::number(control->sequence.numColumns());
      QString n2 = QString::number(control->sequence.currentColumn().numRows());

      QString c1 = QString::number(control->sequence.currentColumnIndex()+1);
      QString c2 = QString::number(control->sequence.currentRowIndex()+1);
      QString c3 = QString::number(control->sequence.currentFrameIndex()+1);
      QString c4 = QString::number(control->stack.size());
      ui->statusBarInfo_FrameInfo->setText(n1+":"+c1+"/"+n2+":"+c2+"/"+c3+"("+c4+")");
    }

}
