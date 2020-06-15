#include "ui.h"
#include "serverwidget.h"
#include "clientwidget.h"
#include "clientb.h"
#include "ks.h"
#include "highlevel.h"
#include "ui_ui.h"

UI::UI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UI)
{
    ui->setupUi(this);
    msg.addButton(QMessageBox::Ok);
    msg.setWindowTitle("提示");
    msg.button(QMessageBox::Ok)->hide();
    msg.hide();

    model = new QStandardItemModel();

}

UI::~UI()
{
    delete ui;
}

// 设置当前用户属性
void UI::setAttr(QString attr)
{
    this->attr = attr;

    if(attr == "A"){
        ui->label->setText("教授");
        portCSP = 1111;
        portKS = 5555;
        ui->checkBox->setCheckState(Qt::Checked);
        ui->checkBox->setDisabled(true);
    }
    if(attr == "B"){
        ui->label->setText("副教授");
        portCSP = 2222;
        portKS = 6666;
        ui->checkBox_2->setCheckState(Qt::Checked);
        ui->checkBox_2->setDisabled(true);
    }
    if(attr == "C"){
        ui->label->setText("讲师");
        portCSP = 3333;
        portKS = 7777;
        ui->checkBox_3->setCheckState(Qt::Checked);
        ui->checkBox_3->setDisabled(true);
    }
}

// 设置客户端的Socket
void UI::socketInit()
{
    //文件传送相关变量初始化
    perDataSize = 64*1024;
    totalBytes = 0;
    bytestoWrite = 0;
    bytesWritten = 0;
    bytesReceived = 0;
    filenameSize = 0;

    //监听端口，接收CSP消息
    this->fileSocket2 = new QTcpSocket(this);
    this->fileserver = new QTcpServer(this);
    this->fileserver->listen(QHostAddress::Any,portCSP);
    connect(this->fileserver,SIGNAL(newConnection()),this,SLOT(acceptFileConnection()));


    //监听端口，接收KS消息
    this->ksSocket2 = new QTcpSocket(this);
    this->ksServer2 = new QTcpServer(this);
    this->ksServer2->listen(QHostAddress::Any,portKS);
    connect(this->ksServer2,SIGNAL(newConnection()),this,SLOT(acceptKsConnection()));
}

// 检测到CSP消息
void UI::acceptFileConnection()
{
    bytesWritten = 0;

    //每次发送数据大小为64kb
    perDataSize = 1024*1024;
    this->fileSocket2 = this->fileserver->nextPendingConnection();

    //接收CSP数据
    connect(fileSocket2,SIGNAL(readyRead()),this,SLOT(acceptCSP()));
}

// 检测到KS消息
void UI::acceptKsConnection()
{
    bytesWritten = 0;

    //每次发送数据大小为64kb
    perDataSize = 1024*1024;
    this->ksSocket2 = this->ksServer2->nextPendingConnection();

    //接受服务器端数据
    connect(ksSocket2,SIGNAL(readyRead()),this,SLOT(acceptKs()));
}


