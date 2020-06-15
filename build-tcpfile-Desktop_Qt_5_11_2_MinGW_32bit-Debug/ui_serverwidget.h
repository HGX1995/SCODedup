/********************************************************************************
** Form generated from reading UI file 'serverwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SERVERWIDGET_H
#define UI_SERVERWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ServerWidget
{
public:
    QGridLayout *gridLayout;
    QTextBrowser *textBrowser;
    QLabel *label;
    QProgressBar *progressBar_fileProgres;
    QLabel *label_2;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *horizontalSpacer;
    QLabel *label_3;
    QProgressBar *progressBar;
    QPushButton *pushButton_xn;

    void setupUi(QWidget *ServerWidget)
    {
        if (ServerWidget->objectName().isEmpty())
            ServerWidget->setObjectName(QStringLiteral("ServerWidget"));
        ServerWidget->resize(509, 388);
        gridLayout = new QGridLayout(ServerWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        textBrowser = new QTextBrowser(ServerWidget);
        textBrowser->setObjectName(QStringLiteral("textBrowser"));

        gridLayout->addWidget(textBrowser, 2, 0, 1, 4);

        label = new QLabel(ServerWidget);
        label->setObjectName(QStringLiteral("label"));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\255\227\351\205\267\345\240\202\346\270\205\346\245\267\344\275\223"));
        font.setPointSize(26);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label, 1, 2, 1, 1);

        progressBar_fileProgres = new QProgressBar(ServerWidget);
        progressBar_fileProgres->setObjectName(QStringLiteral("progressBar_fileProgres"));
        progressBar_fileProgres->setValue(24);

        gridLayout->addWidget(progressBar_fileProgres, 3, 2, 1, 2);

        label_2 = new QLabel(ServerWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 3, 0, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(120, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 1, 3, 1, 1);

        horizontalSpacer = new QSpacerItem(120, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 1, 0, 1, 1);

        label_3 = new QLabel(ServerWidget);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 4, 0, 1, 1);

        progressBar = new QProgressBar(ServerWidget);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setValue(24);

        gridLayout->addWidget(progressBar, 4, 2, 1, 2);

        pushButton_xn = new QPushButton(ServerWidget);
        pushButton_xn->setObjectName(QStringLiteral("pushButton_xn"));

        gridLayout->addWidget(pushButton_xn, 0, 3, 1, 1);


        retranslateUi(ServerWidget);

        QMetaObject::connectSlotsByName(ServerWidget);
    } // setupUi

    void retranslateUi(QWidget *ServerWidget)
    {
        ServerWidget->setWindowTitle(QApplication::translate("ServerWidget", "ServerWidget", nullptr));
        label->setText(QApplication::translate("ServerWidget", "\346\234\215\345\212\241\345\231\250", nullptr));
        label_2->setText(QApplication::translate("ServerWidget", "\346\226\207\344\273\266\344\270\212\344\274\240\350\277\233\345\272\246", nullptr));
        label_3->setText(QApplication::translate("ServerWidget", "\346\226\207\344\273\266\344\270\213\350\275\275\350\277\233\345\272\246", nullptr));
        pushButton_xn->setText(QApplication::translate("ServerWidget", "KS-Xn", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ServerWidget: public Ui_ServerWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERVERWIDGET_H
