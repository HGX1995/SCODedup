#include "highlevel.h"
#include "ui_highlevel.h"

HighLevel::HighLevel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HighLevel)
{
    ui->setupUi(this);


    //文件传送相关变量初始化
    //每次发送数据大小为64kb
    perDataSize = 64*1024;
    totalBytes = 0;
    bytestoWrite = 0;
    bytesWritten = 0;
    bytesReceived = 0;
    filenameSize = 0;

//    getFile();

    //监听1111端口是否有连接，有的话，触发接受连接的函数
    this->fileSocket2 = new QTcpSocket(this);
    this->fileserver = new QTcpServer(this);
    this->fileserver->listen(QHostAddress::Any,1111);
    connect(this->fileserver,SIGNAL(newConnection()),this,SLOT(acceptKsConnection()));

}

HighLevel::~HighLevel()
{
    delete ui;
}


void HighLevel::acceptKsConnection()
{
    bytesWritten = 0;

    //每次发送数据大小为64kb
    perDataSize = 1024*1024;
    this->fileSocket2 = this->fileserver->nextPendingConnection();

    //接受服务器端数据
    connect(fileSocket2,SIGNAL(readyRead()),this,SLOT(acceptKs()));
}



// 从KS接收数据
void HighLevel::acceptKs()
{
    qDebug()<<"Client从KS接收数据 feat.acceptKs()";
    QDataStream inFile(this->fileSocket2);
    inFile.setVersion(QDataStream::Qt_4_8);

    qDebug()<<"1";


    //如果接收到的数据小于16个字节，保存到来的文件头结构
    if(bytesReceived <= sizeof(qint64)*2)
    {
        if((fileSocket2->bytesAvailable()>=(sizeof(qint64))*2) && (filenameSize==0))
        {
            qDebug()<<"2";
            inFile>>totalBytes>>filenameSize;
            bytesReceived += sizeof(qint64)*2;
        }
        if((fileSocket2->bytesAvailable()>=filenameSize) && (filenameSize != 0))
        {
            qDebug()<<"3";
            inFile>>filename;
            qDebug()<<bytesReceived<<totalBytes;

            qDebug()<<"从KS收到内容："<<filename;


            //从KS接收tokens
            if(filename.startsWith("tokens"))
            {
                qDebug()<<"从KS接收到权限更新后所有的tokens";
                filename = filename.right(filename.length()-6);
                tokens = filename;
                qDebug()<<"token:"<<tokens;

                this->command = new QTcpSocket(this);
                command->abort();
                command->connectToHost("127.0.0.1",4444);


                QDataStream sendout(&outBlock,QIODevice::WriteOnly);
                sendout.setVersion(QDataStream::Qt_4_8);
                totalBytes += outBlock.size();


                QString tmp = "new";
                //保留总代大小信息空间、文件名大小信息空间、文件名
                sendout<<qint64(0)<<qint64(0)<<tmp;
                sendout.device()->seek(0);
                sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

                command->write(outBlock);
                outBlock.resize(0);

                bytesReceived = 0;
                totalBytes = 0;
                filenameSize = 0;

            }


            if(filename.startsWith("success"))
            {
                sendFile("C://Duplicate//argument//HighLevel//C_xa.txt");
                bytesReceived = 0;
                totalBytes = 0;
                filenameSize = 0;
            }


            if(filename.startsWith("policy"))
            {
                qDebug()<<"利用Elgamal加密policy.txt";
                QProcess p_elg(nullptr);
                p_elg.start("cmd",QStringList()<<"/c"<<"java -jar C:\\Duplicate\\Elgamal.jar 0 C:\\Duplicate\\argument\\Client\\policy.txt C:\\Duplicate\\argument\\HighLevel\\Cpolicy.txt");
                p_elg.waitForStarted();
                p_elg.waitForFinished();
                p_elg.close();

                sendFile("C://Duplicate//argument//HighLevel//Cpolicy.txt");
                bytesReceived = 0;
                totalBytes = 0;
                filenameSize = 0;

            }

            // 从KS接收文件
            if(filename.startsWith("filename"))
            {
                filename = filename.right(filename.length()-8);
                qDebug()<<"从KS接收文件"<<filename;
                isString = false;
                if(filename.contains("C_xa"))
                    filename = "C_xa.txt";

                bytesReceived += filenameSize;

                //接收的文件放在指定目录下
                filename = "C://Duplicate//argument//HighLevel//"+filename;
                localFile = new QFile(filename);
                if(!localFile->open(QFile::WriteOnly))
                {
                    qDebug()<<"Server::open file error!";
                    return;
                }
            }

            if(filename.startsWith("update"))
                qDebug()<<"=========="<<clock()-time1;
        }
        else
        {
            qDebug()<<"4";
            return;
        }
    }

    //如果接收的数据小于总数据，则写入文件
    if(bytesReceived < totalBytes && !isString)
    {
        qDebug()<<"5";
        bytesReceived += fileSocket2->bytesAvailable();
        inBlock = fileSocket2->readAll();
        localFile->write(inBlock);
        inBlock.resize(0);
    }

    //数据接收完成时
    if(bytesReceived == totalBytes && !isString)
    {
        qDebug()<<"从KS接收文件"<<localFile->fileName()<<"完毕";
        bytesReceived = 0;
        totalBytes = 0;
        filenameSize = 0;
        localFile->close();
        isString = true;

        if(localFile->fileName().contains("C_xa"))
        {
            update_CSP();
        }
    }

}


