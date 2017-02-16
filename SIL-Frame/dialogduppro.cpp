#include "dialogduppro.h"
#include "ui_dialogduppro.h"
#include "dialogpass.h"
#include <mainwindow.h>
#include <QListWidget>
#include <QListWidgetItem>
#include <QThread>

MainWindow * mainWindow;

/**
 * @brief the constructor of the class DialogDupPro
 * @param parent
 */
DialogDupPro::DialogDupPro(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDupPro)
{
    ui->setupUi(this);
    mainWindow = qobject_cast<MainWindow *>(this->parent());
    threadWorker = new QThread();
    handler = new PluginHandler();
    diaPass = new DialogPass(this);
    canStop = true;
    ui->progressBar->setVisible(false);
    handler->moveToThread(threadWorker);
    QObject::connect(ui->checkBox, SIGNAL(toggled(bool)), this, SLOT(CheckAllPeers()));
    QObject::connect(ui->checkBox_2, SIGNAL(toggled(bool)), this, SLOT(CheckAllPlugs()));
    QObject::connect(handler, SIGNAL(sendMessage(QString, bool)), this, SLOT(sendToMain(QString, bool)));
    QObject::connect(this, SIGNAL(startHandler(QStringList, QStringList)), handler, SLOT(mainCall(QStringList, QStringList)));
    QObject::connect(handler, SIGNAL(passRequest(QString)), this, SLOT(getPassWord(QString)));
    QObject::connect(this, SIGNAL(wasCorrect(bool)), handler, SLOT(passAnswere(bool)), Qt::DirectConnection);
    QObject::connect(handler, SIGNAL(doneWith()), this, SLOT(closeMeNow()));
    QObject::connect(handler, SIGNAL(doneWith()), threadWorker, SLOT(quit()));
    QObject::connect(handler, SIGNAL(doneWith()), this, SLOT(close()));
    QObject::connect(handler, SIGNAL(step()), this, SLOT(setSteps()));
    QObject::connect(diaPass, SIGNAL(returnAns(bool)), this, SLOT(passRight(bool)));
    settings_File = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath() + "/sil.ini";
    threadWorker->start();
}

/**
 * @brief the destroctor of the class DialogDupPro
 */
DialogDupPro::~DialogDupPro()
{
    threadWorker->quit();
    threadWorker->wait();
    delete ui;
}

/**
 * @brief sets up the lists of peers and plugins
 * @param listPeer
 * @param listPlug
 */
void DialogDupPro::setUpLists(QStringList listPeer, QStringList listPlug){

    for(int i = 0; i < listPeer.length(); i++){
        QListWidgetItem* item = new QListWidgetItem(listPeer.at(i), ui->listWidget);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
    }
    for(int j = 0; j < listPlug.length(); j++){
        QListWidgetItem * item2 = new QListWidgetItem(listPlug.at(j), ui->listWidget_2);
        item2->setFlags(item2->flags() | Qt::ItemIsUserCheckable);
        item2->setCheckState(Qt::Unchecked);
    }
}

/**
 * @brief checks all peers
 */
void DialogDupPro::CheckAllPeers(){
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
 * @brief checks all plugins
 */
void DialogDupPro::CheckAllPlugs(){
    if(ui->checkBox_2->isChecked()){
        for(int i = 0; i < ui->listWidget_2->count(); i++){
            ui->listWidget_2->item(i)->setCheckState(Qt::Checked);
        }
    } else {
        for(int j = 0; j < ui->listWidget_2->count(); j++){
            ui->listWidget_2->item(j)->setCheckState(Qt::Unchecked);
        }
    }
}

/**
 * @brief slot to start the deploy-process when the push-button is clicked
 */
void DialogDupPro::on_pushButton_clicked()
{
    ui->progressBar->setVisible(true);
    ui->pushButton->setDisabled(true);
    ui->textBrowser->clear();
    QString waitText = "Plugin Deployment is running now, please remain patient. The process can last several minutes";
    ui->textBrowser->setText(waitText);
    ui->textBrowser->setAlignment(Qt::AlignCenter);
    QList <QListWidgetItem *> itemsPeer;
    QStringList peers;
    for(int j = 0; j < ui->listWidget->count(); j++){
        itemsPeer.append(ui->listWidget->item(j));
    }
    for(int i = 0; i < itemsPeer.length(); i++){
        if(itemsPeer.at(i)->checkState() == Qt::Checked){
            peers.append(itemsPeer.at(i)->text());
        }
    }
    QList <QListWidgetItem *> itemsPlug;
    QStringList plugins;
    for(int v = 0; v < ui->listWidget_2->count(); v++){
        itemsPlug.append(ui->listWidget_2->item(v));
    }
    for(int u = 0; u < itemsPlug.length(); u++){
        if(itemsPlug.at(u)->checkState() == Qt::Checked){
            plugins.append(itemsPlug.at(u)->text());
        }
    }
    if(peers.isEmpty()){
        mainWindow->print_out_info("No peers have been selected for a deployment", false);
        this->close();
        return;
    }
    if(plugins.isEmpty()){
        mainWindow->print_out_info("No plugins have been selected for a deployment", false);
        this->close();
        return;
    }
    canStop = false;
    this->setCursor(Qt::BusyCursor);
    ui->progressBar->setMaximum((peers.length() * plugins.length()) +1);
    emit startHandler(peers, plugins);
}

/**
 * @brief sends a message to the mainwindow
 * @param message
 * @param color
 */
void DialogDupPro::sendToMain(QString message, bool color)
{
    mainWindow->print_out_info(message, color);
}

/**
 * @brief passes the answere of the password-dialog
 * @param answere
 */
void DialogDupPro::passRight(bool answere)
{
    emit wasCorrect(answere);
}

/**
 * @brief opens the password-dialog
 * @param peer
 */
void DialogDupPro::getPassWord(QString peer)
{
    diaPass->setValues(peer);
    diaPass->exec();
}

/**
 * @brief sets a step on the processbar
 */
void DialogDupPro::setSteps(){
    int count = ui->progressBar->value();
    count ++;
    ui->progressBar->setValue(count);
    QGuiApplication::processEvents();
}

/**
 * @brief changes the close event
 * @param event
 */
void DialogDupPro::closeEvent(QCloseEvent *event)
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
void DialogDupPro::closeMeNow(){
    canStop = true;
}
