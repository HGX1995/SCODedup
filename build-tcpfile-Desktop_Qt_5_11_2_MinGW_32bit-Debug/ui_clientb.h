/********************************************************************************
** Form generated from reading UI file 'clientb.ui'
**
** Created by: Qt User Interface Compiler version 5.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLIENTB_H
#define UI_CLIENTB_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ClientB
{
public:
    QPushButton *Btn_getfile;
    QPushButton *Btn_upload;
    QPushButton *Btn_download;

    void setupUi(QWidget *ClientB)
    {
        if (ClientB->objectName().isEmpty())
            ClientB->setObjectName(QStringLiteral("ClientB"));
        ClientB->resize(400, 300);
        Btn_getfile = new QPushButton(ClientB);
        Btn_getfile->setObjectName(QStringLiteral("Btn_getfile"));
        Btn_getfile->setGeometry(QRect(30, 70, 93, 28));
        Btn_upload = new QPushButton(ClientB);
        Btn_upload->setObjectName(QStringLiteral("Btn_upload"));
        Btn_upload->setGeometry(QRect(30, 20, 93, 28));
        Btn_download = new QPushButton(ClientB);
        Btn_download->setObjectName(QStringLiteral("Btn_download"));
        Btn_download->setGeometry(QRect(40, 140, 93, 28));

        retranslateUi(ClientB);

        QMetaObject::connectSlotsByName(ClientB);
    } // setupUi

    void retranslateUi(QWidget *ClientB)
    {
        ClientB->setWindowTitle(QApplication::translate("ClientB", "Form", nullptr));
        Btn_getfile->setText(QApplication::translate("ClientB", "get file", nullptr));
        Btn_upload->setText(QApplication::translate("ClientB", "upload", nullptr));
        Btn_download->setText(QApplication::translate("ClientB", "download", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ClientB: public Ui_ClientB {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLIENTB_H
