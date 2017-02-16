#include "newhandler.h"
#include <QString>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

/**
 * @brief constructor of the class NewHandler
 * @param parent
 */
NewHandler::NewHandler(QObject *parent) :
    QObject(parent)
{

}

/**
 * @brief trys to load a new Plugin to the list and saves it
 * @param name
 * @param path
 * @param middlewarePath
 * @return
 */
bool NewHandler::newProject(QString name, QString path, QString middlewarePath){
    std::string pathSt = path.toStdString();
    std::string upName = name.toUpper().toStdString();
    std::string lowName = name.toLower().toStdString();
    QString upFirstName = name;
    upFirstName.replace(0, 1, upFirstName[0].toUpper());
    std::string upFirstNameSt = upFirstName.toStdString();
    QString mktpath = path + upFirstName;
    QString copyPath = path + QDir::separator() + upFirstName + QDir::separator() + "Middleware.tar.gz";
    QString copyDoxyPath = mktpath + QDir::separator() + "Documentation/Doxyfile";

    QString exPath = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath();
    QFile::copy(":/Scripts/Scripts/genPro.sh", exPath + "/genPro.sh");
    int value = system("chmod +x genPro.sh");
    (void)value;
    QString fullpath = "genPro.sh";
    std::string fullpathSt = fullpath.toStdString();

    std::string doxypath = pathSt + upFirstNameSt + "/Documentation";
    std::string inputDox = pathSt + upFirstNameSt + "/Plugin/" + upFirstNameSt;

    /**
 * @brief builds a fork-process, so the execl of the Script can run simultaneously to the main-process
 */
    QDir dir(mktpath);
    if (!dir.exists()) {
        dir.mkpath(".");
    } else {
        emit sendToDialog("Something went wrong, Plugin-Path already exists", false);
        return false;
    }
    QDir dirDoc(mktpath + QDir::separator() + "Documentation");
    if (!dirDoc.exists()) {
        dirDoc.mkpath(".");
    }
    pid_t pid = fork();
    if(pid == -1){
        printf("Something went wrong with the fork-process");
        exit(0);
    }
    if(pid == 0){
        /**
     * @brief execl calls a Script, which generates a new Project
     */
        if(middlewarePath.isEmpty()){
            QFile::copy(":/Middleware/Middleware.tar.gz", copyPath);
        } else {
            QFile::copy(middlewarePath, copyPath);

        }
        QFile::copy(":/Doxygen/Scripts/Doxyfile", copyDoxyPath);
        execl(fullpathSt.c_str(),fullpathSt.c_str(),upFirstNameSt.c_str(), pathSt.c_str(), upName.c_str(), doxypath.c_str(), inputDox.c_str(),lowName.c_str(), (char*) 0);

    } else {
        /**
     * @brief waits for the child-process zu finish, so there can't be any problems after
     */
        int returnStatus;
        waitpid(pid, &returnStatus, 0);
        emit sendToDialog("Plugin: "+ name +" has been generated", true);

        return true;
    }
    return false;
}
