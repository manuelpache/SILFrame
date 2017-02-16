#include "dialogdelete.h"
#include "ui_dialogdelete.h"
#include <QStringList>
#include "mainwindow.h"
#include "dialogpass.h"
#include <QTextStream>
#include <QCoreApplication>

MainWindow * callWindow;

/**
 * @brief The constructor of the Class DialogDelete
 * @param parent
 */
DialogDelete::DialogDelete(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDelete)
{
    ui->setupUi(this);
    diaPassW = new DialogPass();
    callWindow = qobject_cast<MainWindow *>(this->parent());
    threadWorker = new QThread();
    handler = new DeleteHandler();
    canStop = true;
    ui->progressBar->setVisible(false);
    handler->moveToThread(threadWorker);
    QObject::connect(ui->checkBox, SIGNAL(toggled(bool)), this, SLOT(CheckAllPlugs()));
    QObject::connect(handler, SIGNAL(sendMessage(QString, bool)), this, SLOT(sendToMain(QString, bool)));
    QObject::connect(this, SIGNAL(startHandler(QStringList, QStringList)), handler, SLOT(mainCall(QStringList, QStringList)));
    QObject::connect(handler, SIGNAL(passRequest(QString)), this, SLOT(getPassWord(QString)));
    QObject::connect(this, SIGNAL(wasCorrect(bool)), handler, SLOT(passAnswere(bool)), Qt::DirectConnection);
    QObject::connect(handler, SIGNAL(doneLoad()), this, SLOT(closeMeNow()));
    QObject::connect(handler, SIGNAL(doneWith()), this, SLOT(closeMeNow()));
    QObject::connect(handler, SIGNAL(doneWith()), threadWorker, SLOT(quit()));
    QObject::connect(handler, SIGNAL(doneWith()), this, SLOT(close()));
    QObject::connect(handler, SIGNAL(step()), this, SLOT(setSteps()));
    QObject::connect(diaPassW, SIGNAL(returnAns(bool)), this, SLOT(passRight(bool)));
    QObject::connect(handler, SIGNAL(foundPlugs()), this, SLOT(loadTheList()));
    QObject::connect(ui->listWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(unchecker(QListWidgetItem*)));
    QObject::connect(this, SIGNAL(loadPlugins(QStringList)), handler, SLOT(getPlugins(QStringList)));
    QObject::connect(handler, SIGNAL(textSet(QString)), this, SLOT(setTextTo(QString)));
    QObject::connect(ui->listWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(LoadPluginNow()));
    settings_File = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath() + "/sil.ini";
    threadWorker->start();
}

/**
 * @brief The Destructor of the class DialogDelete
 */
DialogDelete::~DialogDelete()
{
    threadWorker->quit();
    threadWorker->wait();
    delete ui;
}

/**
 * @brief Loads the Peers to the list
 * @param listPeer
 */
void DialogDelete::setUpLists(QStringList listPeer){
    for(int i = 0; i < listPeer.length(); i++){
        QListWidgetItem* item = new QListWidgetItem(listPeer.at(i), ui->listWidget);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
    }
}

/**
 * @brief Unchecks all peers
 * @param item
 */
void DialogDelete::unchecker(QListWidgetItem * item){
    if(item->checkState() == Qt::Checked){
        ui->listWidget_2->clear();
        for(int i = 0; i < ui->listWidget->count(); i++){
            if(ui->listWidget->item(i) != item){
                ui->listWidget->item(i)->setCheckState(Qt::Unchecked);
            }
        }
    }
}

/**
 * @brief checks alle Peers
 */
void DialogDelete::CheckAllPlugs(){
    if(ui->checkBox->isChecked()){
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
 * @brief Slot process to load Plugins from Peer
 */
void DialogDelete::LoadPluginNow()
{
    ui->textBrowser->clear();
    ui->listWidget_2->clear();
    for(int i = 0; i < ui->listWidget->count(); i++){
        if(ui->listWidget->item(i)->checkState() == Qt::Checked){
            QStringList peers;
            peers.append(ui->listWidget->item(i)->text());
            this->setCursor(Qt::BusyCursor);
            ui->progressBar->setVisible(true);
            ui->progressBar->setValue(true);
            ui->progressBar->setMaximum(2);
            ui->progressBar->setValue(1);
            emit loadPlugins(peers);
            ui->listWidget->setDisabled(true);
            canStop = false;
            return;
        } if(i == ui->listWidget->count() -1){
            emit setTextTo("No peer has been selected");
        }
    }
}

/**
 * @brief Sends output-message to mainwindow
 * @param message
 * @param color
 */
void DialogDelete::sendToMain(QString message, bool color)
{
    callWindow->print_out_info(message, color);
}

/**
 * @brief signal to send answere to thread
 * @param answere
 */
void DialogDelete::passRight(bool answere)
{
    emit wasCorrect(answere);
}

/**
 * @brief opens Passwort-Dialog
 * @param peer
 */
void DialogDelete::getPassWord(QString peer)
{
    diaPassW->setValues(peer);
    diaPassW->exec();
}

/**
 * @brief Sets a step to the processbar
 */
void DialogDelete::setSteps(){
    int count = ui->progressBar->value();
    count ++;
    ui->progressBar->setValue(count);
    QGuiApplication::processEvents();
}

/**
 * @brief changes the close event, if process is running.
 * @param event
 */
void DialogDelete::closeEvent(QCloseEvent *event)
{
    if(canStop){
        handler->quit();
        threadWorker->quit();
        event->accept();
    } else {
        event->ignore();
    }
}

/**
 * @brief sets the Dialog closeable again
 */
void DialogDelete::closeMeNow(){
    canStop = true;
    ui->listWidget->setDisabled(false);
    ui->progressBar->setVisible(false);
    this->setCursor(Qt::ArrowCursor);
}

/**
 * @brief loads the list of plugins to the list
 */
void DialogDelete::loadTheList(){
    ui->textBrowser->clear();
    QString path = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath();
    QFile plugins(path + QDir::separator() + "plugin_list");
    QStringList list;
    plugins.open(QIODevice::ReadWrite);
    QTextStream in (&plugins);
    while(!in.atEnd()){
        QString content = in.readLine();
        list << content;
    }
    list.removeDuplicates();
    if(list.isEmpty()){
        emit setTextTo("There are no plugins installed on this peer");
        return;
    }
    ui->listWidget_2->clear();
    plugins.close();
    for(int i = 0; i < list.length(); i++){
        QListWidgetItem* item = new QListWidgetItem(list.at(i), ui->listWidget_2);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
    }
    plugins.remove();
}

/**
 * @brief slot to start the Delete Process, when the push-button is clicked
 */
void DialogDelete::on_pushButton_2_clicked()
{
    ui->textBrowser->clear();
    QString waitText = "Delete Plugins is running now, please remain patient. The process can last several minutes";
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
        emit setTextTo("No peer has been selected");
        return;
    }
    if(plugins.isEmpty()){
        emit setTextTo("No plugin has been selected to delete it");
        return;
    }
    ui->progressBar->setVisible(true);
    canStop = false;
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum((peers.length() * plugins.length()) +1);
    this->setCursor(Qt::BusyCursor);
    emit startHandler(peers, plugins);
}

/**
 * @brief sets text of the textbrowser
 * @param text
 */
void DialogDelete::setTextTo(QString text){
    ui->textBrowser->clear();
    ui->textBrowser->setAlignment(Qt::AlignCenter);
    ui->textBrowser->setHtml("<font color=#DF7373 >" + text + "</font>");
}
