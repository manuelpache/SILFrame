#include "sshhandler.h"
#include <QString>
#include <QFileInfo>
#include <QCoreApplication>
#include <QProcess>
#include "dialogdepsil.h"
#include "mainwindow.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <QStringList>
#include <QDebug>

/**
 * @brief the construtor of the class SshHandler
 */
SshHandler::SshHandler(QThread *parent) :
    QThread(parent)
{
    passAns = false;
    haveToWait = false;
    b1 = &haveToWait;
    b2 = &passAns;
}

/**
 * @brief changes the parameters, when the password process is over
 * @param answere
 */
void SshHandler::passAnswere(bool answere){
    *b2 = answere;
    *b1 = true;
}

/**
 * @brief this function handels most of the remote proceedings
 */
void SshHandler::mainCall(QStringList peers){
    QString settings_File = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath() + "/sil.ini";
    QSettings settings(settings_File, QSettings::NativeFormat);
    bool state = settings.value("activated").toBool();
    QString proxy = settings.value("hostname").toString();
    if(peers.contains(proxy) && state){
        peers.removeAll(proxy);
        emit sendMessage("Setup on Peer: " + proxy + " failed, Proxy and Peer can't be the same", false);
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
                                emit sendMessage("Something went wrong, Setup SIL has failed!", false);
                                emit doneWith();
                                return;
                            }
                        }
                    } else {
                        counter = 0;
                        QCoreApplication::processEvents();
                        emit sendMessage("Passphrase for peer: " + proxy + " (proxy) was entered incorrect", false);
                        if(counter == 0){
                            emit sendMessage("Something went wrong, Setup SIL has failed!", false);
                            emit doneWith();
                            return;
                        }
                    }
                }
            }
        }
        if(ssh_SILSetup(peers)){
            emit sendMessage("SIL-Setup has finished", true);
            emit doneWith();
            return;
        } else {
            emit sendMessage("Something went wrong, Setup SIL has failed!", false);
            emit doneWith();
            return;
        }
    }
    else {
        emit sendMessage("Something went wrong, the ini-File seems to be corrupted", false);
        emit sendMessage("Something went wrong, Setup SIL has failed!", false);
        emit doneWith();
        return;
    }
}

/**
 * @brief trys to run the deployment, if something goes wrong, it prints some infos
 * @return
 */
