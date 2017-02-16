#include "passwhandler.h"
#include <QString>
#include <QSettings>
#include <QFile>
#include <QCoreApplication>
#include <QFileInfo>
#include <QTextStream>

/**
 * @brief constructor of the class PasswHandler
 * @param parent
 */
PasswHandler::PasswHandler(QObject *parent) :
    QObject(parent)
{

}

/**
 * @brief trys to use the password, if it does not work, the user has 3 chances
 * @param peer
 * @param pass
 * @return
 */
bool PasswHandler::handlePassW(QString peer, QString pass){
    QString settings_File = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath() + "/sil.ini";
    QSettings settings(settings_File, QSettings::NativeFormat);
    bool active = settings.value("activated").toBool();
    QString proxy = settings.value("hostname").toString();
    QFile script("try.sh");
    if(!script.exists()){
        QFile::copy(":/Scripts/Scripts/try.sh", "try.sh");
        int val1 = system("chmod +x try.sh");
        (void)val1;
    }
    if(active){
        if(peer == proxy){
            settings.beginGroup(peer);
            QString file = settings.value("rsa_path").toString();
            settings.endGroup();
            QString passphrase = pass;
            QString restart = "echo " + passphrase +  " | SSH_ASKPASS=./try.sh ssh-add " + file;
            std::string restartStd = restart.toStdString();
            int returnStatus = system(restartStd.c_str());
            if(returnStatus != 0){
                passphrase = "";
                QTextStream(stdout) << "password was wrong" << endl;
                return false;
            } else {
                passphrase = "";
                QTextStream(stdout) << "password was right" << endl;
                return true;
            }
        } else {
            QString peerRSA = peer;
            peerRSA.remove(".");
            QString file = "~/.ssh/id_" + peerRSA;
            QString passphrase = pass;
            QString restart = "echo " + passphrase +  " | SSH_ASKPASS=./try.sh ssh-add " + file;
            std::string restartStd = restart.toStdString();
            int returnStatus = system(restartStd.c_str());
            if(returnStatus != 0){
                passphrase = "";
                QTextStream(stdout) << "password was wrong" << endl;
                return false;
            } else {
                passphrase = "";
                QTextStream(stdout) << "password was right" << endl;
                return true;
            }
        }
    } else {
        settings.beginGroup(peer);
        QString file = settings.value("PubKey").toString();
        settings.endGroup();
        QString passphrase = pass;
        QString restart = "echo " + passphrase +  " | SSH_ASKPASS=./try.sh ssh-add " + file;
        std::string restartStd = restart.toStdString();
        int returnStatus = system(restartStd.c_str());
        if(returnStatus != 0){
            passphrase = "";
            QTextStream(stdout) << "password was wrong" << endl;
            return false;
        } else {
            passphrase = "";
            QTextStream(stdout) << "password was right" << endl;
            return true;
        }
    }
}
