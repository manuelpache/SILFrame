#ifndef PLUGINHANDLER_H
#define PLUGINHANDLER_H

#include <QObject>
#include <QThread>
#include <QWidget>
#include "sshhelper.h"

class PluginHandler : public QThread
{
    Q_OBJECT

public:
    explicit PluginHandler(QThread * parent = 0);
    bool ssh_SILSetup(QStringList, QStringList);
    int copyHandler(QString, QStringList, bool);

private slots:
    void passAnswere(bool);
    void mainCall(QStringList, QStringList);

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

#endif // PLUGINHANDLER_H
