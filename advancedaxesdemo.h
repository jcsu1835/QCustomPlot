#ifndef ADVANCEDAXESDEMO_H
#define ADVANCEDAXESDEMO_H

#include <QMainWindow>
#include "qcustomplot.h"

namespace Ui {
class AdvancedAxesDemo;
}

class AdvancedAxesDemo : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdvancedAxesDemo(QWidget *parent = nullptr);
    ~AdvancedAxesDemo();

private:
    Ui::AdvancedAxesDemo *ui;

    QCustomPlot* customPlot = nullptr;
};

#endif // ADVANCEDAXESDEMO_H
