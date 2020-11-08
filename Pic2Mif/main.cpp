#include "mainwindow.h"
#include<QApplication>
#include<opencv2/opencv.hpp>
#include<QString>
#include<cstdlib>
#include<string>
/*RadioButton的选项*/
#define RGB565	1
#define RGB888	2
#define GRAY8	3
using namespace cv;

/*进行mif文件转换的函数*/
int opencvProc(MainWindow::win2opencv* pw2o,MainWindow::opencv2win* po2w);
/*给窗体反馈图片长宽数据的函数*/
int getWidthAndHeight(MainWindow::win2opencv* pw2o,MainWindow::opencv2win* po2w);
QString strTypeError="\t\t\n出错了,请检查你的文件路径及文件类型!\n\n";	//出错提示信息,很有可能不是一张图片
QString strStillError="\t\t\n又出错了,我tm让你检查文件类型看不见吗?\n\n";		//暴躁老哥,在线骂娘
QString strPreViewInfo="PerView Of Each Point";
QString strSucceed="\n\t转换成功!";

int main(int argc, char* argv[]){

    QApplication a(argc, argv);
    MainWindow w;
    /*测试一下你导入的opencv库是否成功*/
    //cv::Mat srcMat=cv::imread("C:\\Users\\Daniel\\Desktop\\lena.jpg");
    //cv::imshow("src",srcMat);
    w.show();

    a.exec();
    return 0;
}

/*
 * 得到图片的属性信息:长和宽
 * 成功返回0
 * 失败返回-1,并打印提示信息
*/
int getWidthAndHeight(MainWindow::win2opencv* pw2o,MainWindow::opencv2win* po2w){
    cv::Mat srcMat;

    try{
        /*这里如果读不到图片opencv竟然不会抛异常,看来不止是图片,什么都可以读,不能显示的时候才会报异常*/
        srcMat=cv::imread(pw2o->pathImage.toStdString());

        /*
         * 这里show一下图片,一是为了抛出可能的异常(读入的不是图片),
         * 再者opencv的图片预览在Qt里还能放大直接看像素
         * 无心插柳柳成荫
        */
        cv::imshow(strPreViewInfo.toStdString(),srcMat);
        po2w->height=srcMat.rows;
        po2w->width=srcMat.cols;
        return 0;
    }catch(cv::Exception){
        /*一旦出现异常就弹出出错对话框,并返回-1*/
        QDialog* errDialog=new QDialog();
        QLabel* errLabel=new QLabel(strTypeError,errDialog);
        errDialog->setAttribute(Qt::WA_DeleteOnClose);
        errDialog->setFixedSize(QSize(500,90));
        errDialog->setFont(QFont("Times",15,QFont::Bold));
        errDialog->show();
        return -1;
    }
}

