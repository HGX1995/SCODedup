#include "serverwidget.h"
#include "clientwidget.h"
#include "clientb.h"
#include "ks.h"
#include "ui.h"
#include "highlevel.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ServerWidget w;
    ks s;

//    ClientWidget t;
//    t.show();


    UI clientA;
    clientA.setAttr("A");
    clientA.socketInit();
    clientA.show();

    UI clientB;
    clientB.setAttr("B");
    clientB.socketInit();
    clientB.show();

    UI clientC;
    clientC.setAttr("C");
    clientC.socketInit();
    clientC.show();


    return a.exec();
}
