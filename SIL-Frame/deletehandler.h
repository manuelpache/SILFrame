#ifndef DELETEHANDLER_H
#define DELETEHANDLER_H

#include <QObject>
#include <QThread>
#include "sshhandler.h"

class DeleteHandler : public QThread
{
    Q_OBJECT

public:
    explicit DeleteHandler(QThread * parent = 0);
    bool ssh_SILSetup(QStringList, QStringList);
    bool copyHandler(QString, QStringList);
    bool loader(QString);
    ~DeleteHandler();

private slots:
    void passAnswere(bool);
    void mainCall(QStringList, QStringList);
    void getPlugins(QStringList);

signals:
    void sendMessage(QString, bool);
    void passRequest(QString);
    void doneWith();
    void step();
    void doneLoad();
    void foundPlugs();
    void textSet(QString);

private:
    bool passAns, haveToWait;
    bool * b1, * b2;
    Sshhelper helper;

};

#endif // DELETEHANDLER_H
