#include "samepos.h"
#include "ui_samepos.h"

samePos::samePos(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::samePos)
{
    ui->setupUi(this);
}

samePos::~samePos()
{
    delete ui;
}

void samePos::on_addButton_clicked()
{
    accept();
}

void samePos::on_cancelButton_clicked()
{
    reject();
}
