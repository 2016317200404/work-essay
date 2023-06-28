#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QRunnable>
#include <QStyledItemDelegate>
#include <QCache>
#include <QThreadPool>
#include <QMutex>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class ThumbnailTask : public QObject, public QRunnable
{
    Q_OBJECT

public:
    explicit ThumbnailTask(const QString& filePath, QSize size, QObject* parent = nullptr);

    void run() override;

signals:
    void finished(QPixmap);

public:
    QString m_filePath;
    QSize m_size;
    QPixmap m_pixmap;
};

class MyItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    MyItemDelegate(QObject* parent = nullptr);

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

private:
    Ui::MainWindow *ui;

    QFileSystemModel *fileModel;
};
#endif // MAINWINDOW_H
