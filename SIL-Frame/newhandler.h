#ifndef NEWHANDLER_H
#define NEWHANDLER_H

#include <QObject>

class NewHandler : public QObject
{
    Q_OBJECT
public:
    explicit NewHandler(QObject * parent = 0);
    bool newProject(QString, QString, QString);

signals:
    void sendToDialog(QString, bool);
};

#endif // NEWHANDLER_H
