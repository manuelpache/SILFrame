#ifndef DIALOGREMOVEPLUG_H
#define DIALOGREMOVEPLUG_H

#include <QDialog>

namespace Ui {
class DialogRemovePlug;
}

class DialogRemovePlug : public QDialog
{
    Q_OBJECT

public:
    explicit DialogRemovePlug(QWidget *parent = 0);
    void setPlugin(QString);
    ~DialogRemovePlug();

private slots:
    void on_pushButton_clicked();

private:
    Ui::DialogRemovePlug *ui;
};

#endif // DIALOGREMOVEPLUG_H
