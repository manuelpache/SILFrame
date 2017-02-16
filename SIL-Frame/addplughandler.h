#ifndef ADDPLUGHANDLER_H
#define ADDPLUGHANDLER_H

#include <QObject>
#include <QWidget>
#include <QString>
#include <QThread>

class AddPlugHandler : public QObject
{
    Q_OBJECT

public:
    explicit AddPlugHandler(QObject * parent = 0);
    bool addPlugin(QString);

signals:
    void sendToDialog(QString, bool);
    void sendProName(QString);
};

#endif // ADDPLUGHANDLER_H
