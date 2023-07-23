#include "ArrowLine.h"

ArrowLine::ArrowLine(QCustomPlot* parent)
    : QCPItemLine(parent)
{
    // 선택 될 경우의 색상 지정
    setSelectedPen(QColor(Qt::gray));
    setHead(QCPLineEnding::esSpikeArrow);
}

void ArrowLine::selectEvent(QMouseEvent *event, bool additive, const QVariant &details, bool *selectionStateChanged)
{
    QCPItemLine::selectEvent(event, additive, details, selectionStateChanged);
}

void ArrowLine::deselectEvent(bool *selectionStateChanged)
{
    QCPItemLine::deselectEvent(selectionStateChanged);
}

void ArrowLine::mouseDoubleClickEvent(QMouseEvent *event, const QVariant &details)
{

    QCPItemLine::mouseDoubleClickEvent(event, details);
}
