#include "mainwindow.h"
#include <QApplication>
#include <stdio.h>
#include <QFile>
#include <QtGlobal>

/**
 * @brief The Calling Main-Class
 * @param argc unused
 * @param argv unused
 * @return
 */
int main(int argc, char *argv[])
{
    QFile sil("sil.ini");
    if(!sil.exists()){
        QFile::copy(":/Config/sil.ini", "sil.ini");
        int num = system("chmod +wr sil.ini");
        (void)num;
        QTextStream(stdout) << "New INI-File was created" << endl;
    }
    QTextStream(stdout) << "SIL-FRAME is running" << endl;
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    QTextStream(stdout) << "MainWindow is running" << endl;
    w.loadPeerList();
    QTextStream(stdout) << "Peers have been loaded" << endl;
    w.loadPluginList();
    QTextStream(stdout) << "Plugins have been loaded" << endl;
    return a.exec();
}

