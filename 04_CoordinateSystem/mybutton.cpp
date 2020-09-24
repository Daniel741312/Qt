#include "mybutton.h"

MyButton::MyButton(QWidget *parent) : QPushButton(parent)
{

}

MyButton::~MyButton(){
    qDebug()<<"Button has been delete";
}
