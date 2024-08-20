//
// Created by happy on 2024/8/20.
//

#ifndef FLOWCHARVIEW_H
#define FLOWCHARVIEW_H


#include <QGraphicsView>

class FlowCharView : public QGraphicsView {
    Q_OBJECT

public:
    FlowCharView(QWidget * parent = nullptr);
    ~FlowCharView();

protected:
    void initUI();


};



#endif //FLOWCHARVIEW_H
