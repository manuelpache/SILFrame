#include "pluginhandler.h"
#include <QStringList>
#include <QString>
#include <QApplication>
#include <QSettings>
#include <QProcess>
#include <QDir>
#include <QTextStream>

/**
 * @brief the construtor of the class PluginHandler
 * @param parent
 */
PluginHandler::PluginHandler(QThread *parent) :
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
void PluginHandler::passAnswere(bool answere){
    *b2 = answere;
    *b1 = true;
}

/**
 * @brief this function handels most of the remote proceedings
 * @param peers
 * @param plugins
 */
void PluginHandler::mainCall(QStringList peers, QStringList plugins){
    QString settings_File = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath() + "/sil.ini";
    QSettings settings(settings_File, QSettings::NativeFormat);
    bool state = settings.value("activated").toBool();
    QString proxy = settings.value("hostname").toString();
    if(peers.contains(proxy) && state){
        peers.removeAll(proxy);
        emit sendMessage("Deployment on Peer: " + proxy + " failed, Proxy and Peer can't be the same", false);
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
                    QTextStream(stdout) << "password request for proxy" << endl;
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
        if(ssh_SILSetup(peers, plugins)){
            emit sendMessage("Plugin Deployment has finished", true);
            emit doneWith();
            return;
        } else {
            emit sendMessage("Something went wrong, Plugin Deployment has failed!", false);
            emit doneWith();
            return;
        }
    }
    else {
        emit sendMessage("Something went wrong, the ini-File seems to be corrupted", false);
        emit sendMessage("Something went wrong, Plugin Deployment has failed!", false);
        emit doneWith();
        return;
    }
}

/**
 * @brief trys to run the sil-setup, if something goes wrong, it prints some infos
 * @param peers
 * @param plugins
 * @return
 */
bool PluginHandler::ssh_SILSetup(QStringList peers, QStringList plugins){
    *b1 = false;
    *b2 = false;
    bool firstOne = true;
    for(int i = 0; i < peers.length(); i++){
        if(helper.ping_check(peers.at(i))){
            int worked = copyHandler(peers.at(i), plugins, firstOne);
            if(worked == 1){
                emit passRequest(peers.at(i));
                while(!*b1){
                    sleep(1);
                }
                if(*b2){
                    int worked2 = copyHandler(peers.at(i), plugins, firstOne);
                    if(worked2 == 0){
                        emit step();
                        emit sendMessage("Plugin Deployment on " + peers.at(i) + " has ended successfully", true);
                        firstOne = false;
                        QCoreApplication::processEvents();
                        if(i == (peers.length() -1)){
                            return true;
                        }
                        continue;
                    } else {
                        emit step();
                        emit sendMessage("Plugin Deployment on " + peers.at(i) + " has ended with errors", false);
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
            if(worked == 0){
                emit step();
                emit sendMessage("Plugin Deployment on " + peers.at(i) + " has ended successfully", true);
                firstOne = false;
                QCoreApplication::processEvents();
                if(i == (peers.length() -1)){
                    return true;
                }
                continue;
            }
            if(worked == -1){
                emit step();
                emit sendMessage("Plugin Deployment on " + peers.at(i) + " has ended with errors", false);
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
    }
    return false;
}

/**
 * @brief uses to remote connection to setup the SIL-environment
 * @param peer
 * @param plugins
 * @return
 */
int PluginHandler::copyHandler(QString peer, QStringList plugins, bool firstOne){
    QString path = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath();
    int exitCode;
    int exitCodeTar;
    int exitCodeRec;
    int exitCopyBack;
    int exitCopyRight;
    int exitRecCopy;
    for(int i = 0; i < plugins.length(); i++){
        QString pluginlocal = path + QDir::separator() + "pluginTemp/";
        if(firstOne){
            if(QDir(pluginlocal).exists()){

            } else {
                QDir().mkdir(pluginlocal);
            }
        }
        QString pluginCop = plugins.at(i) + ".tar.gz";
        QString pluginPath = path + "/Plugins/" + pluginCop;

        QProcess commandExe;
        commandExe.waitForFinished();
        if(firstOne){
            QString transferCommand = ("scp -F ssh_config -oStrictHostKeyChecking=no -oBatchMode=yes " + pluginPath + " "
                                       + peer + ":~/sil/SIL");
            std::string transferCommandStd = transferCommand.toStdString();
            exitCode = system(transferCommandStd.c_str());
            if(exitCode != 0){
                QTextStream(stdout) << "remote access failed, trying to get the passphrase now" << endl;
                return 1;
            }
            QString unpackCommand = ("ssh -T -F ssh_config -oStrictHostKeyChecking=no "
                                     + peer + " \"cd ~/sil/SIL && tar -xvzf " + pluginCop + " &> /dev/null && rm -f " + pluginCop +" \" || exit");
            exitCodeTar = commandExe.execute(unpackCommand);
            if(exitCodeTar != 0){
                QTextStream(stdout) << "remove access failed again, the problem seems to be the system-requirements" << endl;
                return -1;
            }
            QString receiverCommand = ("ssh -T -F ssh_config -oStrictHostKeyChecking=no "
                                       + peer + " \"nohup ~/sil/silReceiver/silReceiver add " + plugins.at(i) + " \" || exit");
            exitCodeRec = commandExe.execute(receiverCommand);
            if(exitCodeRec != 0){
                emit sendMessage("Missing System Requirements on Peer: " + peer, false);
                return -1;
            }
            QString copyBackCommand = ("scp -r -F ssh_config -oStrictHostKeyChecking=no " + peer + ":~/sil/SIL/"
                                       + plugins.at(i) + " " + pluginlocal);
            std::string copyBackCommandStd = copyBackCommand.toStdString();
            exitCopyBack = system(copyBackCommandStd.c_str());
            if(exitCopyBack != 0){
                return -1;
            } else {
                emit step();
                QCoreApplication::processEvents();
            }
        } else {
            QString copyRightBackCommand = ("scp -r -F ssh_config -oStrictHostKeyChecking=no " + pluginlocal + QDir::separator()
                                            + plugins.at(i) + " " + peer + ":~/sil/SIL");
            std::string copyRightBackCommandStd = copyRightBackCommand.toStdString();
            exitCopyRight = system(copyRightBackCommandStd.c_str());
            if(exitCopyRight != 0){
                return 1;
            }
            QString receiverCopyCommand = ("ssh -T -F ssh_config -oStrictHostKeyChecking=no "
                                           + peer + " \"nohup ~/sil/silReceiver/silReceiver copy " + plugins.at(i) + " \" || exit");
            exitRecCopy = commandExe.execute(receiverCopyCommand);
            if(exitRecCopy != 0){
                return -1;
            } else {
                emit step();
                QCoreApplication::processEvents();
            }
        }
    }
    if(firstOne){
        if(exitCode == 0 && exitCodeTar == 0 && exitCodeRec == 0){
            return 0;
        } else {
            return -1;
        }
    } else {
        if(exitCopyRight == 0 && exitRecCopy == 0){
            return 0;
        } else{
            return -1;
        }
    }
}
