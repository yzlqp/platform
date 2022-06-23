#include "errorratepaint.h"
#include "ui_errorratepaint.h"

ErrorRatePaint::ErrorRatePaint(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ErrorRatePaint)
{
    ui->setupUi(this);
}

ErrorRatePaint::~ErrorRatePaint()
{
    delete ui;
}
void ErrorRatePaint::initmychart()
{
    maxSize = 51;
    maxX = 5000;
    maxY = 1000;

    splineSeries = new QSplineSeries();
    chart = new QChart();
    chartView = new QChartView();

    /* 坐标轴 */
     axisY = new QValueAxis();
     axisX = new QValueAxis();
     /* 定时器 */
     timer = new QTimer(this);
     chart->legend()->hide();
      /* chart 设置标题 */
      chart->setTitle("误码个数曲线");
      /* 添加一条曲线 splineSeries */
      chart->addSeries(splineSeries);

      //axisY->setBase(10);
      /* 设置显示格式 */
      axisY->setLabelFormat("%i");
      axisY->setTickCount(11);
      /* y 轴标题 */
      axisY->setTitleText("N");
      /* y 轴标题位置（设置坐标轴的方向） */
      chart->addAxis(axisY, Qt::AlignLeft);
      /* 设置 y 轴范围 */
      axisY->setRange(0, maxY);
      axisY->setMinorTickCount(4);
      /* 将 splineSeries 附加于 y 轴上 */
      splineSeries->attachAxis(axisY);

      /* 设置显示格式 */
      axisX->setLabelFormat("%i");
      /* x 轴标题 */
      axisX->setTitleText("t/ms");
      /* x 轴标题位置（设置坐标轴的方向） */
      chart->addAxis(axisX, Qt::AlignBottom);
      /* 设置 x 轴范围 */
      axisX->setRange(0, maxX);
      /* 将 splineSeries 附加于 x 轴上 */

      axisX->setMinorTickCount(4); //设置小刻度线的数目
      axisX->setLabelsVisible(true);
      axisX->setTickCount(6);
      axisX->setGridLineVisible(true); //设置是否显示网格线
      splineSeries->attachAxis(axisX);

//      /* 将图表的内容设置在图表视图上 */
      chartView->setChart(chart);
//      /* 设置抗锯齿 */
      chartView->setRenderHint(QPainter::Antialiasing);


      timer->start(1000 );
      /* 信号槽连接 */
      connect(timer, SIGNAL(timeout()), this, SLOT(timerTimeOut()));

      qsrand(time(NULL));
      ui->gridLayout_2->addWidget(chartView);



}
void ErrorRatePaint::timerTimeOut()
 {
    /* 产生随机 0~maxY 之间的数据 */
    receivedData(qrand()%maxY );

 }
void ErrorRatePaint::receivedData(int value)
{
   /* 将数据添加到 data 中 */
   data.append(value);

   /* 当储存数据的个数大于最大值时，把第一个数据删除 */
    while (data.size() > maxSize) {
    /* 移除 data 中第一个数据 */
       data.removeFirst();
   }

    /* 先清空 */
   splineSeries->clear();

   /* 计算 x 轴上的点与点之间显示的间距 */
   int xSpace = maxX / (maxSize - 1);

   /* 添加点， xSpace * i 表示第 i 个点的 x 轴的位置 */
   for (int i = 0; i < data.size(); ++i) {
       splineSeries->append(xSpace * i, data.at(i));
   }
}
