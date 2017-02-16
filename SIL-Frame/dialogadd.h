#ifndef DIALOGADD_H
#define DIALOGADD_H

#include <QDialog>

namespace Ui {
class DialogAdd;
}

class DialogAdd : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAdd(QWidget *parent = 0);
    void setEditLines(QString,QString,QString,QString,QString);
    ~DialogAdd();

private slots:
    void on_pushButton_clicked();

    void on_toolButton_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

    void on_lineEdit_HostName_textChanged(const QString &arg1);

    void on_lineEdit_User_textChanged(const QString &arg1);

    void on_lineEdit_RSA_textChanged(const QString &arg1);

private:
    Ui::DialogAdd *ui;
};

#endif // DIALOGADD_H
