#include "sshhelper.h"
#include <QStringList>
#include <QSettings>
#include <QFileInfo>
#include <QCoreApplication>
#include <QString>
#include <QProcess>

/**
 * @brief the constructor of the class Sshhelper
 * @param parent
 */
Sshhelper::Sshhelper(QThread *parent) :
    QThread(parent)
{

}

/**
 * @brief sets up the ssh_config-file
 * @param peerList
 * @return
 */
bool Sshhelper::config_builder(QStringList peerList){
    QString settings_File = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath() + "/sil.ini";
    QSettings settings(settings_File, QSettings::NativeFormat);
    bool state = settings.value("activated").toBool();
    QString proxHost = settings.value("host").toString();
    QString proxHostName = settings.value("hostname").toString();
    QString proxUser = settings.value("user").toString();
    QString proxPort = settings.value("port").toString();
    QString proxRSA = settings.value("rsa_path").toString();
    if(proxPort.isEmpty()){
        proxPort = "22";
    }
    if(settings.status() == QSettings::NoError){
        if(state){
            QString proxCommand =
                    "echo \"Host "
                    + proxHost + "\nHostName "
                    + proxHostName + "\nUser "
                    + proxUser + "\nPort "
                    + proxPort + "\nIdentityFile "
                    + proxRSA +"\n\"> ssh_config";
            std::string proxCommandStd = proxCommand.toStdString();
            int val2 = system(proxCommandStd.c_str());
            (void)val2;
            for(int i = 0; i < peerList.size(); i++){
                settings.beginGroup(peerList.at(i));
                QString host = settings.value("Host").toString();
                QString hostName = settings.value("HostName").toString();
                QString port = settings.value("Port").toString();
                QString user = settings.value("User").toString();
                QString hostNameCop = hostName;
                QString fileName = hostNameCop.remove(".");
                if(port.isEmpty()){
                    port = "22";
                }
                settings.endGroup();
                QString command =
                        "echo \"Host "
                        + host + "\nHostName "
                        + hostName + "\nUser "
                        + user +"\nPort "
                        + port + "\nIdentityFile "
                        + "~/.ssh/id_" + fileName +".pub"
                        + "\nProxyCommand ssh -T -F ssh_config -oStrictHostKeyChecking=no -p "
                        + proxPort + " " + proxHostName + " nc %h %p \n\">> ssh_config";
                std::string commandStd = command.toStdString();
                int val1 = system(commandStd.c_str());
                (void)val1;
            }
        } else {
            for(int i = 0; i < peerList.length(); i++){
                settings.beginGroup(peerList.at(i));
                QString host = settings.value("Host").toString();
                QString hostName = settings.value("HostName").toString();
                QString port = settings.value("Port").toString();
                QString user = settings.value("User").toString();
                QString rsa = settings.value("PubKey").toString();
                if(port.isEmpty()){
                    port = "22";
                }
                settings.endGroup();
                if(i == 0){
                    QString command1 =
                            "echo \"Host "
                            + host + "\nHostName "
                            + hostName + "\nUser "
                            + user + "\nPort "
                            + port + "\nIdentityFile "
                            + rsa + "\n\"> ssh_config";
                    std::string commandStd1 = command1.toStdString();
                    int val = system(commandStd1.c_str());
                    (void)val;
                } else {
                    QString command2 =
                            "echo \"Host "
                            + host + "\nHostName "
                            + hostName + "\nUser "
                            + user + "\nPort "
                            + port + "\nIdentityFile "
                            + rsa +"\n\">> ssh_config";
                    std::string commandStd2 = command2.toStdString();
                    int val3 = system(commandStd2.c_str());
                    (void)val3;
                }
            }
        }
        return true;
    } else {
        return false;
    }
}

/**
 * @brief copys the needed ssh-keys from the Proxy if one is used
 * @param peer
 * @return
 */
bool Sshhelper::rsa_proxykey_handler(QString peer){
    QString settings_File = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath() + "/sil.ini";
    QSettings settings(settings_File, QSettings::NativeFormat);
    QString proxy = settings.value("hostname").toString();
    settings.beginGroup(peer);
    QString remoteKey = settings.value("PubKey").toString();
    QString remoteKeyCop = remoteKey;
    QString remotepriv = remoteKeyCop.remove(".pub");
    QString user = settings.value("User").toString();
    QString fileName = peer.remove(".");
    settings.endGroup();
    QString command = "scp -oStrictHostKeyChecking=no " + user + "@" + proxy + ":" + remoteKey + " ~/.ssh/id_" + fileName + ".pub";
    std::string commandStd = command.toStdString();
    QString command2 = "scp -oStrictHostKeyChecking=no " + user + "@" + proxy + ":" + remotepriv + " ~/.ssh/id_" + fileName;
    std::string commandStd2 = command2.toStdString();
    int exitCode = system(commandStd.c_str());
    int exitCode2 = system(commandStd2.c_str());
    if(exitCode == 0 && exitCode2 == 0){
        return true;
    } else {
        return false;
    }
}

/**
 * @brief this function is used to check if a peer or proxy is online
 * @param peer
 * @return
 */
bool Sshhelper::ping_check(QString peer){
    QString parameter = "-c 1 -q";
    QProcess proc;
    proc.waitForFinished();
    int exitCode = proc.execute("ping", QStringList() << parameter << peer);
    if (exitCode == 0) {
        return true;
    } else {
        return false;
    }
}
