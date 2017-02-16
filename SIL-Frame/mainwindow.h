#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <dialognew.h>
#include <dialogdepsil.h>
#include <dialogaddplug.h>
#include <dialogremoveplug.h>
#include <dialogadd.h>
#include <dialogremove.h>
#include <dialogsettings.h>
#include <dialogduppro.h>
#include <dialogdelete.h>
#include <QtCore>
#include <QtGui>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void print_out_info(QString, bool);
    void loadPeerList();
    void addNewPeer(QString, QString, QString, QString, QString);
    void removePeer();
    QString getMiddlewarePath();
    void setMiddlewarePath(QString);
    void addPlugin(QString, QString);
    void loadPluginList();
    void removePlugin();
    void setRSAPath(QString path);
    QString getRSAPath();
    void setSSHpath(QString path);
    QString getSSHpath();
    void setHostName(QString);
    QString getHostName();
    void setPort(QString);
    QString getPort();
    void setSSHUser(QString);
    QString getSSHUser();
    void setProxyState(bool);
    bool getProxyState();

    ~MainWindow();

public slots:


private slots:
    void on_pushButtonNew_clicked();

    void on_pushButtonAdd_clicked();

    void on_pushButtonRemove_clicked();

    void on_pushButtonClear_clicked();

    void on_pushButtonsett_clicked();

    void on_pushButtonAdd_2_clicked();

    void on_pushButtonRemove_2_clicked();

    void on_pushButtonSetup_clicked();

    void on_pushbuttonDep_clicked();

    void on_pushButton_clicked();

    void on_pushButtonDeinst_clicked();

private:
    Ui::MainWindow *ui;
    DialogDepSil * diaDepSil;
    DialogNew * diaNew;
    DialogAdd * diaAdd;
    DialogRemove * diaRem;
    DialogSettings * disSet;
    DialogAddPlug * diaAddP;
    DialogRemovePlug * diaRemP;
    DialogDupPro * diaDupPro;
    DialogDelete * diadel;

    QString settings_File;


};

#endif // MAINWINDOW_H
