#include "widget.h"
#include "ui_widget.h"
#include<QPainter>
#include<QBrush>
#include<QMap>
#include<QString>
#include<QDebug>
#include<QFile>
#include<samepos.h>
#include<algorithm>
#include<vector>
#include<QBuffer>
#include<QTextStream>
#include<fstream>
#include<QFileDialog>
#include<QDebug>
#include<QMessageBox>
#include<QProcess>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->forceText_2->setVisible(false);
    ui->posText_2->setVisible(false);
    ui->forceLabel_2->setVisible(false);
    ui->posLabel_2->setVisible(false);
    ui->label->setText(QString::fromUtf8("载荷/N"));
    ui->label_2->setText("位置/M");


    Qt::WindowFlags flags = 0;
    flags |= Qt::WindowMinimizeButtonHint;
    flags |= Qt::WindowCloseButtonHint;
    flags |= Qt::MSWindowsFixedSizeDialogHint;
    setWindowFlags (flags);

    beam1=new continBeam(30,200e9,0.05);

    /*QFile file(":/qss/style2.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet=tr(file.readAll());
    qApp->setStyleSheet(styleSheet);*/
    QString strPath = QCoreApplication::applicationDirPath();
    qDebug()<<strPath;
    QString strCssFile = strPath + "/qss/continueBeamCalc.qss";
    QFile fCss(strCssFile);
    if( !fCss.open(QFile::ReadOnly))
    {
            //qDebug("css File %s load false",strCssFile);
            return;
    }
    QString strCssContent(fCss.readAll());
    setStyleSheet(strCssContent);
    fCss.close();


}

Widget::~Widget()
{
    delete ui;
    delete beam1;
}

void Widget::paintEvent(QPaintEvent* event)
{
    //QPoint(0,0) is the point of the left-up corner

    QPainter painter;
    painter.begin(this);

    drawBeam(painter);

    double* iter=std::max_element(beam1->bend,beam1->bend+101,[](double x,double y){return (fabs(x)<fabs(y));});
    ui->bendingMaxLabel->setText("绝对值最大挠度(m):"+QString::number(*iter));
    ui->maxBendingPos->setText("位置(m):"+QString::number((iter-beam1->bend)/100.0*beam1->leng));
    iter=std::max_element(beam1->Fsy,beam1->Fsy+101,[](double x,double y){return (fabs(x)<fabs(y));});
    ui->FsyMaxLabel->setText("绝对值最大剪力(N):"+QString::number(*iter));
    ui->maxFsyPos->setText("位置(m):"+QString::number((iter-beam1->Fsy)/100.0*beam1->leng));
    iter=std::max_element(beam1->innerMz,beam1->innerMz+101,[](double x,double y){return (fabs(x)<fabs(y));});
    ui->MzMaxLabel->setText("绝对值最大弯矩(N*m):"+QString::number(*iter));
    ui->maxMzPos->setText("位置(m):"+QString::number((iter-beam1->innerMz)/100.0*beam1->leng));
    painter.end();

}


