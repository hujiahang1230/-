#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include "face.h"
#include <QList>
#include <QPair>

#include <QMessageBox>
#include <QTextToSpeech>

#include <QTabWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButton_clicked();

    void recv();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void face_success(QString user_id,double number);

    void face_failure();

    void showErrorMsg(QString errorMsg);     //用于处理打卡失败的报错信息

    void showLoginErrorMsg(QString errorMsg);//用于处理登陆失败

    void speakText(); // 声明用于执行语音播报的函数

    QString generateFailureText();

    void setMessageBoxStyleSheet();

    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::Widget *ui;

    QTcpSocket *client;
    int length;
    QByteArray pic;

    face *AI;

    QList<QPair<QString, QDateTime>> checkInRecords; // 存储多条打卡记录

    QTextToSpeech *textToSpeech; // 新增语音播报对象成员变量

    QTabWidget *tabWidget;       //分页操作的指针



};
#endif // WIDGET_H
