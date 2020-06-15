#include "clientwidget.h"
#include "ui_clientwidget.h"
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include<QFileDialog>

ClientWidget::ClientWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientWidget)
{
    ui->setupUi(this);


    this->tcpSocket = new QTcpSocket(this);
    this->ksSocket = new QTcpSocket(this);

    // 连接KS
    ksSocket->abort();
    ksSocket->connectToHost("127.0.0.1",5555);

    // 连接CSP
    tcpSocket->abort();
    tcpSocket->connectToHost("127.0.0.1",8888);

    //文件传送相关变量初始化
    //每次发送数据大小为64kb
    perDataSize = 64*1024;
    totalBytes = 0;
    bytestoWrite = 0;
    bytesWritten = 0;
    bytesReceived = 0;
    filenameSize = 0;

    connect(this->ui->pushButton_openFile,SIGNAL(clicked()),this,SLOT(selectFile()));
    connect(this->ui->pushButton_sendAttr,SIGNAL(clicked()),this,SLOT(sendAttr()));
    connect(this->ui->pushButton_csp,SIGNAL(clicked()),this,SLOT(sendToken()));
    connect(this->ui->pushButton_ks,SIGNAL(clicked()),this,SLOT(sendKS()));
    connect(this->ui->pushButton_sendFile,SIGNAL(clicked()),this,SLOT(sendCSP()));



    //监听7777端口是否有连接，有的话，触发接受连接的函数
    this->fileSocket2 = new QTcpSocket(this);
    this->fileserver = new QTcpServer(this);
    this->fileserver->listen(QHostAddress::Any,7777);
    connect(this->fileserver,SIGNAL(newConnection()),this,SLOT(acceptFileConnection()));


    this->ksSocket2 = new QTcpSocket(this);
    this->ksServer2 = new QTcpServer(this);
    this->ksServer2->listen(QHostAddress::Any,3333);
    connect(this->ksServer2,SIGNAL(newConnection()),this,SLOT(acceptKsConnection()));
}



ClientWidget::~ClientWidget()
{
}

// 检测到KS的连接
void ClientWidget::acceptKsConnection()
{
    bytesWritten = 0;

    //每次发送数据大小为64kb
    perDataSize = 1024*1024;
    this->ksSocket2 = this->ksServer2->nextPendingConnection();

    //接受服务器端数据
    connect(ksSocket2,SIGNAL(readyRead()),this,SLOT(acceptKs()));
}


// 数据发送
void ClientWidget::updateFileProgress(qint64 numBytes)
{
    ////已经发送的数据大小
    bytesWritten = numBytes + bytesWritten;

    ////如果已经发送了数据
    if(bytestoWrite > 0)
    {

        outBlock = localFile->read(qMin(bytestoWrite,perDataSize));
        ///发送完一次数据后还剩余数据的大小
        bytestoWrite = bytestoWrite - fileSocket->write(outBlock);
        ///清空发送缓冲区
        outBlock.resize(0);
    }
    else
    {
        localFile->close();
    }


    ////如果发送完毕
    if(bytesWritten == totalBytes)
    {
        qDebug()<<"文件发送成功 feat.Client";
        localFile->close();
    }
}


// 检测到CSP的连接
void ClientWidget::acceptFileConnection()
{
    bytesWritten = 0;

    //每次发送数据大小为64kb
    perDataSize = 1024*1024;
    this->fileSocket2 = this->fileserver->nextPendingConnection();

    //接受服务器端数据
    connect(fileSocket2,SIGNAL(readyRead()),this,SLOT(updateFileProgress()));
}


//选择要上传的文件
void ClientWidget::selectFile()
{
    this->fileSocket = new QTcpSocket(this);
    fileSocket->abort();
    fileSocket->connectToHost("127.0.0.1",8888);

    //设置文件传送进度更新
    connect(fileSocket,SIGNAL(bytesWritten(qint64)),this,SLOT(updateFileProgress(qint64)));
    connect(fileSocket,SIGNAL(readyRead()),this,SLOT(updateFileProgress()));

    this->filename = QFileDialog::getOpenFileName(this,"Open a file","/","files (*)");
    ui->lineEdit_filename->setText(filename);

}


