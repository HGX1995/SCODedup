#ifndef UI_H
#define UI_H

#include <QWidget>
#include <QTcpSocket>
#include <QTcpServer>
#include <QFile>
#include <QCryptographicHash>
#include <QByteArray>
#include <QMessageBox>
#include <QStandardItemModel>

namespace Ui {
class UI;
}

class UI : public QWidget
{
    Q_OBJECT

public:
    explicit UI(QWidget *parent = nullptr);
    ~UI();

private:
    Ui::UI *ui;
    QString attr;
    quint16 portCSP;
    quint16 portKS;
    QString uploadFileName;

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

    QString HF_A;
    QList<QString> tokens;
    bool isString = true;
    QString sign;
    QList<QString> pow_chal_dir;
    QString pow_ver;

    QMessageBox msg;
    QString file_sm3;
    QString newTokens;
    bool update=false;

    QString uploadName;
    int count = 0;
    QStandardItemModel *model;

    QMap<QString,QString> HF;
    QMap<QString,QString> fileHash;



public:
    void setAttr(QString attr);
    void socketInit();

private:
    void sendAttr();
    void sendToken();
    void sendFile(QString filename);
    void getPOW();
    void sendCSP();
    void sendKS();
    void generateSCO();
    void sendNewAttr();
    void update_CSP();

private slots:
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void acceptCSP();
    void acceptKs();
    void acceptFileConnection();
    void acceptKsConnection();
    void updateFileProgress(qint64 numBytes);
    void on_pushButton_7_clicked();
    void on_pushButton_5_clicked();
};

#endif // UI_H
