#include "clientb.h"
#include "ui_clientb.h"

ClientB::ClientB(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientB)
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

    //监听1111端口是否有连接，有的话，触发接受连接的函数
    this->fileSocket2 = new QTcpSocket(this);
    this->fileserver = new QTcpServer(this);
    this->fileserver->listen(QHostAddress::Any,2222);
    connect(this->fileserver,SIGNAL(newConnection()),this,SLOT(acceptConnection()));
}

ClientB::~ClientB()
{
    delete ui;
}



void ClientB::acceptConnection()
{
    bytesWritten = 0;

    //每次发送数据大小为64kb
    perDataSize = 1024*1024;
    this->fileSocket2 = this->fileserver->nextPendingConnection();

    //接受服务器端数据
    connect(fileSocket2,SIGNAL(readyRead()),this,SLOT(acceptData()));
}


// 接收数据
void ClientB::acceptData()
{
    qDebug()<<"ClientB接收数据 feat.acceptData()";
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

            qDebug()<<"ClientB收到内容："<<filename;


            // 从KS接收H(H(F)||Kb)
            if(filename.startsWith("HF_B"))
            {
                HF_B = filename.right(filename.length()-4);
                qDebug()<<"ClientB收到KS发送的H(H(F)||Kb)"<<HF_B;

                bytesReceived = 0;
                totalBytes = 0;
                filenameSize = 0;
            }


            // 从CSP接收POW
            if(filename.startsWith("pow"))
            {
                filename = filename.right(filename.length()-3);
                QList<QString> tmp = filename.split("\n");

                pow_chal_dir = tmp[0].split(";");
                pow_ver = tmp[1];

                qDebug()<<"收到POW:"<<pow_chal_dir<<pow_ver;

                bytesReceived = 0;
                totalBytes = 0;
                filenameSize = 0;

                getPOW();
            }


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
    }

}


// 计算pow并返回给CSP pow||id||name
void ClientB::getPOW()
{
    qDebug()<<"计算POW，并将结果返回给CSP";

    uploadName = "C://全国密码技术竞赛//128M.rar";

    QProcess p_exe(nullptr);
    p_exe.start("cmd",QStringList()<<"/c"<<"C:\\Duplicate\\POW1.exe  " + uploadName +" 10 "+pow_chal_dir[0]+" "+pow_chal_dir[1]+ " " +pow_chal_dir[2]);
    p_exe.waitForStarted();
    p_exe.waitForFinished();
    QString strTemp=QString::fromLocal8Bit(p_exe.readAllStandardOutput());
    p_exe.close();
    strTemp = strTemp.left(30);

    this->fileSocket = new QTcpSocket(this);
    fileSocket->abort();
    fileSocket->connectToHost("127.0.0.1",8888);

    QString FileName = uploadName.right(uploadName.size()-uploadName.lastIndexOf('/')-1);

    QDataStream sendout(&outBlock,QIODevice::WriteOnly);
    sendout.setVersion(QDataStream::Qt_4_8);
    QString currentFileName = "pow"+strTemp+"\n2345\n"+FileName;
    totalBytes += outBlock.size();

    //保留总代大小信息空间、文件名大小信息空间、文件名
    sendout<<qint64(0)<<qint64(0)<<currentFileName;
    sendout.device()->seek(0);
    sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

    qDebug()<<"============"<<clock()-time1;

    fileSocket->write(outBlock);
    outBlock.resize(0);
    totalBytes = 0;
    fileSocket->close();
}



// 计算文件的sm3,向KS发送H(F)||B
void ClientB::on_Btn_getfile_clicked()
{
    isDownload = true;
    filename = "D:\\2018考虫托福\\美音特训（完结）\\1.rar";
    uploadName = filename;

    if(filename.isEmpty() == false)
    {
        this->localFile = new QFile(filename);
        if(!localFile->open(QFile::ReadOnly))
            return;
    }
    else
    {
        qDebug()<<"filename nullptr";
    }

    QString array = localFile->readAll();
    std::string rawText_str = utf8Togb18030(array);

    std::string hash1 = SM3::hash(rawText_str, rawText_str.size());
    file_sm3 = QString::fromStdString(hash1);
    QString file = file_sm3;
    QString sm3_attr = file.append("B");

    qDebug()<<"ClientB计算H(F)||B "<<sm3_attr;

    this->fileSocket = new QTcpSocket(this);
    fileSocket->abort();
    fileSocket->connectToHost("127.0.0.1",4444);


    QDataStream sendout(&outBlock,QIODevice::WriteOnly);
    sendout.setVersion(QDataStream::Qt_4_8);
    QString currentFileName = "BHash"+sm3_attr;
    totalBytes += outBlock.size();


    //保留总代大小信息空间、文件名大小信息空间、文件名
    sendout<<qint64(0)<<qint64(0)<<currentFileName;
    sendout.device()->seek(0);
    sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;


    fileSocket->write(outBlock);
    outBlock.resize(0);
    totalBytes = 0;

}



