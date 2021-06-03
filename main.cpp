#include "dialog.h"
#include <QApplication>
#include<QIcon>
#include<ckernel.h>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CKernel::GetInstance();
    return a.exec();
}
