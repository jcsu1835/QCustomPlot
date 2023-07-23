#ifndef PLOTTEXTITEM_H
#define PLOTTEXTITEM_H

#include <QObject>

#include "qcustomplot.h"
#include "ArrowLine.h"


class PlotTextItem : public QCPItemText
{
    Q_OBJECT

public:
    PlotTextItem(QCustomPlot* parent=nullptr);
    ~PlotTextItem();

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event, const QVariant &details);

private:
    ArrowLine* _arrow = nullptr;
};

#endif // PLOTTEXTITEM_H
