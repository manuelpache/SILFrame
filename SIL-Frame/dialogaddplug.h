#ifndef DIALOGADDPLUG_H
#define DIALOGADDPLUG_H
#include <QDialog>
#include "addplughandler.h"

namespace Ui {
class DialogAddPlug;
}

class DialogAddPlug : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAddPlug(QWidget *parent = 0);
    ~DialogAddPlug();

private slots:
    void on_toolButton_clicked();

    void on_pushButton_clicked();

    void sendToMain(QString, bool);

    void callMainToAdd(QString);

private:
    Ui::DialogAddPlug *ui;
    AddPlugHandler *handler;
};

#endif // DIALOGADDPLUG_H
