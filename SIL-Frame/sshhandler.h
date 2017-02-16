#ifndef SSHHANDLER_H
#define SSHHANDLER_H
#include <QSettings>
#include <QStringList>
#include <QDialog>
#include <QWidget>
#include <QThread>
#include "dialogpass.h"
#include "sshhelper.h"


class SshHandler : public QThread
{
    Q_OBJECT

public:
    explicit SshHandler(QThread * parent = 0);
    bool ssh_SILSetup(QStringList);
    int copyHandler(QString, bool);


private slots:
    void passAnswere(bool);
    void mainCall(QStringList);

signals:
    void sendMessage(QString, bool);
    void passRequest(QString);
    void doneWith();
    void step();

private:
    bool passAns, haveToWait;
    bool * b1, * b2;
    Sshhelper helper;
};

#endif // SSHHANDLER_H
