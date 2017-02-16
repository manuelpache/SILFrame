#include <dialognew.h>
#include "ui_dialognew.h"
#include "QFileDialog"
#include "QString"
#include "QStringList"
#include "stdlib.h"
#include <unistd.h>
#include "QTextStream"
#include <iostream>
#include <sys/wait.h>
#include <mainwindow.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <QFile>

QString user = getenv("USER");
QString path;
QString name;
MainWindow * mainW;

/**
 * @brief The constructor of the class DialogNew
 * @param parent
 */
DialogNew::DialogNew(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNew)
{
    ui->setupUi(this);

    ui->path->setText("/home/" + user + "/");
    ui->name->setText("untitled");
    mainW = qobject_cast<MainWindow *>(this->parent());
    handler = new NewHandler();
    QObject::connect(handler, SIGNAL(sendToDialog(QString,bool)), this, SLOT(sendToMain(QString,bool)));
}

/**
 * @brief Destructor of the class DialogNew
 */
DialogNew::~DialogNew()
{
    delete ui;
}

/**
 * @brief opens a file-chooser Dialog
 */
void DialogNew::on_toolButton_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);
    QString start = "/home/" + user;
    dialog.setDirectory(start);
    dialog.exec();
    QStringList paths = dialog.selectedFiles();
    path = paths.at(0);
    DialogNew::ui->path->setText(path);

}

/**
 * @brief
 * This funktion generates a new Project, deploys the Middleware and builds a doxygen ducumentation
 */
void DialogNew::on_pushButton_clicked()
{
    QFile middle(mainW->getMiddlewarePath());
    if(mainW->getMiddlewarePath().isEmpty() || middle.exists()){
        name = DialogNew::ui->name->text();
        path = DialogNew::ui->path->text() + "/";
        QString middlePath = mainW->getMiddlewarePath();

        if(name.length() > 2 && path.length() > 5){

        if(handler->newProject(name, path, middlePath)){
            this->close();
        } else {
            this->close();
        }

        } else {
            if(name.length() <= 2){
                this->ui->name->setStyleSheet("QLineEdit{ border: 1px solid #ff0000; }");
            } else {
                this->ui->path->setStyleSheet("QLineEdit{ border: 1px solid #ff0000; }");
            }
        }
    } else {
        mainW->print_out_info("Middleware-path is wrong, please change it in the settings!", false);
        this->close();
        return;
    }
}

/**
 * @brief if the string in the name line-edit is longer that 2, the stylesheets gets removed
 * @param arg1
 */
void DialogNew::on_name_textChanged(const QString &arg1)
{
    (void)arg1;
    if(DialogNew::ui->name->text().length() > 2)
    {
        DialogNew::ui->name->setStyleSheet("");
    }
}

/**
 * @brief if the path line-edit is longer that 5, the stylesheet gets removed
 * @param arg1
 */
void DialogNew::on_path_textChanged(const QString &arg1)
{
    (void)arg1;
    if(DialogNew::ui->path->text().length() > 5)
    {
        DialogNew::ui->path->setStyleSheet("");
    }
}

/**
 * @brief sends a message to the mainwindow
 * @param mess
 * @param color
 */
void DialogNew::sendToMain(QString mess, bool color){
    mainW->print_out_info(mess, color);
}
