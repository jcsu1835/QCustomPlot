#include "WaveBracket.h"

WaveBracket::WaveBracket(QCustomPlot* parent)
    : QCPItemBracket(parent)
{
    // 선택 될 경우의 색상 지정
    setSelectedPen(QColor(Qt::gray));

    // add the text label at the top:
    _wavePacketText = new PlotTextItem(parentPlot());
    _wavePacketText->position->setParentAnchor(center);

    if(left->coords().x() < right->coords().x())
    {
        _wavePacketText->position->setCoords(0, -10); // move 10 pixels to the top from bracket center anchor
    }
    else
    {
        _wavePacketText->position->setCoords(0, +15); // move 15 pixels to the bottom from bracket center anchor

    }
    _wavePacketText->setPositionAlignment(Qt::AlignBottom|Qt::AlignHCenter);
    _wavePacketText->setText("Wavepacket");

}

void WaveBracket::selectEvent(QMouseEvent *event, bool additive, const QVariant &details, bool *selectionStateChanged)
{
    QCPItemBracket::selectEvent(event, additive, details, selectionStateChanged);
}

void WaveBracket::deselectEvent(bool *selectionStateChanged)
{
    QCPItemBracket::deselectEvent(selectionStateChanged);
}

void WaveBracket::mouseDoubleClickEvent(QMouseEvent *event, const QVariant &details)
{

    QCPItemBracket::mouseDoubleClickEvent(event, details);
}
