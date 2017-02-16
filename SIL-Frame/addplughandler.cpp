#include "addplughandler.h"
#include <QString>
#include <QStringList>
#include <QFileInfoList>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QCoreApplication>

/**
 * @brief The construktor of the Class AddPlugHandler
 * @param parent
 */
AddPlugHandler::AddPlugHandler(QObject *parent) :
    QObject(parent)
{

}

/**
 * @brief This Function loads a seleced Plugin and checks if the Plugin provides the needed Code
 * @param pathForPlug
 * @return
 */
bool AddPlugHandler::addPlugin(QString pathForPlug)
{
    QString pathPlug = pathForPlug;
    QFileInfoList list = QDir(pathPlug).entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries);

    /**
     * Check if Dir is empty
     */
    if(list.count() == 0){
        emit sendToDialog("The choosen Plugin directory is empty", false);
        return false;
    }
    bool pro = false;
    bool cpp = false;
    QString proFile;
    QString proName;

    /**
     * check if Dir contains a pro and at least one cpp file
     */
    for(int i = 0; i < list.length(); i++){
        if(list.at(i).completeSuffix() == "pro"){
            QString test = list.at(i).fileName();
            QString test2 = list.at(i).baseName();
            pro = true;
            if(test != "Test.pro" && !(test.contains("Main"))){
                proFile = test;
                proName = test2;
            }
        }
        if(list.at(i).completeSuffix() == "cpp"){
            cpp = true;
        }
    }
    if(!(pro && cpp)){
        emit sendToDialog("The choosen Plugin directory does not contain the required Files", false);
        return false;
    }

    /**
     * @brief check if pro file contains needed code
     */
    QString pathPro = pathPlug + "/" + proFile;
    QFile checkPro(pathPro);
    checkPro.open(QIODevice::ReadWrite);
    QString searchString("/../../build/ -lPluginInterface");
    QString searchString2("TEMPLATE = lib");
    QTextStream in (&checkPro);
    const QString content = in.readAll();
    if(!(content.contains(searchString, Qt::CaseSensitive)) || !(content.contains(searchString2, Qt::CaseSensitive))){
        emit sendToDialog("The choosen Plugin Pro-File does not contain the required Code", false);
        return false;
    }

    QString corePath = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath();
    QString dirPath = corePath + QDir::separator() + "Plugins" + QDir::separator() + proName;
    QDir dirPlug(corePath + QDir::separator() + "Plugins");
    if (!dirPlug.exists()) {
        dirPlug.mkpath(".");
    }
    QDir dir = QDir::root();
    dir.mkdir(dirPath);
    for(int i = 0; i < list.length(); i++){
        QString file = list.at(i).fileName();
        QString copyPath = dirPath + QDir::separator() + file;
        QString rootFile = pathPlug + QDir::separator() + file;
        if(list.at(i).isDir()){
            QString copyDir = "cp -R " + rootFile + " " + copyPath;
            std::string copyDirStd = copyDir.toStdString();
            int num = system(copyDirStd.c_str());
            (void)num;
        }
        QFile::copy(rootFile, copyPath);
    }
    /**
     * @brief removes Doxygen line, which is not needed on sil-peers
     * @return
     */
    QFile fpro(dirPath + QDir::separator() + proFile);
    if(fpro.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QString spro;
        QTextStream tpro(&fpro);
        while(!tpro.atEnd())
        {
            QString line = tpro.readLine();
            if(!line.contains("doxygen"))
                spro.append(line + "\n");
        }
        fpro.resize(0);
        tpro << spro;
        fpro.close();
    }

    /**
     * @brief removes Test and Main_*.pro Files, thses are not needed on the peers
     * @return
     */
    QDir plugDir(dirPath);
    QStringList entrys = plugDir.entryList();
    for(int i = 0; i < entrys.length(); i++){
        if(entrys.at(i).contains("Main_") || entrys.at(i).contains("Test.pro") || entrys.at(i).contains("tst_")){
            QFile::remove(dirPath + QDir::separator() + entrys.at(i));
        }
    }

    std::string tarPath = "cd "+ corePath.toStdString()+ "/Plugins && " + "tar -zcf "+proName.toStdString()+".tar.gz "
            +proName.toStdString();
    const char * tarc = tarPath.c_str();
    int num1 = system(tarc);
    (void)num1;
    std::string del ="cd "+ corePath.toStdString()+ "/Plugins &&  rm -r "+ proName.toStdString();
    const char * delc = del.c_str();
    int num2 = system(delc);
    (void)num2;
    emit sendProName(proName);
    return true;
}

