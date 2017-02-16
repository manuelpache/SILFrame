#ifndef DIALOGDUPPRO_H
#define DIALOGDUPPRO_H
#include <QDialog>
#include "pluginhandler.h"
#include "dialogpass.h"


namespace Ui {
class DialogDupPro;
}

class DialogDupPro : public QDialog
{
    Q_OBJECT
    QThread *threadWorker;

public:
    explicit DialogDupPro(QWidget *parent = 0);
    void setUpLists(QStringList, QStringList);
    QString settings_File;
    void call_print_out(QString, bool);

    ~DialogDupPro();

private slots:
    void on_pushButton_clicked();
    void CheckAllPeers();
    void CheckAllPlugs();
    void sendToMain(QString,bool);
    void getPassWord(QString);
    void setSteps();
    void closeMeNow();
    void passRight(bool);

signals:
    void setPeerList(QStringList);
    void startHandler(QStringList, QStringList);
    void wasCorrect(bool);

private:
    Ui::DialogDupPro *ui;
    PluginHandler *handler;
    bool canStop;
    DialogPass * diaPass;


protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
};

#endif // DIALOGDUPPRO_H
