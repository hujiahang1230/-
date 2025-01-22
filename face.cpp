#include "face.h"

face::face(QObject *parent) : QObject(parent)
{

}

face::face(QString ak,QString sk)
{
    api_key = ak;
    secret_key = sk;
    //实例化网络请求管理工具
    my_access_manager = new QNetworkAccessManager;
    //关联信号与槽
    connect(my_access_manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(recv_json(QNetworkReply*)));
    get_access_token();

}
//获取access_token值,发起网络请求
void face::get_access_token()
{
    QNetworkRequest req;
    req.setUrl(QUrl(QString("https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials&client_id=%0&client_secret=%1").arg(api_key).arg(secret_key)));
    req.setHeader(QNetworkRequest::ContentTypeHeader,"Content-Type:application/json");
    QByteArray buf;
    my_access_manager->post(req,buf);
}
//发起人脸检测的网络请求
void face::search_face(QImage image,QString group_id)
{
    //将接收到的image类型转化为qpixmap类
    QPixmap pic = QPixmap::fromImage(image);

    //存储图像数据到字节数组内
    QByteArray buf;
    QBuffer b(&buf);
    b.open(QIODevice::ReadWrite);
    pic.save(&b,"jpg");
    b.close();
    //封装百度api请求需要的json格式类型数据
    QJsonObject obj;
    //图像数据格式要求base64格式
    obj.insert("image",QString(buf.toBase64()));
    obj.insert("image_type","BASE64");
    obj.insert("group_id_list",group_id);
    //实例化网络请求
    QNetworkRequest req;
    //表明网络请求发起的请求地址
    req.setUrl(QUrl(QString("https://aip.baidubce.com/rest/2.0/face/v3/search?access_token=%0").arg(access_token)));
    //设置网络请求头
    req.setHeader(QNetworkRequest::ContentTypeHeader,"Content-Type:application/json");
    //发起post网络请求
    my_access_manager->post(req,QJsonDocument(obj).toJson());
}
//发起添加用户的网络请求
void face::insert_user(QImage image,QString group_id,QString user_id)
{
    QPixmap pic = QPixmap::fromImage(image);
    QByteArray buf;
    QBuffer b(&buf);
    b.open(QIODevice::ReadWrite);
    pic.save(&b,"jpg");
    b.close();
    //封装数据内添加需要增加到的用户组，用户id以及该用户的图像数据
    QJsonObject obj;
    obj.insert("image",QString(buf.toBase64()));
    obj.insert("image_type","BASE64");
    obj.insert("group_id",group_id);
    obj.insert("user_id",user_id);
    obj.insert("action_type","APPEND");

    QNetworkRequest req;
    req.setUrl(QUrl(QString("https://aip.baidubce.com/rest/2.0/face/v3/faceset/user/add?access_token=%0").arg(access_token)));
    req.setHeader(QNetworkRequest::ContentTypeHeader,"Content-Type:application/json");
    my_access_manager->post(req,QJsonDocument(obj).toJson());
}
//发起添加用户组的网络请求
void face::insert_group(QString group_id)
{
    //封装组id
    QJsonObject obj;
    obj.insert("group_id",group_id);

    QNetworkRequest req;
    req.setUrl(QUrl(QString("https://aip.baidubce.com/rest/2.0/face/v3/faceset/group/add?access_token=%0").arg(access_token)));
    req.setHeader(QNetworkRequest::ContentTypeHeader,"Content-Type:application/json");
    my_access_manager->post(req,QJsonDocument(obj).toJson());
}
//解析接收到的网络请求的返回数据
void face::recv_json(QNetworkReply *rep)
{
    //读取网络请求后的返回数据
    QByteArray replay = rep->readAll();
    //将数据转化为json文档
    QJsonDocument doc = QJsonDocument::fromJson(replay);
    if(doc.isObject())
    {
        //将json文档转化为json对象
        QJsonObject replay_obj = doc.object();
        //判定json数据内所包含的字样，并获取字样对应的数据值
        if(replay_obj.contains("result"))
        {
            QJsonValue val = replay_obj.value("result");
            if(val.isObject())
            {

                QJsonObject replay_obj = val.toObject();

                if(replay_obj.contains("user_list"))
                {
                    QJsonValue val = replay_obj.value("user_list");
                    if(val.isArray())
                    {
                        QJsonArray arr = val.toArray();
                        QJsonValue val = arr.at(0);
                        if(val.isObject())
                        {
                            QJsonObject replay_obj = val.toObject();
                            if(replay_obj.contains("user_id"))//解析用户id
                            {
                                user_id = replay_obj.value("user_id").toString();
                            }
                            if(replay_obj.contains("score"))//解析人脸比对的阈值
                            {
                                score = replay_obj.value("score").toDouble();
                                if(score > 80)//比对得分超过80认为是同一个人，可以触发信号返回用户id和对比值
                                {
                                    qDebug()<<score;
                                    emit this->recv_nub(user_id,score);
                                }
                                else
                                {
                                    emit this->recv_dis();
                                }
                            }
                        }
                    }
                }
            }
        }
        //解析access_token值
        if(replay_obj.contains("access_token"))
        {
            access_token = replay_obj.value("access_token").toString();
        }
        //解析错误码
        if(replay_obj.contains("error_msg"))
        {
            QString error_msg = replay_obj.value("error_msg").toString();
            if(error_msg!="SUCCESS")
            {
                //触发信号返回错误信息
                emit this->recv_error_msg(error_msg);
            }
        }
        //解析access_token值的错误码
        else if(replay_obj.contains("error_description"))
        {
            QString error_description = replay_obj.value("error_description").toString();
            emit this->recv_error_msg(error_description);
        }



    }
}