// 接收CSP的消息
void UI::acceptCSP()
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
            qDebug()<<"接收到CSP发送的消息:"<<filename;


            // 从CSP得到去重结果
            if(filename.startsWith("exist"))
            {
                if(filename.endsWith("yes"))
                {
                    qDebug()<<"当前文件已保存，无需上传！您可选择想要分享的对象";
                }
                else
                {
                    msg.setText("正在上传文件...");
                    sign = filename.mid(filename.indexOf(":")+1);
                    qDebug()<<"用户获取CSP的签名"<<sign;
                    sendKS();
                }

                bytesReceived = 0;
                totalBytes = 0;
                filenameSize = 0;

            }

            // CSP接收token完毕
            if(filename.startsWith("token"))
            {
                qDebug()<<"CSP接收token完毕,现在发送SCO";

                bytesReceived = 0;
                totalBytes = 0;
                filenameSize = 0;

                this->fileSocket = new QTcpSocket(this);
                fileSocket->abort();
                fileSocket->connectToHost("127.0.0.1",8888);

                connect(fileSocket,SIGNAL(bytesWritten(qint64)),this,SLOT(updateFileProgress(qint64)));

                filename = "C://Duplicate//argument//Client//SCO.jar";
                this->localFile = new QFile(filename);
                if(!localFile->open(QFile::ReadOnly))
                {
                    qDebug()<<"open file fail!";
                    return;
                }
                else
                    qDebug()<<"open file success!";


                // 获取文件大小
                this->totalBytes = localFile->size();
                QDataStream sendout(&outBlock,QIODevice::WriteOnly);
                sendout.setVersion(QDataStream::Qt_4_8);
                QString currentFileName = filename.right(filename.size()-filename.lastIndexOf('/')-1);


                // 保留总代大小信息空间、文件名大小信息空间、文件名
                sendout<<qint64(0)<<qint64(0)<<currentFileName;
                totalBytes += outBlock.size();
                sendout.device()->seek(0);
                sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

                bytestoWrite = totalBytes-fileSocket->write(outBlock);
                outBlock.resize(0);
            }

            // 上传完毕
            if(filename.startsWith("upload"))
            {
                bytesReceived = 0;
                totalBytes = 0;
                filenameSize = 0;
                qDebug()<<"上传完毕";
                msg.setText("文件上传完毕");
                msg.button(QMessageBox::Ok)->show();

                QStandardItem *Item = new QStandardItem();
                Item->setCheckable( true );
                model->setItem( count, Item );
                count++;
                Item->setText(uploadName);
                ui->listView->setModel(model);

            }

            // 从CSP接收到文件
            if(filename.startsWith("fileName"))
            {
                isString = false;
                filename = filename.right(filename.length()-8);
                qDebug()<<"Client从CSP接收到文件"<<filename;
                bytesReceived += filenameSize;

                if(filename.contains("SCO"))
                    filename = "SCO.jar";

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

            // 所有权证明成功
            if(filename.startsWith("success"))
            {
                bytesReceived = 0;
                totalBytes = 0;
                filenameSize = 0;
                msg.setText("当前文件已存在，无需再次上传");
                msg.button(QMessageBox::Ok)->show();

                QStandardItem *Item = new QStandardItem();
                Item->setCheckable( true );
                model->setItem( count, Item );
                count++;
                Item->setText(uploadName);
                ui->listView->setModel(model);

            }

            // 更新成功
            if(filename.startsWith("update_success"))
            {
                sendFile("C://Duplicate//argument//Client//C_xa.txt");
                bytesReceived = 0;
                totalBytes = 0;
                filenameSize = 0;
            }


            if(filename.startsWith("updateSuccess"))
            {
                msg.setText("权限更新成功");
                msg.button(QMessageBox::Ok)->show();
                msg.show();

                update = false;

                bytesReceived = 0;
                totalBytes = 0;
                filenameSize = 0;
            }

            //从CSP接受fail
            if(filename.startsWith("fail"))
            {
                msg.setText("当前文件已失效，请重新上传！");
                msg.button(QMessageBox::Ok)->show();
                msg.show();

                for(int i=0;i<count;i++)
                {
                    if(model->item(i)->checkState()==Qt::Checked)
                    {

                        qDebug()<<model->item(i)->text();
                        QStandardItem *tmp = model->takeItem(i);
                        delete tmp;
                        break;
                    }
                }

                count--;
                bytesReceived = 0;
                totalBytes = 0;
                filenameSize = 0;
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
        bytesReceived += fileSocket2->bytesAvailable();
        inBlock = fileSocket2->readAll();
        localFile->write(inBlock);
        inBlock.resize(0);
    }



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
            QProcess p_xf(nullptr);
            p_xf.setWorkingDirectory("C:\\Duplicate\\argument\\Client");
            p_xf.start("cmd",QStringList()<<"/c"<<"jar -xf "<<"C:\\Duplicate\\argument\\Client\\SCO.jar");
            p_xf.waitForStarted();
            p_xf.waitForFinished();
            p_xf.close();

            //            QProcess p(nullptr);
            //            p.setWorkingDirectory("C:\\Duplicate");
            //            p.start("cmd", QStringList()<<"/c"<<"java -jar Dec.jar");
            //            p.waitForStarted();
            //            p.waitForFinished();

            QProcess p(nullptr);
            p.setWorkingDirectory("C:\\Duplicate");
            p.start("cmd", QStringList()<<"/c"<<"SM4.exe 2 C:\\Duplicate\\argument\\Client\\C.sm4 C:\\Duplicate\\argument\\Client\\M C:\\Duplicate\\argument\\Client\\key.txt");
            p.waitForStarted();
            p.waitForFinished();

            qDebug()<<"下载解密成功";
            msg.setText("下载成功！");
            msg.button(QMessageBox::Ok)->show();
            msg.show();

        }

    }
}


