#ifndef DIALOGNEW_H
#define DIALOGNEW_H
#include <QDialog>
#include "newhandler.h"
#include <QString>

namespace Ui {
class DialogNew;
}

class DialogNew : public QDialog
{
    Q_OBJECT

public:
    explicit DialogNew(QWidget *parent = 0);
    ~DialogNew();

private:
    void write_to_file(QString);

private slots:
    void on_toolButton_clicked();

    void on_pushButton_clicked();

    void on_name_textChanged(const QString &arg1);

    void on_path_textChanged(const QString &arg1);

    void sendToMain(QString, bool);

signals:


private:
    Ui::DialogNew *ui;
    NewHandler *handler;
};

#endif // DIALOGNEW_H
