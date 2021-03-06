#ifndef NETFRMMAIN_H
#define NETFRMMAIN_H

#include <QWidget>

namespace Ui {
    class frmMain;
}

class frmMain : public QWidget
{
    Q_OBJECT

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();

private:
    Ui::frmMain *ui;

private slots:
    void initForm();
    void initConfig();
    void saveConfig();
};

#endif // NETFRMMAIN_H
