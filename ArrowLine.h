#ifndef ARROWLINE_H
#define ARROWLINE_H

#include <QObject>
#include "qcustomplot.h"

class ArrowLine : public QCPItemLine
{
    Q_OBJECT

public:
    ArrowLine(QCustomPlot* parent=nullptr);

protected:
    void selectEvent(QMouseEvent *event, bool additive, const QVariant &details, bool *selectionStateChanged);
    void deselectEvent (bool *selectionStateChanged);
    virtual void mouseDoubleClickEvent(QMouseEvent *event, const QVariant &details);


private:
    QCPItemText* _descripText = nullptr;



};

#endif // ARROWLINE_H
