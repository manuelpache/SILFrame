#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextStream>
#include <QFile>
#include <stdio.h>
#include <QStringList>
#include <QDateTime>
#include <dialogsettings.h>
#include <QSettings>
#include <dialognew.h>
#include <dialogdepsil.h>
#include <dialogaddplug.h>
#include <dialogremoveplug.h>
#include <dialogadd.h>
#include <dialogremove.h>
#include <dialogsettings.h>

QString peer;
QStringList peers;
QString plugin;
QStringList plugins;

/**
     * @brief the constructor of the class MainWindow
     * @param parent
     */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    settings_File = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath() + "/sil.ini";
    QSettings settings(settings_File, QSettings::NativeFormat);
    QString val = settings.value("activated").toString();
    if(val.isEmpty()){
        settings.setValue("activated", "false");
    }
}

/**
     * @brief the destrutor of the class MainWindow
     */
MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief prints out info in the output
 * @param line
 * @param color
 */
void MainWindow::print_out_info(QString line, bool color){
    QString time = QDateTime::currentDateTime().toString("hh:mm:ss");
    std::string lineStd = line.toStdString();
    QTextStream(stdout) << line << endl;
    if(!color){
        line = QString::fromUtf8("<font color=#DF7373 >%1</font>").arg(tr(lineStd.c_str()));
    }
    ui->textEdit->append(time+":~ "+line);
}

/**
 * @brief loads the peers list
 */
void MainWindow::loadPeerList(){
    QSettings settings(settings_File, QSettings::NativeFormat);
    settings.beginGroup("Peers");
    QStringList keys = settings.allKeys();
    for(int i = 0; i < keys.length(); i++){
        peers.append(settings.value(keys.at(i)).toString());
        QTextStream(stdout) << "Peer " + settings.value(keys.at(i)).toString() + " loaded to list" << endl;

    }
    settings.endGroup();
    peers.removeDuplicates();
    ui->listWidget->addItems(peers);
}

/**
 * @brief adds a new peer and saves it
 * @param peerg
 * @param hostName
 * @param user
 * @param nport
 * @param nrsa
 */
void MainWindow::addNewPeer(QString peerg, QString hostName, QString user, QString nport, QString nrsa)
{
    peers.append(peerg);
    peers.removeDuplicates();
    QSettings settings(settings_File, QSettings::NativeFormat);
    settings.beginGroup("Peers");
    QStringList keys = settings.allKeys();
    for(int i = 0; i < keys.length(); i++){
        settings.remove(keys.at(i));
    }
    for(int j = 0; j < peers.length(); j++){
        QString name = "p" + QString::number(j);
        settings.setValue(name, peers.at(j));
    }
    settings.endGroup();

    settings.beginGroup(peerg);
    settings.setValue("Host", peerg);
    settings.setValue("HostName", hostName);
    settings.setValue("User", user);
    settings.setValue("Port", nport);
    settings.setValue("PubKey", nrsa);
    settings.endGroup();

    ui->listWidget->clear();
    this->loadPeerList();
    this->print_out_info(peerg +" has been added to list", true);
    QTextStream(stdout) << peerg + "has been added to list" << endl;
}

/**
 * @brief removes a peer
 */
void MainWindow::removePeer()
{
    if(peers.contains(peer)){
        peers.removeOne(peer);
        QSettings settings(settings_File, QSettings::NativeFormat);
        settings.beginGroup("Peers");
        QStringList keys = settings.allKeys();
        for(int i = 0; i < keys.length(); i++){
            settings.remove(keys.at(i));
        }
        for(int j = 0; j < peers.length(); j++){
            QString name = "p" + QString::number(j);
            settings.setValue(name, peers.at(j));
        }
        settings.endGroup();

        settings.beginGroup(peer);
        settings.remove("");
        settings.endGroup();

        this->print_out_info(peer +" has been removed from list", true);
        peer = "";
        ui->listWidget->clear();
        peers.clear();
        QTextStream(stdout) << peer +" has been removed from list" << endl;
        this->loadPeerList();
    }
}


/**
 * @brief opens the dialogNew
 */
void MainWindow::on_pushButtonNew_clicked()
{
    diaNew = new DialogNew(this);
    diaNew->setAttribute(Qt::WA_DeleteOnClose);
    diaNew->exec();
    QTextStream(stdout) << "DialogNew is running " << endl;
}

/**
 * @brief opens the dialogAdd
 */
void MainWindow::on_pushButtonAdd_clicked()
{
    diaAdd = new DialogAdd(this);
    diaAdd->setAttribute(Qt::WA_DeleteOnClose);
    diaAdd->exec();
    QTextStream(stdout) << "DialogAdd is running " << endl;
}

/**
 * @brief opens the DialogRemove
 */
void MainWindow::on_pushButtonRemove_clicked()
{
    if(!ui->listWidget->selectedItems().isEmpty()){
        QListWidgetItem * item = ui->listWidget->selectedItems().at(0);
        peer = item->text();
        diaRem = new DialogRemove(this);
        diaRem->setAttribute(Qt::WA_DeleteOnClose);
        diaRem->setPeer(peer);
        diaRem->exec();
        QTextStream(stdout) << "DialogRemove is running " << endl;
    } else {
        this->print_out_info("No peer to remove has been selected", false);
    }
}

