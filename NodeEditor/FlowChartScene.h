//
// Created by happy on 2024/8/20.
//

#ifndef FLOWCHARTSCENE_H
#define FLOWCHARTSCENE_H
#include <qgraphicsscene.h>
#include <qobject.h>


class FlowChartScene : public QGraphicsScene{

public:
    FlowChartScene(QObject* parent = nullptr);
    ~FlowChartScene();

    void setSceneRange(qreal width, qreal height);

protected:
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;

private:
    qreal sceneWidth_ = 30000;
    qreal sceneHeight_ = 30000;
    QPen penLigth_;
    QPen penDark_;
    int gridSize_ = 40;
    int gridSquare_ = 5;

};



#endif //FLOWCHARTSCENE_H
