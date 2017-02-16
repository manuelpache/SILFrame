#include "deletehandler.h"
#include <QString>
#include <QStringList>
#include <QProcess>
#include <QThread>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>

/**
 * @brief The constructor of the Class DeleteHandler
 * @param parent
 */
DeleteHandler::DeleteHandler(QThread *parent) :
    QThread(parent)
{

    passAns = false;
    haveToWait = false;
    b1 = &haveToWait;
    b2 = &passAns;
}

/**
 * @brief the destructor of the class DeleteHandler
 */
DeleteHandler::~DeleteHandler(){
    this->deleteLater();
}

/**
 * @brief This Slot is called when the password-check is over
 * @param answere
 */
void DeleteHandler::passAnswere(bool answere){
    *b2 = answere;
    *b1 = true;
}

/**
 * @brief This Slot is called to start the delete-process of one or more Plugins from a Peer
 * @param peers
 * @param plugins
 */
void DeleteHandler::mainCall(QStringList peers, QStringList plugins){
    QString settings_File = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath() + "/sil.ini";
    QSettings settings(settings_File, QSettings::NativeFormat);
    bool state = settings.value("activated").toBool();
    QString proxy = settings.value("hostname").toString();
    if(peers.contains(proxy) && state){
        emit sendMessage("Remove Plugins from Peer: " + proxy + " failed, Proxy and Peer can't be the same", false);
        emit doneLoad();
        QCoreApplication::processEvents();
        return;
    }
    if(helper.config_builder(peers)){
        emit step();
        QCoreApplication::processEvents();
        if(state){
            if(!helper.ping_check(proxy)){
                emit sendMessage("Something went wrong, " + proxy + " (proxy) seems to be offline", false);
                emit doneWith();
                return;
            }
            int counter = peers.length();
            for(int t = 0; t < peers.length(); t++){
                if(helper.rsa_proxykey_handler(peers.at(t))){

                } else {
                    emit passRequest(proxy);
                    while(!*b1){
                        sleep(1);
                    }
                    if(*b2){
                        if(helper.rsa_proxykey_handler(peers.at(t))){
                        } else {
                            counter --;
                            QCoreApplication::processEvents();
                            emit sendMessage("RSA_key setup for " + peers.at(t) + " has failed", false);
                            if(counter == 0){
                                emit sendMessage("Something went wrong, Delete Plugins has failed!", false);
                                emit doneWith();
                                return;
                            }
                        }
                    } else {
                        counter = 0;
                        QCoreApplication::processEvents();
                        emit sendMessage("Passphrase for peer: " + proxy + " (proxy) was entered incorrect", false);
                        if(counter == 0){
                            emit sendMessage("Something went wrong, Delete Plugins has failed!", false);
                            emit doneWith();
                            return;
                        }
                    }
                }
            }
        }
        if(ssh_SILSetup(peers, plugins)){
            emit doneWith();
            return;
        } else {
            emit sendMessage("Something went wrong, Delete Plugins has failed!", false);
            emit doneWith();
            return;
        }
    }
    else {
        emit sendMessage("Something went wrong, the ini-File seems to be corrupted", false);
        emit sendMessage("Something went wrong, Delete Plugins has failed!", false);
        emit doneWith();
        return;
    }
}

/**
 * @brief This function checks for some possible errors and starts the actual delete process
 * @param peers
 * @param plugins
 * @return
 */
bool DeleteHandler::ssh_SILSetup(QStringList peers, QStringList plugins){
    *b1 = false;
    *b2 = false;
    for(int i = 0; i < peers.length(); i++){
        if(helper.ping_check(peers.at(i))){
            if(!copyHandler(peers.at(i), plugins)){
                emit passRequest(peers.at(i));
                while(!*b1){
                    sleep(1);
                }
                if(*b2){
                    if(copyHandler(peers.at(i), plugins)){
                        emit step();
                        emit sendMessage("Delete Plugins on " + peers.at(i) + " has ended successfully", true);
                        QCoreApplication::processEvents();
                        if(i == (peers.length() -1)){
                            return true;
                        }
                        continue;
                    } else {
                        emit step();
                        emit sendMessage("Delete Plugins on " + peers.at(i) + " has ended with errors", false);
                        QCoreApplication::processEvents();
                        if(i == (peers.length() -1)){
                            return true;
                        }
                        continue;
                    }
                } else {
                    emit step();
                    emit sendMessage("Passphrase for peer: " + peers.at(i) + " was entered incorrect", false);
                    QCoreApplication::processEvents();
                    if(i == (peers.length() -1)){
                        return true;
                    }
                    continue;
                }
            }
            emit step();
            emit sendMessage("Delete Plugins on " + peers.at(i) + " has ended successfully", true);
            QCoreApplication::processEvents();
            if(i == (peers.length() -1)){
                return true;
            }
            continue;
        } else {
            emit step();
            emit sendMessage("Something went wrong, " + peers.at(i) + " seems to be offline", false);
            QCoreApplication::processEvents();
            if(i == (peers.length() -1)){
                return true;
            }
            continue;
        }
    }
    return false;
}

