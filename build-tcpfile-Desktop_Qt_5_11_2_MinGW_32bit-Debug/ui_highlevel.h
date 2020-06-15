/********************************************************************************
** Form generated from reading UI file 'highlevel.ui'
**
** Created by: Qt User Interface Compiler version 5.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HIGHLEVEL_H
#define UI_HIGHLEVEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_HighLevel
{
public:
    QPushButton *Btn_change;
    QPushButton *Btn_getFile;
    QPushButton *Btn_csp;

    void setupUi(QWidget *HighLevel)
    {
        if (HighLevel->objectName().isEmpty())
            HighLevel->setObjectName(QStringLiteral("HighLevel"));
        HighLevel->resize(400, 300);
        Btn_change = new QPushButton(HighLevel);
        Btn_change->setObjectName(QStringLiteral("Btn_change"));
        Btn_change->setGeometry(QRect(290, 20, 93, 28));
        Btn_getFile = new QPushButton(HighLevel);
        Btn_getFile->setObjectName(QStringLiteral("Btn_getFile"));
        Btn_getFile->setGeometry(QRect(20, 20, 93, 28));
        Btn_csp = new QPushButton(HighLevel);
        Btn_csp->setObjectName(QStringLiteral("Btn_csp"));
        Btn_csp->setGeometry(QRect(30, 100, 93, 28));

        retranslateUi(HighLevel);

        QMetaObject::connectSlotsByName(HighLevel);
    } // setupUi

    void retranslateUi(QWidget *HighLevel)
    {
        HighLevel->setWindowTitle(QApplication::translate("HighLevel", "Form", nullptr));
        Btn_change->setText(QApplication::translate("HighLevel", "change", nullptr));
        Btn_getFile->setText(QApplication::translate("HighLevel", "getFile", nullptr));
        Btn_csp->setText(QApplication::translate("HighLevel", "change-CSP", nullptr));
    } // retranslateUi

};

namespace Ui {
    class HighLevel: public Ui_HighLevel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HIGHLEVEL_H
