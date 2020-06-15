#ifndef HIGHLEVEL_H
#define HIGHLEVEL_H

#include <QWidget>
#include <QTcpSocket>
#include <QTcpServer>
#include <QFile>
#include <QMap>
#include <QProcess>
#include <QCryptographicHash>
#include "stringutil.h"
#include "sm3_hash.h"
#include "sm3.h"

namespace Ui {
class HighLevel;
}

class HighLevel : public QWidget
{
    Q_OBJECT

public:
    explicit HighLevel(QWidget *parent = nullptr);
    ~HighLevel();

private slots:
    void acceptKsConnection();
    void acceptKs();
    void update_CSP();
    void sendFile(QString filename);
    void updateFileProgress(qint64 numBytes);


    void on_Btn_getFile_clicked();

    void on_Btn_change_clicked();

private:
    Ui::HighLevel *ui;

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

    bool isString=true;
    QString file_sm3;
    QString tokens;


    double time1;
};

#endif // HIGHLEVEL_H
