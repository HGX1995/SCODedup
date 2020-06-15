#include "serverwidget.h"
#include "ui_serverwidget.h"

#include <QFileDialog>
#include <QDebug>
#include <QFileInfo>

ServerWidget::ServerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerWidget)
{
    ui->setupUi(this);

    //文件传送套接字
    this->filesocket = new QTcpSocket(this);
    this->fileserver = new QTcpServer(this);
    this->fileserver->listen(QHostAddress::Any,8888);
    connect(this->fileserver,SIGNAL(newConnection()),this,SLOT(acceptFileConnection()));


    // 文件传送相关变量初始化
    bytesReceived = 0;
    totalBytes = 0;
    filenameSize = 0;


    // 建立数据库
    db = QSqlDatabase::addDatabase("QSQLITE","server");
    db.setDatabaseName("testDB.db");
    db.setUserName("root");
    db.setPassword("123456");


    if(db.open())
    {
        qDebug()<<"open database success";

        QSqlQuery query(db);
        if(db.tables().contains("duplicate"))
        {
            qDebug()<<"duplicate已存在";
            QString deleteTable = "DROP TABLE duplicate";
            qDebug()<<query.exec(deleteTable);
        }
        else
        {
            QString createTable = "create TABLE duplicate (token varchar(100) not null, SCO_dir varchar(100) not null, pow_chal_dir varchar(100) not null, pow_ver varchar(100) not null, id varchar(100), name varchar(100) not null)";
            if(query.exec(createTable))
                qDebug()<<"create table success";
            else
                qDebug()<<"create table Fail!";
        }

    }
    else
        qDebug()<<"open database Fail!";

    db.close();


    qDebug()<<"CSP start";

}


ServerWidget::~ServerWidget()
{

}


void ServerWidget::acceptFileConnection()
{
    bytesWritten = 0;
    perDataSize = 64*1024;
    this->filesocket = this->fileserver->nextPendingConnection();
    connect(filesocket,SIGNAL(readyRead()),this,SLOT(updateFileProgress()));
}


