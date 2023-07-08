#include "gamescene.h"
#include <QTimer>
#include <QString>
#include <QRect>
#include <QFont>
#include <QPainter>
#include <QRandomGenerator>
#include <QFile>
#include <QDataStream>
#include <QTextStream>
#include <QtMath>

//刷新界面的时间间隔（毫秒）
const int t = 10;
//地面纵坐标
const int ground_y = 1000;

//人物跳起高度
const int fig_jh = 800;
//人物下落速度
const int fig_v_d = 100;
//人物横坐标
const int fig_px = 200;


//获取随机数
int getRandomNumber(int range)
{
    return QRandomGenerator::global()->bounded(range);
}


GameScene::GameScene(QWidget *parent) : QMainWindow(parent)
{
    //设置基础信息
    setFixedSize(1500, 1500);
    //setWindowTitle("Game");
    time = 0;

    fig_pix.load(":/image/human1.png");
    fig_s.setWidth(fig_pix.width());
    fig_s.setHeight(fig_pix.height());

    fig_py = ground_y - fig_s.height();

    score = 0;
    score_v = 1;
    ac_score = 200;

    //加载最大得分
    QFile file("maxScore.txt");
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);
    in >> max_score;
    file.close();

    //创建保存、退出按钮
    pauceCancelBtn = new QPushButton(this);
    pauseSaveBtn = new QPushButton(this);

    pauseSaveBtn->setText("Save");
    pauceCancelBtn->setText("Cancel");

    pauseSaveBtn->move(600, 550);
    pauseSaveBtn->setFixedSize(300, 50);
    pauseSaveBtn->hide();

    pauceCancelBtn->move(600, 900);
    pauceCancelBtn->setFixedSize(300, 50);
    pauceCancelBtn->hide();

    pause = 0;

    //创建退出按钮
    exitBtn = new QPushButton(this);
    exitBtn->setText("Exit");
    exitBtn->move(600, 850);
    exitBtn->setFixedSize(300, 50);
    exitBtn->hide();

    //实现保存按钮
    connect(pauseSaveBtn, &QPushButton::clicked, this, [=](){
        saveScene();
        //提示保存成功
        QFont font;
        font.setFamily("Microsoft YaHei");
        font.setPointSize(28);
        QLabel * success = new QLabel("Save successfully!", this);
        success->setFont(font);
        success->setFixedSize(success->sizeHint());
        success->move((this->width() - success->width()) / 2, 300);
        success->show();
        QTimer::singleShot(500, this, [=](){
            success->hide();
        });
    });
    //实现返回按钮
    connect(pauceCancelBtn, &QPushButton::clicked, this, [=](){
        this->pause = 0;
        timer->start();
        this->setFocus();
    });
    //实现退出按钮
    connect(exitBtn, &QPushButton::clicked, this, [=](){
        this->hide();
        widget->show();
    });

    //设置得分
    QFont font;
    font.setFamily("Microsoft YaHei");
    font.setPointSize(20);
    score_l = new QLabel(this);
    score_l->setText("Score:0");
    score_l->setFont(font);
    score_l->move(1100, 0);
    score_l->setFixedSize(score_l->sizeHint());
    score_l->show();

    //定时器
    timer = new QTimer(this);
    timer->start(t);
    connect(timer, &QTimer::timeout, this, &GameScene::updateTime);
}

void GameScene::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    //跳起
    if ((key == Qt::Key_Up || key == Qt::Key_Space) && fig_py + fig_s.height() == ground_y)
    {
        fig_m = 1;
        fig_t = time;
    }
    //下降
    else if (key == Qt::Key_Down && fig_py + fig_s.height() != ground_y)
    {
        fig_m = 2;
        fig_t = time;
    }
    //暂停
    else if (key == Qt::Key_P)
    {
        pause = 1;
    }
    else
        QWidget::keyPressEvent(event);
}

