#include "dialogdepsil.h"
#include "ui_dialogdepsil.h"
#include <mainwindow.h>
#include <QListWidget>
#include <QListWidgetItem>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "dialogpass.h"
#include <QtConcurrent/QtConcurrent>

MainWindow * window_A;

/**
 * @brief constructor of the class DialogDepSil
 * @param parent
 */
DialogDepSil::DialogDepSil(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDepSil)
{
    ui->setupUi(this);
    threadWorker = new QThread();
    handler = new SshHandler();
    dialogPass = new DialogPass(this);
    canStop = true;
    ui->progressBar->setVisible(false);
    window_A = qobject_cast<MainWindow *>(this->parent());
    handler->moveToThread(threadWorker);
    QObject::connect(ui->checkBox, SIGNAL(toggled(bool)), this, SLOT(onAllChecked()));
    QObject::connect(handler, SIGNAL(sendMessage(QString, bool)), this, SLOT(sendToMain(QString, bool)));
    QObject::connect(this, SIGNAL(startHandler(QStringList)), handler, SLOT(mainCall(QStringList)));
    QObject::connect(handler, SIGNAL(passRequest(QString)), this, SLOT(getPassWord(QString)));
    QObject::connect(this, SIGNAL(wasCorrect(bool)), handler, SLOT(passAnswere(bool)), Qt::DirectConnection);
    QObject::connect(handler, SIGNAL(doneWith()), this, SLOT(closeMeNow()));
    QObject::connect(handler, SIGNAL(doneWith()), threadWorker, SLOT(quit()));
    QObject::connect(handler, SIGNAL(doneWith()), this, SLOT(close()));
    QObject::connect(handler, SIGNAL(step()), this, SLOT(setSteps()));
    QObject::connect(dialogPass, SIGNAL(returnAns(bool)), SLOT(passRight(bool)));
    settings_File = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath() + "/sil.ini";
    threadWorker->start();
}

/**
 * @brief destructor of the class DialogDepSil
 */
DialogDepSil::~DialogDepSil()
{
    threadWorker->quit();
    threadWorker->wait();
    delete ui;
}

/**
 * @brief sets the Peer list
 * @param list
 */
void DialogDepSil::setUpPeerList(QStringList list)
{
    for(int i = 0; i < list.length(); i++){
        QListWidgetItem* item = new QListWidgetItem(list.at(i), ui->listWidget);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
    }
}

/**
 * @brief slot to start setup-sil process when the push-button is clicked
 */
void DialogDepSil::on_pushButton_clicked()
{
    ui->progressBar->setVisible(true);
    ui->pushButton->setDisabled(true);
    ui->textBrowser->clear();
    QString waitText = "SIL-Setup is running now, please remain patient. The process can last several minutes";
    ui->textBrowser->setText(waitText);
    ui->textBrowser->setAlignment(Qt::AlignCenter);
    QList <QListWidgetItem *> items;
    QStringList stringItems;
    for(int j = 0; j < ui->listWidget->count(); j++){
        items.append(ui->listWidget->item(j));
    }
    for(int i = 0; i < items.length(); i++){
        if(items.at(i)->checkState() == Qt::Checked){
            stringItems.append(items.at(i)->text());
        }
    }
    if(stringItems.isEmpty()){
        window_A->print_out_info("No peers have been selected for a setup", false);
        this->close();
    }
    canStop = false;
    ui->progressBar->setMaximum(stringItems.length() +1);
    this->setCursor(Qt::BusyCursor);
    emit startHandler(stringItems);
}

/**
 * @brief checks all Peers
 */
void DialogDepSil::onAllChecked(){
    if(ui->checkBox->isChecked()){
        for(int i = 0; i < ui->listWidget->count(); i++){
            ui->listWidget->item(i)->setCheckState(Qt::Checked);
        }
    } else {
        for(int j = 0; j < ui->listWidget->count(); j++){
            ui->listWidget->item(j)->setCheckState(Qt::Unchecked);
        }
    }
}

/**
 * @brief sends message to mainwindow
 * @param message
 * @param color
 */
void DialogDepSil::sendToMain(QString message, bool color)
{
    window_A->print_out_info(message, color);
}

/**
 * @brief sends the answere of the passwort-Dialog to the thread
 * @param answere
 */
void DialogDepSil::passRight(bool answere)
{
    emit wasCorrect(answere);
}

/**
 * @brief opens the Passwort-Dialog
 * @param peer
 */
void DialogDepSil::getPassWord(QString peer)
{
    dialogPass->setValues(peer);
    dialogPass->exec();
}

/**
 * @brief sets step to the processbar
 */
void DialogDepSil::setSteps(){
    int count = ui->progressBar->value();
    count ++;
    ui->progressBar->setValue(count);
    QGuiApplication::processEvents();
}

/**
 * @brief changes the close event
 * @param event
 */
void DialogDepSil::closeEvent(QCloseEvent *event)
{
    if(canStop){
        event->accept();
    } else {
        event->ignore();
    }
}

/**
 * @brief sets the Dialog closeable again
 */
void DialogDepSil::closeMeNow(){
    canStop = true;
}
