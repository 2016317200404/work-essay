#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    //获得CPU线程数
    void getNumberOfThreads();

    //获取CPU核数
    QString getNumberOfCores();

    //获取CPU型号
    QString getCPUModel();

    //获取一级数据缓存大小
    QString getL1DataCache();

    //获取一级指令缓存大小
    QString getL1InstructCache();

    //获取二级缓存大小
    QString getSecondLevelCache();

    //获取三级缓存大小
    QString getL3Cache();

    //获取内存条厂商
    QString getManufacturer();

    //获取内存条型号
    QString getMemoryModel();

    //获取内存条存取速度
    QString getMemorySpeed();

    //获取内存条类型
    QString getMemoryType();

    //获取总的内存条容量大小
    QString getMemorySize();

    //获取内存条的带宽
    QString getMemoryBandWidth();

    //获取内存条厂家+型号
    QString getMemoryModelDetail();

    //初始化
    void init();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
