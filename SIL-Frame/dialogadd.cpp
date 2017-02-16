#include "dialogadd.h"
#include "ui_dialogadd.h"
#include "mainwindow.h"
#include <qobject.h>
#include <QFileDialog>

MainWindow * m_window;

/**
 * @brief the constructor of the class DialogAdd
 * @param parent
 */
DialogAdd::DialogAdd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAdd)
{
    ui->setupUi(this);
    m_window = qobject_cast<MainWindow*>(this->parent());
}

/**
 * @brief the destructor of the class Dialogadd
 */
DialogAdd::~DialogAdd()
{
    delete ui;
}

/**
 * @brief slot is called when the push_button is clicked, its used to add a Peer and checks some possible errors
 */
void DialogAdd::on_pushButton_clicked()
{
    QString newPeer = ui->lineEdit->text().remove(" ");
    QString newHostName = ui->lineEdit_HostName->text().remove(" ");
    QString newUser = ui->lineEdit_User->text().remove(" ");
    QString newPort = ui->lineEdit_Port->text().remove(" ");
    QString newRSA = ui->lineEdit_RSA->text().remove(" ");

    if(newPeer.length() > 1 && newHostName.length() > 6 && newUser.length() > 1 && newRSA.length() > 5){
    m_window->addNewPeer(newPeer, newHostName, newUser, newPort, newRSA);
    ui->lineEdit->setText("");
    this->close();
    } else {
        if(!(newPeer.length() > 1)){
        ui->lineEdit->setStyleSheet("border: 1px solid red");
        } if(!(newHostName.length() > 6)){
            ui->lineEdit_HostName->setStyleSheet("border: 1px solid red");
        } if(!(newUser.length() > 2)){
            ui->lineEdit_User->setStyleSheet("border: 1px solid red");
        } if(!(newRSA.length() > 5)){
            ui->lineEdit_RSA->setStyleSheet("border: 1px solid red");
        }
    }
}

/**
 * @brief Adds a new Peer to the INI-File
 * @param host
 * @param hostName
 * @param user
 * @param port
 * @param rsa
 */
void DialogAdd::setEditLines(QString host,QString hostName, QString user, QString port, QString rsa)
{
    ui->lineEdit->setText(host);
    ui->lineEdit_HostName->setText(hostName);
    ui->lineEdit_User->setText(user);
    ui->lineEdit_Port->setText(port);
    ui->lineEdit_RSA->setText(rsa);
}

/**
 * @brief opens a File-selector-Dialog
 */
void DialogAdd::on_toolButton_clicked()
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
 * @brief checks if line_edit has changed
 * @param arg1
 */
void DialogAdd::on_lineEdit_textChanged(const QString &arg1)
{
    (void)arg1;
    if(ui->lineEdit->text().length() > 1){
        ui->lineEdit->setStyleSheet("");
    }
}

/**
 * @brief checks if line_edit hostname has changed
 * @param arg1
 */
void DialogAdd::on_lineEdit_HostName_textChanged(const QString &arg1)
{
    (void)arg1;
    if(ui->lineEdit_HostName->text().length() > 6){
        ui->lineEdit_HostName->setStyleSheet("");
    }
}

/**
 * @brief checks if the line_edit User has changed
 * @param arg1
 */
void DialogAdd::on_lineEdit_User_textChanged(const QString &arg1)
{
    (void)arg1;
    if(ui->lineEdit_User->text().length() > 2){
        ui->lineEdit_User->setStyleSheet("");
    }
}

/**
 * @brief checks if the line_edit RSA has changed
 * @param arg1
 */
void DialogAdd::on_lineEdit_RSA_textChanged(const QString &arg1)
{
    (void)arg1;
    if(ui->lineEdit_RSA->text().length() > 5){
        ui->lineEdit_RSA->setStyleSheet("");
    }
}
