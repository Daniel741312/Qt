#include "mywidget.h"
#include<QPushButton>

MyWidget::MyWidget(QWidget *parent) : QWidget(parent)
{
    /*move是基类MyWidget的方法,按钮和窗口都可以调用*/
    this->move(100,100);

    QPushButton* b1=new QPushButton(this);
    b1->setText("^_^");
    b1->move(100,100);
    b1->resize(200,200);

    /*套娃版本*/
    QPushButton* b2=new QPushButton(b1);
    b2->setText("@_@");
    b2->move(10,10);

    MyButton* b3=new MyButton(this);
    b3->setText("123");
}

MyWidget::~MyWidget()
{

}
