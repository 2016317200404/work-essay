#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "thumbnailtask.h"
#include "customiconprovider.h"
#include <QKeyEvent>
#include <qinputdialog.h>
#include <QMessageBox>
#include <QFile>
#include <QDebug>
#include <iostream>
#include <string>
#include <cstdlib>
#include <QPainter>
#include <QEvent>

// MainWindow constructor: sets up the user interface and initializes the file model
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_threadPool.setMaxThreadCount(3);

    // Set up the file model
    fileModel = new MyFileSystemModel(this);
    QString path = QDir::rootPath();
    ui->line_currentPath->setText(path);
    fileModel->setRootPath(path);
    iconProvider = new CustomIconProvider();
    fileModel->setIconProvider(iconProvider);
    ui->listView->setModel(fileModel);
    connect(iconProvider, &CustomIconProvider::thumbnailAvailable, this, [&](const QString &videoPath){
        QModelIndex index = fileModel->index(videoPath);
        if (index.isValid()) {
            QIcon icon = iconProvider->icon(QFileInfo(videoPath));
            fileModel->setData(index, icon, Qt::DecorationRole);
//            ui->listView->update();

//            emit fileModel->dataChanged(index, index);
//            ui->listView->setModel(nullptr);
//            ui->listView->setModel(fileModel);
        }
//        QString filePath = videoPath.mid(videoPath.lastIndexOf("/") + 1);
//        for (int row = 0; row < fileModel->rowCount(); ++row) {
//            for (int column = 0; column < fileModel->columnCount(); ++column) {
//                QModelIndex index = fileModel->index(row, column);
//                qDebug() <<index.data().toString() << row << column;
//                if (index.data().toString() == filePath) {
//                    // 触发数据改变信号，以便更新视图
//                    emit fileModel->dataChanged(index, index);
//                    return;
//                }
//            }
//        }
    });
    connect(iconProvider, &CustomIconProvider::requireThumbnail, this, &MainWindow::getThumbnail);

    // Set up the item delegate
    ivdelegate = new IconViewDelegate();
    ui->listView->setItemDelegate(ivdelegate);
    ui->listView->setRootIndex(fileModel->index(path));
    ivdelegate->setFilePath(path);

    ui->listView->setModelColumn(0);

    // Connect the double-click signal to the slot
    connect(ui->listView,&MyListView::itemDoubleClicked,this,&MainWindow::on_listView_doubleClicked);
}

MainWindow::~MainWindow()
{
    delete fileModel;
    delete ui;
}

//Slot for handling double-click events in the list view
void MainWindow::on_listView_doubleClicked(const QModelIndex &index)
{
    // If the item is a directory, navigate into it
    if (fileModel->isDir(index)) {
        ui->listView->setRootIndex(index);

        QString filePath = fileModel->filePath(index);
        ui->line_currentPath->setText(filePath);
        fileModel->setRootPath(filePath);
        ivdelegate->setFilePath(filePath);
    }
    // If the item is a file, show a dialog to rename it
    else{
        QModelIndexList selection = ui->listView->selectionModel()->selectedIndexes();
        if (!selection.isEmpty()) {
            QModelIndex index = selection.at(0);
            QString oldName = fileModel->fileName(index);
            QString newName = QInputDialog::getText(this, tr("Rename File"), tr("New name:"), QLineEdit::Normal, oldName);
            if (!newName.isEmpty() && newName != oldName) {
                QString oldNamePath = fileModel->filePath(index);
                QString newNamePath = oldNamePath;
                newNamePath.replace(oldName,newName);
                if (std::rename(oldNamePath.toStdString().c_str(),newNamePath.toStdString().c_str())) {
                    QMessageBox::warning(this, tr("Error"), tr("Failed to rename file."));
                }
            }
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Backspace:
        {
            backToPrevious();
        }
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
        {
            QModelIndexList selection = ui->listView->selectionModel()->selectedIndexes();
            if(!selection.isEmpty()) {
                QModelIndex index = selection.at(0);

                ui->listView->setRootIndex(index);

                QString filePath = fileModel->filePath(index);
                ui->line_currentPath->setText(filePath);
                fileModel->setRootPath(filePath);
            }
        }
            break;
        default:
            break;
    }
}

void MainWindow::on_comboBox_currentTextChanged(const QString &arg1)
{
    fileModel->setNameFilters(QStringList() << "*" + arg1);
    fileModel->setNameFilterDisables(false);
}

void MainWindow::backToPrevious()
{
    QString currentPath = fileModel->rootPath();

    QString parentPath = currentPath.left(currentPath.lastIndexOf("/") + 1);
    if(parentPath.isEmpty())
            return;
    ui->line_currentPath->setText(parentPath);
    fileModel->setRootPath(parentPath);
    ui->listView->setRootIndex(fileModel->index(parentPath));
    ivdelegate->setFilePath(currentPath);
}

void MainWindow::getThumbnail(const QString &videoPath)
{
    ThumbnailTask* task = new ThumbnailTask(videoPath);
    connect(task, &ThumbnailTask::thumbnailGenerated, iconProvider, &CustomIconProvider::onThumbnailGenerated);
    m_threadPool.start(dynamic_cast<QRunnable*>(task));
}

// Custom item delegate for painting items in the list view
void MyItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid())
        return;

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    QString fileName = opt.text;

    // 获取文件路径
    QString filePath = m_fileSystemModel->rootPath() + QDir::separator() + fileName;

    // 创建 QFileInfo 对象
    QFileInfo fileInfo(filePath);

    // If the item is an image or video file, show a thumbnail
    if (fileInfo.isFile() &&
        (fileInfo.suffix().toLower() == "jpg" || fileInfo.suffix().toLower() == "png" ||
        fileInfo.suffix().toLower() == "mp4" || fileInfo.suffix().toLower() == "avi")) {
        //调整重绘图像的大小
        QRect rect = opt.rect;
        rect.setWidth(128);
        rect.setHeight(128);
        QSize size = rect.size();

        QPixmap pixmap;
        m_mutex.lock();
        if (m_cache.contains(filePath)) {
            pixmap = *m_cache[filePath];
            m_mutex.unlock();
        } else {
            m_mutex.unlock();
            // 如果缓存中没有缩略图，提交加载缩略图的任务到线程池

            MyItemDelegate* ptr = const_cast<MyItemDelegate*>(this);
            ThumbnailTask* task = new ThumbnailTask(filePath, size, qobject_cast<QObject*>(ptr));

            connect(task, &ThumbnailTask::finished, [this,filePath,index](QPixmap pixmap) {
                QPixmap* temp = new QPixmap(pixmap);
                m_mutex.lock();
                m_cache.insert(filePath, temp);
                m_mutex.unlock();
                emit m_fileSystemModel->dataChanged(index, index);
                m_threadPool.releaseThread();
            });
            m_threadPool.start(dynamic_cast<QRunnable*>(task));
        }

        if (!pixmap.isNull()) {
            painter->drawPixmap(rect, pixmap);

            //调整文本框的Y的位置，且调整框的高度
            rect.setY(rect.y() + 128);
            rect.setHeight(52);
            QTextOption option;
            option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
            painter->drawText(rect, fileInfo.baseName() , option); // 在指定位置绘制修改后的文本
            return;
        }
    }

    // Otherwise, use the default painting
    QStyledItemDelegate::paint(painter, opt, index);
}
