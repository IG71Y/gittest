#include "registerwin.h"
#include "ui_registerwin.h"

#include <QFileDialog>
#include <qfaceobject.h>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QMessageBox>
#include <QDebug>

RegisterWin::RegisterWin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegisterWin)
{
    ui->setupUi(this);
}

RegisterWin::~RegisterWin()
{
    delete ui;
}

// 重置按钮。清空数据
void RegisterWin::on_resetBt_clicked()
{

    ui->nameEdit->clear();
    ui->birthdayEdit->setDate(QDate::currentDate());
    ui->addressEdit->clear();
    ui->phoneEdit->clear();
    ui->picFileEdit->clear();
}

// 注册按钮。
void RegisterWin::on_registerBt_clicked()
{
    QFaceObject faceobj;
    cv::Mat image = cv::imread(ui->picFileEdit->text().toUtf8().data());

    //人脸注册
    int faceID = faceobj.face_register(image);
    qDebug()<<"添加的人脸照片的faceID是："<<faceID;

    //保存图像，名字的Base64编码作为图片的名称
    QString headfile = QString("./data/%1.jpg").arg(QString(ui->nameEdit->text().toUtf8().toBase64()));
    cv::imwrite(headfile.toUtf8().data(), image);


    //信息存储--员工信息表employee
    QSqlTableModel model;
    model.setTable("employee");
    QSqlRecord record = model.record();
    record.setValue("name", ui->nameEdit->text());
    record.setValue("sex", ui->mrb->isChecked()?"男":"女");
    record.setValue("birthday", ui->birthdayEdit->text());
    record.setValue("address", ui->addressEdit->text());
    record.setValue("phone", ui->phoneEdit->text());
    record.setValue("faceID", faceID);
    record.setValue("headfile",headfile); //headfile：数据库员工头像的保存路径
    bool ret = model.insertRecord(0, record);

    if(ret)
    {
        QMessageBox::information(this, "注册提示", "员工注册成功！");
        model.submitAll();
    }
    else
    {
        QMessageBox::information(this, "注册提示", "员工注册失败！");
    }
}

// 添加头像按钮
void RegisterWin::on_addpicBt_clicked()
{

    QString filepath = QFileDialog::getOpenFileName(this);
    ui->picFileEdit->setText(filepath);
    // 显示图片
    QPixmap mmp(filepath);
    mmp = mmp.scaledToWidth(ui->headpicLb->width());
    ui->headpicLb->setPixmap(mmp);
}

//打开摄像头按钮
void RegisterWin::on_videoswitchBt_clicked()
{
    if(ui->videoswitchBt->text() == "打开摄像头")
    {
        if(cap.open(0))
        {
            timerid = startTimer(100);
            ui->videoswitchBt->setText("关闭摄像头");
        }
    }
    else
    {
        killTimer(timerid);
        ui->videoswitchBt->setText("打开摄像头");
        cap.release();
    }
}

// 摄像头数据实时显示
void RegisterWin::timerEvent(QTimerEvent *e)
{
    if (cap.isOpened())
    {
        cap >> image;
        if(image.data == nullptr) return;
    }

    cv::Mat rgbImage;
    cv::cvtColor(image,rgbImage,cv::COLOR_BGR2RGB);
    QImage qImg(rgbImage.data, rgbImage.cols, rgbImage.rows, rgbImage.step1(), QImage::Format_RGB888); //Mat数据转成QImage
    QPixmap mmp=QPixmap::fromImage(qImg);
    mmp = mmp.scaledToWidth(ui->headpicLb->width());
    ui->headpicLb->setPixmap(mmp);
}


//拍照按钮
void RegisterWin::on_cameraBt_clicked()
{
    //保存员工头像。名字的Base64编码作为图片的名称
    QString headfile = QString("./data/%1.jpg").arg(QString(ui->nameEdit->text().toUtf8().toBase64()));
    ui->picFileEdit->setText(headfile);
    cv::imwrite(headfile.toUtf8().data(), image);
    killTimer(timerid);
    ui->videoswitchBt->setText("打开摄像头");
    cap.release();
}
