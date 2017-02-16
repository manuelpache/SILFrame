#include "dialogremove.h"
#include "ui_dialogremove.h"
#include "mainwindow.h"

MainWindow * n_window;

/**
 * @brief the constructor of the class DialogRemove
 * @param parent
 */
DialogRemove::DialogRemove(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogRemove)
{
    ui->setupUi(this);
    n_window = qobject_cast<MainWindow *>(this->parent());
}

/**
 * @brief the destructor of the class DialogRemove
 */
DialogRemove::~DialogRemove()
{
    delete ui;
}

/**
 * @brief removes a Peer from list if push-button is clicked
 */
void DialogRemove::on_pushButton_clicked()
{
    n_window->removePeer();
    this->close();
}

/**
 * @brief sets a peer to the line-edit
 * @param peer
 */
void DialogRemove::setPeer(QString peer)
{
    this->ui->lineEdit->setText(peer);
}
