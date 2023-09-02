#ifndef DIALOG_H
#define DIALOG_H

#include <QMainWindow>
#include <QWidget>
#include <QApplication>
#include <QPushButton>
#include <QTextEdit>

#define SLEEP_TIME 1000
#define RING_BUFFER_SIZE (52*1024)

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct
{
    float v_x;
    float v_y;
    float v_z;

}DATA;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};
#endif // DIALOG_H