void GameScene::paintEvent(QPaintEvent *event)
{
    if (pause == 3)
    {
        QWidget::paintEvent(event);
        return;
    }

    QPainter painter(this);
    //绘制背景
    painter.fillRect(0, 0, this->width(), this->height(), Qt::white);
    QPixmap bg;
    bg.load(":/image/background.jpg");
    painter.drawPixmap(0, 0, 1500, 1000, bg);

    QPen pen(Qt::black, 3);
    painter.setPen(pen);
    painter.drawLine(0, ground_y + 1, this->width(), ground_y + 1);

    //绘制人物
    int k = 10;//跑动速度
    int ind = time % (4 * k);
    if (ind < k)
        fig_pix.load(":/image/human1.png");
    else if (ind < k * 2)
        fig_pix.load(":/image/human2.png");
    else if (ind < k * 3)
        fig_pix.load(":/image/human3.png");
    else
        fig_pix.load(":/image/human4.png");
    painter.drawPixmap(fig_px, fig_py, fig_s.width(), fig_s.height(), fig_pix);

    //绘制结束界面
    if (pause == 2)
    {
        QFont font;
        font.setFamily("Microsoft YaHei");
        font.setPointSize(16);

        QLabel * background = new QLabel(this);
        background->setAlignment(Qt::AlignCenter);
        background->setFixedSize(500, 500);
        background->move(500, 500);
        background->setStyleSheet("QLabel { "
                                  "border: 1px solid black; "
                                  "border-radius: 30px; "
                                  "background-color: white; "
                                  "color: white; "
                                  "}");
        background->stackUnder(exitBtn);
        for (int i = 0; i < pr.size(); ++i) pr[i]->stackUnder(background);
        background->show();

        QLabel * gameover = new QLabel("Game Over!", this);
        gameover->setFont(font);
        gameover->move((this->width() - gameover->sizeHint().width()) / 2, 520);
        gameover->setFixedSize(gameover->sizeHint());
        gameover->show();

        QLabel * yourscore = new QLabel("Your score is: " + QString::number(score), this);
        yourscore->setFont(font);
        yourscore->move((this->width() - yourscore->sizeHint().width()) / 2, 520 + gameover->height());
        yourscore->setFixedSize(yourscore->sizeHint());
        yourscore->show();

        font.setPointSize(8);
        QLabel * maxscore = new QLabel("best score: " + QString::number(max_score), this);
        maxscore->setFont(font);
        maxscore->move((this->width() - maxscore->sizeHint().width()) / 2, 550 + gameover->height() + yourscore->height());
        maxscore->setFixedSize(maxscore->sizeHint());
        maxscore->show();
    }

    QWidget::paintEvent(event);
}

int GameScene::isContact()
{
    int size = pr.size();
    for (int i = 0; i < size ;++i)
    {
        bool ret1 = fig_px >= pr[i]->pos().x() + pr[i]->sizeHint().width();
        bool ret2 = fig_px + fig_s.width() <= pr[i]->pos().x();
        bool ret3 = fig_py >= pr[i]->pos().y() + pr[i]->sizeHint().height();
        bool ret4 = fig_py + fig_s.height() <= pr[i]->pos().y();
        if (!(ret1 || ret2 || ret3 || ret4))
            return i;
    }
    return -1;
}

void GameScene::saveScene()
{
    QFile file("data.bin");
    file.open(QIODevice::WriteOnly);

    QDataStream out(&file);
    //保存格式：时间 人物纵坐标 人物运动状态 运动开始时间 道具数量 道具信息 得分
    out << time << fig_py << fig_m << fig_t;
    int siz = pr.size();
    out << siz;
    for (int i = 0; i < siz; ++i)
    {
        //道具信息：文本 字体 样式 位置 速度
        out << pr[i]->text() << pr[i]->font() << pr[i]->styleSheet() <<  pr[i]->pos() << pr_v[i];
    }
    out << score;

    file.close();
}

void GameScene::failScene()
{
    //读取失败
    pause = 3;

    QFont font;
    font.setFamily("Microsoft YaHei");
    font.setPointSize(16);

    QLabel * background = new QLabel(this);
    background->setAlignment(Qt::AlignCenter);
    background->setFixedSize(500, 500);
    background->move(500, 500);
    background->setStyleSheet("QLabel { "
                              "border: 1px solid black; "
                              "border-radius: 30px; "
                              "background-color: white; "
                              "color: white; "
                              "}");
    background->stackUnder(exitBtn);
    for (int i = 0; i < pr.size(); ++i) pr[i]->stackUnder(background);
    background->show();

    QLabel * fail = new QLabel("Fail to open!", this);
    fail->setFont(font);
    fail->setFixedSize(fail->sizeHint());
    fail->move((this->width() - fail->width()) / 2, 550);
    fail->show();

    exitBtn->show();
}

void GameScene::initScene()
{
    QFile file("data.bin");
    if (!file.open(QIODevice::ReadOnly))
    {
        failScene();
    }

    if (file.size() == 0)
    {
        failScene();
    }

    QDataStream in(&file);
    in >> time >> fig_py >> fig_m >> fig_t;
    int size;
    in >> size;
    for (int i = 0; i < size; ++i)
    {
        QString text;
        QFont font;
        QString stylesheet;
        QPoint pos;
        int v;
        in >> text >> font >> stylesheet >> pos >> v;
        QLabel * label = new QLabel(text, this);
        label->setFont(font);
        label->setStyleSheet(stylesheet);
        label->setFixedSize(label->sizeHint());
        label->move(pos);
        pr.push_back(label);
        pr_v.push_back(v);
    }
    in >> score;

    file.close();
}

