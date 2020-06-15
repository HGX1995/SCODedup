#include "ks.h"

ks::ks(QObject *parent) : QObject(parent)
{
    qDebug()<<"ks strat";

    //文件传送套接字
    this->filesocket = new QTcpSocket(this);
    this->fileserver = new QTcpServer(this);
    this->fileserver->listen(QHostAddress::Any,4444);
    connect(this->fileserver,SIGNAL(newConnection()),this,SLOT(acceptDataConnection()));

    // 文件传送相关变量初始化
    bytesReceived = 0;
    totalBytes = 0;
    filenameSize = 0;

    // 建立数据库
    db = QSqlDatabase::addDatabase("QSQLITE","ks");
    db.setDatabaseName("ksDB.db");
    db.setUserName("root");
    db.setPassword("123456");


    if(db.open())
    {
        qDebug()<<"open database success";

        QSqlQuery query(db);
        if(db.tables().contains("ks_attr"))
        {
            qDebug()<<"ks_attr已存在";
            //                        QString deleteTable = "DROP TABLE ks_attr";
            //                        qDebug()<<query.exec(deleteTable);
        }
        else
        {
            QString createTable = "create TABLE ks_attr (attr varchar(100) not null, Cattr varchar(100) not null)";
            if(query.exec(createTable))
                qDebug()<<"create table success";
            else
                qDebug()<<"create table Fail!";
        }

        if(db.tables().contains("ks_key"))
        {
            qDebug()<<"ks_key已存在";
//                                                QString deleteTable = "DROP TABLE ks_key";
//                                                qDebug()<<query.exec(deleteTable);
        }
        else
        {
            QString createTable = "create TABLE ks_key (token varchar(100) not null, key varchar(1000) not null)";
            if(query.exec(createTable))
                qDebug()<<"create table success";
            else
                qDebug()<<"create table Fail!";
        }


    }
    else
        qDebug()<<"open database Fail!";

    db.close();

}


void ks::acceptDataConnection()
{
    qDebug()<<"文件传输连接成功 feat.KS";
    bytesWritten = 0;
    perDataSize = 64*1024;
    this->filesocket = this->fileserver->nextPendingConnection();

    connect(filesocket,SIGNAL(readyRead()),this,SLOT(receviceData()));
}


