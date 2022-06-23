#ifndef AD9361MAIN_H
#define AD9361MAIN_H

#include <QWidget>
#include <QMessageBox>

namespace Ui {
    class ad9361main;
}

class ad9361main : public QWidget
{
    Q_OBJECT

public:
    explicit ad9361main(QWidget *parent = nullptr);
    ~ad9361main();

private slots:
    void on_pushButton_CONNECT_clicked();

    void on_lineEdit_TXBW_editingFinished();

    void on_lineEdit_RXBW_editingFinished();

    void on_lineEdit_TXLO_editingFinished();

    void on_lineEdit_RXLO_editingFinished();

private:
    Ui::ad9361main *ui;
    QMessageBox::StandardButton result;

};

#endif // AD9361MAIN_H
