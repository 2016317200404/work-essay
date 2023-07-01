#ifndef CALENDARWIDGET_H
#define CALENDARWIDGET_H

#include <QDate>
#include <QWidget>
#include <QDateEdit>
#include <QAbstractButton>

class QLabel;
class QStackedWidget;

class DateIconButton : public QAbstractButton
{
    Q_OBJECT
public:
    enum DayType{
        normal,
        weekend,
        notworking,
        notenable,
        selected,
        hover,
    };

    enum BtnType{
        DayBtn,
        MonthBtn,
        YearBtn
    };
    explicit DateIconButton(BtnType btnTy,QWidget *parent = nullptr);
    void setEnabled(bool value);
    void setType(DayType t,bool enable = true);

protected:
    void paintEvent(QPaintEvent *e);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void drawText(QPainter *painter,QColor color);

private:
    DayType type;
    DayType prevType;
    BtnType currentBtn;
    QSize selectedSize;
};

class TitleButton : public QAbstractButton
{
    Q_OBJECT
public:
    enum DrawType{
        DrawText,
        DrawRightIcon,
        DrawLeftIcon,
    };
    explicit TitleButton(QWidget *parent = nullptr);
    void mouseMoveEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);
    void setType(DrawType ty);

private:
    DrawType type;
};

class CalendarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CalendarWidget(QWidget *parent = nullptr);
    ~CalendarWidget();

    //获取当前日期
    QDate getDate(){return date;}

    //设置最小日期
    void setMinimumDate(QDate date){minDate = date; }

    //设置最大日期
    void setMaximumDate(QDate date){maxDate = date; }

    //设置当前日期
    void setSelectedDate(QDate tDate){date = tDate;}

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

    //滚轮实现翻页
    void wheelEvent(QWheelEvent * event) override;
    void showEvent(QShowEvent *event) override;

    //窗口失去焦点时，自动隐藏
    bool eventFilter(QObject *obj, QEvent *event) override;

signals:
    //选择具体日，发送信号
    void dateSelected(QDate currentDate);

private slots:
    void titleClick();
    void prevClick();
    void nextClick();
    void monthClick();
    void yearClick();
    void daysClick();

private:
    QWidget *_getYearWidet();
    QWidget *_getMonthWidget();
    QWidget *_getDayWidget();

    void initTitle();
    void initWeek();
    void initStackedWidget();
    void resizeTitle();
    void resizeWeek();
    void resizeStackedWidget();
    void resizeDays();
    void resizeMonth();
    void resizeYear();
    int addYear(int value);
    int addMonth(int value);
    void setTitleBtnText();

private:
    enum TitleType{
        DayType,
        MonthType,
        YearType
    };

    const int WeekCount = 7;
    const int RowCount = 6;
    const int minWidth = 260;
    const int minHeight = 260;
    const int margin = 0;
    const int titleSpace = 2;

    QDate date;
    QDate maxDate;
    QDate minDate;
    QList <DateIconButton *> calendarList;
    QList <DateIconButton *> monthList;
    QList <DateIconButton *> yearList;
    QList <QLabel *> weekList;
    TitleType type;
    TitleButton *titleBtn;
    TitleButton *nextBtn;
    TitleButton *prevBtn;
    int titleHeight;
    int weekHeight;
    int year;
    int month;
    int day;
    bool openDays;
    const int YearBeginCount = 5;

    QStackedWidget *stackedWidget;
    QWidget *dayWideget;
    QWidget *monthWidget;
    QWidget *yearWidget;
};

#endif // CALENDARWIDGET_H
