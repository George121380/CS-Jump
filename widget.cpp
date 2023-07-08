#include "widget.h"
#include "ui_widget.h"
#include <QPainter>
#include <QTimer>
#include <QFile>
#include <QTextStream>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    //设置窗口标题
    setWindowTitle("xkrtt");

    //关闭按钮的功能实现
    connect(ui->exitButton, &QPushButton::clicked, this, &Widget::on_exitButton_clicked);

    //帮助按钮的功能实现
    connect(ui->helpButton, &QPushButton::clicked, [this](){
        QMessageBox::information(this,"help","上箭头或空格代表跳跃，下箭头代表下降，躲避bug，吃到ac，尽你所能得到更高的分数吧！");
    });


    //开始按钮的功能实现
    connect(ui->newButton, &QPushButton::clicked, this, [=](){
        //创建游戏界面
        gamescene = new GameScene;
        gamescene->widget = this;
        this->hide();
        gamescene->show();
    });

    //继续按钮的功能实现
    connect(ui->continueButton, &QPushButton::clicked, this, [=](){
        gamescene = new GameScene;
        gamescene->widget = this;
        this->hide();
        gamescene->initScene();
        gamescene->show();
    });

    //清除记录的功能实现
    connect(ui->clearButton, &QPushButton::clicked, this, [=](){
        QFile file("maxScore.txt");
        file.open(QIODevice::WriteOnly);
        QTextStream out(&file);
        out << 0;
        file.close();

        QFile f("data.bin");
        f.open(QIODevice::WriteOnly);
        f.close();
        QMessageBox::information(this, "hint", "清除成功！");
    });

}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_exitButton_clicked()
{
    this->close();
}

void Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/image/xkrtt.png");
    painter.drawPixmap(0,0,this->width(),this->height(),pix);

}