// 从CSP接收数据
void ClientWidget::updateFileProgress()
{
    QDataStream inFile(this->fileSocket2);
    inFile.setVersion(QDataStream::Qt_4_8);


    //如果接收到的数据小于16个字节，保存到来的文件头结构
    if(bytesReceived <= sizeof(qint64)*2)
    {
        qDebug()<<"1";
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

            qDebug()<<filename;

            if(filename.startsWith("success"))
                qDebug()<<"============去重"<<clock()-time7;

            // 从CSP得到去重结果
            if(filename.startsWith("exist"))
            {
                if(filename.endsWith("yes"))
                {
                    qDebug()<<"当前文件已保存，无需上传";
                }
                else
                {
                    sign = filename.mid(filename.indexOf(":")+1);
                    qDebug()<<"用户获取CSP的签名"<<sign;
                }

                bytesReceived = 0;
                totalBytes = 0;
                filenameSize = 0;

            }


            // CSP接收token完毕
            if(filename.startsWith("token"))
            {
                qDebug()<<"CSP接收token完毕,现在发送SCO";
                sendFile("C://Duplicate//argument//Client//SCO.jar");
                bytesReceived = 0;
                filenameSize = 0;
                totalBytes = 0;
            }


            // 从CSP接收到文件
            if(filename.startsWith("fileName"))
            {
                isString = false;
                filename = filename.right(filename.length()-8);
                qDebug()<<"Client从CSP接收到文件"<<filename;
                bytesReceived += filenameSize;
                filename = "C://Duplicate//argument//Client//"+filename;

                localFile = new QFile(filename);
                if(!localFile->open(QFile::WriteOnly))
                {
                    qDebug()<<"Server::open file error!";
                    return;
                }
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
    if(bytesReceived < totalBytes || !isString)
    {
        qDebug()<<"5";
        bytesReceived += fileSocket2->bytesAvailable();
        inBlock = fileSocket2->readAll();
        localFile->write(inBlock);
        inBlock.resize(0);
    }

    this->ui->progressBar_2->setMaximum(int(totalBytes));
    this->ui->progressBar_2->setValue(int(bytesReceived));


    //数据接收完成时
    if(bytesReceived == totalBytes && !isString)
    {
        //this->ui->textEdit->append("Receive file successfully!");
        bytesReceived = 0;
        totalBytes = 0;
        filenameSize = 0;
        isString = true;
        localFile->close();
        //fileSocket2->close();

        if(localFile->fileName().contains("SCO"))
        {

            qDebug()<<"==============下载SCO"<<clock()-time5;

            time5 = clock();
            QProcess p_xf(nullptr);
            p_xf.setWorkingDirectory("C:\\Duplicate\\argument\\Client");
            p_xf.start("cmd",QStringList()<<"/c"<<"jar -xf "<<"C:\\Duplicate\\argument\\Client\\SCO.jar");
            p_xf.waitForStarted();
            p_xf.waitForFinished();
            p_xf.close();
            qDebug()<<"==============解密SCO"<<clock()-time5;

            time5 = clock();
            QProcess p(nullptr);
            p.setWorkingDirectory("C:\\Duplicate");
            p.start("cmd", QStringList()<<"/c"<<"java -jar Dec.jar");
            p.waitForStarted();
            p.waitForFinished();
            qDebug()<<"==============解密SCO"<<clock()-time5;

        }



    }
}


// 从KS接收数据
void ClientWidget::acceptKs()
{
    qDebug()<<"Client从KS接收数据 feat.acceptKs()";
    QDataStream inFile(this->ksSocket2);
    inFile.setVersion(QDataStream::Qt_4_8);

    qDebug()<<"1";


    //如果接收到的数据小于16个字节，保存到来的文件头结构
    if(bytesReceived <= sizeof(qint64)*2)
    {
        if((ksSocket2->bytesAvailable()>=(sizeof(qint64))*2) && (filenameSize==0))
        {
            qDebug()<<"2";
            inFile>>totalBytes>>filenameSize;
            bytesReceived += sizeof(qint64)*2;
        }
        if((ksSocket2->bytesAvailable()>=filenameSize) && (filenameSize != 0))
        {
            qDebug()<<"3";
            inFile>>filename;
            qDebug()<<bytesReceived<<totalBytes;

            qDebug()<<"从KS收到内容："<<filename;

            // 从KS接收H(H(F)||Ka)
            if(filename.startsWith("HF_A"))
            {
                HF_A = filename.right(filename.length()-4);
                qDebug()<<"Client收到KS发送的H(H(F)||Ka)"<<HF_A;

                qDebug()<<"+++++++++++++++生成标签" << (clock() - time1) << endl;

                bytesReceived = 0;
                totalBytes = 0;
                filenameSize = 0;
                sendToken();

            }


            //从KS接收Ki
            if(filename.startsWith("tokens"))
            {
                qDebug()<<"从KS接收到文件所有的token";
                filename = filename.right(filename.length()-6);
                tokens = filename.split(";");

                bytesReceived = 0;
                totalBytes = 0;
                filenameSize = 0;

                this->fileSocket = new QTcpSocket(this);
                fileSocket->abort();
                fileSocket->connectToHost("127.0.0.1",4444);


                QDataStream sendout(&outBlock,QIODevice::WriteOnly);
                sendout.setVersion(QDataStream::Qt_4_8);
                QString currentFileName = "cx";


                //保留总代大小信息空间、文件名大小信息空间、文件名
                sendout<<qint64(0)<<qint64(0)<<currentFileName;
                sendout.device()->seek(0);
                sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

                qDebug()<<qint64(outBlock.size())-sizeof(qint64)*2;

                fileSocket->write(outBlock);
                outBlock.resize(0);
                totalBytes = 0;
            }


            // 从KS接收文件
            if(filename.startsWith("filename"))
            {
                filename = filename.right(filename.length()-8);
                qDebug()<<"从KS接收文件"<<filename;
                isString = false;

                bytesReceived += filenameSize;

                if(filename.contains("C_xa"))
                    filename = "C_xa.txt";

                //接收的文件放在指定目录下
                filename = "C://Duplicate//argument//Client//"+filename;
                localFile = new QFile(filename);
                if(!localFile->open(QFile::WriteOnly))
                {
                    qDebug()<<"Server::open file error!";
                    return;
                }
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
        bytesReceived += ksSocket2->bytesAvailable();
        inBlock = ksSocket2->readAll();
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


// 下载文件
void ClientWidget::on_pushButton_clicked()
{

    time5 = clock();
    this->fileSocket = new QTcpSocket(this);
    fileSocket->abort();
    fileSocket->connectToHost("127.0.0.1",8888);

    QDataStream sendout(&outBlock,QIODevice::WriteOnly);
    sendout.setVersion(QDataStream::Qt_4_8);
    QString currentFileName = "download"+HF_A;
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

    //    filename = "C://Users//25244//Desktop//1.txt";

    //    this->fileSocket = new QTcpSocket(this);
    //    fileSocket->abort();
    //    fileSocket->connectToHost("127.0.0.1",8888);

    //    QString filename=ui->lineEdit->text();

    //    QDataStream sendout(&outBlock,QIODevice::WriteOnly);
    //    sendout.setVersion(QDataStream::Qt_4_8);
    //    QString currentFileName = filename.right(filename.size()-filename.lastIndexOf('/')-1)+'l';
    //    totalBytes += outBlock.size();
    //    qDebug()<<currentFileName.size();
    //    //保留总代大小信息空间、文件名大小信息空间、文件名
    //    sendout<<qint64(0)<<qint64(0)<<currentFileName;
    //    sendout.device()->seek(0);
    //    sendout<<totalBytes<<qint64((outBlock.size()-sizeof(qint64)*2));

    //    fileSocket->write(outBlock);
    //    outBlock.resize(0);
}


// 向CSP发送文件
void ClientWidget::sendFile(QString filename)
{
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
    sendout<<qint64(0)<<qint64(0)<<currentFileName;
    totalBytes += outBlock.size();
    sendout.device()->seek(0);
    sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

    bytestoWrite = totalBytes-fileSocket->write(outBlock);
    outBlock.resize(0);
}


// Client计算SCO
void ClientWidget::generateSCO()
{
    // 利用AES对文件进行加密
    qDebug()<<"对文件"<<uploadFileName<<"使用AES进行加密";

    time3 = clock();
    QProcess p_exe(nullptr);
    p_exe.start("cmd",QStringList()<<"/c"<<"java -jar C:\\Duplicate\\AES_enc.jar "+uploadFileName+" C:\\Duplicate\\argument\\Client");
    p_exe.waitForStarted();
    p_exe.waitForFinished();
    p_exe.close();


    qDebug()<<"利用Elgamal加密key.txt";
    QProcess p_key(nullptr);
    p_key.start("cmd",QStringList()<<"/c"<<"java -jar C:\\Duplicate\\Elgamal.jar 0 C:\\Duplicate\\argument\\Client\\AES_key.txt C:\\Duplicate\\argument\\Client\\Ckey.txt");
    p_key.waitForStarted();
    p_key.waitForFinished();
    p_key.close();

    qDebug()<<"利用Elgamal加密policy.txt";
    QProcess p_elg(nullptr);
    p_elg.start("cmd",QStringList()<<"/c"<<"java -jar C:\\Duplicate\\Elgamal.jar 0 C:\\Duplicate\\argument\\Client\\policy.txt C:\\Duplicate\\argument\\Client\\Cpolicy.txt");
    p_elg.waitForStarted();
    p_elg.waitForFinished();
    p_elg.close();

    qDebug()<<"==============加密时间"<<clock()-time3;

    time4 = clock();
    qDebug()<<"打包C||Ckey||Cpolicy||Cx||Verify";
    QProcess p(nullptr);
    p.start("cmd", QStringList()<<"/c"<<"jar -cfM0 C:\\Duplicate\\argument\\Client\\SCO.jar"
                                        " -C C:\\Duplicate\\argument\\Client\\ C"
                                        " -C C:\\Duplicate\\argument\\Client\\ Cpolicy.txt"
                                        " -C C:\\Duplicate\\argument\\Client\\ Ckey.txt"
                                        " -C C:\\Duplicate\\argument\\Client\\ C_xa.txt"
                                        " -C C:\\Duplicate\\argument\\Client\\ SCO_Verify.jar");
    p.waitForStarted();
    p.waitForFinished();
    p.close();
    qDebug()<<"================封装时间"<<clock()-time4;
}


// 向CSP发送H(H(F)||Ka)
void ClientWidget::sendToken()
{

    time7 = clock();
    if(HF_A.isEmpty())
    {
        QMessageBox::about(nullptr,"警告","请先获取属性密钥");
    }
    else
    {
        qDebug()<<"H(H(F)||Ka):"<<HF_A<<"将其发送给CSP";

        this->fileSocket = new QTcpSocket(this);
        fileSocket->abort();
        fileSocket->connectToHost("127.0.0.1",8888);


        QDataStream sendout(&outBlock,QIODevice::WriteOnly);
        sendout.setVersion(QDataStream::Qt_4_8);
        QString currentFileName = "HF"+HF_A;
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


// 向KS发送H(F)||A
void ClientWidget::sendAttr()
{

    time1 = clock();
    filename = "C://全国密码技术竞赛//607M.rar";
    uploadFileName = filename;


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

    qDebug()<<"Client计算H(F)||A "<<sm3_attr.append("A");

    this->fileSocket = new QTcpSocket(this);
    fileSocket->abort();
    fileSocket->connectToHost("127.0.0.1",4444);


    QDataStream sendout(&outBlock,QIODevice::WriteOnly);
    sendout.setVersion(QDataStream::Qt_4_8);
    QString currentFileName = "HashC"+sm3_attr;
    totalBytes += outBlock.size();


    //保留总代大小信息空间、文件名大小信息空间、文件名
    sendout<<qint64(0)<<qint64(0)<<currentFileName;
    sendout.device()->seek(0);
    sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

    qDebug()<<qint64(outBlock.size())-sizeof(qint64)*2;

    fileSocket->write(outBlock);
    outBlock.resize(0);
    totalBytes = 0;


}


// 向KS发送sign||attr
void ClientWidget::sendKS()
{

    qDebug()<<"Client向KS发送sign||attr";

    QString array = "sign:"+sign+"\nattr:A;B";

    this->fileSocket = new QTcpSocket(this);
    fileSocket->abort();
    fileSocket->connectToHost("127.0.0.1",4444);


    QDataStream sendout(&outBlock,QIODevice::WriteOnly);
    sendout.setVersion(QDataStream::Qt_4_8);
    QString currentFileName = array;
    totalBytes += outBlock.size();


    qDebug()<<currentFileName;
    //保留总代大小信息空间、文件名大小信息空间、文件名
    sendout<<qint64(0)<<qint64(0)<<currentFileName;
    sendout.device()->seek(0);
    sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

    fileSocket->write(outBlock);
    outBlock.resize(0);
    totalBytes = 0;
}


// 向CSP发送token||pow||SCO||id||name
void ClientWidget::sendCSP()
{
    qDebug()<<"tokens"<<tokens;
    QString token_tmp = tokens.at(0);
    for(int i=1;i<tokens.length();i++)
        token_tmp = token_tmp+";"+tokens[i];

    qDebug()<<"Client向服务器发送token||pow||SCO||id||name";
    QString FileName = uploadFileName.right(uploadFileName.size()-uploadFileName.lastIndexOf('/')-1);

    QProcess p_exe(nullptr);
    p_exe.start("cmd",QStringList()<<"/c"<<"C:\\Duplicate\\POW.exe " + uploadFileName +" 10 3");
    p_exe.waitForStarted();
    p_exe.waitForFinished();
    QString strTemp=QString::fromLocal8Bit(p_exe.readAllStandardOutput());
    p_exe.close();

    QList<QString> tmp = strTemp.split(";");
    qDebug()<<"得到POW:"<<tmp;

    QString pow = tmp[0]+";"+tmp[1]+";"+tmp[2];

    tmp[3] = tmp[3].left(30);



    QString array = "token:"+token_tmp+"\npow_chal_dir:"+pow+"\n"+"pow_ver:"+tmp[3]+"\nid:1234\nname:"+FileName;
    //    QString array = "token:"+token_tmp+"\npow_chal_dir:"+"1;2;3"+"\n"+"pow_ver:"+"123"+"\nid:1234\nname:"+FileName;

    this->fileSocket = new QTcpSocket(this);
    fileSocket->abort();
    fileSocket->connectToHost("127.0.0.1",8888);


    QDataStream sendout(&outBlock,QIODevice::WriteOnly);
    sendout.setVersion(QDataStream::Qt_4_8);
    QString currentFileName = array;
    totalBytes += outBlock.size();


    qDebug()<<currentFileName;
    //保留总代大小信息空间、文件名大小信息空间、文件名
    sendout<<qint64(0)<<qint64(0)<<currentFileName;
    sendout.device()->seek(0);
    sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

    fileSocket->write(outBlock);
    outBlock.resize(0);

    time2 = clock();
    generateSCO();
    qDebug()<<"================time use in search keypoint is " << clock() - time2 << " s" << endl;

}


// 计算pow并返回给CSP pow||id||name
void ClientWidget::getPOW()
{
    qDebug()<<"计算POW，并将结果返回给CSP";

    QProcess p_exe(nullptr);
    p_exe.start("cmd",QStringList()<<"/c"<<"C:\\Duplicate\\POW1.exe  " + uploadFileName +" 10 "+pow_chal_dir[0]+" "+pow_chal_dir[1]+ " " +pow_chal_dir[2]);
    p_exe.waitForStarted();
    p_exe.waitForFinished();
    QString strTemp=QString::fromLocal8Bit(p_exe.readAllStandardOutput());
    p_exe.close();
    strTemp = strTemp.left(30);

    this->fileSocket = new QTcpSocket(this);
    fileSocket->abort();
    fileSocket->connectToHost("127.0.0.1",8888);

    QString FileName = uploadFileName.right(uploadFileName.size()-uploadFileName.lastIndexOf('/')-1);

    QDataStream sendout(&outBlock,QIODevice::WriteOnly);
    sendout.setVersion(QDataStream::Qt_4_8);
    QString currentFileName = "pow"+strTemp+"\n1234\n"+FileName;
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


void ClientWidget::on_pushButton_2_clicked()
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


void ClientWidget::on_pushButton_3_clicked()
{


//    QProcess p_xf(nullptr);
//    p_xf.setWorkingDirectory("C:\\Duplicate");
//    p_xf.start("cmd",QStringList()<<"/c"<<"java -jar test.jar C://全国密码技术竞赛//972M.rar 0");
//    p_xf.waitForStarted();
//    p_xf.waitForFinished();
//    p_xf.close();


//    time5 = clock();
//    QProcess p(nullptr);
//    p.setWorkingDirectory("C:\\Duplicate");
//    p.start("cmd", QStringList()<<"/c"<<"java -jar test.jar C://全国密码技术竞赛//224M.rar 1");
//    p.waitForStarted();
//    p.waitForFinished();
//    qDebug()<<"==============解密SCO"<<clock()-time5;

    uploadFileName ="E:\\BYR\\American.Horror.Story.S03.720p.HDTV.x264-Scene\\american.horror.story.s03e06.proper.720p.hdtv.x264-2hd.mkv";
    generateSCO();


    double time = clock();
    filename = uploadFileName;

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



    QProcess p_exe(nullptr);
    p_exe.setWorkingDirectory("C:\\Duplicate");
    p_exe.start("cmd",QStringList()<<"/c"<<"java -jar test.jar " + filename +" C:\\Duplicate\\argument " + sm3_attr + " 0");
    p_exe.waitForStarted();
    p_exe.waitForFinished();
    QString strTemp=QString::fromLocal8Bit(p_exe.readAllStandardOutput());
    p_exe.close();


    qDebug()<<"======"<<clock()-time;
}
