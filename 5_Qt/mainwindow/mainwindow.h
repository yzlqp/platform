#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QTabWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>

namespace Ui {
    class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initPlatform();

private:
    Ui::MainWindow  *ui;

};


#endif // MAINWINDOW_H