bool SshHandler::ssh_SILSetup(QStringList peers){
    bool isFirst = true;
    *b1 = false;
    *b2 = false;
    QString settings_File = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath() + "/sil.ini";
    QSettings settings(settings_File, QSettings::NativeFormat);
    bool state = settings.value("activated").toBool();
    QString proxy = settings.value("hostname").toString();
    for(int i = 0; i < peers.length(); i++){
        if(helper.ping_check(peers.at(i))){
            if(state){
                if(proxy == peers.at(i)){
                    emit sendMessage("Setup on Peer " + peers.at(i) + " failed, Proxy and Peer can't be the same", false);
                    if(i == (peers.length() -1)){
                        return true;
                    } else {
                        continue;
                    }
                }
            }
            int worked = copyHandler(peers.at(i), isFirst);
            if(worked == 1){
                emit passRequest(peers.at(i));
                while(!*b1){
                    sleep(1);
                }
                if(*b2){
                    int worked2 = copyHandler(peers.at(i), isFirst);
                    if(worked2 == 0){
                        emit step();
                        emit sendMessage("SIL-Setup on " + peers.at(i) + " has ended successfully", true);
                        isFirst = false;
                        QCoreApplication::processEvents();
                        if(i == (peers.length() -1)){
                            return true;
                        }
                        continue;
                    } else {
                        emit step();
                        emit sendMessage("SIL-Setup on " + peers.at(i) + " has ended with errors", false);
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
            if(worked == -1){
                emit step();
                emit sendMessage("SIL-Setup on " + peers.at(i) + " has ended with errors", false);
                QCoreApplication::processEvents();
                if(i == (peers.length() -1)){
                    return true;
                }
                continue;
            }
            if(worked == 0){
                emit step();
                emit sendMessage("SIL-Setup on " + peers.at(i) + " has ended successfully", true);
                isFirst = false;
                QCoreApplication::processEvents();
                if(i == (peers.length() -1)){
                    return true;
                }
                continue;
            }
        } else {
            emit step();
            emit sendMessage("Something went wrong, " + peers.at(i) + " seems to be offline", false);
            QCoreApplication::processEvents();
            if(i == (peers.length() -1)){
                return true;
            }
            continue;
        }
        return false;
    }
    return false;
}

/**
 * @brief uses the remote connection to setup the SIL-environment
 * @return
 */
int SshHandler::copyHandler(QString peer, bool firstOne){
    QString settings_File = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath() + "/sil.ini";
    QSettings settings(settings_File, QSettings::NativeFormat);
    QString middlewarepath = settings.value("middleware_path").toString();
    QString path = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath();
    QString copyPath = path + "/Middleware.tar.gz";
    QString middlewareTemp = path + "/middletemp";
    if(firstOne){
        if(QDir(middlewareTemp).exists()){
            QDir().rmdir(middlewareTemp);
        } else {
            QDir().mkdir(middlewareTemp);
        }
    }
    if(middlewarepath.isEmpty()){
        QFile(path + QDir::separator() + "Middleware.tar.gz").remove();
        QFile::copy(":/Middleware/Middleware.tar.gz", copyPath);
    } else {
        QFile(path + QDir::separator() + "Middleware.tar.gz").remove();
        QFile::copy(middlewarepath, copyPath);
        QFile middle(path + QDir::separator() + "Middleware.tar.gz");
        if(!middle.exists()){
            emit sendMessage("Something went wrong, Middleware couldn't be loaded", false);
            return -1;
        }
    }
    QFile::copy(":/Receiver/silReceiver.tar.gz", "silReceiver.tar.gz");

    QProcess commandExe;
    commandExe.waitForFinished();

    QString killandRemove = ("ssh -T -F ssh_config -oStrictHostKeyChecking=no -oBatchMode=yes "
                             + peer + " \"killall -q Middleware &> /dev/null; rm -rf ~/sil\" || exit");
    int killCode = commandExe.execute(killandRemove);
    if(killCode != 0){
        QTextStream(stdout) << "remote access failed, trying to get the passphrase now" << endl;
        return 1;
    }
    if(firstOne){
        QString commandDir = ("ssh -T -F ssh_config -oStrictHostKeyChecking=no "
                              + peer + " \"mkdir sil && cd sil && mkdir SIL && cd SIL && touch plugin_list \" || exit");
        int exitCode = commandExe.execute(commandDir);
        if(exitCode != 0){
            QTextStream(stdout) << "remove access failed again, the problem seems to be the system-requirements" << endl;
            return -1;
        }
        QString commandScp = ("scp -F ssh_config -oStrictHostKeyChecking=no Middleware.tar.gz "
                              + peer + ":~/sil/");
        std::string commandScpStd = commandScp.toStdString();
        int exitScp = system(commandScpStd.c_str());
        if(exitScp != 0){
            emit sendMessage("Scp failed on Peer: " + peer, false);
            return -1;
        }
        QString commandTar = ("ssh -T -F ssh_config -oStrictHostKeyChecking=no "
                              + peer + " \"cd ~/sil/ && tar -xvzf Middleware.tar.gz &> /dev/null && rm -f Middleware.tar.gz \" || exit");
        int tarCode = commandExe.execute(commandTar);
        if(tarCode != 0){
            return -1;
        }
        QString commandBuild = ("ssh -T -F ssh_config -oStrictHostKeyChecking=no "
                                + peer + " \"cd ~/sil/Middleware/Plugin_Interface && qmake && make -s &> /dev/null &&"
                                + " cd ../ && cd Middleware && qmake && make &> /dev/null \" || exit");
        int buildCode = commandExe.execute(commandBuild);
        if(buildCode != 0){
            emit sendMessage("Missing System Requirements on Peer: " + peer, false);
            return -1;
        }
        QString commandScp2 = ("scp -v -F ssh_config -oStrictHostKeyChecking=no silReceiver.tar.gz " + peer + ":~/sil");
        std::string commandScp2Std = commandScp2.toStdString();
        int exitReceiverScp = system(commandScp2Std.c_str());
        if(exitReceiverScp != 0){
            emit sendMessage("Scp receiver failed on Peer: " + peer, false);
            return -1;
        }
        QString commandReceiver = ("ssh -T -F ssh_config -oStrictHostKeyChecking=no "
                                   + peer + " \"cd ~/sil/ && tar -xvzf silReceiver.tar.gz &> /dev/null &&"
                                   + " rm -f silReceiver.tar.gz && cd silReceiver && qmake && make -s &> /dev/null \" || exit");
        int exitReceiver = commandExe.execute(commandReceiver);
        if(exitReceiver != 0){
            emit sendMessage("Missing System Requirements on Peer: " + peer, false);
            return -1;
        }
        QString commandCopyFrom = ("scp -r -F ssh_config -oStrictHostKeyChecking=no "
                                   + peer + ":~/sil " + path + "/middletemp");
        std::string commandCopyFromStd = commandCopyFrom.toStdString();
        int copyFrom = system(commandCopyFromStd.c_str());
        if(copyFrom != 0){
            return -1;
        } else {
            return 0;
        }
    } else {
        QString commandCopyTo = ("scp -r -F ssh_config -oStrictHostKeyChecking=no "
                                 + path + "/middletemp/sil " + peer + ":~/");
        std::string commandCopyToStd = commandCopyTo.toStdString();
        int copyTo = system(commandCopyToStd.c_str());
        if(copyTo != 0){
            int copyTo2 = commandExe.execute(commandCopyTo);
            if(copyTo2 != 0){
                return -1;
            } else {
                return 0;
            }
        } else {
            return 0;
        }
    }
}