// 接收KS的消息
void UI::acceptKs()
{
    qDebug()<<"Client从KS接收到数据";
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
            qDebug()<<"Client收到KS内容："<<filename;

            // 接收到H(H(F)||Ki)
            if(filename.startsWith("HF_A"))
            {
                HF_A = filename.right(filename.length()-4);
                qDebug()<<"Client收到KS发送的去重标签d_token:"<<HF_A;
                bytesReceived = 0;
                totalBytes = 0;
                filenameSize = 0;

                sendToken();
            }


            //接收到tokens
            if(filename.startsWith("tokens"))
            {
                qDebug()<<"从KS接收到文件所有的token";
                filename = filename.right(filename.length()-6);
                tokens = filename.split(";");

                bytesReceived = 0;
                totalBytes = 0;
                filenameSize = 0;

                // 请求KS返回C_xa
                this->fileSocket = new QTcpSocket(this);
                fileSocket->abort();
                fileSocket->connectToHost("127.0.0.1",4444);

                QDataStream sendout(&outBlock,QIODevice::WriteOnly);
                sendout.setVersion(QDataStream::Qt_4_8);
                QString currentFileName = "cx"+attr;


                //保留总代大小信息空间、文件名大小信息空间、文件名
                sendout<<qint64(0)<<qint64(0)<<currentFileName;
                sendout.device()->seek(0);
                sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

                qDebug()<<qint64(outBlock.size())-sizeof(qint64)*2;

                fileSocket->write(outBlock);
                outBlock.resize(0);
                totalBytes = 0;
            }


            // 接收到文件
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


            //从KS接收tokens
            if(filename.startsWith("Newtokens"))
            {
                qDebug()<<"从KS接收到权限更新后所有的tokens";
                filename = filename.right(filename.length()-9);
                newTokens = filename;
                qDebug()<<"token:"<<newTokens;

                QTcpSocket* command = new QTcpSocket(this);
                command->abort();
                command->connectToHost("127.0.0.1",4444);


                QDataStream sendout(&outBlock,QIODevice::WriteOnly);
                sendout.setVersion(QDataStream::Qt_4_8);
                totalBytes += outBlock.size();


                QString tmp = "new";
                //保留总代大小信息空间、文件名大小信息空间、文件名
                sendout<<qint64(0)<<qint64(0)<<tmp+attr;
                sendout.device()->seek(0);
                sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

                command->write(outBlock);
                outBlock.resize(0);

                bytesReceived = 0;
                totalBytes = 0;
                filenameSize = 0;

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
        ksSocket2->close();

        if(localFile->fileName().contains("C_xa")&&!update)
            sendCSP();

        if(localFile->fileName().contains("C_xa")&&update)
            update_CSP();
    }
}


// 点击上传按钮
void UI::on_pushButton_3_clicked()
{
    this->filename = QFileDialog::getOpenFileName(this,"Open a file","/","files (*)");

    // 向KS发送文件标签
    msg.setText("正在进行去重检测...");
    msg.button(QMessageBox::Ok)->hide();
    msg.show();
    sendAttr();

}

