#ifndef CLIENTB_H
#define CLIENTB_H

#include <QWidget>
#include <QTcpSocket>
#include <QTcpServer>
#include <QFile>
#include <QMap>
#include <QProcess>
#include <QMessageBox>
#include "stringutil.h"
#include "sm3_hash.h"
#include "sm3.h"

namespace Ui {
class ClientB;
}

class ClientB : public QWidget
{
    Q_OBJECT

public:
    explicit ClientB(QWidget *parent = nullptr);
    ~ClientB();

private slots:
    void on_Btn_getfile_clicked();
    void acceptConnection();
    void acceptData();
    void sendToken();
    void getPOW();
    void download();

    void on_Btn_upload_clicked();

    void on_Btn_download_clicked();

private:
    Ui::ClientB *ui;
    QTcpSocket *ksSocket;

    QTcpSocket *tcpSocket;
    QTcpSocket *fileSocket;  //发文件用的

    QTcpServer *fileserver;  //接受文件的服务器
    QTcpSocket *fileSocket2; //收文件用的


    QTcpSocket *command;

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

    QString uploadName;


    QList<QString> pow_chal_dir;
    QString pow_ver;

    bool isString=true;
    QString file_sm3;
    QString tokens;
    QString HF_B;
    bool isDownload = false;


    double time1;
};

#endif // CLIENTB_H
