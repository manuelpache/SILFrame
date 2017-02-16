#ifndef SSHHELPER_H
#define SSHHELPER_H

#include <QObject>
#include <QThread>

class Sshhelper : QThread
{
    Q_OBJECT

public:
    explicit Sshhelper(QThread * parent = 0);
    bool ping_check(QString);
    bool rsa_proxykey_handler(QString);
    bool config_builder(QStringList);
};

#endif // SSHHELPER_H
