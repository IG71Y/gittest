#include "attendancewin.h"
#include "selectwin.h"

#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <opencv.hpp>
#include "registerwin.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 注册Qt信号和槽的自定义数据类型
    qRegisterMetaType<cv::Mat>("cv::Mat&");
    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<int64_t>("int64_t");

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("server.db");
    if(!db.open())
    {
        qDebug()<<"无法打开数据库："<<db.lastError().text();
        return -1;
    }

    // 创建员工信息表employee
    QString createsql = "create table if not exists employee(employeeID integer primary key autoincrement,"
                        "name varchar(256),"
                        "sex varchar(32),"
                        "birthday text,"
                        "address text,"
                        "phone text,"
                        "faceID integer unique,"
                        "headfile text)";

    QSqlQuery query;
    if(!query.exec(createsql))
    {
       qDebug()<<"无法创建员工信息表："<<query.lastError().text();
       return -1;
    }

    // 创建考勤表attendance
    createsql = "create table if not exists attendance(attendaceID integer primary key autoincrement,"
                "employeeID integer,"
                "attendaceTime TimeStamp NOT NULL DEFAULT(datetime('now','localtime')))";
    if(!query.exec(createsql))
    {
       qDebug()<<"无法创建考勤表："<<query.lastError().text();
       return -1;
    }


    // 显示主窗口
    AttendanceWin w;
    w.show();

    return a.exec();
}
