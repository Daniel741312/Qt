#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include<QMainWindow>
#include<QMenuBar>
#include<QMenu>
#include<QDialog>
#include<QDebug>
#include<QLabel>
#include<QMessageBox>
#include<QAction>
#include<QFileDialog>
#include<QStatusBar>
#include<QPixmap>
#include<QGraphicsView>
#include<QImage>
#include<iostream>
#include<opencv2/opencv.hpp>
#include<QString>
#include<cstdlib>
#include<cstdio>
#include<string>
#include<algorithm>
#include<iostream>
#include<fstream>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

/*
 * 定义两个结构体作为窗体类的成员变量,其作用是函数之间传递参数;
 * 第一个是窗体对象要给后台opencv函数传递的数据:图片的路径,要保存mif文件的路径和选择的mif格式(默认为2);
 * 第二个是后台opencv函数要传递给窗体对象的数据:第一二个参数没用上,只用了三四,将图片的长宽传递给窗体显示即可;
*/
    struct win2opencv{
        QString pathImage;
        QString pathMif;
        int model=2;
    }w2o;

    struct opencv2win{
        int status;
        QString nameIamge;
        int width=0;
        int height=0;
    }o2w;

/*三个RadioButton的槽函数,在UI中直接GUI添加的,你要是觉得low可以自己写三个connect*/
private slots:
    void on_Rgb565_clicked();

    void on_Rgb888_clicked();

    void on_Gray8_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
