#include "dialogaddplug.h"
#include "ui_dialogaddplug.h"
#include <QFileDialog>
#include <mainwindow.h>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QDebug>
#include <QStyle>

MainWindow * m_Window;

/**
 * @brief the constructor of the class DialogAddPlug
 * @param parent
 */
DialogAddPlug::DialogAddPlug(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAddPlug)
{
    ui->setupUi(this);
    m_Window = qobject_cast<MainWindow *>(this->parent());
    handler = new AddPlugHandler();
    QObject::connect(handler, SIGNAL(sendToDialog(QString,bool)), this, SLOT(sendToMain(QString,bool)));
    QObject::connect(handler, SIGNAL(sendProName(QString)), this, SLOT(callMainToAdd(QString)));
}

/**
 * @brief the destructor of the class DialogAddPlug
 */
DialogAddPlug::~DialogAddPlug()
{
    delete ui;
}

/**
 * @brief Opens a File-selector-Dialog
 */
void DialogAddPlug::on_toolButton_clicked()
{
    QFileDialog dialog(this);
    QString user = getenv("USER");
    dialog.setFileMode(QFileDialog::Directory);
    QString start = "/home/" + user;
    dialog.setDirectory(start);
    dialog.exec();
    QStringList paths = dialog.selectedFiles();
    QString path = paths.at(0);
    ui->lineEdit->setText(path);
}

/**
 * @brief slot for the response of a push_button_click, calls the AddPlugHandler
 */
void DialogAddPlug::on_pushButton_clicked()
{
    if(handler->addPlugin(ui->lineEdit->text())){
        ui->lineEdit->clear();
        this->close();
    } else {
        ui->lineEdit->clear();
        this->close();
    }
}

/**
 * @brief sends a messange to the output of the mainwindow-class
 * @param mess
 * @param color
 */
void DialogAddPlug::sendToMain(QString mess, bool color){
    m_Window->print_out_info(mess, color);
}

/**
 * @brief calls the mainwindow-class to add a plugin
 * @param proName
 */
void DialogAddPlug::callMainToAdd(QString proName){
    m_Window->addPlugin(proName + ".tar.gz", proName);
}
