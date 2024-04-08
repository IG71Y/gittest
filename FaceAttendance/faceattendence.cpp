#include "faceattendence.h"
#include "ui_faceattendence.h"
#include <QImage>
#include <QPainter>

#include <QDebug>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>

FaceAttendence::FaceAttendence(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::FaceAttendence)
{
    ui->setupUi(this);

    cap.open(0); //打开摄像头

    startTimer(100);

    //导入级联分类器文件(训练好的脸部特征数据)
    cascade.load("C:/opencv452/etc/haarcascades/haarcascade_frontalface_default.xml");

    connect(&msocket, &QTcpSocket::disconnected, this, &FaceAttendence::start_connect);

    connect(&msocket, &QTcpSocket::connected, this, &FaceAttendence::stop_connect);

    connect(&mtimer, &QTimer::timeout,this,&FaceAttendence::timer_connect);

    mtimer.start(2000);

    //接收数据的槽函数
    connect(&msocket, &QTcpSocket::readyRead, this, &FaceAttendence::recv_data);

    flag =0;

    ui->widgetLb->hide();
}

FaceAttendence::~FaceAttendence()
{
    delete ui;
}

void FaceAttendence::timerEvent(QTimerEvent *e)
{
    Mat srcImage;
    if(cap.grab()) cap.read(srcImage); // 读取一帧数据
    cv::resize(srcImage,srcImage,Size(480,480));
    Mat grayImage;
    cvtColor(srcImage, grayImage, COLOR_BGR2GRAY); // Mat图转灰度图，加快人脸检测速度

    //终端opencv实时检测跟踪人脸，并发送数据到服务器
    std::vector<Rect> faceRects;
    cascade.detectMultiScale(grayImage, faceRects);
    if(faceRects.size()>0 && flag>=0)
    {
        Rect rect = faceRects.at(0); //第一个人脸的矩形框
        //rectangle(srcImage,rect,Scalar(0,0,255));
        ui->headpicLb->move(rect.x,rect.y);
        if(flag > 2)
        {
            std::vector<uchar> buf;
            //imencode把srcImage编码成jpg格式（为了压缩数据大小）
            cv::imencode(".jpg",srcImage,buf);
            QByteArray byte((const char*)buf.data(),buf.size());
            quint64 backsize = byte.size();
            QByteArray sendData;
            QDataStream stream(&sendData,QIODevice::WriteOnly);
            stream<<backsize<<byte;

            msocket.write(sendData);
            flag = -2;
            qDebug()<<"考勤客户端向服务器发送1次人脸数据！";

            faceMat = srcImage(rect);
            imwrite("./face.jpg",faceMat);
        }
        flag++;
    }

    if(faceRects.size() == 0)
    {
        ui->headpicLb->move(120,120);
        ui->widgetLb->hide();
        flag=0;
    }

    // 摄像头数据在界面上实时显示
    if(srcImage.data == nullptr) return;
    cvtColor(srcImage,srcImage, COLOR_BGR2RGB);
    QImage image(srcImage.data, srcImage.cols, srcImage.rows, srcImage.step1(), QImage::Format_RGB888);
    QPixmap mmp = QPixmap::fromImage(image);

    ui->videoLb->setPixmap(mmp);
}

// 考勤终端接收解析服务器发送的数据
void FaceAttendence::recv_data()
{
    QByteArray array = msocket.readAll();
    qDebug()<<"考勤终端接收到服务器发送的json数据为："<<array;

    //Json数据解析
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(array,&err);

    if(err.error != QJsonParseError::NoError)
    {
        qDebug()<<"考勤终端接收的json数据错误！";
        return;
    }

    QJsonObject obj = doc.object();
    QString employeeID  = obj.value("employeeID").toString();
    QString name        = obj.value("name").toString();
    QString department  = obj.value("department").toString();
    QString timestr     = obj.value("time").toString();

    // 终端显示员工信息（工号，姓名，部门，考勤时间）
    ui->numberEdit      ->setText(employeeID);
    ui->nameEdit        ->setText(name);
    ui->departmentEdit  ->setText(department);
    ui->timeEdit        ->setText(timestr);
    ui->headLb->setStyleSheet("border-radius:75px;border-image: url(./face.jpg);"); //通过样式来显示图片
    ui->widgetLb->show();
}

//终端TCP连接服务器
void FaceAttendence::timer_connect()
{
    msocket.connectToHost("10.100.156.151",9999);
    qDebug()<<"考勤终端正在连接服务器";
}

void FaceAttendence::stop_connect()
{
    mtimer.stop();
    qDebug()<<"考勤终端成功连接服务器";
}

void FaceAttendence::start_connect()
{
    mtimer.start(1000);
    qDebug()<<"考勤终端与服务器断开连接";
}

