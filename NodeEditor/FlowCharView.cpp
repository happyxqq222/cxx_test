//
// Created by happy on 2024/8/20.
//

#include "FlowCharView.h"

FlowCharView::FlowCharView(QWidget *parent) : QGraphicsView(parent) {
    initUI();
}

FlowCharView::~FlowCharView() {
}

/**
 * 视窗的部分设置
 */
void FlowCharView::initUI() {
    //设置渲染选项
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setDragMode(QGraphicsView::RubberBandDrag);

    //进行缩放或者平移操作时，变换中心点将会是鼠标指针所在的位置，而不是视图的中心
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setAcceptDrops(true);
}
