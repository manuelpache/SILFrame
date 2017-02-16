#ifndef PASSWHANDLER_H
#define PASSWHANDLER_H

#include <QObject>
#include <QString>

class PasswHandler : public QObject
{
    Q_OBJECT
public:
    explicit PasswHandler(QObject * parent = 0);
    bool handlePassW(QString, QString);
};

#endif // PASSWHANDLER_H
