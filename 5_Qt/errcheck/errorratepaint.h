#ifndef ERRORRATEPAINT_H
#define ERRORRATEPAINT_H

#include <QWidget>
#include <QChartView>
#include <QSplineSeries>
#include <QScatterSeries>
#include <QDebug>
#include <QValueAxis>
#include <QTimer>
#include <QLogValueAxis>
#include <QChart>
#include <QVector>

QT_CHARTS_BEGIN_NAMESPACE
class QChartView;
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

namespace Ui {
    class ErrorRatePaint;
}

class ErrorRatePaint : public QWidget
{
    Q_OBJECT

public:
    explicit ErrorRatePaint(QWidget *parent = nullptr);
    ~ErrorRatePaint();

    void initmychart();
    void receivedData(int);
private slots:
    void timerTimeOut();
public:
    Ui::ErrorRatePaint *ui;
    int maxSize;
    int maxX;
    int maxY;
    QValueAxis *axisY;
    QValueAxis *axisX;
    QList<int> data;
    QSplineSeries *splineSeries;
    QChart *chart;
    QChartView *chartView;
    QTimer *timer;
    QVector<char> testdata;

};

#endif // ERRORRATEPAINT_H
