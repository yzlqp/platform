#include "qmylabel.h"

QMyLabel::QMyLabel(QWidget *parent) : QLabel(parent) { }


void QMyLabel::enterEvent(QEvent * )
{
    this->setCursor(QCursor(Qt::PointingHandCursor));
}

void QMyLabel::mousePressEvent(QMouseEvent * )
{
    /*if (this->objectName() == "label_serialClearShow")
    {*/
        emit sg_clicked();

    /*}
    if (this->objectName() == "label_serialSaveData")
    {
        emit sg_saveSerialData();

    }*/
}