/*
 * 进行mif文件转换
 * 成功返回0
 * 失败返回-1,并打印提示信息
*/
int opencvProc(MainWindow::win2opencv* pw2o,MainWindow::opencv2win* po2w){
    cv::Mat srcMat;
    try{
        srcMat=cv::imread(pw2o->pathImage.toStdString());
        /*这里col的作用纯属是为了抛出可能的异常(读入的不是图片),我都没有接受他的返回值*/
        srcMat.col(0);

        std::ofstream ofs(pw2o->pathMif.toStdString(),std::ios::out|std::ios::binary);

        int i=0;
        int j=0;
        /*如果选择的是RGB565*/
        if(pw2o->model==RGB565){
            unsigned char r=0;
            unsigned char g=0;
            unsigned char b=0;
            unsigned int rgb=0;

            /*mif文件的头部信息*/
            ofs<<"WIDTH=16;\n"<<"DEPTH="<<(po2w->width)*(po2w->height)<<";\n\n";
            ofs<<"ADDRESS_RADIX=UNS;\nDATA_RADIX=HEX;\n\n";
            ofs<<"CONTENT BEGIN\n";
            /*这个处理起来有点麻烦*/
            for(i=0;i<po2w->height;++i){
                for(j=0;j<po2w->width;++j){
                    ofs<<"\t"<<std::dec<<j+i*po2w->width<<"\t:\t";
                    rgb=0;
                    /*先将24位真彩图的每个通道值做移位除法进行压缩,因为最终的mif文件每个像素点只有16位色彩信息*/
                    /*opencv的是BGR哦,所以反过来*/
                    r=((unsigned char)srcMat.at<cv::Vec3b>(j,i)[2])>>3;
                    g=((unsigned char)srcMat.at<cv::Vec3b>(j,i)[1])>>2;
                    b=((unsigned char)srcMat.at<cv::Vec3b>(j,i)[0])>>3;
                    /*把上面被压缩的三个通道的数据"位或"到一个32位int型数据上*/
                    rgb=((((rgb|r)<<6)|g)<<5)|b;
                    /*以十六进制打印这个数就可以了*/
                    ofs<<std::hex<<rgb<<";\n";
                }
            }
            /*mif文件尾部信息*/
            ofs<<"END;";
        }
        /*如果选择的是RGB888*/
        else if(pw2o->model==RGB888){
            ofs<<"WIDTH=24;\n"<<"DEPTH="<<(po2w->width)*(po2w->height)<<"\n\n";
            ofs<<"ADDRESS_RADIX=UNS;\nDATA_RADIX=HEX;\n\n";
            ofs<<"CONTENT BEGIN\n";
            for(i=0;i<po2w->height;++i){
                for(j=0;j<po2w->width;++j){
                    /*这个转化比较简单*/
                    ofs<<"\t"<<std::dec<<j+i*po2w->width<<"\t:\t";
                    ofs<<std::hex<<(unsigned int)srcMat.at<cv::Vec3b>(j,i)[2]
                      <<std::hex<<(unsigned int)srcMat.at<cv::Vec3b>(j,i)[1]
                     <<std::hex<<(unsigned int)srcMat.at<cv::Vec3b>(j,i)[0]<<";\n";
                }
            }
            ofs<<"END;";
        }
        /*如果选的是Gray8*/
        else{
            ofs<<"WIDTH=8;\n"<<"DEPTH="<<(po2w->width)*(po2w->height)<<"\n\n";
            ofs<<"ADDRESS_RADIX=UNS;\nDATA_RADIX=HEX;\n\n";
            ofs<<"CONTENT BEGIN\n";
            unsigned char gray=0;
            for(i=0;i<po2w->height;++i){
                for(j=0;j<po2w->width;++j){
                    /*这个跟现成的软件出来的结果有一定差距,可能是采用了不同的灰度化策略,理论上这样做是没问题的*/
                    gray=0;
                    ofs<<"\t"<<std::dec<<j+i*po2w->width<<"\t:\t";
                    /*取三个通道的均值作为灰度值*/
                    gray=(srcMat.at<Vec3b>(j,i)[0]+srcMat.at<Vec3b>(j,i)[1]+srcMat.at<Vec3b>(j,i)[2]) / 3;
                    ofs<<std::hex<<(unsigned int)gray<<";\n";
                }
            }
            ofs<<"END;";
        }
        ofs.close();
        QDialog* succeedDialog=new QDialog();
        QLabel* succeedLabel=new QLabel(strSucceed,succeedDialog);
        succeedDialog->setAttribute(Qt::WA_DeleteOnClose);
        succeedDialog->setFixedSize(QSize(240,70));
        succeedDialog->setFont(QFont("Times",15,QFont::Bold));
        succeedDialog->show();
        return 0;
    }catch(cv::Exception err){
        QDialog* errDialog=new QDialog();
        QLabel* errLabel=new QLabel(strStillError,errDialog);
        errDialog->setAttribute(Qt::WA_DeleteOnClose);
        errDialog->setFixedSize(QSize(500,90));
        errDialog->setFont(QFont("Times",15,QFont::Bold));
        errDialog->show();
        return -1;
    }
}
