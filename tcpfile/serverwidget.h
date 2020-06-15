#ifndef SERVERWIDGET_H
#define SERVERWIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QTimer>
#include <ctime>
#include <QProcess>
#include<QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QList>

namespace Ui {
class ServerWidget;
}

class ServerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ServerWidget(QWidget *parent = 0);
    ~ServerWidget();

    QTcpServer *fileserver;   //收文件用的
    QTcpSocket *filesocket;   //收文件用的

    QTcpSocket *filesocket2;  //发文件用的

    QTcpSocket *command;

    QSqlDatabase db;

    QTcpSocket *fileSocket;

private slots:


    void acceptFileConnection();
    void updateFileProgress();
    //选择发送的文件
//    void selectFile();
    void sendFile(QString);
    //更新文件传送进度
    void updateFileProgress(qint64);
    void getXn();

private:
    Ui::ServerWidget* ui;

    ////传送文件相关变量
    qint64 totalBytes;
    qint64 bytesReceived;
    qint64 bytestoWrite;
    qint64 bytesWritten;
    qint64 filenameSize;
    QString filename;
    ///每次发送数据大小
    qint64 perDataSize;
    QFile *localFile;
    ////本地缓冲区
    QByteArray inBlock;
    QByteArray outBlock;

    QList<QString> token;
    QString pow_chal_dir;
    QString pow_ver;
    QString SCO_dir;
    QString id;
    QString name;
    QString download_dir;
    QString download_token;

    QString veri_token;

    int count = 0;

    bool isString=true;
    clock_t  time_kp;
    double time1;
    double time2;

    quint16 port=4444;

};
#endif // SERVERWIDGET_H
