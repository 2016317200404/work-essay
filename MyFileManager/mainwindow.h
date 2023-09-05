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

    void onThumbnailGenerated(const QString &videoPath, const QImage &thumbnail);

private:
    Ui::MainWindow *ui;

    IconViewDelegate *ivdelegate;

    MyFileSystemModel *fileModel;

    CustomIconProvider* iconProvider;

    QThreadPool m_threadPool;

    QVector<QString> m_VecFilePath;
};
#endif // MAINWINDOW_H
