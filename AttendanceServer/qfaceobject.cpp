#include "qfaceobject.h"

#include <QDebug>

/*
 * 服务器人脸识别模块
 */
QFaceObject::QFaceObject(QObject *parent) : QObject(parent)
{
    // 初始化人脸识别引擎
    seeta::ModelSetting  FDmode("C:/SeetaFace/bin/model/fd_2_00.dat",seeta::ModelSetting::GPU,0);
    seeta::ModelSetting  PDmode("C:/SeetaFace/bin/model/pd_2_00_pts5.dat",seeta::ModelSetting::GPU,0);
    seeta::ModelSetting  FRmode("C:/SeetaFace/bin/model/fr_2_10.dat",seeta::ModelSetting::GPU,0);

    this->fengineptr = new seeta::FaceEngine(FDmode,PDmode,FRmode);
    this->fengineptr->Load("./face.db");
}

QFaceObject::~QFaceObject()
{
    delete fengineptr;
}

/*
 * 人脸注册
 */
int64_t QFaceObject::face_register(cv::Mat &faceImage)
{
    // opencv的Mat数据转为seetaface的数据
    SeetaImageData simage;
    simage.data = faceImage.data;
    simage.width = faceImage.cols;
    simage.height = faceImage.rows;
    simage.channels = faceImage.channels();

    //注册时如果为同一个人，则返回同一个人的faceid
    int64_t faceid = this->fengineptr->Register(simage);

    if(faceid >=0)
        fengineptr->Save("./face.db");

    return faceid;
}

/*
 * 服务器人脸查询
 */
int QFaceObject::face_query(cv::Mat &faceImage)
{
    // opencv的Mat数据转为seetaface的数据
    SeetaImageData simage;
    simage.data = faceImage.data;
    simage.width = faceImage.cols;
    simage.height = faceImage.rows;
    simage.channels = faceImage.channels();
    float similarity=0;
    int64_t faceid = fengineptr->Query(simage,&similarity);
    qDebug()<<"服务器查询到的faceID是："<<faceid<<"相似度是："<<similarity;
    if(similarity >= 0.6)
    {
        emit send_faceid(faceid);
    }
    else
    {
        emit send_faceid(-1);
    }
    return faceid;
}


