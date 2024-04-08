#include "attendancewin.h"
#include "ui_attendancewin.h"

#include <QDateTime>
#include <QSqlRecord>
#include <QThread>
#include <opencv.hpp>
#include <QSqlQuery>
#include <QSqlError>

AttendanceWin::AttendanceWin(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::AttendanceWin)
{
    ui->setupUi(this);

    // 初始化接收数据的大小
    bsize = 0;

    // 当客户端连接时，发送 newConnection 信号
    connect(&mserver, &QTcpServer::newConnection, this, &AttendanceWin::accept_client);

    // 监听客户端连接，启动服务器
    mserver.listen(QHostAddress::Any,9999);

    // 设置 SQL 模型表为 "employee"
    model.setTable("employee");

    // 创建新线程对象并将 QFaceObject 移动到该线程中执行
    QThread *thread = new QThread();
    fobj.moveToThread(thread);
    thread->start();

    // 连接查询信号和 QFaceObject 的人脸查询函数
    connect(this, &AttendanceWin::query, &fobj, &QFaceObject::face_query);

    // 连接 QFaceObject 的发送人脸 ID 信号和接收查询结果的槽函数
    connect(&fobj, &QFaceObject::send_faceid, this, &AttendanceWin::recv_faceid);
}

AttendanceWin::~AttendanceWin()
{
    delete ui;
}

void AttendanceWin::accept_client()
{
    msocket = mserver.nextPendingConnection();

    connect(msocket, &QTcpSocket::readyRead, this, &AttendanceWin::read_data);
}

/**
 * @brief 读取客户端发送的数据并进行人脸识别。
 */
void AttendanceWin::read_data()
{
    QDataStream stream(msocket);
    //stream.setVersion(QDataStream::Qt_5_12);

    // 如果接收的数据大小为 0，则返回
    if(bsize == 0){
        if(msocket->bytesAvailable()<(qint64)sizeof(bsize)) return ;
        stream>>bsize;
    }

    // 如果接收的数据不完整，则返回
    if(static_cast<quint64>(msocket->bytesAvailable()) < bsize) return;


    // 读取数据
    QByteArray data;
    stream>>data;
    bsize = 0;
    if(data.size() == 0) return;

    // 显示接收到的图片--TabWidget1
    QPixmap mmp;
    mmp.loadFromData(data,"jpg");
    mmp = mmp.scaled(ui->picLb->size());
    ui->picLb->setPixmap(mmp);

    // 识别人脸
    cv::Mat faceImage;
    std::vector<uchar> decode;
    decode.resize(data.size());
    memcpy(decode.data(),data.data(),data.size());
    faceImage = cv::imdecode(decode, cv::IMREAD_COLOR);
    //int faceid = fobj.face_query(faceImage);

    // 发送人脸查询信号
    emit query(faceImage);
}

/**
 * @brief 接收人脸 ID，并根据 ID 查询个人信息，并添加到考勤表中。
 * @param faceid 接收到的人脸 ID。
 */
void AttendanceWin::recv_faceid(int64_t faceid)
{
    qDebug()<<"识别到的人脸id:"<<faceid;

    if(faceid < 0)
    {
        QString sdmsg = QString("{\"employeeID\":\"\",\"name\":\"\",\"department\":\"\",\"time\":\"\"}");
        msocket->write(sdmsg.toUtf8());
        qDebug()<<"未识别到有效人脸，考勤失败！";
        return ;
    }

    // 查询并添加考勤信息到考勤表
    model.setFilter(QString("faceID=%1").arg(faceid));
    model.select();
    if(model.rowCount() == 1)
    {
        QSqlRecord record = model.record(0);
        QString sdmsg = QString("{\"employeeID\":\"%1\",\"name\":\"%2\",\"department\":\"软件部门\",\"time\":\"%3\"}")
                .arg(record.value("employeeID").toString())
                .arg(record.value("name").toString())
                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

        QString insertSql = QString("insert into attendance(employeeID) values('%1')").arg(record.value("employeeID").toString());
        QSqlQuery query;

        if(!query.exec(insertSql))
        {
            QString sdmsg = QString("{\"employeeID\":\"\",\"name\":\"\",\"department\":\"\",\"time\":\"\"}");
            msocket->write(sdmsg.toUtf8());
            qDebug()<<"考勤失败的原因："<<query.lastError().text();
            return ;
        }
        else
        {
            msocket->write(sdmsg.toUtf8());
        }
    }
}
