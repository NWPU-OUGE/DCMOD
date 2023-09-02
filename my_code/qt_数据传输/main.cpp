#include "mainwindow.h"
#include <QApplication>
#include <QLibrary>
#include <QDebug>
#include <QMessageBox>
#include <windows.h>

typedef int (*Fun)(unsigned long ucComNo, unsigned long baud);
typedef int (*Fun_1)();

QLibrary mylib("D:\\workspace\\test\\Debug\\test.dll");   //声明所用到的dll文件

Fun_1 getX=(Fun_1)mylib.resolve("get_x");
Fun_1 getY=(Fun_1)mylib.resolve("get_y");
Fun_1 getZ=(Fun_1)mylib.resolve("get_z");
Fun_1 get_data=(Fun_1)mylib.resolve("run");

float v_x = 0;
float v_y = 0;
float v_z = 0;

int main(int argc, char *argv[])
{
    //setbuf(stdout,NULL);
    QApplication app(argc,argv);
    QLibrary mylib("D:\\workspace\\test\\Debug\\test.dll");   //声明所用到的dll文件
    if (mylib.load())              //判断是否正确加载
    {
        QMessageBox::information(NULL,"OK","DLL load is OK!");
        if (get_data != NULL && getX != NULL && getY != NULL && getZ != NULL)
        {
            get_data();	//调用接口函数
            Sleep(100);
            v_x = (float)getX();
            v_y = (float)getY();
            v_z = (float)getZ();
            printf("%f %f %f\n", v_x, v_y, v_z);
            //qDebug()<<v_x<<v_y<<v_z<<Qt::endl;
           // fflush(stdout);
        }

        QMessageBox::information(NULL,"NO","Linke to Function is not OK!!!!");
    }
    else
        QMessageBox::information(NULL,"NO","DLL is not loaded!");

    return 0;  //加载失败则退出28
}