/**
 * @brief clears the output
 */
void MainWindow::on_pushButtonClear_clicked()
{
    ui->textEdit->clear();
    QTextStream(stdout) << "Output was cleared " << endl;
}

/**
 * @brief opens the DialogSettings
 */
void MainWindow::on_pushButtonsett_clicked()
{
    disSet = new DialogSettings(this);
    disSet->setAttribute(Qt::WA_DeleteOnClose);
    disSet->exec();
    QTextStream(stdout) << "DialogSettings is running " << endl;
}

/**
 * @brief gets the middlewarepath from the INI-File
 */
QString MainWindow::getMiddlewarePath()
{
    QSettings settings(settings_File, QSettings::NativeFormat);
    QString path = settings.value("middleware_path").toString();
    return path;
}

/**
 * @brief sets the middlewarepath to the INI-File
 */
void MainWindow::setMiddlewarePath(QString path)
{
    QSettings settings(settings_File, QSettings::NativeFormat);
    settings.setValue("middleware_path", path);
    if(!path.isEmpty()){
        this->print_out_info("Middleware path has been changed to " + path, true);
        QTextStream(stdout) << "Middleware-Path has been set " << endl;
    } else {
        this->print_out_info("Middleware path has been removed", true);
        QTextStream(stdout) << "Middleware-Path has been removed " << endl;
    }
}

/**
 * @brief opens the DialogAddPlug
 */
void MainWindow::on_pushButtonAdd_2_clicked()
{
    diaAddP = new DialogAddPlug(this);
    diaAddP->setAttribute(Qt::WA_DeleteOnClose);
    QTextStream(stdout) << "DialogAddPlug is running " << endl;
    diaAddP->exec();
}

/**
 * @brief Adds a Plugin to the list and saves it
 * @param tarFile
 * @param fileName
 */
void MainWindow::addPlugin(QString tarFile, QString fileName)
{
    QSettings settings(settings_File, QSettings::NativeFormat);
    settings.beginGroup("Plugins");
    settings.setValue(fileName, tarFile);
    settings.endGroup();
    loadPluginList();
    QTextStream(stdout) << "Plugin: " + fileName + " has been successfully added to the list" << endl;
    print_out_info("Plugin: " + fileName + " has been successfully added to the list", true);
}

/**
 * @brief loads the peerlist
 */
void MainWindow::loadPluginList()
{
    QSettings settings(settings_File, QSettings::NativeFormat);
    settings.beginGroup("Plugins");
    QStringList keys = settings.allKeys();
    ui->listWidget_3->clear();
    plugins.clear();
    settings.beginGroup("Plugins");
    settings.remove(plugin);
    settings.endGroup();
    for(int i = 0; i < keys.length(); i++){
        plugins.append(keys.at(i));
        QTextStream(stdout) << "Plugin " + keys.at(i) + " loaded to list" << endl;

    }
    settings.endGroup();
    plugins.removeDuplicates();
    ui->listWidget_3->addItems(plugins);
}

/**
 * @brief removes a plugin and resets the list
 */
void MainWindow::removePlugin(){
    QSettings settings(settings_File, QSettings::NativeFormat);
    settings.beginGroup("Plugins");
    QString plug = settings.value(plugin).toString();
    QString corePath = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath();
    QFile::remove(corePath + QDir::separator() + "Plugins" + QDir::separator() + plug);
    settings.remove(plugin);
    settings.endGroup();
    loadPluginList();
    print_out_info("Plugin: " + plugin + " has been removed", true);
    QTextStream(stdout) << "Plugin: " + plugin + " has been removed" << endl;
}

/**
 * @brief opens the DialogRemovePlug
 */
void MainWindow::on_pushButtonRemove_2_clicked()
{
    if(!ui->listWidget_3->selectedItems().isEmpty()){
        QListWidgetItem * item = ui->listWidget_3->selectedItems().at(0);
        plugin = item->text();
        diaRemP = new DialogRemovePlug(this);
        diaRemP->setAttribute(Qt::WA_DeleteOnClose);
        diaRemP->setPlugin(plugin);
        QTextStream(stdout) << "DialogRemovePlug is running " << endl;
        diaRemP->exec();
    } else {
        print_out_info("No plugin has been selected", false);
    }
}

/**
 * @brief opens the DialogDepSil and sets up list
 */
void MainWindow::on_pushButtonSetup_clicked()
{
    diaDepSil = new DialogDepSil(this);
    diaDepSil->setAttribute(Qt::WA_DeleteOnClose);
    diaDepSil->setUpPeerList(peers);
    QTextStream(stdout) << "DialogDepSil is running" << endl;
    diaDepSil->exec();
}

/**
 * @brief opens the DialogDupPro
 */
