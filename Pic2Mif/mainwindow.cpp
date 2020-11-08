#include "mainwindow.h"
#include "ui_mainwindow.h"
int opencvProc(MainWindow::win2opencv* pw2o,MainWindow::opencv2win* po2w);
int getWidthAndHeight(MainWindow::win2opencv* pw2o,MainWindow::opencv2win* po2w);
#define RGB565	1
#define RGB888	2
#define GRAY8	3

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow){

    ui->setupUi(this);
    this->setFixedSize(1100,700);
    this->setStyleSheet("MainWindow{background-color:rgb(46,47,48);"
                        "}");

    QString strHelp="使用说明:\n1. 点击\"加载图片\"按钮加载图片, 加载成功后按钮上的文字变成\"加载成功\". 在图片显示框中可以看到显示的图片, 在图片属性框中显示加载图片名称和分辨率;\n2. 选择图片输出数据格式, 有RGB565, RGB888和Gray8三种格式, 默认为RGB565格式. 其中RGB565格式数据位宽16bit, RGB888数据格式为24bit, Gray8数据位宽为8bit;\n3. 点击\"一键转换\"按钮生成文件, 选择文件存放的路径之后mif文件生成成功.";
    QString strAbout="作者:Daniel\t\n2020-11-06";

    /*窗体上方的菜单栏*/
    QMenuBar* mBar=menuBar();
    /*菜单栏的"帮助"选项*/
    QAction* help=mBar->addAction("帮助");
    connect(help,&QAction::triggered,
            [=](){
                QMessageBox::about(this,"帮助",strHelp);
            }
            );

    /*菜单栏的"关于"选项*/
    QAction* about=mBar->addAction("关于");
    connect(about,&QAction::triggered,
            [=](){
                QMessageBox::about(this,"关于",strAbout);
            }
            );

    /*窗体下方添加状态栏*/
    QStatusBar* sBar=statusBar();
    QLabel* operationStatus=new QLabel(this);
    operationStatus->setFont(QFont("Times", 15, QFont::Bold));
    operationStatus->setText("请先加载图片");
    sBar->addWidget(operationStatus);

    QLabel* copyRightStatus=new QLabel(this);
    copyRightStatus->setText("copyRight:Daniel");
    sBar->addPermanentWidget(copyRightStatus);

    QLabel* imageStatus=new QLabel(this);
    imageStatus->setFont(QFont("Times", 15, QFont::Bold));
    sBar->addWidget(imageStatus);
    QString imageStatusStr;

    /*美化一下按钮样式*/
    ui->loadFile->setStyleSheet("QPushButton{background-color:rgb(85,85,86);"
                                "color:rgb(208,208,208);"
                                "}");
    /*给UI中的"加载图片"按钮添加信号,打开文件对话框*/
    connect(ui->loadFile,&QPushButton::clicked,
            [=]() mutable{
                w2o.pathImage=QFileDialog::getOpenFileName(this,"选择一张图片","../",
                                                       "image:(*.png *.jpg *.bmp);;"
                                                       "all(*.*)");
                //qDebug()<<w2o.pathImage;
                /*预览图片*/
                QPixmap pix;
                pix.load(w2o.pathImage);
                ui->imageView->setPixmap(pix);
                if(!getWidthAndHeight(&(this->w2o),&(this->o2w))){
                    operationStatus->setText("图片加载成功!");
                    imageStatus->setText(imageStatusStr.sprintf("\t\t图片分辨率:%d*%d",this->o2w.width,this->o2w.height));
                }
                else{
                    operationStatus->setText("图片加载失败!");
                    imageStatus->setText("\t\t图片分辨率:0*0");
                }
                //qDebug()<<this->w2o.model;
            });
    /*美化一下按钮样式*/
    ui->transform->setStyleSheet("QPushButton{background-color:rgb(85,85,86);"
                                "color:rgb(208,208,208);"
                                "}");
    /*给UI中的"一键转换"按钮添加信号,打开保存文件对话框*/
    connect(ui->transform,&QPushButton::clicked,
            [=]() mutable{
              w2o.pathMif=QFileDialog::getSaveFileName(this,"选择保存路径","/","mif(*.mif)");
              opencvProc(&(this->w2o),&(this->o2w));
            });
}

/*析构函数*/
MainWindow::~MainWindow(){
    delete ui;
}

/*三个Radio的信号处理函数,根据选择的不同mif格式赋值*/
void MainWindow::on_Rgb565_clicked(){
    this->w2o.model=RGB565;
}

void MainWindow::on_Rgb888_clicked(){
    this->w2o.model=RGB888;
}

void MainWindow::on_Gray8_clicked(){
    this->w2o.model=GRAY8;
}
