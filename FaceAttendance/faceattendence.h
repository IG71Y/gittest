#ifndef FACEATTENDENCE_H
#define FACEATTENDENCE_H

#include <QMainWindow>
#include <opencv.hpp>
#include <QTcpSocket>
#include <QTimer>
using namespace cv;
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class FaceAttendence; }
QT_END_NAMESPACE

/**
 * @brief FaceAttendence 类代表终端人脸考勤程序的主窗口。
 */
class FaceAttendence : public QMainWindow
{
    Q_OBJECT

public:
    FaceAttendence(QWidget *parent = nullptr);
    ~FaceAttendence();

    void timerEvent(QTimerEvent *e);

protected slots:

    // 考勤终端接收解析服务器发送的数据
    void recv_data();

private slots:


    void timer_connect();
    void stop_connect();
    void start_connect();

private:
    Ui::FaceAttendence *ui;

    VideoCapture cap; // 摄像头对象

    cv::CascadeClassifier cascade; //haar--级联分类器对象

    QTcpSocket msocket; // 网络套接字对象

    QTimer mtimer; // 定时器对象

    int flag; // 标志是否是同一个人脸进入到识别区域

    cv::Mat faceMat; // 客户端向服务器发送的人脸数据
};

#endif // FACEATTENDENCE_H
