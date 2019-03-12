#include "command/commandcontrol.h"
#include "ui_mainwindow.h"


CommandControl::CommandControl(QWidget *parent) : QWidget(parent)
{
}

CommandControl::~CommandControl()
{
}

void CommandControl::setupUi(Ui::MainWindow * ui) {

  this->ui = ui;
    //connect(ui->lineEditCommandControlLine,SIGNAL(returnPressed()),this,SLOT(run()));
    connect(ui->toolButtonCommandControlLineEnter,SIGNAL(clicked(bool)),this,SLOT(run()));
}

void CommandControl::setControl(Control * control) {
  this->control = control;
}

void CommandControl::controlChanged() {


}

void CommandControl::run() {

    if(commandHistory.isEmpty()) {
        commandHistory.push_back(ui->lineEditCommandControlLine->text());
    } else if (commandHistory.back() != ui->lineEditCommandControlLine->text()) {
        commandHistory.push_back(ui->lineEditCommandControlLine->text());
    }

    control->showMsg(ui->lineEditCommandControlLine->text().toStdString());

    QString program = ui->lineEditCommandControlLine->text().remove(QRegExp("^\\[(.+)\\]"));

  //QStringList commandList = ui->lineEditCommandControlLine->text().split(";");
  ui->lineEditCommandControlLine->setDisabled(true);
  ui->toolButtonCommandControlLineEnter->setDisabled(true);

  ////control->engine.evaluate(program);

  ui->lineEditCommandControlLine->setDisabled(false);
  //ui->lineEditCommandControlLine->setFocus();
  ui->toolButtonCommandControlLineEnter->setDisabled(false);

}

