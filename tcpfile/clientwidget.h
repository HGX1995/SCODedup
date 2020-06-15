#ifndef CLIENTWIDGET_H
#define CLIENTWIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QFile>
#include <QTcpServer>
#include <QProcess>
#include <ctime>
#include <QCryptographicHash>
#include "stringutil.h"
#include "sm3_hash.h"
#include "sm3.h"

namespace Ui {
class ClientWidget;
}

class ClientWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ClientWidget(QWidget *parent = nullptr);
    ~ClientWidget();

    //    void initTCP();
    void newConnect();

private slots:
    //连接服务器
    //        void connectServer();
    void acceptFileConnection();
    void acceptKsConnection();
    void acceptKs();
    void sendKS();
    //浏览文件
    void selectFile();
    //发送文件
    void sendToken();

    //更新文件接收进度
    void updateFileProgress();

    void on_pushButton_clicked();

    void sendAttr();
    void sendCSP();
    void sendFile(QString filename);
    void updateFileProgress(qint64 numBytes);
    void generateSCO();
    void getPOW();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::ClientWidget *ui;
    QTcpSocket *tcpSocket;
    QTcpSocket *fileSocket;  //发文件用的

    QTcpServer *fileserver;  //接受文件的服务器
    QTcpSocket *fileSocket2; //收文件用的

    QTcpServer *ksServer2;
    QTcpSocket *ksSocket2;

    ///文件传送
    QFile *localFile;
    ///文件大小
    qint64 totalBytes;      //文件总字节数
    qint64 bytesWritten;    //已发送的字节数
    qint64 bytestoWrite;    //尚未发送的字节数
    qint64 filenameSize;    //文件名字的字节数
    qint64 bytesReceived;   //接收的字节数
    ///每次发送数据大小
    qint64 perDataSize;
    QString filename;
    ///数据缓冲区
    QByteArray inBlock;
    QByteArray outBlock;


    // 与KS建立连接
    QTcpServer *ksServer;
    QTcpSocket *ksSocket;

    // 计算文件的SM3
    QString file_sm3;
    QString Cattr;
    QString HF_A;
    QString sign;
    QString Cx;
    QString uploadFileName;
    QList<QString> tokens;
    QList<QString> pow_chal_dir;
    QString pow_ver;

    bool isString=true;

    double time1;
    double time2;
    double time3;
    double time4;
    double time5;
    double time6;
    double time7;
    double time8;

};

#endif // CLIENTWIDGET_H
