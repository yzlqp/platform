#ifndef QMYLABEL_H
#define QMYLABEL_H

#include <QLabel>
#include <QEvent>
#include <QMouseEvent>

class QMyLabel : public QLabel
{
    Q_OBJECT
public:
    explicit QMyLabel(QWidget *parent = nullptr);

signals:
    void sg_clicked();
public:
    void enterEvent(QEvent *);
    void mousePressEvent(QMouseEvent *);

};

#endif // QMYLABEL_H
