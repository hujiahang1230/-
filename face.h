#ifndef FACE_H
#define FACE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QImage>
#include <QBuffer>
#include <QDebug>
#include <QPixmap>

class face : public QObject
{
    Q_OBJECT
public:
    explicit face(QObject *parent = nullptr);

    //构造face类，需要传递api_key值和secret_key
    face(QString,QString);

    /*********************************************************************************************
     * 进行人脸库搜索
     * 参数说明：
     *  1.图像内容为Qimage类型
     *  2.查询的用户组
     *********************************************************************************************/
    void search_face(QImage,QString);

    /*********************************************************************************************
     * 添加用户信息，即实现用户人脸注册，当用户存在时添加用户图片，用户不存在添加用户和图片
     * 参数说明：
     *  1.图像内容为Qimage类型
     *  2.添加的用户组
     *  3.添加到的用户名id
     *********************************************************************************************/
    void insert_user(QImage,QString,QString);

    /*********************************************************************************************
     * 添加用户组
     * 参数说明：
     *  添加的用户
     *********************************************************************************************/
    void insert_group(QString);

    /*********************************************************************************************
     *获取access_token值
     * api_key和secret_key来自于构造该对象时传的参数
     *********************************************************************************************/
    void get_access_token();

signals:
    /*********************************************************************************************
     * 该对象的信号，触发该信号返回人脸搜索的用户名id和对比度得分
     * 触发条件，人脸对比与用户信息阈值超过80则触发
     *********************************************************************************************/
    void recv_nub(QString,double);

    /*********************************************************************************************
     * 触发条件，人脸对比与用户信息阈值低于80则触发
     *********************************************************************************************/
    void recv_dis();

    //用于返回错误码的信号，该信号触发返回错误信息
    void recv_error_msg(QString);



public slots:
    //接收由networkaccessmanager发起的网络请求的网络返回数据，并解析
    void recv_json(QNetworkReply*);

private:
    QNetworkAccessManager *my_access_manager;

    QString api_key;
    QString secret_key;

    QString access_token;
    QString user_id;
    double score;
};

#endif // FACE_H
