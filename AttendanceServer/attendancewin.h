#ifndef ATTENDANCEWIN_H
#define ATTENDANCEWIN_H

#include "qfaceobject.h"

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include <QSqlTableModel>
#include <QSqlRecord>

QT_BEGIN_NAMESPACE
namespace Ui { class AttendanceWin; }
QT_END_NAMESPACE

// 服务器端考勤应用程序的主窗口类。
class AttendanceWin : public QMainWindow
{
    Q_OBJECT

public:
    AttendanceWin(QWidget *parent = nullptr);
    ~AttendanceWin();

signals:
    // 当收到查询请求时发射该信号。
    void query(cv::Mat& image);

protected slots:

    void accept_client();

    void read_data();

    /**
     * @brief 接收人脸 ID 的槽函数。
     * @param faceid 接收到的人脸 ID。
     */
    void recv_faceid(int64_t faceid);

private:
    Ui::AttendanceWin *ui;

    QTcpServer mserver;
    QTcpSocket *msocket;

    quint64 bsize; // 服务器接收的数据大小。

    QFaceObject fobj; // 人脸识别对象。

    QSqlTableModel model;
};
#endif // ATTENDANCEWIN_H
