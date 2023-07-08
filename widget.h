#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QProcess>
#include <QMessageBox>
#include <QRect>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QGraphicsScene>

#include "gamescene.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

    void paintEvent(QPaintEvent *event) override;

    GameScene * gamescene = NULL;

private slots:


    void on_exitButton_clicked();

private:
    Ui::Widget *ui;

};

#endif // WIDGET_H
