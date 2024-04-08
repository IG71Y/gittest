#ifndef REGISTERWIN_H
#define REGISTERWIN_H

#include <QWidget>

#include <opencv.hpp>

namespace Ui {
class RegisterWin;
}

/**
 * @brief RegisterWin类表示注册窗口，用于实现注册功能的界面。
 */
class RegisterWin : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterWin(QWidget *parent = nullptr);
    ~RegisterWin();

    // 定时器事件处理函数
    void timerEvent(QTimerEvent *e);

private slots:

    // 重置按钮点击槽函数
    void on_resetBt_clicked();

    // 添加头像按钮点击槽函数
    void on_addpicBt_clicked();

    // 注册按钮点击槽函数
    void on_registerBt_clicked();

    // 打开/关闭摄像头按钮点击槽函数
    void on_videoswitchBt_clicked();

    // 拍照按钮点击槽函数
    void on_cameraBt_clicked();

private:
    Ui::RegisterWin *ui;

    int timerid; // 定时器

    cv::VideoCapture cap; // 视频捕获对象

    cv::Mat image;  //图像数据（全局变量，用于显示和保存）
};

#endif // REGISTERWIN_H
