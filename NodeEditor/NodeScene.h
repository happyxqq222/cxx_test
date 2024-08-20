//
// Created by happy on 2024/8/20.
//

#ifndef NODESCENE_H
#define NODESCENE_H
#include <qglobal.h>
#include <qobject.h>

QT_BEGIN_NAMESPACE
class FlowChartScene;
QT_END_NAMESPACE

class NodeScene {

public:
    NodeScene(QObject *parent = nullptr);
    ~NodeScene();
    FlowChartScene* getFcScene() const;

protected:
    void initUI();
protected:
    qreal sceneWidth_ = 30000;
    qreal sceneHeight_ = 30000;
    FlowChartScene *flowChartScene_;
};



#endif //NODESCENE_H