// 接收从Client传输的数据
void ServerWidget::updateFileProgress()
{

    QDataStream inFile(this->filesocket);
    inFile.setVersion(QDataStream::Qt_4_8);


    //如果接收到的数据小于16个字节，保存到来的文件头结构
    if(bytesReceived <= sizeof(qint64)*2)
    {
        qDebug()<<"1";

        if((filesocket->bytesAvailable()>=(sizeof(qint64))*2) && (filenameSize==0))
        {
            qDebug()<<"2";
            inFile>>totalBytes>>filenameSize;
            bytesReceived += sizeof(qint64)*2;
        }
        if((filesocket->bytesAvailable()>=filenameSize) && (filenameSize != 0))
        {
            qDebug()<<"3";
            inFile>>filename;
            qDebug()<<filename;

            // 接收到的数据为token
            if(filename.startsWith("HF"))
            {
                QString token = filename.right(filename.length()-3);
                if(filename[2]=="A")
                    port = 1111;
                if(filename[2]=="B")
                    port = 2222;
                if(filename[2]=="C")
                    port = 3333;
                qDebug()<<"用户port:"<<port;

                qDebug()<<"收到Client传来的 token"<<token;

                if(db.open())
                    qDebug()<<"db open success! feat.token";
                else
                    qDebug()<<"db open Fail! feat.token";

                QSqlQuery query(db);
                QString search = "select * from duplicate where token like '"+token+"'";
                query.exec(search);

                db.close();

                QString sign;
                if(query.next())
                {
                    veri_token = token;
                    sign = "exist yes";
                    qDebug()<<"当前文件存在，需要进行所有权证明";

                    if(db.open())
                        qDebug()<<"db open success! feat.token";
                    else
                        qDebug()<<"db open Fail! feat.token";

                    db.close();


                    pow_chal_dir = query.value(2).toString();
                    pow_ver = query.value(3).toString();

                    QString sign = "pow"+pow_chal_dir+"\n"+pow_ver;
                    this->command = new QTcpSocket(this);
                    command->abort();
                    command->connectToHost("127.0.0.1",port);


                    QDataStream sendout(&outBlock,QIODevice::WriteOnly);
                    sendout.setVersion(QDataStream::Qt_4_8);
                    totalBytes += outBlock.size();


                    //保留总代大小信息空间、文件名大小信息空间、文件名
                    sendout<<qint64(0)<<qint64(0)<<sign;
                    sendout.device()->seek(0);
                    sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

                    command->write(outBlock);
                    outBlock.resize(0);

                    bytesReceived = 0;
                    totalBytes = 0;
                    filenameSize = 0;
                }
                else
                {
                    sign = "exist no:signed string";
                    qDebug()<<"当前文件不存在,向用户传输数字签名";

                    this->command = new QTcpSocket(this);
                    command->abort();
                    command->connectToHost("127.0.0.1",port);


                    QDataStream sendout(&outBlock,QIODevice::WriteOnly);
                    sendout.setVersion(QDataStream::Qt_4_8);
                    totalBytes += outBlock.size();


                    //保留总代大小信息空间、文件名大小信息空间、文件名
                    sendout<<qint64(0)<<qint64(0)<<sign;
                    sendout.device()->seek(0);
                    sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

                    command->write(outBlock);
                    outBlock.resize(0);

                    bytesReceived = 0;
                    totalBytes = 0;
                    filenameSize = 0;
                }

            }


            // 接收到的数据为token||filePOW||id||name
            if(filename.startsWith("token"))
            {
                if(filename[filename.length()-1]=="A")
                    port = 1111;
                if(filename[filename.length()-1]=="B")
                    port = 2222;
                if(filename[filename.length()-1]=="C")
                    port = 3333;
                qDebug()<<"用户port:"<<port;

                filename = filename.left(filename.length()-1);

                qDebug()<<"服务器收到pow||SCO||id"<<filename;
                QList<QString> tmp = filename.split("\n");

                token = tmp[0].split(":")[1].split(";");
                pow_chal_dir = tmp[1].split(":")[1];
                pow_ver = tmp[2].split(":")[1];
                id = tmp[3].split(":")[1];
                name =tmp[4].split(":")[1];

                qDebug()<<"token:"<<token;
                qDebug()<<"pow_chal_dir"<<pow_chal_dir;
                qDebug()<<"pow_ver"<<pow_ver;
                qDebug()<<"id"<<id;
                qDebug()<<"name"<<name;

                this->command = new QTcpSocket(this);
                command->abort();
                command->connectToHost("127.0.0.1",port);


                QDataStream sendout(&outBlock,QIODevice::WriteOnly);
                sendout.setVersion(QDataStream::Qt_4_8);
                totalBytes += outBlock.size();


                //保留总代大小信息空间、文件名大小信息空间、文件名
                QString sign = "token success!";
                sendout<<qint64(0)<<qint64(0)<<sign;
                sendout.device()->seek(0);
                sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

                command->write(outBlock);
                outBlock.resize(0);

                bytesReceived = 0;
                totalBytes = 0;
                filenameSize = 0;

            }


            if(filename.startsWith("SCO"))
            {
                isString = false;
                qDebug()<<"服务器收到"<<filename;

                time_kp = clock();

                bytesReceived += filenameSize;
                ////接收的文件放在指定目录下
                filename = "C://Duplicate//argument//Server//SCO"+QString::number(count)+".jar";
                count++;

                // 将记录插入数据库中
                if(db.open())
                    qDebug()<<"db open success! feat.H(F)||A";
                else
                    qDebug()<<"db open Fail! feat.H(F)||A";

                QSqlQuery query(db);

                for(int i=0;i<token.length();i++)
                {

                    QString add;

                    if(i==0)
                    {
                        add = "insert into duplicate (token,SCO_dir,pow_chal_dir,pow_ver,id,name) values (:token, :SCO_dir, :pow_chal_dir, :pow_ver, :id, :name)";
                        qDebug()<<"insert token:"<<token[i];
                        qDebug()<<"insert SCO_dir:"<<filename;
                        qDebug()<<"insert pow_chal_dir:"<<pow_chal_dir;
                        qDebug()<<"insert pow_ver:"<<pow_ver;
                        qDebug()<<"insert id:"<<id;
                        qDebug()<<"insert name:"<<name;

                        query.prepare(add);
                        query.bindValue(":token",token[i]);
                        query.bindValue(":SCO_dir",filename);
                        query.bindValue(":pow_ver",pow_ver);
                        query.bindValue(":pow_chal_dir",pow_chal_dir);
                        query.bindValue(":id",id);
                        query.bindValue(":name",name);
                    }
                    else
                    {
                        add = "insert into duplicate (token,SCO_dir,pow_chal_dir,pow_ver,name) values (:token, :SCO_dir, :pow_chal_dir, :pow_ver, :name)";
                        qDebug()<<"insert token:"<<token[i];
                        qDebug()<<"insert SCO_dir:"<<filename;
                        qDebug()<<"insert pow_chal_dir:"<<pow_chal_dir;
                        qDebug()<<"insert pow_ver:"<<pow_ver;
                        qDebug()<<"insert name:"<<name;

                        query.prepare(add);
                        query.bindValue(":token",token[i]);
                        query.bindValue(":SCO_dir",filename);
                        query.bindValue(":pow_ver",pow_ver);
                        query.bindValue(":pow_chal_dir",pow_chal_dir);
                        query.bindValue(":name",name);
                    }


                    int res = query.exec();
                    if(res)
                        qDebug()<<"插入成功";
                    else
                        qDebug()<<"Insert Fail!"<<db.lastError().text();
                }

                db.close();


                localFile = new QFile(filename);
                if(!localFile->open(QFile::WriteOnly))
                {
                    qDebug()<<"Server::open file error!";
                    return;
                }

            }


            if(filename.startsWith("download"))
            {
                QString token = filename.right(filename.length()-8);
                if(token[0]=="A")
                    port = 1111;
                if(token[0]=="B")
                    port = 2222;
                if(token[0]=="C")
                    port = 3333;
                qDebug()<<"用户port:"<<port;

                token = token.right(token.length()-1);
                download_token = token;

                bytesReceived = 0;
                totalBytes = 0;
                filenameSize = 0;

                if(db.open())
                    qDebug()<<"db open success! feat.H(F)||A";
                else
                    qDebug()<<"db open Fail! feat.H(F)||A";

                QSqlQuery query(db);
                QString search = "select SCO_dir from duplicate where token like '"+token+"'";
                query.exec(search);
                db.close();

                if(query.next())
                {
                    download_dir = query.value(0).toString();
                    getXn();
                }
                else
                {
                    qDebug()<<"当前SCO不存在！！！";

                    this->command = new QTcpSocket(this);
                    command->abort();
                    command->connectToHost("127.0.0.1",port);

                    QDataStream sendout(&outBlock,QIODevice::WriteOnly);
                    sendout.setVersion(QDataStream::Qt_4_8);
                    totalBytes += outBlock.size();
                    count++;

                    QString msg = "fail";
                    //保留总代大小信息空间、文件名大小信息空间、文件名
                    sendout<<qint64(0)<<qint64(0)<<msg;
                    sendout.device()->seek(0);
                    sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

                    command->write(outBlock);
                    outBlock.resize(0);

                    bytesReceived = 0;
                    totalBytes = 0;
                    filenameSize = 0;
                }
            }


            if(filename.startsWith("pow"))
            {
                filename = filename.right(filename.length()-3);
                bytesReceived = 0;
                totalBytes = 0;
                filenameSize = 0;

                if(filename[filename.length()-1]=="A")
                    port = 1111;
                if(filename[filename.length()-1]=="B")
                    port = 2222;
                if(filename[filename.length()-1]=="C")
                    port = 3333;
                qDebug()<<"用户port:"<<port;

                filename = filename.left(filename.length()-1);

                QString pow = filename.split("\n")[0];
                QString id = filename.split("\n")[1];
                QString name = filename.split("\n")[2];

                qDebug()<<"pow_ver:"<<pow_ver;
                qDebug()<<"pow:"<<pow;

                if(pow == pow_ver)
                {
                    qDebug()<<"验证成功"<<"token:"<<veri_token;

                    // 将记录插入数据库中
                    if(db.open())
                        qDebug()<<"db open success! feat.H(F)||A";
                    else
                        qDebug()<<"db open Fail! feat.H(F)||A";

                    QSqlQuery query(db);
                    QString add = "update duplicate set id = '"+id+"', name = '"+name+"' where token like '"+veri_token+"'";


                    int res = query.exec(add);
                    if(res)
                        qDebug()<<"新用户插入成功";
                    else
                        qDebug()<<query.lastError();
                    db.close();

                    this->command = new QTcpSocket(this);
                    command->abort();
                    command->connectToHost("127.0.0.1",port);


                    QDataStream sendout(&outBlock,QIODevice::WriteOnly);
                    sendout.setVersion(QDataStream::Qt_4_8);
                    totalBytes += outBlock.size();


                    //保留总代大小信息空间、文件名大小信息空间、文件名
                    QString sign = "success";
                    sendout<<qint64(0)<<qint64(0)<<sign;
                    sendout.device()->seek(0);
                    sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

                    command->write(outBlock);
                    outBlock.resize(0);

                    bytesReceived = 0;
                    totalBytes = 0;
                    filenameSize = 0;

                }
            }


            if(filename.startsWith("filename"))
            {
                isString = false;
                filename = filename.right(filename.length()-8);
                qDebug()<<"服务器收到"<<filename;

                if(filename.contains("Ckey_Cloud"))
                    filename = "Ckey_Cloud.txt";

                bytesReceived += filenameSize;
                ////接收的文件放在指定目录下
                filename = "C://Duplicate//argument//Server//"+filename;

                localFile = new QFile(filename);
                if(!localFile->open(QFile::WriteOnly))
                {
                    qDebug()<<"Server::open file error!";
                    return;
                }

            }


            // 收到update token
            if(filename.startsWith("update"))
            {
                filename = filename.right(filename.length()-6);
                token.clear();

                if(filename[0]=="A")
                    port=1111;
                if(filename[0]=="B")
                    port=2222;
                if(filename[0]=="C")
                    port=3333;
                qDebug()<<"用户port:"<<port;
                filename = filename.right(filename.length()-1);

                token = filename.split(";");

                if(db.open())
                    qDebug()<<"db open success! feat.update";
                else
                    qDebug()<<"db open Fail! feat.update";


                QSqlQuery query(db);
                QString search = "select * from duplicate where token like '"+token[0]+"'";
                query.exec(search);
                QString SCO_dir,pow_chal_dir,pow_ver,id,name;
                if(query.next())
                {
                    SCO_dir = query.value(1).toString();
                    pow_chal_dir = query.value(2).toString();
                    pow_ver = query.value(3).toString();
                    name = query.value(5).toString();
                    qDebug()<<"需要更新的SCO位置:"<<SCO_dir;

                    QProcess p(nullptr);
                    p.setWorkingDirectory("C:\\Duplicate\\argument\\Server\\");
                    p.start("cmd",QStringList()<<"/c"<<"jar -xf " + SCO_dir);
                    p.waitForStarted();
                    p.waitForFinished();
                    p.close();
                }


                this->command = new QTcpSocket(this);
                command->abort();
                command->connectToHost("127.0.0.1",port);


                QDataStream sendout(&outBlock,QIODevice::WriteOnly);
                sendout.setVersion(QDataStream::Qt_4_8);
                totalBytes += outBlock.size();


                //保留总代大小信息空间、文件名大小信息空间、文件名
                QString sign = "update_success";
                sendout<<qint64(0)<<qint64(0)<<sign;
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

        bytesReceived += filesocket->bytesAvailable();
        inBlock = filesocket->readAll();
        localFile->write(inBlock);
        inBlock.resize(0);
    }


    //数据接收完成时
    if(bytesReceived == totalBytes && !isString)
    {
        qDebug()<<"Server接收文件"<<localFile->fileName()<<"成功";
        bytesReceived = 0;
        totalBytes = 0;
        filenameSize = 0;
        localFile->close();
        isString = true;

        if(localFile->fileName().contains("SCO"))
        {
            this->command = new QTcpSocket(this);
            command->abort();
            command->connectToHost("127.0.0.1",port);

            QDataStream sendout(&outBlock,QIODevice::WriteOnly);
            sendout.setVersion(QDataStream::Qt_4_8);
            totalBytes += outBlock.size();


            //保留总代大小信息空间、文件名大小信息空间、文件名
            QString sign = "upload success!";
            sendout<<qint64(0)<<qint64(0)<<sign;
            sendout.device()->seek(0);
            sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

            command->write(outBlock);
            outBlock.resize(0);

            bytesReceived = 0;
            totalBytes = 0;
            filenameSize = 0;

        }

        if(localFile->fileName().contains("Ckey_Cloud"))
        {
            bytesReceived = 0;
            totalBytes = 0;
            filenameSize = 0;

            qDebug()<<"CSP进行部分解密，生成update_key和update_policy。SCO位置为"+download_dir;

            QProcess p(nullptr);
            p.setWorkingDirectory("C:\\Duplicate\\argument\\Server");
            p.start("cmd",QStringList()<<"/c"<<"jar -xf "<<download_dir);
            p.waitForStarted();
            p.waitForFinished();
            p.close();

            QProcess p_exe(nullptr);
            p.setWorkingDirectory("C:\\Duplicate\\argument\\Server");
            p_exe.start("cmd",QStringList()<<"/c"<<"java -jar C:\\Duplicate\\update.jar");
            p_exe.waitForStarted();
            p_exe.waitForFinished();
            p_exe.close();


            QProcess p_pack(nullptr);
            p_pack.start("cmd", QStringList()<<"/c"<<"jar -cfM0 " + download_dir +
                         " -C C:\\Duplicate\\argument\\Server\\ C.sm4" +
                         " -C C:\\Duplicate\\argument\\Server\\ update_key.txt" +
                         " -C C:\\Duplicate\\argument\\Server\\ update_policy.txt" +
                         " -C C:\\Duplicate\\argument\\Server\\ C_xa.txt" +
                         " -C C:\\Duplicate\\argument\\Server\\ SCO_Verify.jar");
            p_pack.waitForStarted();
            p_pack.waitForFinished();
            qDebug()<<QString::fromLocal8Bit(p_pack.readAllStandardOutput());
            p_pack.close();

            qDebug()<<"将SCO返回给Client";
            sendFile(download_dir);

        }

        if(localFile->fileName().contains("C_xa"))
        {

            qDebug()<<token;
            if(db.open())
                qDebug()<<"db open success! feat.update";
            else
                qDebug()<<"db open Fail! feat.update";


            QSqlQuery query(db);
            QString search = "select * from duplicate where token like '"+token[0]+"'";
            query.exec(search);
            QString SCO_dir,pow_chal_dir,pow_ver,id,name;
            if(query.next())
            {
                SCO_dir = query.value(1).toString();
                pow_chal_dir = query.value(2).toString();
                pow_ver = query.value(3).toString();
                name = query.value(5).toString();
                qDebug()<<"需要更新的SCO位置:"<<SCO_dir;

                qDebug()<<"更新SCO";
                time2 = clock();
                QProcess p_pack(nullptr);
                p_pack.setWorkingDirectory("C:\\Duplicate\\argument\\Server");
                p_pack.start("cmd",QStringList()<<"/c"<<"jar -cfM0 " + SCO_dir + " C Ckey.txt Cpolicy.txt C_xa.txt SCO_Verify.jar");
                p_pack.waitForStarted();
                p_pack.waitForFinished();
                p_pack.close();
                qDebug()<<"权限更新，CSP更新"<<clock()-time2;

                search = "select * from duplicate where SCO_dir like '"+SCO_dir+"'";
                query.exec(search);
                QList<QString> tmp;
                QString pow_chal_dir,pow_ver,id,name;
                while(query.next())
                {
                    qDebug()<<"开始更新tokens";
                    for(int i=0;i<6;i++)
                        qDebug()<<query.value(i).toString();
                    tmp.append(query.value(0).toString());
                    pow_chal_dir = query.value(2).toString();
                    pow_ver = query.value(3).toString();
                    name = query.value(5).toString();
                }

                for(int i=0;i<tmp.length();i++)
                {
                    if(!token.contains(tmp[i]))
                    {
                        if(!token.contains(tmp[i]))
                        {
                            search = "delete from duplicate where SCO_dir like '"+SCO_dir+"' and token like '"+tmp[i]+"'";
                            int res = query.exec(search);
                            if(res)
                                qDebug()<<"删除成功";
                            else
                                qDebug()<<"删除失败";
                        }
                    }
                }

                for(int i=0;i<token.length();i++)
                {
                    if(!tmp.contains(token[i]))
                    {
                        QString add = "insert into duplicate (token,SCO_dir,pow_chal_dir,pow_ver,name) values (:token, :SCO_dir, :pow_chal_dir, :pow_ver, :name)";
                        qDebug()<<"insert token:"<<token[i];
                        qDebug()<<"insert SCO_dir:"<<SCO_dir;
                        qDebug()<<"insert pow_chal_dir:"<<pow_chal_dir;
                        qDebug()<<"insert pow_ver:"<<pow_ver;
                        qDebug()<<"insert name:"<<name;

                        query.prepare(add);
                        query.bindValue(":token",token[i]);
                        query.bindValue(":SCO_dir",SCO_dir);
                        query.bindValue(":pow_ver",pow_ver);
                        query.bindValue(":pow_chal_dir",pow_chal_dir);
                        query.bindValue(":name",name);

                        int res = query.exec();
                        if(res)
                            qDebug()<<"插入成功";
                        else
                            qDebug()<<"插入失败";
                    }
                }


            }
            else
                qDebug()<<"当前SCO不存在！！";


            bytesReceived = 0;
            totalBytes = 0;
            filenameSize = 0;

            this->command = new QTcpSocket(this);
            command->abort();
            command->connectToHost("127.0.0.1",port);


            QDataStream sendout(&outBlock,QIODevice::WriteOnly);
            sendout.setVersion(QDataStream::Qt_4_8);
            totalBytes += outBlock.size();

            QString sign = "updateSuccess";
            sendout<<qint64(0)<<qint64(0)<<sign;
            sendout.device()->seek(0);
            sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

            command->write(outBlock);
            outBlock.resize(0);

            bytesReceived = 0;
            totalBytes = 0;
            filenameSize = 0;



        }

        if(localFile->fileName().contains("Cpolicy"))
        {
            this->command = new QTcpSocket(this);
            command->abort();
            command->connectToHost("127.0.0.1",1111);


            QDataStream sendout(&outBlock,QIODevice::WriteOnly);
            sendout.setVersion(QDataStream::Qt_4_8);
            totalBytes += outBlock.size();

            QString sign = "policy";
            sendout<<qint64(0)<<qint64(0)<<sign;
            sendout.device()->seek(0);
            sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

            command->write(outBlock);
            outBlock.resize(0);

            bytesReceived = 0;
            totalBytes = 0;
            filenameSize = 0;
        }

        filesocket->close();
    }

}



//打开文件读取内容并发送
void ServerWidget::sendFile(QString file)
{
    qDebug()<<"sendfile to Client"<<port;
    this->filesocket2 = new QTcpSocket(this);
    filesocket2->abort();
    filesocket2->connectToHost("127.0.0.1",port);   //打算和客户端7777端口建立文件连接
    //设置文件传送进度更新
    connect(filesocket2,SIGNAL(bytesWritten(qint64)),this,SLOT(updateFileProgress(qint64)));


    filename = file;
    this->localFile = new QFile(filename);
    if(!localFile->open(QFile::ReadOnly))
    {
        return;
    }
    //获取文件大小
    this->totalBytes = localFile->size();
    QDataStream sendout(&outBlock,QIODevice::WriteOnly);
    sendout.setVersion(QDataStream::Qt_4_8);
    QString currentFileName = filename.right(filename.size()-filename.lastIndexOf('/')-1);

    qDebug()<<sizeof(currentFileName);
    //保留总代大小信息空间、文件名大小信息空间、文件名
    sendout<<qint64(0)<<qint64(0)<<"fileName"+currentFileName;
    totalBytes += outBlock.size();

    sendout.device()->seek(0);
    sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

    bytestoWrite = totalBytes-filesocket2->write(outBlock);
    outBlock.resize(0);


}


void ServerWidget::updateFileProgress(qint64 numBytes)
{
    //已经发送的数据大小
    bytesWritten += (int)numBytes;

    //如果已经发送了数据
    if(bytestoWrite > 0)
    {
        outBlock = localFile->read(qMin(bytestoWrite,perDataSize));
        ///发送完一次数据后还剩余数据的大小
        bytestoWrite -= ((int)filesocket2->write(outBlock));
        ///清空发送缓冲区
        outBlock.resize(0);
    }
    else
        localFile->close();

    //更新进度条
    ui->progressBar->setMaximum(totalBytes);
    ui->progressBar->setValue(bytesWritten);

    //如果发送完毕
    if(bytesWritten == totalBytes)
    {
        localFile->close();
        qDebug()<<"文件发送完毕 feat.CSP";
        bytesReceived = 0;
        totalBytes = 0;
        filenameSize = 0;

    }
}


void ServerWidget::getXn()
{
    this->fileSocket = new QTcpSocket(this);
    fileSocket->abort();
    fileSocket->connectToHost("127.0.0.1",4444);


    QDataStream sendout(&outBlock,QIODevice::WriteOnly);
    sendout.setVersion(QDataStream::Qt_4_8);
    QString currentFileName = "download"+download_token;
    totalBytes += outBlock.size();


    //保留总代大小信息空间、文件名大小信息空间、文件名
    sendout<<qint64(0)<<qint64(0)<<currentFileName;
    sendout.device()->seek(0);
    sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;


    fileSocket->write(outBlock);
    outBlock.resize(0);

    bytesReceived = 0;
    totalBytes = 0;
    filenameSize = 0;
}
