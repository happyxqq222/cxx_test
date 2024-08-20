//
// Created by happy on 2024/8/20.
//

#include "MainWidget.h"
#include "FlowCharView.h"
#include "NodeScene.h"
#include "FlowChartScene.h"

#include <QGraphicsRectItem>

MainWidget::MainWidget(QWidget *parent) : QMainWindow(parent){
    resize(1200,800);
    initUI();
    addDebugItems();
}

void MainWidget::initUI() {
    flowCharView_ = new FlowCharView();
    setCentralWidget(flowCharView_);

    scene_ = std::make_shared<NodeScene>(this);
    flowCharView_->setScene(  scene_->getFcScene());

}

void MainWidget::addDebugItems() {
    //添加一个矩形
    QBrush greenBrush = QBrush(Qt::green);
    QPen blackPen(Qt::black);
    blackPen.setWidth(2);
    auto rect = scene_->getFcScene()->addRect(0,0,100,100,blackPen,greenBrush);
    //设置矩形可移动
    rect->setFlags(rect->flags() | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable );
    rect->setPos(0,0);

    //添加一个item 到场景中
    QGraphicsRectItem *item = new QGraphicsRectItem(0,0,100,100);
    item->setPos(-100,-100);
    scene_->getFcScene()->addItem(item);
}
