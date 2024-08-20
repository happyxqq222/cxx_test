//
// Created by happy on 2024/8/20.
//

#include "FlowChartScene.h"

#include <cmath>
#include <QDebug>
#include <QPainter>

FlowChartScene::FlowChartScene(QObject *parent) : QGraphicsScene(parent){
    setSceneRange(30000,30000);
    QColor backgroundColor = QColor("#393939");
    setBackgroundBrush(backgroundColor);
    QColor colorLight = QColor("#2f2f2f");
    penLigth_.setColor(colorLight);
    penLigth_.setWidth(1);

    QColor colorDark = QColor("#292929");
    penDark_.setColor(colorDark);
    penDark_.setWidth(2);
}

FlowChartScene::~FlowChartScene() {
}

void FlowChartScene::setSceneRange(qreal width, qreal height) {
    setSceneRect(-width/2, -height /2 , width, height);
}

void FlowChartScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event) {
    QGraphicsScene::dragEnterEvent(event);
}

void FlowChartScene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event) {
    QGraphicsScene::dragLeaveEvent(event);
}

void FlowChartScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsScene::mousePressEvent(event);
}

void FlowChartScene::drawBackground(QPainter *painter, const QRectF &rect) {
    QGraphicsScene::drawBackground(painter,rect);

    //返回不大于浮点数的最大整数值
    int left = std::floor(rect.left());
    //返回不小于指定浮点数最小整数值
    int right = std::ceil(rect.right());
    int top = std::floor(rect.top());
    int bottom = std::ceil(rect.bottom());
    qDebug() << rect;
    qDebug() << left << "," << top << "," << right << "," << bottom;

    int firstLeft = left - abs(left % gridSize_) ;
    int firstTop = top - (top % gridSize_);

    //首先创建一个QList<QLine>类型的linesLight列表，然后向其中添加QLine对象，
    //最后 使用painter->drawLines(linesHeight) 函数将这些直线绘制到画布上
    QList<QLine>  linesLight;
    QList<QLine>  linesDark;

    for(int x = firstLeft; x < right; x = x + gridSize_) {
        if( x % (gridSize_ * gridSquare_) != 0) {
            linesLight.append(QLine(x,top,x,bottom));
        } else {
            linesDark.append(QLine(x,top,x,bottom));
        }
    }

    for(int y = firstTop; y < bottom; y = y + gridSize_) {
        if( y %(gridSize_ * gridSquare_) != 0) {
            linesLight.append(QLine(left,y,right,y));
        } else {
            linesDark.append(QLine(left,y,right,y));
        }
    }

    painter->setPen(penLigth_);
    painter->drawLines(linesLight.toVector());
    painter->setPen(penDark_);
    painter->drawLines(linesDark.toVector());
}