// 点击下载按钮
void UI::on_pushButton_4_clicked()
{
    if(count==0)
    {
        QMessageBox::about(nullptr,"警告","请先上传文件");
    }
    else
    {
        QString hash;
        qDebug()<<"====="<<count;
        for(int i=0;i<count;i++)
        {
            if(model->item(i)->checkState()==Qt::Checked)
            {
                hash = HF[model->item(i)->text()];
                qDebug()<<model->item(i)->text()<<hash;
                break;
            }
        }

        if(hash.isEmpty())
            QMessageBox::about(nullptr,"警告","请选择要下载的文件！");
        else
        {
            msg.setText("正在下载中...");
            msg.button(QMessageBox::Ok)->hide();
            msg.show();

            this->fileSocket = new QTcpSocket(this);
            fileSocket->abort();
            fileSocket->connectToHost("127.0.0.1",8888);

            QDataStream sendout(&outBlock,QIODevice::WriteOnly);
            sendout.setVersion(QDataStream::Qt_4_8);
            QString currentFileName = "download"+attr+hash;
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


}

// 点击更新按钮
void UI::on_pushButton_7_clicked()
{
    if(count==0)
    {
        QMessageBox::about(nullptr,"警告","请先上传文件");
    }
    else
    {
        QString hash;
        qDebug()<<"====="<<count;
        for(int i=0;i<count;i++)
        {
            if(model->item(i)->checkState()==Qt::Checked)
            {
                hash = fileHash[model->item(i)->text()];
                qDebug()<<model->item(i)->text()<<hash;
                break;
            }
        }

        if(hash.isEmpty())
            QMessageBox::about(nullptr,"警告","请选择要更新的文件！");
        else
        {
            msg.setText("正在进行权限更新...");
            msg.button(QMessageBox::Ok)->hide();
            msg.show();
            update = true;

            file_sm3 = hash;

            sendNewAttr();
        }
    }

}


// 点击分享按钮
void UI::on_pushButton_5_clicked()
{
    on_pushButton_7_clicked();
}


// 发送[H(F)||attr]->KS
void UI::sendAttr()
{
    //    filename = "C://全国密码技术竞赛//128M.rar";
    uploadFileName = filename;
    uploadName = uploadFileName.right(uploadFileName.size()-uploadFileName.lastIndexOf('/')-1);


    QFile localFile(filename);
    if (!localFile.open(QFile::ReadOnly))
        qDebug() << "file open error.";

    QCryptographicHash ch(QCryptographicHash::Md5);
    qint64 totalBytes = 0;
    qint64 bytesWritten = 0;
    qint64 bytesToWrite = 0;
    qint64 loadSize = 1024 * 4;
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
            break;

        if(bytesWritten == totalBytes)
            break;
    }

    localFile.close();
    QByteArray md5 = ch.result();
    QString sm3_attr = md5.toHex();

    fileHash[uploadName] = sm3_attr;

    qDebug()<<"Client计算文件标签："<<sm3_attr.append(attr);

    this->fileSocket = new QTcpSocket(this);
    fileSocket->abort();
    fileSocket->connectToHost("127.0.0.1",4444);


    QDataStream sendout(&outBlock,QIODevice::WriteOnly);
    sendout.setVersion(QDataStream::Qt_4_8);
    QString currentFileName = "Hash"+attr+sm3_attr;
    totalBytes += outBlock.size();


    //保留总代大小信息空间、文件名大小信息空间、文件名
    sendout<<qint64(0)<<qint64(0)<<currentFileName;
    sendout.device()->seek(0);
    sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

    fileSocket->write(outBlock);
    outBlock.resize(0);
    totalBytes = 0;

    bytesReceived = 0;
    this->totalBytes = 0;
    filenameSize = 0;

}


// 发送token->CSP
void UI::sendToken()
{

    HF[uploadName]=HF_A;
    qDebug()<<"将去重标签"<<HF_A<<"发送给CSP";

    QTcpSocket* command = new QTcpSocket(this);
    command->abort();
    command->connectToHost("127.0.0.1",8888);


    QDataStream sendout(&outBlock,QIODevice::WriteOnly);
    sendout.setVersion(QDataStream::Qt_4_8);
    totalBytes += outBlock.size();


    //保留总代大小信息空间、文件名大小信息空间、文件名
    sendout<<qint64(0)<<qint64(0)<<"HF"+attr+HF_A;
    sendout.device()->seek(0);
    sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

    command->write(outBlock);
    outBlock.resize(0);

    bytesReceived = 0;
    totalBytes = 0;
    filenameSize = 0;
}


// 向CSP发送token||pow||SCO||id||name
void UI::sendCSP()
{
    qDebug()<<"tokens"<<tokens;

    QString token_tmp = tokens.at(0);
    for(int i=1;i<tokens.length();i++)
        token_tmp = token_tmp+";"+tokens[i];

    qDebug()<<"Client向服务器发送token||pow||SCO||id||name";
    QString FileName = uploadFileName.right(uploadFileName.size()-uploadFileName.lastIndexOf('/')-1);

    //    QProcess p_exe(nullptr);
    //    p_exe.start("cmd",QStringList()<<"/c"<<"C:\\Duplicate\\POW.exe " + uploadFileName +" 10 3");
    //    p_exe.waitForStarted();
    //    p_exe.waitForFinished();
    //    QString strTemp=QString::fromLocal8Bit(p_exe.readAllStandardOutput());
    //    p_exe.close();

    //    QList<QString> tmp = strTemp.split(";");
    //    qDebug()<<"得到POW:"<<tmp;

    QList<QString> tmp;
    tmp.append("1");
    tmp.append("2");
    tmp.append("3");
    tmp.append("123");

    QString pow = tmp[0]+";"+tmp[1]+";"+tmp[2];
    //    tmp[3] = tmp[3].left(30);

    QString array = "token:"+token_tmp+"\npow_chal_dir:"+pow+"\n"+"pow_ver:"+tmp[3]+"\nid:1234\nname:"+FileName+attr;

    this->fileSocket = new QTcpSocket(this);
    fileSocket->abort();
    fileSocket->connectToHost("127.0.0.1",8888);


    QDataStream sendout(&outBlock,QIODevice::WriteOnly);
    sendout.setVersion(QDataStream::Qt_4_8);
    QString currentFileName = array;
    totalBytes += outBlock.size();

    //保留总代大小信息空间、文件名大小信息空间、文件名
    sendout<<qint64(0)<<qint64(0)<<currentFileName;
    sendout.device()->seek(0);
    sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

    fileSocket->write(outBlock);
    outBlock.resize(0);

    generateSCO();

}


// Client计算SCO
void UI::generateSCO()
{
    // 利用SM4对文件进行加密
    qDebug()<<"对文件"<<uploadFileName<<"使用SM4进行加密";

    //    QProcess p_exe(nullptr);
    //    p_exe.setWorkingDirectory("C:\\Duplicate");
    //    p_exe.start("cmd",QStringList()<<"/c"<<"java -jar C:\\Duplicate\\AES_enc.jar "+uploadFileName+" C:\\Duplicate\\argument\\Client");
    //    p_exe.waitForStarted();
    //    p_exe.waitForFinished();
    //    p_exe.close();


    QProcess p_exe(nullptr);
    p_exe.setWorkingDirectory("C:\\Duplicate");
    p_exe.start("cmd",QStringList()<<"/c"<<"C:\\Duplicate\\SM4.exe 1 "+uploadFileName+" C:\\Duplicate\\argument\\Client\\C.sm4 C:\\Duplicate\\argument\\Client\\key.txt");
    p_exe.waitForStarted();
    p_exe.waitForFinished();
    p_exe.close();


    qDebug()<<"利用Elgamal加密key.txt";
    QProcess p_key(nullptr);
    p_key.start("cmd",QStringList()<<"/c"<<"java -jar C:\\Duplicate\\Elgamal.jar 0 C:\\Duplicate\\argument\\Client\\key.txt C:\\Duplicate\\argument\\Client\\Ckey.txt");
    p_key.waitForStarted();
    p_key.waitForFinished();
    p_key.close();

    qDebug()<<"利用Elgamal加密policy.txt";
    QProcess p_elg(nullptr);
    p_elg.start("cmd",QStringList()<<"/c"<<"java -jar C:\\Duplicate\\Elgamal.jar 0 C:\\Duplicate\\argument\\Client\\policy.txt C:\\Duplicate\\argument\\Client\\Cpolicy.txt");
    p_elg.waitForStarted();
    p_elg.waitForFinished();
    p_elg.close();


    qDebug()<<"打包C||Ckey||Cpolicy||Cx||Verify";
    QProcess p(nullptr);
    p.start("cmd", QStringList()<<"/c"<<"jar -cfM0 C:\\Duplicate\\argument\\Client\\SCO.jar"
                                        " -C C:\\Duplicate\\argument\\Client\\ C.sm4"
                                        " -C C:\\Duplicate\\argument\\Client\\ Cpolicy.txt"
                                        " -C C:\\Duplicate\\argument\\Client\\ Ckey.txt"
                                        " -C C:\\Duplicate\\argument\\Client\\ C_xa.txt"
                                        " -C C:\\Duplicate\\argument\\Client\\ SCO_Verify.jar");
    p.waitForStarted();
    p.waitForFinished();
    p.close();

}


// 向CSP发送文件
void UI::sendFile(QString filename)
{
    bytesReceived = 0;
    totalBytes = 0;
    filenameSize = 0;

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
        qDebug()<<"open file success!";


    // 获取文件大小
    this->totalBytes = localFile->size();
    QDataStream sendout(&outBlock,QIODevice::WriteOnly);
    sendout.setVersion(QDataStream::Qt_4_8);
    QString currentFileName = filename.right(filename.size()-filename.lastIndexOf('/')-1);


    // 保留总代大小信息空间、文件名大小信息空间、文件名
    sendout<<qint64(0)<<qint64(0)<<"filename"+currentFileName;
    totalBytes += outBlock.size();
    sendout.device()->seek(0);
    sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

    bytestoWrite = totalBytes-fileSocket->write(outBlock);
    outBlock.resize(0);
}


// 计算pow并返回给CSP pow||id||name
void UI::getPOW()
{
    qDebug()<<"计算POW，并将结果返回给CSP";

    //    QProcess p_exe(nullptr);
    //    p_exe.start("cmd",QStringList()<<"/c"<<"C:\\Duplicate\\POW1.exe  " + uploadFileName +" 10 "+pow_chal_dir[0]+" "+pow_chal_dir[1]+ " " +pow_chal_dir[2]);
    //    p_exe.waitForStarted();
    //    p_exe.waitForFinished();
    //    QString strTemp=QString::fromLocal8Bit(p_exe.readAllStandardOutput());
    //    p_exe.close();
    //    strTemp = strTemp.left(30);

    QString strTemp="123";

    this->fileSocket = new QTcpSocket(this);
    fileSocket->abort();
    fileSocket->connectToHost("127.0.0.1",8888);

    QString FileName = uploadFileName.right(uploadFileName.size()-uploadFileName.lastIndexOf('/')-1);

    QDataStream sendout(&outBlock,QIODevice::WriteOnly);
    sendout.setVersion(QDataStream::Qt_4_8);
    QString currentFileName = "pow"+strTemp+"\n1234\n"+FileName+attr;
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


// 向KS发送sign||attr
void UI::sendKS()
{

    qDebug()<<"Client向KS发送sign||attr";

    QString attrs = "";

    if(ui->checkBox->isChecked())
        attrs = attrs+";A";
    if(ui->checkBox_2->isChecked())
        attrs = attrs+";B";
    if(ui->checkBox_3->isChecked())
        attrs = attrs+";C";

    QString array = "sign:"+sign+"\nattr:"+attrs.right(attrs.length()-1)+attr;

    this->fileSocket = new QTcpSocket(this);
    fileSocket->abort();
    fileSocket->connectToHost("127.0.0.1",4444);


    QDataStream sendout(&outBlock,QIODevice::WriteOnly);
    sendout.setVersion(QDataStream::Qt_4_8);
    QString currentFileName = array;
    totalBytes += outBlock.size();

    //保留总代大小信息空间、文件名大小信息空间、文件名
    sendout<<qint64(0)<<qint64(0)<<currentFileName;
    sendout.device()->seek(0);
    sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

    fileSocket->write(outBlock);
    outBlock.resize(0);
    totalBytes = 0;
}


// 数据发送
void UI::updateFileProgress(qint64 numBytes)
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


// 向KS发送更新属性
void UI::sendNewAttr()
{
    QString attrs = ";"+attr;

    if(ui->checkBox->isChecked()&&attr!="A")
        attrs = attrs+";A";
    if(ui->checkBox_2->isChecked()&&attr!="B")
        attrs = attrs+";B";
    if(ui->checkBox_3->isChecked()&&attr!="C")
        attrs = attrs+";C";

    QTcpSocket* ksSocket = new QTcpSocket(this);
    ksSocket->abort();
    ksSocket->connectToHost("127.0.0.1",4444);

    QString sm3_attr = file_sm3;

    QDataStream sendout(&outBlock,QIODevice::WriteOnly);
    sendout.setVersion(QDataStream::Qt_4_8);
    QString currentFileName = "sm3"+attr+sm3_attr+"\n"+attrs.right(attrs.length()-1);
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
void UI::update_CSP()
{
    qDebug()<<"向CSP发送update tokens"<<newTokens;
    this->fileSocket = new QTcpSocket(this);
    fileSocket->abort();
    fileSocket->connectToHost("127.0.0.1",8888);


    QDataStream sendout(&outBlock,QIODevice::WriteOnly);
    sendout.setVersion(QDataStream::Qt_4_8);
    QString currentFileName = newTokens;
    totalBytes += outBlock.size();

    //保留总代大小信息空间、文件名大小信息空间、文件名
    sendout<<qint64(0)<<qint64(0)<<"update"+attr+currentFileName;
    sendout.device()->seek(0);
    sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;


    fileSocket->write(outBlock);
    outBlock.resize(0);
    totalBytes = 0;
    fileSocket->close();

}


