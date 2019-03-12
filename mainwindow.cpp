#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->menuView->addAction(ui->dockWidget->toggleViewAction());
    ui->dockWidget->setVisible(false);

    ui->openGLWidget->setControl(&control);

    setCurrentTitle();

    ui->widget->setupUi(ui);
    ui->widget->setControl(&control);

    ui->widget_2->setupUi(ui);
    ui->widget_2->setControl(&control);

    ui->statusBar->setupUi(ui);
    ui->statusBar->setControl(&control);

    connect(&control,SIGNAL(changed()),ui->openGLWidget,SLOT(controlChanged()));;
    connect(&control,SIGNAL(changed()),ui->widget,SLOT(controlChanged()));;
    connect(&control,SIGNAL(changed()),ui->widget_2,SLOT(controlChanged()));;
    connect(&control,SIGNAL(changed()),this,SLOT(controlChanged()));;
    connect(&control,SIGNAL(changed()),ui->statusBar,SLOT(controlChanged()));

    connect(&control,SIGNAL(error(QString)),this,SLOT(showError(QString)));
    connect(&control,SIGNAL(message(QString)),this,SLOT(showMessage(QString)));
    connect(&control,SIGNAL(error(QString)),this,SLOT(showMessage(QString)));

    loadCommand(":/scripts/commands.js");
    loadDisplay(":/scripts/displays.js");

    //loadCommand(QCoreApplication::applicationDirPath()+"/configs/command.config");
    //loadDisplay(QCoreApplication::applicationDirPath()+"/configs/display.config");

    //loadCommand("C:\\Users\\manrique\\Dropbox\\Developing\\testCodes\\Edit3D\\configs\\command.config");
    //loadDisplay("C:\\Users\\manrique\\Dropbox\\Developing\\testCodes\\Edit3D\\configs\\display.config");

    //loadCommand("/Users/david/Dropbox/Developing/testCodes/Edit3D/configs/command.config");
    //loadDisplay("/Users/david/Dropbox/Developing/testCodes/Edit3D/configs/display.config");

    control.settings["atoms"] = "ON";
    control.settings["axeslines"] = "ON";
    control.settings["measurelines"] = "ON";
    control.settings["pickhalos"] = "ON";
    control.settings["halos"] = "ON";
    control.settings["labels"] = "SYMBOL";
    control.settings["measurelabels"] = "ON";
    control.settings["bonds"] = "ON";
    control.settings["lines"] = "ON";
    control.settings["halos"] = "ON";
    control.settings["cell"] = "ON";
    control.settings["celllabels"] = "ON";
    control.settings["help"] = "ON";

    //control.script_fileopen("/Users/david/Dropbox/Developing/testCodes/build-EdiTest2-Desktop_Qt_5_4_1_MinGW_32bit-Debug/py.xyz");

    QDateTime WDT(QDate(2019,9,14));
    if(QDateTime::currentDateTimeUtc() > WDT) {
        QMessageBox::critical(this,tr("Expired"),tr("ANE - Atomic Nanostructure Editor\nVersion beta 0.1\nThe testing period is ended on ")+WDT.toString("dd/MM/yyyy")+tr("!\nThis version is no longer working.\nPlease download the latest upgraded version!\n(c) David Zsolt Manrique"));
        exit(0);
      }

    ////QScriptValue objectValue = control.engine.newQObject(&control, QScriptEngine::QtOwnership, QScriptEngine::ExcludeChildObjects | QScriptEngine::ExcludeSuperClassMethods | QScriptEngine::ExcludeSuperClassProperties);
    ////control.engine.globalObject().setProperty("control", objectValue);

    QFile mFile(":/scripts/defaults.js");
    if(mFile.open(QFile::ReadOnly | QFile::Text)){
        QTextStream in(&mFile);
        ////control.engine.evaluate(in.readAll());
    } else qDebug() << "nofound";
    mFile.close();

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}
void MainWindow::dropEvent(QDropEvent *e)
{
    foreach (const QUrl &url, e->mimeData()->urls()) {
        const QString &fileName = url.toLocalFile();

        if( fileName.isEmpty() ) continue;
        if(!QFileInfo::exists(fileName)) continue;

        control.script_fileopen(fileName);

    }
}

void MainWindow::fileCreate()
{
  QString filename = QFileDialog::getSaveFileName(this, tr("Create"), "noname.edi" );
  if( !filename.isEmpty() ) {

  control.script_filecreate(filename);
  control.script_filesave(filename);
  }

}
void MainWindow::fileOpen()
{
  QString filename = QFileDialog::getOpenFileName( this, tr("Open") );
  if( filename.isEmpty() )
    return;

  control.script_fileopen(filename);
}
void MainWindow::fileSaveAs()
{
  if(control.sequence.isEmpty()) return;
  QString currentFilename = control.sequence.currentColumn().data().fileInfo.canonicalFilePath();
  QString filename = QFileDialog::getSaveFileName( this, tr("Save As"), currentFilename , "EDI (*.edi);; XYZ (*.xyz)" );
  if( filename.isEmpty() )
    return;

  control.script_filesave(filename);
}
void MainWindow::fileSave()
{
  if(control.sequence.isEmpty()) return;
  QString currentFilename = control.sequence.currentColumn().data().fileInfo.canonicalFilePath();

  if (currentFilename.isEmpty()) { fileSaveAs(); return; }

  control.script_filesave(currentFilename);
}
void MainWindow::fileClose()
{
  control.script_fileclose();
}