// 选择文件，生成文件的sm3
void HighLevel::on_Btn_getFile_clicked()
{
    filename = "C://全国密码技术竞赛//972M.rar";

    QFile localFile(filename);

    if (!localFile.open(QFile::ReadOnly))
    {
        qDebug() << "file open error.";
    }

    QCryptographicHash ch(QCryptographicHash::Md5);

    quint64 totalBytes = 0;
    quint64 bytesWritten = 0;
    quint64 bytesToWrite = 0;
    quint64 loadSize = 1024 * 4;
    QByteArray buf;

    totalBytes = localFile.size();
    bytesToWrite = totalBytes;

    while (1)
    {
        if(bytesToWrite > 0)
        {
            buf = localFile.read(qMin(bytesToWrite, loadSize));
            ch.addData(buf);
            bytesWritten += buf.length();
            bytesToWrite -= buf.length();
            buf.resize(0);
        }
        else
        {
            break;
        }

        if(bytesWritten == totalBytes)
        {
            break;
        }
    }

    localFile.close();
    QByteArray md5 = ch.result();


    QString sm3_attr = md5.toHex();
    file_sm3 = sm3_attr;

}


// 向KS发送更新信息
void HighLevel::on_Btn_change_clicked()
{

    time1 = clock();

    this->ksSocket = new QTcpSocket(this);
    ksSocket->abort();
    ksSocket->connectToHost("127.0.0.1",4444);

    QString sm3_attr = file_sm3;

    QDataStream sendout(&outBlock,QIODevice::WriteOnly);
    sendout.setVersion(QDataStream::Qt_4_8);
    QString currentFileName = "sm3"+sm3_attr+"\n"+"B;C";
    totalBytes += outBlock.size();

    //保留总代大小信息空间、文件名大小信息空间、文件名
    sendout<<qint64(0)<<qint64(0)<<currentFileName;
    sendout.device()->seek(0);
    sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

    qDebug()<<"update attr";

    ksSocket->write(outBlock);
    outBlock.resize(0);
    totalBytes = 0;
    ksSocket->close();

}


// 向CSP发送update tokens
void HighLevel::update_CSP()
{
    qDebug()<<"向CSP发送update tokens"<<tokens;
    this->fileSocket = new QTcpSocket(this);
    fileSocket->abort();
    fileSocket->connectToHost("127.0.0.1",8888);


    QDataStream sendout(&outBlock,QIODevice::WriteOnly);
    sendout.setVersion(QDataStream::Qt_4_8);
    QString currentFileName = tokens;
    totalBytes += outBlock.size();

    //保留总代大小信息空间、文件名大小信息空间、文件名
    sendout<<qint64(0)<<qint64(0)<<"update"+currentFileName;
    sendout.device()->seek(0);
    sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;


    fileSocket->write(outBlock);
    outBlock.resize(0);
    totalBytes = 0;
    fileSocket->close();

}


void HighLevel::sendFile(QString filename)
{
    qDebug()<<"向CSP发送更新的C_xa";
    this->fileSocket = new QTcpSocket(this);
    fileSocket->abort();
    fileSocket->connectToHost("127.0.0.1",8888);

    connect(fileSocket,SIGNAL(bytesWritten(qint64)),this,SLOT(updateFileProgress(qint64)));

    this->localFile = new QFile(filename);
    if(!localFile->open(QFile::ReadOnly))
    {
        qDebug()<<"open file fail!";
        return;
    }
    else
    {
        qDebug()<<"open file success!";
    }

    ///获取文件大小
    this->totalBytes = localFile->size();
    QDataStream sendout(&outBlock,QIODevice::WriteOnly);
    sendout.setVersion(QDataStream::Qt_4_8);
    QString currentFileName = filename.right(filename.size()-filename.lastIndexOf('/')-1);


    ////保留总代大小信息空间、文件名大小信息空间、文件名
    sendout<<qint64(0)<<qint64(0)<<"filename"+currentFileName;
    totalBytes += outBlock.size();
    sendout.device()->seek(0);
    sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

    bytestoWrite = totalBytes-fileSocket->write(outBlock);
    outBlock.resize(0);
}


void HighLevel::updateFileProgress(qint64 numBytes)
{
    // 已经发送的数据大小
    bytesWritten = numBytes + bytesWritten;

    // 如果已经发送了数据
    if(bytestoWrite > 0)
    {
        outBlock = localFile->read(qMin(bytestoWrite,perDataSize));
        ///发送完一次数据后还剩余数据的大小
        bytestoWrite = bytestoWrite - fileSocket->write(outBlock);
        ///清空发送缓冲区
        outBlock.resize(0);
    }
    else
        localFile->close();


    ////如果发送完毕
    if(bytesWritten == totalBytes)
    {
        qDebug()<<"文件"<<localFile->fileName()<<"发送成功 feat.highLevel";
        localFile->close();
        fileSocket->close();
    }
}


