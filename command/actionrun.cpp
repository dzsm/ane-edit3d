#include "actionrun.h"



ActionRun::ActionRun(QWidget * parent) : QAction (parent) {
  connect(this,SIGNAL(triggered(bool)),this,SLOT(triggerCommand()));
}

void ActionRun::triggerCommand() {
  emit triggeredCommand(command);
}

/*
//#include <QDebug>
void CommandControl::loadCommands() {


  QList<QAction*> actionList;
  ActionRun * action;

  QStringList alist; // = {QString("[Avogadro] read.xyz"),QString("[Avogadro2] read.xyz")};

  //qDebug() << QCoreApplication::applicationDirPath()+"/commands.config";
  QFile inputFile(QCoreApplication::applicationDirPath()+"/commands.config");
  if (inputFile.open(QIODevice::ReadOnly))
  {
     QTextStream in(&inputFile);
     while (!in.atEnd())
     {
        QString line = in.readLine();
        alist.push_back(line);
     }
     inputFile.close();
  }

  for(QString command : alist)  {

      QRegExp exp("^\\[(.+)\\].*");
      if(exp.indexIn(command.trimmed())>=0) {
          QString name = exp.capturedTexts()[1];

          action = new ActionRun(this);
          action->setText(name);
          action->command = command;
          connect(action,SIGNAL(triggeredCommand(QString)),this,SLOT(actionRun(QString)));
          actionList.push_back(action);

      }

    }

  ui->menuCommands->addActions(actionList);

}

void CommandControl::actionRun(QString command) {
  ui->lineEditCommandControlLine->setText(command);
  run(true);
}

void CommandControl::addActionRun() {

  QList<QAction*> actionList;
  ActionRun * action = new ActionRun(this);

  QString command = ui->lineEditCommandControlLine->text();

  QRegExp exp("^\\[(.+)\\].*");
  if(exp.indexIn(command.trimmed())>=0) {
      QString name = exp.capturedTexts()[1];

      action = new ActionRun(this);
      action->setText(name);
      action->command = command;
      connect(action,SIGNAL(triggeredCommand(QString)),this,SLOT(actionRun(QString)));
      actionList.push_back(action);

  }
  ui->menuCommands->addActions(actionList);


}
*/
