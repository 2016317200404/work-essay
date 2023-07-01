#include "dateedit.h"
#include "ui_dateedit.h"
#include "calendarwidget.h"
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>

const QString dateString = "%1/%2/%3";

DateEdit::DateEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DateEdit)
{
    ui->setupUi(this);
    init();
}

DateEdit::~DateEdit()
{
    delete ui;
}

void DateEdit::on_pushButton_edit()
{
    m_calendarWidget->show();
}

void DateEdit::init()
{
    m_calendarWidget = new CalendarWidget(this);
    m_calendarWidget->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    m_calendarWidget->hide();

    QPoint bottomLeft = this->mapToGlobal(QPoint(0, this->height()));
    m_calendarWidget->move(bottomLeft);
    m_calendarWidget->installEventFilter(this);

    QDate currenDate = QDate::currentDate();
    updateDate(currenDate);

    connect(m_calendarWidget, &CalendarWidget::dateSelected, [=](const QDate &date) {
        updateDate(date);
        m_calendarWidget->hide();
    });
}

void DateEdit::updateDate(QDate date)
{
    QString strCurDate = dateString.arg(date.year()).arg(date.month()).arg(date.day());
    ui->label->setText(strCurDate);
}
