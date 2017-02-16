#ifndef DIALOGDELETE_H
#define DIALOGDELETE_H

#include <QDialog>
#include <QStringList>
#include <QListWidgetItem>
#include "deletehandler.h"
#include <QThread>

namespace Ui {
class DialogDelete;
}

class DialogDelete : public QDialog
{
    Q_OBJECT
    QThread *threadWorker;

public:
    explicit DialogDelete(QWidget *parent = 0);
    void setUpLists(QStringList);
    QString settings_File;
    void call_print_out(QString, bool);

    ~DialogDelete();

private slots:
    void unchecker(QListWidgetItem*);
    void CheckAllPlugs();
    void sendToMain(QString,bool);
    void getPassWord(QString);
    void setSteps();
    void closeMeNow();
    void passRight(bool);
    void loadTheList();
    void setTextTo(QString);
    void on_pushButton_2_clicked();
    void LoadPluginNow();

signals:
    void setPeerList(QStringList);
    void startHandler(QStringList, QStringList);
    void wasCorrect(bool);
    void loadPlugins(QStringList);

private:
    Ui::DialogDelete *ui;
    DeleteHandler *handler;
    bool canStop;
    DialogPass * diaPassW;

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

};

#endif // DIALOGDELETE_H
