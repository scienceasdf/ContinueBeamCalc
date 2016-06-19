#ifndef SAMEPOS_H
#define SAMEPOS_H

#include <QDialog>

namespace Ui {
class samePos;
}

class samePos : public QDialog
{
    Q_OBJECT

public:
    explicit samePos(QWidget *parent = 0);
    ~samePos();

private slots:
    void on_addButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::samePos *ui;
};

#endif // SAMEPOS_H
