#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QProcess>
#include <QDebug>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CACHE_SIZE_LINE "cache size"
#define LEVEL_LINE "cache level"
#define WAYS_OF_ASSOCIATIVITY_LINE "ways of associativity"
#define NUMBER_OF_SETS_LINE "number of sets"
#define CACHE_TYPE_LINE "cache type"

#define KB (1024)
#define MB (1024 * 1024)

QString runLinuxCommand(QStringList &arguments){
    qDebug() << arguments << "arguments";
    QProcess process;
    process.setProgram("/bin/bash");

    process.setArguments(arguments);

    process.start();
    if (process.waitForFinished(-1)) {
        QByteArray output = process.readAllStandardOutput();
//        QByteArray error = process.readAllStandardError();
        qDebug() << output << "output";
        return output.simplified();
    } else {
        return "Command execution failed.";
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::getNumberOfThreads()
{
    QString result = runLinuxCommand(QStringList() << "-c" << "cat /proc/cpuinfo | grep 'processor' | wc -l");
}

QString MainWindow::getNumberOfCores()
{
    return runLinuxCommand(QStringList() << "-c" << "cat /proc/cpuinfo | grep 'cpu cores' | cut -f2 -d: | uniq");
}

QString MainWindow::getCPUModel()
{
    return runLinuxCommand(QStringList() << "-c" << "cat /proc/cpuinfo | grep name | cut -f2 -d: | uniq");
}

QString MainWindow::getL1DataCache()
{
    return runLinuxCommand(QStringList() << "-c" << "lscpu | awk '/L1d/ {print $3$4}'");
}

QString MainWindow::getL1InstructCache()
{
    return runLinuxCommand(QStringList() << "-c" << "lscpu | awk '/L1i/ {print $3$4}'");
}

QString MainWindow::getSecondLevelCache()
{
    return runLinuxCommand(QStringList() << "-c" << "lscpu | awk '/L2/ {print $3$4}'");
}

QString MainWindow::getL3Cache()
{
    return runLinuxCommand(QStringList() << "-c" << "lscpu | awk '/L3/ {print $3$4}'");
}

QString MainWindow::getManufacturer()
{
    return runLinuxCommand(QStringList() << "-c" << "echo hh123 | sudo -S dmidecode --type 17 | grep Manufacturer: | cut -f2 -d:");
}

QString MainWindow::getMemoryModel()
{
    return runLinuxCommand(QStringList() << "-c" << "echo hh123 | sudo -S dmidecode --type 17 | grep Part | cut -f2 -d:");
}

QString MainWindow::getMemorySpeed()
{
    return runLinuxCommand(QStringList() << "-c" << "echo hh123 | sudo -S dmidecode --type 17 | grep 'Configured Memory Speed:' | cut -f2 -d: | uniq");
}

QString MainWindow::getMemoryType()
{
    QString type = runLinuxCommand(QStringList() << "-c" << "echo hh123 | sudo -S dmidecode --type 17 | grep Type: | cut -f2 -d: | uniq");
    QString technology = runLinuxCommand(QStringList() << "-c" << "echo hh123 | sudo -S dmidecode --type 17 | grep 'Memory Technology:' | cut -f2 -d: | uniq");
    return type + "-" + technology;
}

QString MainWindow::getMemorySize()
{
    //这个数据不准确，这是总可用的内存，即物理内存减去一些保留位和内核二进制代码
    //return runLinuxCommand(QStringList() << "-c" << "cat /proc/meminfo | grep MemTotal: | cut -f2 -d:");
    QString text = runLinuxCommand(QStringList() << "-c" << "echo hh123 | sudo -S lshw -class memory");
    int index = text.indexOf("-memory");
    int pos = text.indexOf("size: ",index) + 6;  //跳过这size: 的位置，免得下次寻找，找到size：的空格位置
    int linebreak = text.indexOf(" ",pos);
    return text.mid(pos,linebreak - pos);
}

QString MainWindow::getMemoryBandWidth()
{
    //带宽 = 内存模块的读取速度 x 内存模块的数据总线的宽度 / 8
    QString speed = runLinuxCommand(QStringList() << "-c" << "echo hh123 | sudo -S dmidecode --type 17 | grep 'Configured Memory Speed:' | awk '{print $4}' | uniq");
    QString width = runLinuxCommand(QStringList() << "-c" << "echo hh123 | sudo -S dmidecode --type memory | grep 'Data Width:' | uniq | awk '{print $3}'");
    int nWidth = speed.toInt() * width.toInt() / 8;
    return QString::number(nWidth) + "MB/s";
}

QString MainWindow::getMemoryModelDetail()
{
    QString manufacturer = getManufacturer();
    QString model = getMemoryModel();
    QStringList list1 = manufacturer.split(" ");
    QStringList list2 = model.split(" ");
    QString result = "";
    for(int i = 0; i < list1.size(); i++){
        result += list1.at(i) + " " + list2.at(i);
        if(i != list1.size() -1)
            result += "\n";
    }
    return result;
}

void MainWindow::init()
{
    //CPU
    ui->label_CPUModel->setText(getCPUModel());
    ui->label_CPUCoreNum->setText(getNumberOfCores());
    ui->label_L1d->setText(getL1DataCache());
    ui->label_L1i->setText(getL1InstructCache());
    ui->label_L2->setText(getSecondLevelCache());
    ui->label_L3->setText(getL3Cache());

    //内存
    ui->label_memoryModel->setText(getMemoryModelDetail());
    ui->label_memorySize->setText(getMemorySize());
    ui->label_memoryType->setText(getMemoryType());
    ui->label_memorySpeed->setText(getMemorySpeed());
    ui->label_memoryWidth->setText(getMemoryBandWidth());
}