void Widget::on_pushButton_2_clicked()
{
    if ((ui->posText->text().toDouble()<0 || ui->posText->text().toDouble()>beam1->leng) ||
            ui->posText_2->text().toDouble()<0 || ui->posText_2->text().toDouble()>beam1->leng )
    {
        int ret=QMessageBox::information(this,"警告","非法位置输入",QMessageBox::Ok);
        return;
    }
    QString valueStr=ui->forceText->text();
    double load,pos;
    load=valueStr.toDouble();
    valueStr=ui->posText->text();
    pos=valueStr.toDouble();
    switch(ui->comboBox->currentIndex())
    {
    case 0:
        if (!beam1->jizhongPos.count(pos))
        {   beam1->loadjizhongli(pos,load);
            break;
        }
    else {
            samePos dlg;
            if(dlg.exec()==QDialog::Accepted)
            {
                beam1->loadjizhongli(pos,load);
                break;

            }
        }
        break;
    case 1:
        if (!beam1->jizhongPos.count(pos))
        {   beam1->loadMoment(pos,load);
            break;
        }
    else {
            samePos dlg;
            if(dlg.exec()==QDialog::Accepted)
            {
                beam1->loadMoment(pos,load);
                break;
            }
        }
        break;
    case 2:
        double pos2,load2;
        valueStr=ui->posText_2->text();
        pos2=valueStr.toDouble();
        beam1->loadlin((pos<pos2)?pos:pos2,(pos>pos2)?pos:pos2,load,load);
        break;
    case 3:
        valueStr=ui->posText_2->text();
        pos2=valueStr.toDouble();
        valueStr=ui->forceText_2->text();
        load2=valueStr.toDouble();
        if(pos<pos2){
            beam1->loadlin(pos,pos2,load,load2);}
        else{
            beam1->loadlin(pos2,pos,load2,load);
        }
        break;

    }
    update();
}



void Widget::on_IzText_textChanged(const QString &arg1)
{
    double iz;
    QString valueStr=ui->IzText->text();
    iz=valueStr.toDouble();
    beam1->intia=iz;

}

void Widget::on_lengthText_textChanged(const QString &arg1)
{
    txtChange=1;

}

void Widget::on_ElaText_textChanged(const QString &arg1)
{
    double e;
    QString valueStr=ui->ElaText->text();
    e=valueStr.toDouble();
    beam1->Ela=e*1e9;

}

void Widget::on_comboBox_currentIndexChanged(int index)
{
    switch(index)
    {
    case 0:
        ui->forceText_2->setVisible(false);
        ui->posText_2->setVisible(false);
        ui->forceLabel_2->setVisible(false);
        ui->posLabel_2->setVisible(false);
        ui->label_2->setText("位置/M");
        ui->label->setText(QString::fromUtf8("载荷/N"));
        ui->posText_2->setText("0");
        ui->forceText_2->setText("0");
        break;
    case 1:
        ui->forceText_2->setVisible(false);
        ui->posText_2->setVisible(false);
        ui->forceLabel_2->setVisible(false);
        ui->posLabel_2->setVisible(false);
        ui->label->setText("力偶/N·M");
        ui->label_2->setText("位置/M");
        ui->posText_2->setText("0");
        ui->forceText_2->setText("0");
        break;
    case 2:
        ui->forceText_2->setVisible(false);
        ui->posText_2->setVisible(true);
        ui->forceLabel_2->setVisible(false);
        ui->posLabel_2->setVisible(true);
        ui->label->setText("集度/(N/M)");
        break;
    case 3:
        ui->forceText_2->setVisible(true);
        ui->posText_2->setVisible(true);
        ui->forceLabel_2->setVisible(true);
        ui->posLabel_2->setVisible(true);
        ui->forceLabel_2->setText("集度2/(N/M)");
        ui->label->setText("集度1/(N/M)");

        break;
    }
}

void Widget::on_pushButton_3_clicked()
{
    beam1->loads.clear();
    beam1->jizhongPos.clear();
    beam1->meLoads.clear();
    for(int i=0;i<101;++i){
        beam1->bend[i]=0;
    }
    update();
}




void Widget::wheelEvent(QWheelEvent *event){
    if(boolScale){
        ratio=(event->delta()>0)?ratio*1.5:ratio/1.5;
        update();
        ui->scaleLabel->setText(tr("Y轴变形相对于X坐标放大倍数:")+QString::number((long long)(ratio*5000)));
    }
}

