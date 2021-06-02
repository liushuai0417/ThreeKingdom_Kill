#include "dialog.h"
#include <QApplication>
#include<QIcon>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    w.setWindowIcon(QIcon(":/res/icon/icon.png"));
    w.setWindowTitle("三国Kill");

    w.show();
//test pull
    //push test
    return a.exec();
}