void MainWindow::toolErase() {
    if(!control.sequence.isEmpty()) {
        control.script_erase(1);
    }
}
void MainWindow::toolCopy() {
    if(!control.sequence.isEmpty()) {
        control.script_copy(1,255,2);
        control.script_group(255,0,1);
        control.script_group(255,1,2);
        double mx,my,mz,Mx,My,Mz;

        QVariantList qvl = control.script_boundingbox(1).value<QVariantList>();

        mx = qvl[0].value<double>();
        my = qvl[1].value<double>();
        mz = qvl[2].value<double>();
        Mx = qvl[3].value<double>();
        My = qvl[4].value<double>();
        Mz = qvl[5].value<double>();

        double d[3] = {Mx-mx,My-my,Mz-mz};
        double mi = d[0];
        for(int i = 0; i < 3; i++) if (mi>d[i]) mi = d[i];
        for(int i = 0; i < 3; i++) if (d[i]!=mi) d[i] = 0.0; else d[i]+=2.0;
        control.script_translate(1,d[0],d[1],d[2]);
    }
}
void MainWindow::toolDuplicate() {
    if(!control.sequence.isEmpty()) {
        control.script_duplicate();
    }
}
void MainWindow::toolRemove() {
    if(!control.sequence.isEmpty()) {
        control.script_remove();
    }
}
void MainWindow::toolNext() {
    if(!control.sequence.isEmpty()) {
        control.script_next();
    }
}
void MainWindow::toolPrevious() {
    if(!control.sequence.isEmpty()) {
        control.script_previous();
    }
}

void MainWindow::toolPush() {
    if(!control.sequence.isEmpty()) {
        control.script_push(1);
    }
}
void MainWindow::toolPop() {
    if(!control.sequence.isEmpty()) {
        control.script_pop(255,1);
    }
}
void MainWindow::toolCaptureImage() {
    QString file = QFileDialog::getSaveFileName(this, "Save as...", "name", "PNG (*.png);; BMP (*.bmp);;TIFF (*.tiff *.tif);; JPEG (*.jpg *.jpeg)");
    ui->openGLWidget->grab().save(file);
}

void MainWindow::toolCenter() {
    // update outside of opengl possible error TODO by outclassing camera / thread safe?
    if(!control.sequence.isEmpty()) {
        if (control.sequence.currentFrame().picks().size()>0) {

            QVariantList qvl = control.script_picks().value<QVariantList>();

            if ( qvl.size() > 0 ) {

                int idx = qvl[0].value<int>();

                QVariantList qvl2 = control.script_coord(idx).value<QVariantList>();

                double x,y,z;

                x = qvl2[0].value<double>();
                y = qvl2[1].value<double>();
                z = qvl2[2].value<double>();

                ui->openGLWidget->camera.setCenter({float(x),float(y),float(z)});
                ui->openGLWidget->update();

            }

        } else {

            QVariantList qvl2 = control.script_center(0).value<QVariantList>();

            double x,y,z;

            x = qvl2[0].value<double>();
            y = qvl2[1].value<double>();
            z = qvl2[2].value<double>();

            if(ui->openGLWidget->camera.m_center.isNull())
            ui->openGLWidget->camera.setCenter({float(x),float(y),float(z)});
            else ui->openGLWidget->camera.setCenter({float(0),float(0),float(0)});

            ui->openGLWidget->update();
        }
    } else {
        ui->openGLWidget->camera.setCenter({float(0),float(0),float(0)});
        ui->openGLWidget->update();
    }
}

void MainWindow::controlChanged() {
    setCurrentTitle();
    if(!control.sequence.isEmpty()) {
        //What if is not valid folder TODO / thread safe?
        QDir::setCurrent(control.sequence.currentColumn().data().fileInfo.absoluteDir().absolutePath());
    }
}

void MainWindow::setCurrentTitle() {
  if(control.sequence.isEmpty()) {
      this->setWindowTitle(tr("ANE - Atomic Nanostructure Editor"));
    } else {

      QString savedMark;
      if(control.sequence.currentColumn().data().isSaved) savedMark = ""; else savedMark = "*";
      this->setWindowTitle(control.sequence.currentColumn().data().fileInfo.fileName()+":"+
                           QString::number(control.sequence.currentRowIndex()+1)+savedMark+" - Atomic Nanostructure Editor");
    }
}
void MainWindow::showAbout()
{
  QMessageBox::about(this,tr("About"),tr("ANE - Atomic Nanostructure Editor\nVersion beta 0.1\nThis is a test version.\nAfter the testing period this version is no longer working.\nPlease download then the newest version!\n(c) David Zsolt Manrique"));
}

void MainWindow::loadCommand(QString configfile) {

  QList<QAction*> actionList;
  ActionRun * action;

  QStringList alist;
  QFile inputFile(configfile);

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

  ui->menuCommand->addActions(actionList);

}
void MainWindow::loadDisplay(QString configfile) {

  QList<QAction*> actionList;
  ActionRun * action;

  QStringList alist;
  QFile inputFile(configfile);

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

  ui->menuDisplay->addActions(actionList);

}
void MainWindow::actionRun(QString command) {
  ui->lineEditCommandControlLine->setText(command);
  ui->widget->run();
}

void MainWindow::showError(QString msg) {
    QMessageBox::critical(this,tr("Error"),msg);
}

void MainWindow::showMessage(QString msg) {
    ui->plainTextEdit->appendPlainText(msg);
}