void Widget::mouseMoveEvent(QMouseEvent *e) {
    int x=e->pos().rx();
    if(x<750 && x>250 ){
    ui->XPosLabel->setText("X坐标(m):"+QString::number((x-250)*beam1->leng/500));
    ui->vLabel->setText("挠度(m):"+QString::number(beam1->bending((x-250)*beam1->leng/500)));
    ui->FsyLabel->setText("剪力(N):"+QString::number(beam1->neili((x-250)*beam1->leng/500)));
    ui->MLabel->setText("弯矩(N*m):"+QString::number(beam1->wanju((x-250)*beam1->leng/500)));
    }
    if(x==250 || x==750){
        ui->XPosLabel->setText("X坐标(m):"+QString::number((x-250)*beam1->leng/500));
        ui->vLabel->setText("挠度(m):"+QString::number(0));
        ui->FsyLabel->setText("剪力(N):"+QString::number(0));
        ui->MLabel->setText("弯矩(N*m):"+QString::number(0));
    }
}

void Widget::on_savePicButton_clicked()
{
    QImage iim(800,600,QImage::Format_RGB32);
    QPainter painter(&iim);

    QString fileName=QFileDialog::getSaveFileName(this,tr("保存图片"),"C:",tr("PNG格式(*png)"));

    QFile file(fileName);
    if (!file.open(QIODevice::ReadWrite))   {
        return;
    }
    QBrush brush(Qt::white);
    painter.setBrush(brush);
    painter.drawRect(0,0,800,600);
    drawBeam(painter);

    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::ReadWrite);
    iim.save(&buffer, "PNG");
    file.write(ba);
    painter.end();
}

void Widget::drawBeam(QPainter& painter)
{
    //beam1->calc();
    //int index=ui->BeamTypeBox->currentIndex();

    //draw the border of three areas
    QPen pen(Qt::black,4,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin);
    painter.setPen(pen);
    QRect rec=QRect(240,10,520,180);
    painter.drawRoundedRect(rec,12,12);
    rec=QRect(240,210,520,180);
    painter.drawRoundedRect(rec,12,12);
    rec=QRect(240,410,520,180);
    painter.drawRoundedRect(rec,12,12);


    pen.setStyle(Qt::DashLine);
    pen.setWidth(1);
    painter.setPen(pen);
    painter.drawLine(QPoint(200+50,100),QPoint(700+50,100));


    //draw loads
    bool nonLoad=(beam1->loads.empty() && beam1->meLoads.empty());
    bool nonForce=beam1->loads.empty();
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);
    double fmax=(nonForce)?1:std::max_element(beam1->loads.begin(),beam1->loads.end(),
            [](std::pair<double,double> p1,std::pair<double,double> p2) {return fabs(p1.second)<fabs(p2.second);})->second;

    double fScale=45.0/fabs(fmax);
    std::map<double,double>::iterator p=beam1->loads.end();
    for(std::map<double,double>::iterator iter=beam1->loads.begin();iter!=p;++iter){
        painter.drawLine(QPoint(250+iter->first/beam1->leng*500,100-iter->second*fScale),QPoint(250+iter->first/beam1->leng*500,100));
    }

    //draw restraint
    int max=beam1->resPos.size();
    for(int i=0;i<max;i++){
        painter.drawLine(QPoint(200+50+500*beam1->resPos[i]/beam1->leng,100),QPoint(192+50+500*beam1->resPos[i]/beam1->leng,109));
        painter.drawLine(QPoint(200+50+500*beam1->resPos[i]/beam1->leng,100),QPoint(208+50+500*beam1->resPos[i]/beam1->leng,109));
        painter.drawLine(QPoint(192+50+500*beam1->resPos[i]/beam1->leng,109),QPoint(208+50+500*beam1->resPos[i]/beam1->leng,109));
    }
    //ui->testlabel->setText(QString::number(max));

    //draw moments



    painter.setPen(pen);

        /*painter.drawLine(QPoint(200+50,100),QPoint(192+50,109));
        painter.drawLine(QPoint(200+50,100),QPoint(208+50,109));
        painter.drawLine(QPoint(192+50,109),QPoint(208+50,109));
        painter.drawLine(QPoint(700+50,100),QPoint(692+50,109));
        painter.drawLine(QPoint(700+50,100),QPoint(708+50,109));
        painter.drawLine(QPoint(692+50,109),QPoint(708+50,109));*/

        pen.setWidth(2);
        painter.setPen(pen);

        for(int j=0;j<99;j++)
        {
            painter.drawLine(QPoint((5.0*j+250),(-ratio*5000.0*beam1->bend[j]*beam1->leng/100.0+100)),
                                    QPoint(5.0*(j+1)+250,-ratio*5000.0*beam1->bend[j+1]*beam1->leng/100.0+100));
        }
        painter.drawLine((QPoint((5.0*99+200+50),(-ratio*5000*beam1->bend[99]*beam1->leng/100+100))),
                         (QPoint((5.0*100+200+50),(ratio*5000.0*0+100))));


    //draw inner forces on the center area
    painter.drawLine(QPoint(250,300),QPoint(750,300));
    painter.drawLine(QPoint(250,500),QPoint(750,500));
    if(!nonLoad){
        if( nonForce) {}
        else{
            fmax=*std::max_element(beam1->Fsy,beam1->Fsy+101,[](double x,double y){return (fabs(x)<fabs(y));});
            if(fmax!=0){
                fScale=45.0/fabs(fmax);
                for(int counter=0;counter<101;counter++){
                    painter.drawLine(QPoint(counter*5+250,300),QPoint(5*counter+250,300-fScale*beam1->Fsy[counter]));
                }
            }
        }

        fmax=*std::max_element(beam1->innerMz,beam1->innerMz+101,[](double x,double y){return (fabs(x)<fabs(y));});
        if(fmax!=0){
            fScale=45.0/fabs(fmax);
            for(int counter=0;counter<101;counter++){
                painter.drawLine(QPoint(counter*5+250,500),QPoint(5*counter+250,500-fScale*beam1->innerMz[counter]));
            }
        }
    }
}



