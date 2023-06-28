#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QKeyEvent>
#include <qinputdialog.h>
#include <QProcess>
#include <QMessageBox>
#include <QFile>
#include <QDebug>
#include <iostream>
#include <string>
#include <cstdlib>
#include <QPainter>
#include <QMovie>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    fileModel = new QFileSystemModel(this);
    QString path = QDir::rootPath();
    ui->line_currentPath->setText(path);
    fileModel->setRootPath(path);
    ui->listView->setModel(fileModel);

    MyItemDelegate* delegate = new MyItemDelegate(ui->listView);
    delegate->setFileSystemModel(fileModel);
    ui->listView->setItemDelegate(delegate);
    ui->listView->setRootIndex(fileModel->index(path));

    ui->listView->setModelColumn(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_listView_doubleClicked(const QModelIndex &index)
{
    if (fileModel->isDir(index)) {
        ui->listView->setRootIndex(index);

        QString filePath = fileModel->filePath(index);
        ui->line_currentPath->setText(filePath);
        fileModel->setRootPath(filePath);
    }
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
}

MyItemDelegate::MyItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
    m_threadPool.setMaxThreadCount(8);
}

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

    QStyledItemDelegate::paint(painter, opt, index);
}

ThumbnailTask::ThumbnailTask(const QString &filePath, QSize size, QObject *parent)
    : QObject(parent)
    , m_filePath(filePath)
    , m_size(size)
{
}

void ThumbnailTask::run()
{
    QFileInfo fileInfo(m_filePath);
    if(!fileInfo.isFile())
        return;

    // 如果是图片文件，使用 QPixmap 加载缩略图
    if (fileInfo.suffix().toLower() == "jpg" || fileInfo.suffix().toLower() == "png") {
        QPixmap pixmap(m_filePath);
        if (!pixmap.isNull()) {
            m_pixmap = pixmap.scaled(m_size, Qt::KeepAspectRatio,Qt::SmoothTransformation);
        }
    }
    // 如果是动画文件，使用 QMovie 获取第一帧并生成缩略图
    else if (fileInfo.suffix().toLower() == "gif" || fileInfo.suffix().toLower() == "apng") {
        QMovie movie(m_filePath);
        movie.setCacheMode(QMovie::CacheNone);
        movie.jumpToFrame(0);
        QPixmap pixmap = movie.currentPixmap();
        if (!pixmap.isNull()) {
            m_pixmap = pixmap.scaled(m_size, Qt::KeepAspectRatio,Qt::SmoothTransformation);
        }
    }
    else if (fileInfo.suffix().toLower() == "mp4" || fileInfo.suffix().toLower() == "avi") {
        //确保缩略图的文件夹存在
        QString dirName = fileInfo.path() + "/thumb";
        qDebug() << fileInfo.path() << "fileInfo.path()";
        QDir dir(dirName);
        if(!dir.exists()){
            if(dir.mkpath(dirName)){
                qDebug() << "Folder created successfully.";
            }else{
                qDebug() << "Failed to create folder.";
            }
        }

        QString thumbnail = dirName + "/" + fileInfo.baseName() + ".jpg";

        if(!QFileInfo::exists(thumbnail)){
                QProcess process;
                process.setWorkingDirectory(fileInfo.path());
                QString command = QString("ffmpeg -i \"%1.mp4\" -ss 00:00:00 -vframes 1 -s 640x360 \"thumb/%1.jpg\"").arg(fileInfo.baseName());
                process.start(command);
                if(!process.waitForFinished(-1)){
                    qWarning() << "Failed to generate thumb image with ffmpeg";
                    return;
                }
        }
        m_pixmap = QPixmap(thumbnail);
    }

    emit finished(m_pixmap);
}
