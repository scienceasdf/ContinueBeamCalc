#ifndef WIDGET_H
#define WIDGET_H

#include<QMap>
#include <QWidget>
#include<QTimer>
#include<unordered_map>
#include<set>
#include<QWheelEvent>
#include"beam.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    continBeam* beam1;
    double ratio=1;
private:
    Ui::Widget *ui;
    void drawBeam(QPainter& painter);
protected:
    void paintEvent(QPaintEvent* event);
    bool boolScale=true;
private slots:
    void on_pushButton_2_clicked();
    void on_IzText_textChanged(const QString &arg1);
    void on_lengthText_textChanged(const QString &arg1);
    void on_ElaText_textChanged(const QString &arg1);
    void on_comboBox_currentIndexChanged(int index);
    void on_pushButton_3_clicked();

    void wheelEvent(QWheelEvent* event);
    void mouseMoveEvent(QMouseEvent *e);
    void on_savePicButton_clicked();
    void on_saveFileButton_clicked();

    void on_scaleOpt_clicked();
    void on_addResButton_clicked();
    void on_calcButton_clicked();
    void on_resClrButton_clicked();
};


#endif // WIDGET_H
