#ifndef DIALOGDEPSIL_H
#define DIALOGDEPSIL_H
#include <QStringList>
#include <QButtonGroup>
#include <QDialog>
#include <QWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include "sshhandler.h"

namespace Ui {
class DialogDepSil;
}

class DialogDepSil : public QDialog
{
    Q_OBJECT
    QThread *threadWorker;

public:
    explicit DialogDepSil(QWidget *parent = 0);
    void setUpPeerList(QStringList);
    QString settings_File;
    void call_print_out(QString);

    ~DialogDepSil();

private slots:
    void on_pushButton_clicked();
    void onAllChecked();
    void sendToMain(QString,bool);
    void getPassWord(QString);
    void setSteps();
    void closeMeNow();
    void passRight(bool);

signals:
    void setPeerList(QStringList);
    void startHandler(QStringList);
    void wasCorrect(bool);

private:
    Ui::DialogDepSil *ui;
    SshHandler *handler;
    bool canStop;
    DialogPass * dialogPass;

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

};

#endif // DIALOGDEPSIL_H
