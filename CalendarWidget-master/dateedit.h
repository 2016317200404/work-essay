#ifndef DATEEDIT_H
#define DATEEDIT_H

#include <QWidget>

namespace Ui {
class DateEdit;
}

class CalendarWidget;

class DateEdit : public QWidget
{
    Q_OBJECT

public:
    explicit DateEdit(QWidget *parent = nullptr);
    ~DateEdit();

private slots:
    void on_pushButton_edit();

private:
    void init();

    void updateDate(QDate date);

private:
    Ui::DateEdit *ui;

    CalendarWidget *m_calendarWidget;
};

#endif // DATEEDIT_H
