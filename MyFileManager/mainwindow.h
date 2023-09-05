#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QRunnable>
#include <QStyledItemDelegate>
#include <QCache>
#include <QThreadPool>
#include <QMutex>
#include <QListView>
#include <QGestureEvent>

#include "iconview.h"
#include "myfilesystemmodel.h"
#include "customiconprovider.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MyItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit MyItemDelegate(QObject* parent = nullptr);

    void setFileSystemModel(QFileSystemModel* fileSystemModel){m_fileSystemModel = fileSystemModel;}

    void paint(QPainter* painter, const QStyleOptionViewItem& option,const QModelIndex& index) const override;

private:
    QFileSystemModel* m_fileSystemModel;
    mutable QThreadPool m_threadPool;
    mutable QCache<QString, QPixmap> m_cache;
    mutable QMutex m_mutex; //确保数据线程安全
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_listView_doubleClicked(const QModelIndex &index);

    void keyPressEvent(QKeyEvent *event) override;

    void on_comboBox_currentTextChanged(const QString &arg1);

    //返回上一级
    void backToPrevious();

    void getThumbnail(const QString &videoPath);

private:
    Ui::MainWindow *ui;

    IconViewDelegate *ivdelegate;

    MyFileSystemModel *fileModel;

    CustomIconProvider* iconProvider;

    QThreadPool m_threadPool;
};
#endif // MAINWINDOW_H
