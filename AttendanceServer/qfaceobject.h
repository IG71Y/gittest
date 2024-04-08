#ifndef QFACEOBJECT_H
#define QFACEOBJECT_H

#include <QObject>
#include <seeta/FaceEngine.h>
#include <opencv.hpp>

// 人脸数据存储、人脸检测和人脸识别类
class QFaceObject : public QObject
{
    Q_OBJECT
public:
    explicit QFaceObject(QObject *parent = nullptr);
    ~QFaceObject();

public slots:
    /**
     * @brief 人脸注册函数
     * @param Mat图片
     * @return 人脸id（int64_t类型）
     */
    int64_t face_register(cv::Mat& faceImage);

    /**
     * @brief 人脸查询函数
     * @param Mat图片
     * @return 与输入图像匹配的人脸id（int类型）
     */
    int  face_query(cv::Mat& faceImage);

signals:
    // 发送人脸id的信号
    void send_faceid(int64_t faceid);
private:
    seeta::FaceEngine  *fengineptr; // 人脸引擎指针
};

#endif // QFACEOBJECT_H
