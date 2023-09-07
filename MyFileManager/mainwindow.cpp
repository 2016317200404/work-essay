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

    m_threadPool.setMaxThreadCount(QThread::idealThreadCount() / 2 + 1);

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
//            ui->listView->doItemsLayout();
        }
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
    if (m_VecFilePath.contains(videoPath))
            return;
    ThumbnailTask* task = new ThumbnailTask(videoPath);
    connect(task, &ThumbnailTask::thumbnailGenerated, this, &MainWindow::onThumbnailGenerated);
    m_threadPool.start(dynamic_cast<QRunnable*>(task));
    m_VecFilePath.append(videoPath);
    qDebug() << m_threadPool.activeThreadCount() << "activeThreadCount";
}

void MainWindow::onThumbnailGenerated(const QString &videoPath, const QImage &thumbnail)
{
    iconProvider->onThumbnailGenerated(videoPath, thumbnail);
    m_VecFilePath.remove(m_VecFilePath.indexOf(videoPath));
}
