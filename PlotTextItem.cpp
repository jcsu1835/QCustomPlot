#include "PlotTextItem.h"

PlotTextItem::PlotTextItem(QCustomPlot* parent)
    : QCPItemText(parent)
{
    setPadding(QMargins(5,5,5,5));  // 외곽선과 글자 간의 간격을 조정해준다.
    setPositionAlignment(Qt::AlignCenter);
    setPen(QPen(Qt::black));  // show black border around the text

    ArrowLine* arrow = new ArrowLine(parent);
    arrow->start->setParentAnchor(this->bottom);
    //arrow->end->setCoords();

    _arrow = arrow;
}

PlotTextItem::~PlotTextItem()
{
    parentPlot()->removeItem(_arrow);
    _arrow = nullptr;
}

void PlotTextItem::mouseDoubleClickEvent(QMouseEvent *event, const QVariant &details)
{
    qDebug() << "Text item double clicked !!!";

    _arrow->start->setParentAnchor(this->top);

    QCPItemText::mouseDoubleClickEvent(event, details);
}
