#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QKeyEvent>
#include <qinputdialog.h>
#include <QMessageBox>
#include <QFile>
#include <QDebug>
#include <iostream>
#include <string>
#include <cstdlib>

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
    ui->listView->setRootIndex(fileModel->index(QDir::homePath()));
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
        qDebug() << index << "index";
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
    qDebug() << currentPath << "currentpath";

    QString parentPath = currentPath.left(currentPath.lastIndexOf("/") + 1);
    qDebug() << parentPath << "parentPath";
    if(parentPath.isEmpty())
            return;
    ui->line_currentPath->setText(parentPath);
    fileModel->setRootPath(parentPath);
    ui->listView->setRootIndex(fileModel->index(parentPath));
    //qDebug() << parentPath; // 输出上一级目录路径
}
