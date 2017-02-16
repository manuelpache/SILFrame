#ifndef DIALOGPASS_H
#define DIALOGPASS_H

#include <QDialog>
#include "sshhandler.h"
#include "passwhandler.h"

namespace Ui {
class DialogPass;
}

class DialogPass : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPass(QWidget *parent = 0);
    ~DialogPass();
    void setValues(QString);

private slots:
    void on_pushButton_clicked();

private:
    Ui::DialogPass *ui;
    bool passHandler();
    QString peer;
    int count;

signals:
    void returnAns(bool);

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    bool used;
    PasswHandler *handler;
};

#endif // DIALOGPASS_H
