//
// Created by happy on 2024/8/20.
//

#ifndef MAINWIDGET_H
#define MAINWIDGET_H


#include <QWidget>
#include <QMainWindow>


QT_BEGIN_NAMESPACE
class FlowCharView;
class NodeScene;
class FlowChartScene;
QT_END_NAMESPACE


class MainWidget  : public QMainWindow{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);

protected:
    void initUI();
    void addDebugItems();

private:
    FlowCharView * flowCharView_;
    std::shared_ptr<NodeScene>  scene_;
};



#endif //MAINWIDGET_H
