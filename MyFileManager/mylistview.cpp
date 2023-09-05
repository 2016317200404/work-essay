#include "mylistview.h"
#include "qdebug.h"

#include <QScroller>

MyListView::MyListView(QWidget *parent)
    : QListView(parent)
{
    //接受点击手势
    setAttribute(Qt::WA_AcceptTouchEvents);
    grabGesture(Qt::TapGesture);

    //设置滑动条，滑动条接受滑动手势
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QScroller::grabGesture(this,QScroller::TouchGesture);
    QScrollerProperties properties = QScroller::scroller(this)->scrollerProperties();

    properties.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy,QScrollerProperties::OvershootAlwaysOff);
    properties.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy,QScrollerProperties::OvershootAlwaysOff);
    QScroller::scroller(this)->setScrollerProperties(properties);
}

void MyListView::onThumbnailAvailable(const QString &videoPath)
{
    QAbstractItemModel *model = this->model();
    QString filePath = videoPath.mid(videoPath.lastIndexOf("/") + 1);
    for (int row = 0; row < model->rowCount(); ++row) {
        for (int column = 0; column < model->columnCount(); ++column) {
            QModelIndex index = model->index(row, column);
            qDebug() <<index.data().toString();
            if (index.data().toString() == filePath) {
                // 触发数据改变信号，以便更新视图
                emit model->dataChanged(index, index);
                return;
            }
        }
    }
}

bool MyListView::event(QEvent *event)
{
    if (event->type() == QEvent::Gesture){
        return gestureEvent(static_cast<QGestureEvent *>(event));
    }
    return QListView::event(event);
}

bool MyListView::gestureEvent(QGestureEvent *event)
{
    // If the gesture is a double-tap, emit the itemDoubleClicked signal
    if(QGesture *gesture = event->gesture(Qt::TapGesture)){
        QTapGesture *tapsture = static_cast<QTapGesture *>(gesture);
        QModelIndex index = indexAt(tapsture->position().toPoint());
        if(index.isValid()){
            emit itemDoubleClicked(index);
            event->accept();
            return true;
        }
    }
    return false;
}
