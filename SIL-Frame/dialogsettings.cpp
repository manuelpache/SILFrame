#include "dialogsettings.h"
#include "ui_dialogsettings.h"
#include <QFileDialog>
#include <mainwindow.h>

MainWindow * mainWind;

/**
 * @brief the constructor of the class DialogSettings
 * @param parent
 */
DialogSettings::DialogSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSettings)
{
    ui->setupUi(this);
    mainWind = qobject_cast<MainWindow *>(this->parent());
    ui->lineEditmiddle->setText(mainWind->getMiddlewarePath());
    ui->lineEdit_RSA->setText(mainWind->getRSAPath());
    ui->lineEdit_Host->setText(mainWind->getSSHpath());
    ui->lineEdit_HostName->setText(mainWind->getHostName());
    ui->lineEdit_Port->setText(mainWind->getPort());
    ui->lineEdit_User->setText(mainWind->getSSHUser());
    ui->checkBox->setChecked(mainWind->getProxyState());
    ui->label_hidden->setVisible(false);
}

/**
 * @brief the destructor of the class DialogSettings
 */
DialogSettings::~DialogSettings()
{
    delete ui;
}

/**
 * @brief saves the settings if all lines have the right length
 */
void DialogSettings::on_pushButton_clicked()
{
    if(ui->lineEditmiddle->text() != mainWind->getMiddlewarePath()){
        QFile exister(ui->lineEditmiddle->text());
        if((ui->lineEditmiddle->text().contains("tar.gz") && exister.exists()) || ui->lineEditmiddle->text().isEmpty()){
            mainWind->setMiddlewarePath(ui->lineEditmiddle->text().remove(" "));
        } else {
            ui->lineEditmiddle->setStyleSheet("border: 1px solid red");
            ui->label_hidden->setVisible(true);
            return;
        }
    }
    if(ui->lineEdit_Port->text() != mainWind->getPort()){
        mainWind->setPort(ui->lineEdit_Port->text().remove(" "));
    }
    if(ui->checkBox->isChecked()){
        int count = 0;
        if(ui->lineEdit_RSA->text().length() > 4 ){
            mainWind->setRSAPath(ui->lineEdit_RSA->text().remove(" "));
            count++;
        } else {
            ui->lineEdit_RSA->setStyleSheet("border: 1px solid red");
        }
        if(ui->lineEdit_Host->text().length() > 1 ){
            mainWind->setSSHpath(ui->lineEdit_Host->text().remove(" "));
            count++;
        } else {
            ui->lineEdit_Host->setStyleSheet("border: 1px solid red");
        }
        if(ui->lineEdit_HostName->text().length() > 1 ){
            mainWind->setHostName(ui->lineEdit_HostName->text().remove(" "));
            count++;
        } else {
            ui->lineEdit_HostName->setStyleSheet("border: 1px solid red");
        }
        if(ui->lineEdit_User->text().length() > 1 ){
            mainWind->setSSHUser(ui->lineEdit_User->text().remove(" "));
            count++;
        } else {
            ui->lineEdit_User->setStyleSheet("border: 1px solid red");
        }
        if(count == 4){
            if(!(mainWind->getProxyState() == ui->checkBox->isChecked())){
                mainWind->setProxyState(ui->checkBox->isChecked());
            }
            this->close();
        }
    } else {
        if(ui->lineEdit_Host->text().isEmpty()){
            mainWind->setSSHpath("");
        }
        if(ui->lineEdit_HostName->text().isEmpty()){
            mainWind->setHostName("");
        }
        if(ui->lineEdit_Port->text().isEmpty()){
            mainWind->setPort("");
        }
        if(ui->lineEdit_User->text().isEmpty()){
            mainWind->setSSHUser("");
        }
        if(ui->lineEdit_RSA->text().isEmpty()){
            mainWind->setRSAPath("");
        }
        if(mainWind->getProxyState()){
            mainWind->setProxyState(false);
        }
        this->close();
    }
}

/**
 * @brief opens a file-choose dialog
 */
void DialogSettings::on_toolButtonmiddle_clicked()
{
    QFileDialog dialog(this);
    QString name = qgetenv("USER");
    QString start = "/home/" + name;
    QString path = dialog.getOpenFileName(this, tr("Open File"),
                                          start,
                                          tr("Compressed Files(*.gz)"));
    ui->lineEditmiddle->setText(path);
}

/**
 * @brief opens a file-choose dialog for the ssh-key
 */
void DialogSettings::on_toolButton_RSA_clicked()
{
    QFileDialog dialog(this);
    QString name = qgetenv("USER");
    QString start = "/home/" + name + "/.ssh";
    QString path = dialog.getOpenFileName(this, tr("Any File"),
                                          start,
                                          tr("Files(*)"));
    ui->lineEdit_RSA->setText(path);
}

/**
 * @brief DialogSettings::on_lineEdit_Host_textChanged
 * @param arg1
 */
void DialogSettings::on_lineEdit_Host_textChanged(const QString &arg1)
{
    (void)arg1;
    if(ui->lineEdit_Host->text().length() > 1){
        ui->lineEdit_Host->setStyleSheet("");
    }
}

/**
 * @brief DialogSettings::on_lineEdit_HostName_textChanged
 * @param arg1
 */
void DialogSettings::on_lineEdit_HostName_textChanged(const QString &arg1)
{
    (void)arg1;
    if(ui->lineEdit_HostName->text().length() > 1){
        ui->lineEdit_HostName->setStyleSheet("");
    }
}

/**
 * @brief DialogSettings::on_lineEdit_User_textChanged
 * @param arg1
 */
void DialogSettings::on_lineEdit_User_textChanged(const QString &arg1)
{
    (void)arg1;
    if(ui->lineEdit_User->text().length()){
        ui->lineEdit_User->setStyleSheet("");
    }
}

/**
 * @brief DialogSettings::on_lineEdit_RSA_textChanged
 * @param arg1
 */
void DialogSettings::on_lineEdit_RSA_textChanged(const QString &arg1)
{
    (void)arg1;
    if(ui->lineEdit_RSA->text().length()){
        ui->lineEdit_RSA->setStyleSheet("");
    }
}

/**
 * @brief DialogSettings::on_lineEditmiddle_textChanged
 * @param arg1
 */
void DialogSettings::on_lineEditmiddle_textChanged(const QString &arg1)
{
    (void)arg1;
    if(ui->lineEditmiddle->text().contains("tar.gz") || ui->lineEditmiddle->text().isEmpty()){
        ui->lineEditmiddle->setStyleSheet("");
        ui->label_hidden->setVisible(false);
    }
}

/**
 * @brief DialogSettings::on_checkBox_toggled
 * @param checked
 */
void DialogSettings::on_checkBox_toggled(bool checked)
{
    if(!checked){
        ui->lineEdit_RSA->setStyleSheet("");
        ui->lineEdit_Host->setStyleSheet("");
        ui->lineEdit_HostName->setStyleSheet("");
        ui->lineEdit_User->setStyleSheet("");
    }
}