void ks::receviceData()
{
    qDebug()<<"KS收到消息";

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

            // 接收到的数据为H(F)||A
            if(filename.startsWith("Hash"))
            {
                filename = filename.right(filename.length()-4);

                if(filename[0]=="A")
                    port = 5555;
                if(filename[0]=="B")
                    port = 6666;
                if(filename[0]=="C")
                    port = 7777;

                qDebug()<<"用户port:"<<port;

                filename = filename.right(filename.length()-1);

                QString fileHash = filename.left(filename.length()-1);
                QString attr = filename.right(1);

                qDebug()<<"KS收到用户传来的 H(F)"<<fileHash;
                qDebug()<<"KS收到用户传来的 attr"<<attr;

                file_sm3 = fileHash;

                if(db.open())
                    qDebug()<<"db open success! feat.H(F)||A";
                else
                    qDebug()<<"db open Fail! feat.H(F)||A";


                QSqlQuery query(db);
                QString search = "select Cattr from ks_attr where attr like '"+attr+"'";
                query.exec(search);

                db.close();

                QString sign;
                if(query.next())
                {
                    sign = query.value(0).toString();
                    qDebug()<<"当前Cattr存在";
                }
                else
                {

                    QString attr_sm3;
                    QByteArray str;
                    str = QCryptographicHash::hash(attr.toLatin1(),QCryptographicHash::Md5);
                    attr_sm3.append(str.toHex());

                    sign = attr_sm3;

                    qDebug()<<"当前Cattr不存在，将其插入数据库ks_attr";

                    if(db.open())
                        qDebug()<<"db open success! feat.H(F)||A";
                    else
                        qDebug()<<"db open Fail! feat.H(F)||A";

                    QSqlQuery query(db);
                    QString add = "insert into ks_attr (attr,Cattr) values (:attr, :Cattr)";
                    query.prepare(add);

                    qDebug()<<"insert attr:"<<attr;
                    qDebug()<<"insert Cattr:"<<attr_sm3;

                    query.bindValue(":attr",attr);
                    query.bindValue(":Cattr",attr_sm3);


                    int res = query.exec();
                    if(res)
                        qDebug()<<"插入成功";
                    else
                        qDebug()<<"Insert Fail!"<<db.lastError().text();

                    db.close();
                }

                QString HF_A = fileHash.append(sign);
                QString HF_A_res;
                QByteArray str;
                str = QCryptographicHash::hash(HF_A.toLatin1(),QCryptographicHash::Md5);
                HF_A_res.append(str.toHex());

                qDebug()<<"向Client发送H(H(F)||Ka)"<<HF_A_res;

                this->command = new QTcpSocket(this);
                command->abort();
                command->connectToHost("127.0.0.1",port);


                QDataStream sendout(&outBlock,QIODevice::WriteOnly);
                sendout.setVersion(QDataStream::Qt_4_8);
                totalBytes += outBlock.size();


                //保留总代大小信息空间、文件名大小信息空间、文件名
                sendout<<qint64(0)<<qint64(0)<<"HF_A"+HF_A_res;
                sendout.device()->seek(0);
                sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

                command->write(outBlock);
                outBlock.resize(0);

                bytesReceived = 0;
                totalBytes = 0;
                filenameSize = 0;
            }


            // 接收到的数据为sign||attr
            if(filename.startsWith("sign"))
            {
                if(filename[filename.length()-1]=="A")
                    port = 5555;
                if(filename[filename.length()-1]=="B")
                    port = 6666;
                if(filename[filename.length()-1]=="C")
                    port = 7777;

                qDebug()<<"用户port:"<<port;
                filename = filename.left(filename.length()-1);

                qDebug()<<filename;

                QList<QString> tmp = filename.split("\n");

                QString sign = tmp[0].mid(tmp[1].indexOf(":")+1);
                QList<QString> attrs = tmp[1].split(":")[1].split(";");
                QString attr =" ";
                for(int i=0;i<attrs.length();i++)
                {
                    attr = attr+"sn:"+attrs[i]+" ";
                }

                int number;
                if(attrs.length()==1)
                {
                    attr = attr + " sn:" + attrs[0]+" ";
                    attr = attr+"1of"+QString::number(attrs.length()+1);
                    number = attrs.length()+1;
                }
                else
                {
                    attr = attr+"1of"+QString::number(attrs.length());
                    number = attrs.length();
                }

                qDebug()<<"接收到Client的"<<"sign:"<<sign<<"attr:"<<attr;

                if(db.open())
                    qDebug()<<"db open success! feat.getAttr";
                else
                    qDebug()<<"db open Fail! feat.getAttr";

                QList<QString> tokens;
                QSqlQuery query(db);

                for(int i=0;i<attrs.length();i++)
                {
                    QString search = "select Cattr from ks_attr where attr like '"+attrs[i]+"'";
                    query.exec(search);

                    if(query.next())
                    {
                        QString token = file_sm3;
                        token.append(query.value(0).toString());

                        QString tmp;
                        QByteArray str;
                        str = QCryptographicHash::hash(token.toLatin1(),QCryptographicHash::Md5);
                        tmp.append(str.toHex());
                        tokens.append(tmp);

                        qDebug()<<"当前Cattr"+attrs[i]+"存在"<<query.value(0).toString();
                    }
                    else
                    {
                        QString tmp;
                        QByteArray str;
                        str = QCryptographicHash::hash(attrs[i].toLatin1(),QCryptographicHash::Md5);
                        tmp.append(str.toHex());

                        QString token = file_sm3;
                        token.append(tmp);

                        QString tmp_token;
                        QByteArray str_token;
                        str_token = QCryptographicHash::hash(token.toLatin1(),QCryptographicHash::Md5);
                        tmp_token.append(str.toHex());
                        tokens.append(tmp_token);

                        qDebug()<<"当前Cattr不存在，将其插入数据库ks_attr";

                        if(db.open())
                            qDebug()<<"db open success! feat.getAttr";
                        else
                            qDebug()<<"db open Fail! feat.getAttr";

                        QSqlQuery query(db);
                        QString add = "insert into ks_attr (attr,Cattr) values (:attr, :Cattr)";
                        query.prepare(add);

                        qDebug()<<"insert attr:"<<attrs[i];
                        qDebug()<<"insert Cattr:"<<tmp;

                        query.bindValue(":attr",attrs[i]);
                        query.bindValue(":Cattr",tmp);


                        int res = query.exec();
                        if(res)
                            qDebug()<<"插入成功";
                        else
                            qDebug()<<"Insert Fail!"<<db.lastError().text();


                    }
                }

                db.close();

                qDebug()<<"所有的token生成"<<attr;


                QProcess p(nullptr);
                p.start("cmd", QStringList()<<"/c"<<"java -jar C:\\Duplicate\\CPABE_Enc.jar "+QString::number(number)+" "+QString::number(count)+attr);
                p.waitForStarted();
                p.waitForFinished();

                qDebug()<<"CP-ABE加密,将Ckey_Cloud.txt插入数据库";

                if(db.open())
                    qDebug()<<"db open success! feat.Ckey_Cloud";
                else
                    qDebug()<<"db open Fail! feat.Ckey_Cloud";


                key_dir = "C://Duplicate//argument//KS//Ckey_Cloud"+QString::number(count)+".txt";
                cx = "C://Duplicate//argument//KS//C_xa"+QString::number(count)+".txt";
                for(int i=0;i<tokens.length();i++)
                {
                    QSqlQuery query(db);
                    QString add = "insert into ks_key (token,key) values (:token,:key)";
                    query.prepare(add);

                    qDebug()<<"insert token:"<<tokens[i];
                    qDebug()<<"insert key:"<<key_dir;

                    query.bindValue(":token",tokens[i]);
                    query.bindValue(":key",key_dir);

                    int res = query.exec();
                    if(res)
                        qDebug()<<"插入成功";
                    else
                        qDebug()<<"Insert Fail!"<<db.lastError().text();
                }


                while(query.next())
                    qDebug()<<"+++"<<query.value(0).toString()<<query.value(1).toString();

                db.close();

                count++;

                QString token_tmp = tokens.at(0);
                for(int i=1;i<tokens.length();i++)
                    token_tmp = token_tmp+";"+tokens[i];

                qDebug()<<"将所有的token发送给Client";
                this->command = new QTcpSocket(this);
                command->abort();
                command->connectToHost("127.0.0.1",port);


                QDataStream sendout(&outBlock,QIODevice::WriteOnly);
                sendout.setVersion(QDataStream::Qt_4_8);
                totalBytes += outBlock.size();


                //保留总代大小信息空间、文件名大小信息空间、文件名
                sendout<<qint64(0)<<qint64(0)<<"tokens"+token_tmp;
                sendout.device()->seek(0);
                sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

                command->write(outBlock);
                outBlock.resize(0);

                bytesReceived = 0;
                totalBytes = 0;
                filenameSize = 0;

            }


            // 计算所有属性的Ki返回给用户
            if(filename.startsWith("attrs"))
            {
                filename = filename.right(filename.length()-6);

                QList<QString> attrs = filename.split(";");


                qDebug()<<"KS收到用户传来的 attrs"<<attrs;

                qDebug()<<"将所有的Ki发送给Client";

                if(db.open())
                    qDebug()<<"db open success! feat.H(F)||A";
                else
                    qDebug()<<"db open Fail! feat.H(F)||A";


                QList<QString> Cattrs;
                for(int i=0;i<attrs.length();i++)
                {

                    QSqlQuery query(db);
                    QString search = "select Cattr from ks_attr where attr like '"+attrs[i]+"'";
                    query.exec(search);


                    if(query.next())
                    {
                        Cattrs.append(query.value(0).toString());
                        qDebug()<<attrs[i]<<"的Cattr存在";
                    }
                    else
                    {
                        QString attr_sm3;
                        QByteArray str;
                        str = QCryptographicHash::hash(attrs[i].toLatin1(),QCryptographicHash::Md5);
                        attr_sm3.append(str.toHex());
                        Cattrs.append(attr_sm3);

                        qDebug()<<attrs[i]<<"的Cattr不存在，将其插入数据库ks_attr";

                        QSqlQuery query(db);
                        QString add = "insert into ks_attr (attr,Cattr) values (:attr, :Cattr)";
                        query.prepare(add);

                        qDebug()<<"insert attr:"<<attrs[i];
                        qDebug()<<"insert Cattr:"<<attr_sm3;

                        query.bindValue(":attr",attrs[i]);
                        query.bindValue(":Cattr",attr_sm3);


                        int res = query.exec();
                        if(res)
                            qDebug()<<"插入成功";
                        else
                            qDebug()<<"Insert Fail!"<<db.lastError().text();


                    }
                }

                db.close();


                QString Cattr = Cattrs.at(0);
                for(int i=1;i<Cattrs.length();i++)
                {
                    Cattr = Cattr+";"+Cattrs[i];
                }
                this->command = new QTcpSocket(this);
                command->abort();
                command->connectToHost("127.0.0.1",3333);


                QDataStream sendout(&outBlock,QIODevice::WriteOnly);
                sendout.setVersion(QDataStream::Qt_4_8);
                totalBytes += outBlock.size();


                //保留总代大小信息空间、文件名大小信息空间、文件名
                sendout<<qint64(0)<<qint64(0)<<"Ki"+Cattr;
                sendout.device()->seek(0);
                sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

                command->write(outBlock);
                outBlock.resize(0);

                bytesReceived = 0;
                totalBytes = 0;
                filenameSize = 0;
            }


            // 将CPABE生成的C_xa.txt发送给用户
            if(filename.startsWith("cx"))
            {
                if(filename[filename.length()-1]=="A")
                    port = 5555;
                if(filename[filename.length()-1]=="B")
                    port = 6666;
                if(filename[filename.length()-1]=="C")
                    port = 7777;

                qDebug()<<"用户port:"<<port;
                qDebug()<<"将生成的C_xa传输给Client";
                sendFile(cx,port);

                bytesReceived = 0;
                totalBytes = 0;
                filenameSize = 0;
            }


            // CSP请求下载
            if(filename.startsWith("download"))
            {
                QString token = filename.right(filename.length()-8);

                if(db.open())
                    qDebug()<<"db open success! feat.download";
                else
                    qDebug()<<"db open Fail! feat.download";


                QSqlQuery query(db);
                QString search = "select key from ks_key where token like '"+token+"'";
                query.exec(search);

                if(query.next())
                {
                    qDebug()<<"向CSP传输"<<query.value(0).toString();
                    sendFile(query.value(0).toString(),8888);
                }
                else
                {
                    qDebug()<<"不存在Ckey_Cloud.txt!!!";
                }

                db.close();

                bytesReceived = 0;
                totalBytes = 0;
                filenameSize = 0;
            }


            // 收到更新请求
            if(filename.startsWith("sm3"))
            {

                filename = filename.right(filename.length()-3);
                if(filename[0]=="A")
                    port = 5555;
                if(filename[0]=="B")
                    port = 6666;
                if(filename[0]=="C")
                    port = 7777;
                qDebug()<<"用户port:"<<port;

                filename = filename.right(filename.length()-1);
                QList<QString> tmp = filename.split("\n");

                QString file_sm3 = tmp[0];
                qDebug()<<file_sm3;
                QList<QString> attrs = tmp[1].split(";");
                QString attr =" ";
                for(int i=0;i<attrs.length();i++)
                {
                    attr = attr+"sn:"+attrs[i]+" ";
                }

                int number;

                if(attrs.length()==1)
                {
                    attr = attr + " sn:" + attrs[0]+" ";
                    attr = attr+"1of"+QString::number(attrs.length()+1);
                    number = attrs.length()+1;
                }
                else
                {
                    attr = attr+"1of"+QString::number(attrs.length());
                    number = attrs.length();
                }


                qDebug()<<"接收到Client的更新attr:"<<attr;

                if(db.open())
                    qDebug()<<"db open success! feat.getAttr";
                else
                    qDebug()<<"db open Fail! feat.getAttr";

                QList<QString> tokens;
                QSqlQuery query(db);
                for(int i=0;i<attrs.length();i++)
                {
                    QString search = "select Cattr from ks_attr where attr like '"+attrs[i]+"'";
                    query.exec(search);

                    if(query.next())
                    {
                        QString token = file_sm3;
                        token.append(query.value(0).toString());

                        QString token_tmp;
                        QByteArray str;
                        str = QCryptographicHash::hash(token.toLatin1(),QCryptographicHash::Md5);
                        token_tmp.append(str.toHex());

                        tokens.append(token_tmp);
                        qDebug()<<"当前Cattr-"+attrs[i]+"存在"<<query.value(0).toString();
                    }
                    else
                    {

                        QString attr_sm3;
                        QByteArray str;
                        str = QCryptographicHash::hash(attrs[i].toLatin1(),QCryptographicHash::Md5);
                        attr_sm3.append(str.toHex());


                        QString token = file_sm3;
                        token.append(attr_sm3);
                        QString token_tmp;
                        str = QCryptographicHash::hash(token.toLatin1(),QCryptographicHash::Md5);
                        token_tmp.append(str.toHex());

                        tokens.append(token_tmp);

                        qDebug()<<"当前Cattr不存在，将其插入数据库ks_attr";

                        if(db.open())
                            qDebug()<<"db open success! feat.getAttr";
                        else
                            qDebug()<<"db open Fail! feat.getAttr";

                        QSqlQuery query(db);
                        QString add = "insert into ks_attr (attr,Cattr) values (:attr, :Cattr)";
                        query.prepare(add);

                        qDebug()<<"insert attr:"<<attrs[i];
                        qDebug()<<"insert Cattr:"<<attr_sm3;

                        query.bindValue(":attr",attrs[i]);
                        query.bindValue(":Cattr",attr_sm3);


                        int res = query.exec();
                        if(res)
                            qDebug()<<"插入成功";
                        else
                            qDebug()<<"Insert Fail!"<<db.lastError().text();


                    }
                }

                QString search = "select key from ks_key where token like '"+tokens[0]+"'";
                qDebug()<<"=========="<<tokens[0];
                query.exec(search);
                QString flag;
                if(query.next())
                {
                    flag = query.value(0).toString();
                    qDebug()<<"需要更新的云密钥位置:"<<flag;
                    search = "delete from ks_key where key like '"+flag+"'";
                    int res = query.exec(search);
                    if(res)
                        qDebug()<<"删除成功 feat.update KS";
                    else
                        qDebug()<<"删除失败！！！ feat.update KS";

                    for(int i=0;i<tokens.length();i++)
                    {
                        QString add = "insert into ks_key (token,key) values (:token,:key)";
                        query.prepare(add);

                        qDebug()<<"insert token:"<<tokens[i];
                        qDebug()<<"insert key:"<<flag;

                        query.bindValue(":token",tokens[i]);
                        query.bindValue(":key",flag);

                        int res = query.exec();
                        if(res)
                            qDebug()<<"插入成功";
                        else
                            qDebug()<<"Insert Fail!"<<db.lastError().text();
                    }



                    time2 = clock();
                    QProcess p(nullptr);
                    p.start("cmd", QStringList()<<"/c"<<"java -jar C:\\Duplicate\\CPABE_Enc.jar "+QString::number(number)+" "+QString::number(count)+attr);
                    p.waitForStarted();
                    p.waitForFinished();
                    qDebug()<<"============CPABE"<<clock()-time2;


                    QString token_tmp = tokens.at(0);
                    for(int i=1;i<tokens.length();i++)
                        token_tmp = token_tmp+";"+tokens[i];

                    qDebug()<<"将所有更新后的token发送给HighLevel-Client"<<token_tmp;
                    this->command = new QTcpSocket(this);
                    command->abort();
                    command->connectToHost("127.0.0.1",port);


                    QDataStream sendout(&outBlock,QIODevice::WriteOnly);
                    sendout.setVersion(QDataStream::Qt_4_8);
                    totalBytes += outBlock.size();

                    cx = "C://Duplicate//argument//KS//C_xa"+QString::number(count)+".txt";
                    count++;

                    //保留总代大小信息空间、文件名大小信息空间、文件名
                    sendout<<qint64(0)<<qint64(0)<<"Newtokens"+token_tmp;
                    sendout.device()->seek(0);
                    sendout<<totalBytes<<qint64(outBlock.size())-sizeof(qint64)*2;

                    command->write(outBlock);
                    outBlock.resize(0);

                    bytesReceived = 0;
                    totalBytes = 0;
                    filenameSize = 0;

                    qDebug()<<"向HighLevel传输新的C_xa";


                }
                else
                {
                    qDebug()<<"当前需要替换的token不存在！！！";
                }

                db.close();

                bytesReceived = 0;
                totalBytes = 0;
                filenameSize = 0;

            }


            // 向HighLevel发送新的Cx
            if(filename.startsWith("new"))
            {
                if(filename[3]=="A")
                    port = 5555;
                if(filename[3]=="B")
                    port = 6666;
                if(filename[3]=="C")
                    port = 7777;
                qDebug()<<"用户port:"<<port;
                sendFile(cx,port);

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

        bytesReceived = 0;
        totalBytes = 0;
        filenameSize = 0;
        localFile->close();
        isString = true;
    }
}


void ks::sendFile(QString filename, quint16 port)
{
    qDebug()<<"======="<<filename;
    this->fileSocket = new QTcpSocket(this);
    fileSocket->abort();
    fileSocket->connectToHost("127.0.0.1",port);

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

void ks::updateFileProgress(qint64 numBytes)
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
        localFile->close();


    ////如果发送完毕
    if(bytesWritten == totalBytes)
    {
        qDebug()<<"文件"<<localFile->fileName()<<"发送成功 feat.KS";
        localFile->close();
        fileSocket->close();
        bytesReceived = 0;
        totalBytes = 0;
        filenameSize = 0;
    }
}