/**
 * @brief This function uses the ssh-protocol to delete plugins from a peer
 * @param peer
 * @param plugins
 * @return
 */
bool DeleteHandler::copyHandler(QString peer, QStringList plugins){
    int exitCode;
    for(int i = 0; i < plugins.length(); i++){

        QString transferCommand = ("ssh -T -F ssh_config -oStrictHostKeyChecking=no -oBatchMode=yes "
                                   + peer + " \" ~/sil/silReceiver/silReceiver rm " + plugins.at(i) + " \" || exit");
        QProcess commandExe;
        commandExe.waitForFinished();
        exitCode = commandExe.execute(transferCommand);
        if(exitCode != 0){
            emit sendMessage("Could not delete " + plugins.at(i), false);
            return false;
        } else {
            emit step();
            emit sendMessage(plugins.at(i) + " has been deleted", true);
            QCoreApplication::processEvents();
        }
    }
    if(exitCode == 0){
        return true;
    } else {
        return false;
    }
}

/**
 * @brief This function is used to start the loading of installed plugins from a selected peer and checks for some errors
 * @param peers
 */
void DeleteHandler::getPlugins(QStringList peers){
    QString settings_File = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath() + "/sil.ini";
    QSettings settings(settings_File, QSettings::NativeFormat);
    bool state = settings.value("activated").toBool();
    QString proxy = settings.value("hostname").toString();
    if(peers.contains(proxy) && state){
        emit textSet("Loading Plugins from Peer: " + proxy + " failed, Proxy and Peer can't be the same");
        emit doneLoad();
        QCoreApplication::processEvents();
        return;
    }
    if(helper.ping_check(peers.at(0))){
        if(!helper.config_builder(peers)){
            emit doneLoad();
            QCoreApplication::processEvents();
            return;
        }
        if(state){
            if(!helper.ping_check(proxy)){
                emit doneLoad();
                emit textSet("Proxy seems to be offline");
                QCoreApplication::processEvents();
                return;
            }
            if(!helper.rsa_proxykey_handler(peers.at(0))){
                emit passRequest(proxy);
                while(!*b1){
                    sleep(1);
                }
                if(*b2){
                    if(!helper.rsa_proxykey_handler(peers.at(0))){
                        emit doneLoad();
                        QCoreApplication::processEvents();
                        return;
                    }
                } else {
                    emit doneLoad();
                    QCoreApplication::processEvents();
                    return;
                }
            }
        }
        *b1 = false;
        *b2 = false;
        if(!loader(peers.at(0))){
            emit passRequest(peers.at(0));
            while(!*b1){
                sleep(1);
            }
            if(*b2){
                if(loader(peers.at(0))){
                    emit doneLoad();
                    QCoreApplication::processEvents();
                    return;
                } else {
                    emit doneLoad();
                    emit textSet("Something went wrong, couldn't load plugins");
                    QCoreApplication::processEvents();
                    return;
                }
            } else {
                emit doneLoad();
                emit textSet("Password was wrong, couldn't load plugins");
                QCoreApplication::processEvents();
                return;
            }
        }
    } else {
        emit doneLoad();
        emit textSet("Selected peer seems to be offline");
        QCoreApplication::processEvents();
        return;
    }
}

/**
 * @brief this function uses the ssh-protcol to copy the list of installed plugins
 * @param peer
 * @return
 */
bool DeleteHandler::loader(QString peer){
    QString path = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath();
    QString transferCommand = ("scp -F ssh_config -oStrictHostKeyChecking=no -oBatchMode=yes "
                               + peer + ":~/sil" + QDir::separator()
                               + "SIL" + QDir::separator() + "plugin_list " + path + QDir::separator() + "plugin_list || exit");
    std::string transferCommandStd = transferCommand.toStdString();
    int exitCode = system(transferCommandStd.c_str());
    if(exitCode == 0){
        emit foundPlugs();
        emit doneLoad();
        QCoreApplication::processEvents();
        return true;
    } else {
        emit doneLoad();
        QCoreApplication::processEvents();
        return false;
    }
}
