#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    //实例化客户端对象
    client=new QTcpSocket;

    //接收信号与接收槽函数的绑定
    //信号对象       信号    槽函数对象    对象
    connect(client, &QTcpSocket::readyRead, this, &Widget::recv);

    //实例化一个百度ai对象
    AI = new face("ZbiUJaXiryRbXDR439oQ8Lgw","Fc8tudm4gzZ5VCyh6IdldKrdkmbH2EEM");   //API_KEY  SECRET_KEY

    //绑定信号与槽
    //1.绑定识别成功的信号与槽
    connect(AI, &face::recv_nub, this, &Widget::face_success);

    //2.绑定识别失败的信号与槽
    connect(AI, &face::recv_dis, this, &Widget::face_failure); //连接人脸识别失败信号到新的槽函数
    connect(AI, &face::recv_error_msg, this, &Widget::showErrorMsg); // 连接接收错误信息信号到显示错误信息的槽函数

    // 初始化语音播报对象
    textToSpeech = new QTextToSpeech(this);

    //显示图片
    QPixmap pix(":/pic_02.png");
    ui->label_7->setPixmap(pix);
    ui->label_7->setScaledContents(true);

    QPixmap pix_03(":/pic_03.png");
    ui->label_12->setPixmap(pix_03);
    ui->label_12->setScaledContents(true);

    QPixmap pix_04(":/pic_04.png");
    ui->label_13->setPixmap(pix_04);
    ui->label_13->setScaledContents(true);

    QPixmap pix_05(":/pic_05.png");
    ui->label_14->setPixmap(pix_05);
    ui->label_14->setScaledContents(true);

    tabWidget = ui->tabWidget; // 获取tabWidget控件

}

Widget::~Widget()
{
    delete ui;
}
//语音识别播报
void Widget::speakText()
{
    QString text = ui->textEdit->toPlainText();
    if (!text.isEmpty())
    {
        textToSpeech->say(text);
    }
}


//登录的槽函数
void Widget::on_pushButton_clicked()
{
    QString IP=ui->lineEdit->text(); //获取IP地址
    int PORT=ui->lineEdit_2->text().toInt(); //获取端口号

    client->connectToHost(IP,PORT);

    if (client->waitForConnected(1000)) {  // 等待1秒看是否连接成功，可根据实际调整时间
        tabWidget->setCurrentIndex(1); // 登录成功，切换到打卡注册页面
    } else {
        QString errorMsg = client->errorString();  // 获取连接失败的错误信息
        showLoginErrorMsg(errorMsg);  // 显示登录失败提示框并传递错误信息
    }
}

void Widget::showLoginErrorMsg(QString errorMsg)
{
    QMessageBox::critical(this, "登录失败", "原因：" + errorMsg);
}

//接收数据的槽函数
void Widget::recv()
{
    static int flag=0;
    //第一次读取图像的大小
    //第二次读取图像的数据
    if(flag==0)
    {
        //判断数据是否有效
        if(client->bytesAvailable()<8)
        {
            return;
        }
        length = client->read(8).toInt();

        flag=1;
    }
    else if(flag==1)
    {
        //判断数据是否有效
        if(client->bytesAvailable()<length)//说明图片没有传完
        {
            return;
        }
        pic = client->read(length);

        QPixmap pix;//定义一个图片类型变量
        pix.loadFromData(pic);

        //显示图像
        ui->label_3->setPixmap(pix);
        ui->label_3->setScaledContents(true);

        flag=0;
    }
}

//打卡的槽函数
void Widget::on_pushButton_2_clicked()
{

    QPixmap pix;//定义一个图片类型变量
    pix.loadFromData(pic);

    //显示图像
    ui->label_4->setPixmap(pix);
    ui->label_4->setScaledContents(true);

    //上传的人脸信息
    QImage image;
    image.loadFromData(pic);

    //获取用户的组
    QString group_id = ui->lineEdit_3->text();

    //进行人脸检测
    AI->search_face(image,group_id);

}

//人脸信息的注册
void Widget::on_pushButton_3_clicked()
{
    QPixmap pix;//定义一个图片类型变量
    pix.loadFromData(pic);

    //显示图像
    ui->label_4->setPixmap(pix);
    ui->label_4->setScaledContents(true);

    //上传的人脸信息
    QImage image;
    image.loadFromData(pic);

    //获取用户的组
    QString group_id = ui->lineEdit_3->text();

    //获取用户的ID
    QString user_id = ui->lineEdit_4->text();

    //进行人脸信息的注册
    AI->insert_user(image,group_id,user_id);

}
//人脸识别成功的槽函数
void Widget::face_success(QString user_id,double number)
{
    QString data = "打卡成功,欢迎" + user_id +"业主回家!"+"  "+"相似度为："+QString::number(number);
    ui->textEdit->setText(data);

    // 执行语音播报
    speakText();


    // 组成包含业主ID和打卡时间的pair
    QPair<QString, QDateTime> record(user_id, QDateTime::currentDateTime());
    // 添加到打卡记录列表
    checkInRecords.append(record);

}
void Widget::face_failure()
{
    QMessageBox::warning(this, "打卡失败", "人脸识别未通过，请重新尝试打卡。");

}

QString Widget::generateFailureText()
{
    return "打卡失败，请重新尝试打卡。";
}

//显示错误信息的槽函数，弹出提示框显示具体错误消息
void Widget::showErrorMsg(QString errorMsg)
{
    QMessageBox::critical(this, "错误", "发生错误：" + errorMsg);
    QString failureText = generateFailureText();
    // 生成打卡失败的文本内容并进行语音播报
    textToSpeech->say(failureText);
}
//用于设置QMessageBox的样式表
void Widget::setMessageBoxStyleSheet()
{
    QString styleSheet = "QMessageBox {"
                         "background-color: white;"
                         "color: white;"
                         "}"
                         "QMessageBox QLabel{"
                         "color: white;"
                         "}";
    qApp->setStyleSheet(styleSheet);
}

//切换logo图片的按钮
void Widget::on_radioButton_clicked()
{
    //显示图片
    QPixmap pix(":/pic_01.png");
    ui->label_7->setPixmap(pix);
    ui->label_7->setScaledContents(true);
}

//切换logo图片的按钮
void Widget::on_radioButton_2_clicked()
{
    //显示图片
    QPixmap pix(":/pic_02.png");
    ui->label_7->setPixmap(pix);
    ui->label_7->setScaledContents(true);
}

//点击显示就会显示打卡信息的槽函数
void Widget::on_pushButton_4_clicked()
{
    QString allRecords;
    for (const auto& record : checkInRecords) {
        QString timeStr = record.second.toString("yyyy-MM-dd hh:mm:ss");
        QString info = "业主ID：" + record.first + "\n打卡时间：" + timeStr + "\n\n";
        allRecords += info;
    }
    ui->label_11->setText(allRecords);
}


