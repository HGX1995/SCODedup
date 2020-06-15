#ifndef KS_H
#define KS_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QFileDialog>
#include <QDebug>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QList>
#include <QProcess>
#include <QCryptographicHash>
#include "sm3.h"
#include "sm3_hash.h"
#include "stringutil.h"

class ks : public QObject
{
    Q_OBJECT
public:
    explicit ks(QObject *parent = nullptr);
    QTcpServer *server;
    QTcpSocket *socket;

    QTcpServer *fileserver;   //收文件用的
    QTcpSocket *filesocket;   //收文件用的
    QTcpSocket *filesocket2;  //发文件用的

    QTcpSocket *fileSocket;


    QTcpSocket *command;
    QSqlDatabase db;

signals:

public slots:
    void acceptDataConnection();
    void receviceData();
    void updateFileProgress(qint64 numBytes);
    void sendFile(QString filename, quint16 port);

private:
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

    bool isString = true;
    int count = 0;
    QString key_dir;
    QString file_sm3;
    QString cx;

    quint16 port = 8888;
    bool update = false;

    double time1;
    double time2;
};

#endif // KS_H