// 向CSP发送H(H(F)||Ka)
void ClientB::sendToken()
{

    if(HF_B.isEmpty())
    {
        QMessageBox::about(nullptr,"警告","请先获取属性密钥");
    }
    else
    {
        qDebug()<<"H(H(F)||Kb):"<<HF_B<<"将其发送给CSP";

        this->fileSocket = new QTcpSocket(this);
        fileSocket->abort();
        fileSocket->connectToHost("127.0.0.1",8888);


        QDataStream sendout(&outBlock,QIODevice::WriteOnly);
        sendout.setVersion(QDataStream::Qt_4_8);
        QString currentFileName = HF_B+"HF";
        totalBytes += outBlock.size();

        //保留总代大小信息空间、文件名大小信息空间、文件名
        sendout<<qint64(0)<<qint64(0)<<currentFileName;
        sendout.device()->seek(0);
        sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

        qDebug()<<qint64(outBlock.size())-sizeof(qint64)*2;

        fileSocket->write(outBlock);
        outBlock.resize(0);
        totalBytes = 0;
        fileSocket->close();
    }


    //    //获取文件大小
    //    this->totalBytes = localFile->size();
    //    QDataStream sendout(&outBlock,QIODevice::WriteOnly);
    //    sendout.setVersion(QDataStream::Qt_4_8);
    //    QString currentFileName = filename.right(filename.size()-filename.lastIndexOf('/')-1);

    //    qDebug()<<sizeof(currentFileName);
    //    //保留总代大小信息空间、文件名大小信息空间、文件名
    //    sendout<<qint64(0)<<qint64(0)<<currentFileName;
    //    totalBytes += outBlock.size();
    //    sendout.device()->seek(0);
    //    sendout<<totalBytes<<qint64((outBlock.size()-sizeof(qint64)*2));

    //    bytestoWrite = totalBytes-fileSocket->write(outBlock);
    //    outBlock.resize(0);
}



// 向CSP发送H(H(F)||Kb)
void ClientB::download()
{

    if(HF_B.isEmpty())
    {
        QMessageBox::about(nullptr,"警告","请先获取属性密钥");
    }
    else
    {
        qDebug()<<"H(H(F)||Kb):"<<HF_B<<"发送给CSP";

        this->fileSocket = new QTcpSocket(this);
        fileSocket->abort();
        fileSocket->connectToHost("127.0.0.1",8888);


        QDataStream sendout(&outBlock,QIODevice::WriteOnly);
        sendout.setVersion(QDataStream::Qt_4_8);
        QString currentFileName = "download"+HF_B;
        totalBytes += outBlock.size();

        //保留总代大小信息空间、文件名大小信息空间、文件名
        sendout<<qint64(0)<<qint64(0)<<currentFileName;
        sendout.device()->seek(0);
        sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;


        fileSocket->write(outBlock);
        outBlock.resize(0);
        totalBytes = 0;
        fileSocket->close();
    }

}



// 向CSP上传文件，进行去重检查
void ClientB::on_Btn_upload_clicked()
{
    time1 = clock();
    HF_B = "123";
    if(HF_B.isEmpty())
    {
        QMessageBox::about(nullptr,"警告","请先获取属性密钥");
    }
    else
    {
        qDebug()<<"H(H(F)||Kb):"<<HF_B<<"将其发送给CSP";

        this->fileSocket = new QTcpSocket(this);
        fileSocket->abort();
        fileSocket->connectToHost("127.0.0.1",8888);


        QDataStream sendout(&outBlock,QIODevice::WriteOnly);
        sendout.setVersion(QDataStream::Qt_4_8);
        QString currentFileName = HF_B+"HF";
        totalBytes += outBlock.size();

        //保留总代大小信息空间、文件名大小信息空间、文件名
        sendout<<qint64(0)<<qint64(0)<<currentFileName;
        sendout.device()->seek(0);
        sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;


        fileSocket->write(outBlock);
        outBlock.resize(0);
        totalBytes = 0;
        fileSocket->close();
    }
}


// 下载文件
void ClientB::on_Btn_download_clicked()
{
    download();
}