void MainWindow::on_pushbuttonDep_clicked()
{
    diaDupPro = new DialogDupPro(this);
    diaDupPro->setAttribute(Qt::WA_DeleteOnClose);
    diaDupPro->setUpLists(peers, plugins);
    QTextStream(stdout) << "DialogDupPro is running" << endl;
    diaDupPro->exec();
}

/**
 * @brief gets the RSA-Key for the Proxy form the INI-File
 * @return
 */
QString MainWindow::getRSAPath()
{
    QSettings settings(settings_File, QSettings::NativeFormat);
    QString path = settings.value("rsa_path").toString();
    return path;
}

/**
 * @brief sets the RSA-Key for the Proxy to the INI-File
 * @param path
 */
void MainWindow::setRSAPath(QString path)
{
    QSettings settings(settings_File, QSettings::NativeFormat);
    QTextStream(stdout) << "RSA_Path is now: " + path << endl;
    settings.setValue("rsa_path", path);
}

/**
 * @brief gets the Host for the Proxy from the INI-File
 * @return
 */
QString MainWindow::getSSHpath(){
    QSettings settings(settings_File, QSettings::NativeFormat);
    QString path = settings.value("host").toString();
    return path;
}

/**
 * @brief sets the Host for the Proxy to the INI-File
 */
void MainWindow::setSSHpath(QString path){
    QSettings settings(settings_File, QSettings::NativeFormat);
    QTextStream(stdout) << "Host is now: " + path << endl;
    settings.setValue("host", path);
}

/**
 * @brief gets the Port for the Proxy from the INI-File
 * @return
 */
QString MainWindow::getPort(){
    QSettings settings(settings_File, QSettings::NativeFormat);
    QString path = settings.value("port").toString();
    return path;
}

/**
 * @brief sets the Port for the Proxy to the INI-File
 * @param path
 */
void MainWindow::setPort(QString path){
    QSettings settings(settings_File, QSettings::NativeFormat);
    QTextStream(stdout) << "Port is now: " + path << endl;
    settings.setValue("port", path);
}

/**
 * @brief gets the Hostname for the Proxy from the INI-File
 * @return
 */
QString MainWindow::getHostName(){
    QSettings settings(settings_File, QSettings::NativeFormat);
    QString path = settings.value("hostname").toString();
    return path;
}

/**
 * @brief sets the Hostname for the Proxy to the INI-File
 * @param path
 */
void MainWindow::setHostName(QString path){
    QSettings settings(settings_File, QSettings::NativeFormat);
    QTextStream(stdout) << "Hostname is now: " + path << endl;
    settings.setValue("hostname", path);
}

/**
 * @brief gets the User for the Proxy from the INI-File
 * @return
 */
QString MainWindow::getSSHUser(){
    QSettings settings(settings_File, QSettings::NativeFormat);
    QString path = settings.value("user").toString();
    return path;
}

/**
 * @brief sets the User for the Proxy to the INI-File
 * @param path
 */
void MainWindow::setSSHUser(QString path){
    QSettings settings(settings_File, QSettings::NativeFormat);
    QTextStream(stdout) << "User is now: " + path << endl;
    settings.setValue("user", path);
}

/**
 * @brief opens the DialogAdd
 */
void MainWindow::on_pushButton_clicked()
{
    if(!ui->listWidget->selectedItems().isEmpty()){
        QSettings settings(settings_File, QSettings::NativeFormat);
        QListWidgetItem * item = ui->listWidget->selectedItems().at(0);
        peer = item->text();

        settings.beginGroup(peer);
        QString host = settings.value("Host").toString();
        QString hostname = settings.value("HostName").toString();
        QString user = settings.value("User").toString();
        QString port = settings.value("Port").toString();
        QString rsa = settings.value("PubKey").toString();
        settings.endGroup();

        diaAdd = new DialogAdd(this);
        diaAdd->setEditLines(host,hostname,user,port,rsa);
        diaAdd->setAttribute(Qt::WA_DeleteOnClose);
        QTextStream(stdout) << "diaAdd is running now" << endl;
        diaAdd->exec();
    } else {
        print_out_info("No Peer has been selected", false);
    }
}

/**
 * @brief activats or deactivats the Proxy
 * @param state
 */
void MainWindow::setProxyState(bool state){
    QSettings settings(settings_File, QSettings::NativeFormat);
    settings.setValue("activated", state);
    if(state == true){
        QTextStream(stdout) << "Proxy has been activated" << endl;
        this->print_out_info("Proxy has been activated", true);
    } else {
        this->print_out_info("Proxy has been disabled", true);
        QTextStream(stdout) << "Proxy has been disabled" << endl;
    }
}

/**
 * @brief gets the Proxy State
 * @return
 */
bool MainWindow::getProxyState(){
    QSettings settings(settings_File, QSettings::NativeFormat);
    bool state = settings.value("activated").toBool();
    return state;
}

/**
 * @brief opens the DialogDelete
 */
void MainWindow::on_pushButtonDeinst_clicked()
{
    diadel = new DialogDelete(this);
    diadel->setAttribute(Qt::WA_DeleteOnClose);
    diadel->setUpLists(peers);
    QTextStream(stdout) << "diadel is running now" << endl;
    diadel->exec();
}
