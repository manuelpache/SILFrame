#ifndef DIALOGREMOVE_H
#define DIALOGREMOVE_H

#include <QDialog>

namespace Ui {
class DialogRemove;
}

class DialogRemove : public QDialog
{
    Q_OBJECT

public:
    explicit DialogRemove(QWidget *parent = 0);
    void setPeer(QString);
    ~DialogRemove();

private slots:
    void on_pushButton_clicked();


private:
    Ui::DialogRemove *ui;
};

#endif // DIALOGREMOVE_H
