//
// Created by happy on 2024/8/20.
//

#include "NodeScene.h"
#include "FlowChartScene.h"

NodeScene::NodeScene(QObject *parent) {
    flowChartScene_ = new FlowChartScene(parent);
    initUI();
}

NodeScene::~NodeScene() {
}

FlowChartScene* NodeScene::getFcScene() const {
    return flowChartScene_;
}

void NodeScene::initUI() {
    flowChartScene_->setSceneRange(sceneWidth_,sceneHeight_);
}
