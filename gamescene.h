#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <vector>
#include <QTimer>
#include <QPoint>
#include <QSize>
#include <QLabel>
#include <QPixmap>
#include <QKeyEvent>
#include <QPushButton>
#include <QMainWindow>

class GameScene : public QMainWindow
{
    Q_OBJECT
public:
    explicit GameScene(QWidget *parent = nullptr);

    //读取并初始化
    void initScene();

    //主界面地址
    QWidget * widget;

signals:

public slots:
    void updateTime();

private:
    //成员变量
    //时间
    int time;
    //人物信息
    int fig_py;//人物纵坐标
    QSize fig_s;//尺寸
    int fig_m;//运动状态，1为跳起，2为下降，0为无动作
    int fig_t;//运动开始时间
    QPixmap fig_pix;
    //背景信息
    std::vector<QPoint> bg_p;
    std::vector<QSize> bg_s;
    std::vector<QPixmap> bg_pix;
    double bg_v;
    //道具信息
    std::vector<QLabel *> pr;
    std::vector<int> pr_v;
    //得分信息
    int score;
    int score_v;
    int ac_score;
    QLabel * score_l;
    int max_score;
    //暂停状态
    int pause;
    //暂停界面按钮
    QPushButton * pauceCancelBtn;
    QPushButton * pauseSaveBtn;
    //退出界面按钮
    QPushButton * exitBtn;
    //定时器
    QTimer * timer;

    //成员函数
    //键盘事件处理函数
    void keyPressEvent(QKeyEvent * event) override;
    //绘图事件
    void paintEvent(QPaintEvent * event) override;
    //是否接触，接触到bug返回1，接触到ac返回2
    int isContact();
    //保存
    void saveScene();
    //读取失败界面
    void failScene();
};

#endif // GAMESCENE_H
