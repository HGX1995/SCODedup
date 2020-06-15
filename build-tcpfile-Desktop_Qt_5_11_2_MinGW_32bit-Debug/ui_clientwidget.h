/********************************************************************************
** Form generated from reading UI file 'clientwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLIENTWIDGET_H
#define UI_CLIENTWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ClientWidget
{
public:
    QGridLayout *gridLayout;
    QProgressBar *progressBar;
    QLabel *label_4;
    QLineEdit *lineEdit_filename;
    QLineEdit *lineEdit;
    QSpacerItem *verticalSpacer_2;
    QLabel *label;
    QLabel *label_2;
    QPushButton *pushButton_openFile;
    QPushButton *pushButton;
    QPushButton *pushButton_sendFile;
    QLabel *label_3;
    QProgressBar *progressBar_2;
    QLabel *label_5;
    QPushButton *pushButton_sendAttr;
    QPushButton *pushButton_csp;
    QPushButton *pushButton_ks;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;

    void setupUi(QWidget *ClientWidget)
    {
        if (ClientWidget->objectName().isEmpty())
            ClientWidget->setObjectName(QStringLiteral("ClientWidget"));
        ClientWidget->resize(558, 333);
        gridLayout = new QGridLayout(ClientWidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        progressBar = new QProgressBar(ClientWidget);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setValue(24);

        gridLayout->addWidget(progressBar, 6, 1, 1, 3);

        label_4 = new QLabel(ClientWidget);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout->addWidget(label_4, 3, 1, 1, 1);

        lineEdit_filename = new QLineEdit(ClientWidget);
        lineEdit_filename->setObjectName(QStringLiteral("lineEdit_filename"));

        gridLayout->addWidget(lineEdit_filename, 4, 0, 1, 5);

        lineEdit = new QLineEdit(ClientWidget);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));

        gridLayout->addWidget(lineEdit, 7, 1, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 74, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_2, 9, 4, 1, 1);

        label = new QLabel(ClientWidget);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 2, 0, 1, 2);

        label_2 = new QLabel(ClientWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 3, 0, 1, 1);

        pushButton_openFile = new QPushButton(ClientWidget);
        pushButton_openFile->setObjectName(QStringLiteral("pushButton_openFile"));

        gridLayout->addWidget(pushButton_openFile, 6, 0, 1, 1);

        pushButton = new QPushButton(ClientWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        gridLayout->addWidget(pushButton, 7, 4, 1, 1);

        pushButton_sendFile = new QPushButton(ClientWidget);
        pushButton_sendFile->setObjectName(QStringLiteral("pushButton_sendFile"));

        gridLayout->addWidget(pushButton_sendFile, 3, 4, 1, 1);

        label_3 = new QLabel(ClientWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\255\227\351\205\267\345\240\202\346\270\205\346\245\267\344\275\223"));
        font.setPointSize(26);
        label_3->setFont(font);
        label_3->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_3, 1, 1, 1, 3);

        progressBar_2 = new QProgressBar(ClientWidget);
        progressBar_2->setObjectName(QStringLiteral("progressBar_2"));
        progressBar_2->setValue(24);

        gridLayout->addWidget(progressBar_2, 9, 1, 1, 3);

        label_5 = new QLabel(ClientWidget);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout->addWidget(label_5, 7, 0, 1, 1);

        pushButton_sendAttr = new QPushButton(ClientWidget);
        pushButton_sendAttr->setObjectName(QStringLiteral("pushButton_sendAttr"));

        gridLayout->addWidget(pushButton_sendAttr, 0, 4, 1, 1);

        pushButton_csp = new QPushButton(ClientWidget);
        pushButton_csp->setObjectName(QStringLiteral("pushButton_csp"));

        gridLayout->addWidget(pushButton_csp, 1, 4, 1, 1);

        pushButton_ks = new QPushButton(ClientWidget);
        pushButton_ks->setObjectName(QStringLiteral("pushButton_ks"));

        gridLayout->addWidget(pushButton_ks, 2, 4, 1, 1);

        pushButton_2 = new QPushButton(ClientWidget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));

        gridLayout->addWidget(pushButton_2, 5, 4, 1, 1);

        pushButton_3 = new QPushButton(ClientWidget);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));

        gridLayout->addWidget(pushButton_3, 0, 0, 1, 1);


        retranslateUi(ClientWidget);

        QMetaObject::connectSlotsByName(ClientWidget);
    } // setupUi

    void retranslateUi(QWidget *ClientWidget)
    {
        ClientWidget->setWindowTitle(QApplication::translate("ClientWidget", "Form", nullptr));
        label_4->setText(QApplication::translate("ClientWidget", "\344\274\240\350\276\223\347\253\257\345\217\243\357\274\2328888", nullptr));
        label->setText(QApplication::translate("ClientWidget", "\346\234\215\345\212\241\345\231\250IP\357\274\232127.0.0.1", nullptr));
        label_2->setText(QApplication::translate("ClientWidget", "\350\277\236\346\216\245\347\253\257\345\217\243\357\274\2326666", nullptr));
        pushButton_openFile->setText(QApplication::translate("ClientWidget", "\351\200\211\346\213\251\344\270\212\344\274\240\347\232\204\346\226\207\344\273\266", nullptr));
        pushButton->setText(QApplication::translate("ClientWidget", "\344\270\213\350\275\275\346\226\207\344\273\266", nullptr));
        pushButton_sendFile->setText(QApplication::translate("ClientWidget", "CSP-file", nullptr));
        label_3->setText(QApplication::translate("ClientWidget", "\345\256\242\346\210\267\347\253\257", nullptr));
        label_5->setText(QApplication::translate("ClientWidget", "\346\226\207\344\273\266\344\270\213\350\275\275", nullptr));
        pushButton_sendAttr->setText(QApplication::translate("ClientWidget", "KS-Ka", nullptr));
        pushButton_csp->setText(QApplication::translate("ClientWidget", "CSP-check", nullptr));
        pushButton_ks->setText(QApplication::translate("ClientWidget", "KS-Cx", nullptr));
        pushButton_2->setText(QApplication::translate("ClientWidget", "update", nullptr));
        pushButton_3->setText(QApplication::translate("ClientWidget", "\346\265\213\350\257\225", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ClientWidget: public Ui_ClientWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLIENTWIDGET_H
