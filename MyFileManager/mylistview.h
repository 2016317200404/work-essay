#ifndef MYLISTVIEW_H
#define MYLISTVIEW_H

#include <QListView>
#include <QGestureEvent>

class MyListView : public QListView
{
    Q_OBJECT
public:
    explicit MyListView(QWidget *parent = nullptr);

public slots:
    void onThumbnailAvailable(const QString &videoPath);

protected:
    bool event(QEvent *event) override;

    bool gestureEvent(QGestureEvent *event);

signals:
    void itemDoubleClicked(const QModelIndex &index);
};

#endif // MYLISTVIEW_H
