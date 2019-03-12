#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMimeData>
#include <QDropEvent>
#include <QDateTime>
#include <QDragEnterEvent>
#include <QFont>
#include <QDir>

#include <control/control.h>
#include <command/actionrun.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    Control control;

    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

    void setCurrentTitle();

    void loadCommand(QString configfile);
    void loadDisplay(QString configfile);




private:
    Ui::MainWindow *ui;

public slots:
    void fileCreate();
    void fileOpen();
    void fileSaveAs();
    void fileSave();
    void fileClose();
    void controlChanged() ;
    void showAbout();
    void actionRun(QString command);

    void toolErase();
    void toolCopy();

    void toolDuplicate();
    void toolRemove() ;
    void toolNext();
    void toolPrevious();

    void toolCaptureImage();

    void toolCenter();
    void toolPush();
    void toolPop();

    void showError(QString msg);
    void showMessage(QString msg);
};

#endif // MAINWINDOW_H
