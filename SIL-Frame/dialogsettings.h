#ifndef DIALOGSETTINGS_H
#define DIALOGSETTINGS_H

#include <QDialog>

namespace Ui {
class DialogSettings;
}

class DialogSettings : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSettings(QWidget *parent = 0);
    ~DialogSettings();

private slots:
    void on_pushButton_clicked();

    void on_toolButtonmiddle_clicked();

    void on_toolButton_RSA_clicked();

    void on_lineEdit_Host_textChanged(const QString &arg1);

    void on_lineEdit_HostName_textChanged(const QString &arg1);

    void on_lineEdit_User_textChanged(const QString &arg1);

    void on_lineEdit_RSA_textChanged(const QString &arg1);

    void on_lineEditmiddle_textChanged(const QString &arg1);

    void on_checkBox_toggled(bool checked);

private:
    Ui::DialogSettings *ui;
};

#endif // DIALOGSETTINGS_H
