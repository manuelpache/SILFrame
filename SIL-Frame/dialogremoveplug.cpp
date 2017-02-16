#include "dialogremoveplug.h"
#include "ui_dialogremoveplug.h"
#include "mainwindow.h"

MainWindow * windowM;

/**
 * @brief the constructor of the class DialogRemovePlug
 * @param parent
 */
DialogRemovePlug::DialogRemovePlug(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogRemovePlug)
{
    ui->setupUi(this);
    windowM = qobject_cast<MainWindow *>(this->parent());
}

/**
 * @brief the destructor of the class DialogRemovePlug
 */
DialogRemovePlug::~DialogRemovePlug()
{
    delete ui;
}

/**
 * @brief removes a Plugin and closes the Dialog if the push-button is clicked
 */
void DialogRemovePlug::on_pushButton_clicked()
{
    windowM->removePlugin();
    this->close();
}

/**
 * @brief sets a Plugin to the line-edit
 * @param plug
 */
void DialogRemovePlug::setPlugin(QString plug){
    ui->lineEdit->setText(plug);
}
