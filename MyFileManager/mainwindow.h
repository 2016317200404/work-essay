#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>

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

private:
    Ui::MainWindow *ui;

    QFileSystemModel *fileModel;
};
#endif // MAINWINDOW_H