void GameScene::updateTime()
{
    //时间增长
    ++time;
    //更新背景位置

    //更新道具位置
    int pr_size = pr.size();
    for (int i = 0; i < pr_size; ++i)
    {
        pr[i]->move(pr[i]->pos().x() - pr_v[i], pr[i]->pos().y());
        pr[i]->show();
    }
    //更新人物位置
    if (fig_m == 1)
    {
        //人物跳起初速度
        double fig_v_j = 7 * qLn(time);
        //人物跳跃加速度
        double fig_a = fig_v_j * fig_v_j / (2 * fig_jh);

        int v = fig_v_j - (time - fig_t) * fig_a;
        fig_py -= v;
        if (fig_py + fig_s.height() >= ground_y)
        {
            fig_m = 0;
            fig_py = ground_y - fig_s.height();
        }
    }
    else if (fig_m == 2)
    {
        fig_py += fig_v_d;
        if (fig_py + fig_s.height() >= ground_y)
        {
            fig_m = 0;
            fig_py = ground_y - fig_s.height();
        }
    }
    //判定人物和道具是否接触，及相应结算
    int ret = isContact();
    if (ret != -1)
    {
        if(pr[ret]->text() == "Accepted") //碰到ac
        {
            delete pr[ret];
            pr.erase(pr.begin() + ret);
            pr_v.erase(pr_v.begin() + ret);
            score += ac_score;
        }
        else //碰到bug
        {
            //暂停，弹出结束界面
            pause = 2;
            if (score > max_score) max_score = score;
            //写入最大得分
            QFile file("maxScore.txt");
            file.open(QIODevice::WriteOnly);
            QTextStream out(&file);
            out << max_score;
            file.close();
        }
    }
    //随机添加环境
    int thr = getRandomNumber(1000);
    int k = pr.size() - 1;
    bool gate = k == -1 ? true : pr[k]->pos().x() + pr[k]->sizeHint().width() <= 1000;
    if (thr >= 980 && gate)
    {
        int index = getRandomNumber(6);
        int h = getRandomNumber(700);
        int v_pr = getRandomNumber(2) + 250 * 7 * qLn(time) / fig_jh - 1;
        int siz = getRandomNumber(5) + 20;
        pr_v.push_back(v_pr);
        QFont font;
        font.setFamily("Microsoft YaHei");
        font.setPointSize(siz);
        if (index == 0) //Accepted
        {
            QLabel * ac = new QLabel(this);
            pr.push_back(ac);
            ac->setFont(font);
            ac->setStyleSheet("color:blue;");
            ac->setText("Accepted");
            ac->move(this->width(), ground_y - h - ac->sizeHint().height());
            ac->setFixedSize(ac->sizeHint());
        }
        else if (index == 1) //Compile Error
        {
            QLabel * ce = new QLabel(this);
            pr.push_back(ce);
            ce->setFont(font);
            ce->setStyleSheet("color:green;");
            ce->setText("Compile\nError");
            ce->move(this->width(), ground_y - h - ce->sizeHint().height());
            ce->setFixedSize(ce->sizeHint());
        }
        else if (index == 2) //Wrong Answer
        {
            QLabel * wa = new QLabel(this);
            pr.push_back(wa);
            wa->setFont(font);
            wa->setStyleSheet("color:red;");
            wa->setText("Wrong\nAnswer");
            wa->move(this->width(), ground_y - h - wa->sizeHint().height());
            wa->setFixedSize(wa->sizeHint());
        }
        else if (index == 3) //Runtime Error
        {
            QLabel * re = new QLabel(this);
            pr.push_back(re);
            re->setFont(font);
            re->setStyleSheet("color:red;");
            re->setText("Runtime\nError");
            re->move(this->width(), ground_y - h - re->sizeHint().height());
            re->setFixedSize(re->sizeHint());
        }
        else if (index == 4) //Time Limit Exceeded
        {
            QLabel * tle = new QLabel(this);
            pr.push_back(tle);
            tle->setFont(font);
            tle->setStyleSheet("color:red;");
            tle->setText("Time Limit\nExceeded");
            tle->move(this->width(), ground_y - h - tle->sizeHint().height());
            tle->setFixedSize(tle->sizeHint());
        }
        else if (index == 5) //Mwmory Limit Exceeded
        {
            QLabel * mle = new QLabel(this);
            pr.push_back(mle);
            mle->setFont(font);
            mle->setStyleSheet("color:red;");
            mle->setText("Memory Limit\nExceeded");
            mle->move(this->width(), ground_y - h - mle->sizeHint().height());
            mle->setFixedSize(mle->sizeHint());
        }
    }

    //移除离开界面的环境
    pr_size = pr.size();
    for (int i = 0; i < pr_size; ++i)
    {
        if (pr[i]->pos().x() + pr[i]->sizeHint().width() <= 0)
        {
            delete pr[i];
            pr.erase(pr.begin() + i);
            pr_v.erase(pr_v.begin() + i);
        }
    }
    //更新道具和得分的速度，更新得分
    if (pause == 0)
        score += score_v;
    score_l->setText("Score:" + QString::number(score));
    score_l->setFixedSize(score_l->sizeHint());
    score_v = 1 + 0.3 * qLn(time);
    ac_score = 100 * qLn(time);
    //判断是否绘制暂停界面
    if (pause == 0)
    {
        pauseSaveBtn->hide();
        pauceCancelBtn->hide();
    }
    else if (pause == 1)
    {
        pauseSaveBtn->show();
        pauceCancelBtn->show();
        timer->stop();
    }
    else if (pause == 2)
    {
        pauseSaveBtn->hide();
        pauceCancelBtn->hide();
        exitBtn->show();
        timer->stop();
    }
    else
    {
        timer->stop();
    }
    //重新绘图
    repaint();
}