void Widget::on_saveFileButton_clicked()
{
    QString fileName=QFileDialog::getSaveFileName(this,tr("保存文件"),"C:",tr("文本文件(*txt)"));

    std::ofstream in(fileName.toStdString());

    in<<"位置\t\t挠度\t\t内力\t\t弯矩\n";
    for(int i=0;i<101;i++){
        in<<beam1->leng*i/100<<"\t\t"
         <<beam1->bend[i]<<"\t\t"
        <<beam1->Fsy[i]<<"\t\t"
        <<beam1->innerMz[i]<<"\n";
    }

    in.close();

}

void Widget::on_scaleOpt_clicked()
{
    boolScale=!boolScale;
}

void Widget::on_addResButton_clicked()
{
    double pos=ui->rResText->text().toDouble();
    if(std::find(beam1->resPos.begin(),beam1->resPos.end(),pos)==beam1->resPos.end()){
        beam1->addRes(pos);
    }
    update();
}

void Widget::on_calcButton_clicked()
{
    if(beam1->resPos.size()<3){
        int ret=QMessageBox::information(this,"警告","约束数目少于3",QMessageBox::Ok);
        return;
    }
    beam1->calc();
    update();
}

void Widget::on_resClrButton_clicked()
{
    beam1->resPos.erase(beam1->resPos.begin(),beam1->resPos.end());
    beam1->resPos.push_back(0);
    beam1->resPos.push_back(beam1->leng);
    update();
}

void Widget::on_lengthText_editingFinished()
{
    if(txtChange) {
        QMessageBox::information(this,"警告","长度改变将会让约束清零，载荷清零",QMessageBox::Ok);
        double length;
        QString valueStr=ui->lengthText->text();
        length=valueStr.toDouble();
        beam1->leng=length;
        beam1->resPos.erase(beam1->resPos.begin(),beam1->resPos.end());
        beam1->resPos.push_back(0);
        beam1->resPos.push_back(beam1->leng);
        update();
        txtChange=!txtChange;
    }
}

void Widget::on_sectionButton_clicked()
{
    QProcess::startDetached(QCoreApplication::applicationDirPath()+"/sectionCalc",QStringList());
}
