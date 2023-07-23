#ifndef WAVEBRACKET_H
#define WAVEBRACKET_H

#include <QObject>
#include "qcustomplot.h"

#include "PlotTextItem.h"

class WaveBracket : public QCPItemBracket
{
    Q_OBJECT

public:
    WaveBracket(QCustomPlot* parent=nullptr);

protected:
    void selectEvent(QMouseEvent *event, bool additive, const QVariant &details, bool *selectionStateChanged);
    void deselectEvent (bool *selectionStateChanged);
    virtual void mouseDoubleClickEvent(QMouseEvent *event, const QVariant &details);


private:
    PlotTextItem* _wavePacketText = nullptr;



};

#endif // WAVEBRACKET_H
