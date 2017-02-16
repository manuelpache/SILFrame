#include "dialogpass.h"
#include "ui_dialogpass.h"
#include <QProcess>
#include <QFile>
#include "dialogdepsil.h"
#include <QCloseEvent>
#include <QFileInfo>

DialogDepSil *parentHandler;

/**
 * @brief the constructor of the class DialogPass
 * @param parent
 */
DialogPass::DialogPass(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPass)
{
    ui->setupUi(this);
    handler = new PasswHandler();
    used = false;
    count = 0;
}

/**
 * @brief The destructor of the class DialogPass
 */
DialogPass::~DialogPass()
{
    delete ui;
}

/**
 * @brief sets the text of the line-edit
 * @param ppeer
 */
void DialogPass::setValues(QString ppeer)
{
    peer = ppeer;
    QString info = "Please enter the passphrase for Peer: " + peer;
    ui->textEdit->setText(info);
}

/**
 * @brief starts the password check process, if the pushbutton is clicked
 */
void DialogPass::on_pushButton_clicked()
{
    if(passHandler()){
        used = true;
        emit returnAns(true);
        this->close();
        return;
    } else {
        ui->lineEdit->clear();
        ui->textEdit->setStyleSheet("color: red; background-color: white;");
        count ++;
    }
    if(count == 3){
        emit returnAns(false);
        this->close();
        return;
    }
}


/**
 * @brief starts the PasswHandler
 * @return
 */
bool DialogPass::passHandler()
{
    QString pass = ui->lineEdit->text();
    if(handler->handlePassW(peer, pass)){
        ui->lineEdit->clear();
        return true;
    } else {
        ui->lineEdit->clear();
        return false;
    }
}

/**
 * @brief changes the close-event, if closed the password is wrong
 * @param event
 */
void DialogPass::closeEvent(QCloseEvent *event)
{
    if(used){
        event->accept();
    } else {
        emit returnAns(false);
        event->accept();
    }
}
