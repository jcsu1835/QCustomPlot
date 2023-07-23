#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include "qcustomplot.h"
#include "advancedaxesdemo.h"
#include "WaveBracket.h"
#include "ArrowLine.h"
#include "PlotTextItem.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event);

private Q_SLOTS:
    void bracketDataSlot();
    void on_pushButton_clicked();
    void customPlotMousePress(QMouseEvent *event);
    void customPlotMouseMove(QMouseEvent *event);
    void customPlotMouseRelease(QMouseEvent *event);
    void on_addBracketRadioButton_toggled(bool checked);
    void on_addArrowRadioButton_toggled(bool checked);
    void on_addTextRadioButton_toggled(bool checked);

private:
    void drawItemBracket();
    void drawItemArrow();
    void drawItemText();
    bool rectContains(QPointF p, QRectF r);

    Ui::MainWindow *ui;
    QCustomPlot *customPlot;
    QTimer dataTimer;
    QCPItemTracer *itemDemoPhaseTracer = nullptr;
    AdvancedAxesDemo* _advancedAxesDemo = nullptr;

    QList<WaveBracket*> _brackets;
    WaveBracket* _selectedBracket = nullptr;

    QList<ArrowLine*> _arrows;
    ArrowLine* _selectedArrow = nullptr;

    QList<PlotTextItem*> _textItems;
    PlotTextItem* _selectedText = nullptr;

    QCPItemRect* _leftRect = nullptr;
    QCPItemRect* _rightRect = nullptr;
    //
    QPointF _leftPos;
    QPointF _rightPos;
    QPointF _mousePressPos;
    QPointF _mouseMovePos;
    QPointF _deltaPos;
    QPointF _leftPos0;
    QPointF _rightPos0;

    bool _mousePressed = false;

    bool _addBracket = false;
    bool _bracketPosCompleted = false;
    bool _bracketItemSelected = false;

    bool _addArrow = false;
    bool _arrowPosCompleted = false;
    bool _arrowItemSelected = false;

    bool _addText = false;
    bool _textPosCompleted = false;
    bool _textItemSelected = false;

    bool _leftRectClicked = false;
    bool _rightRectClicked = false;

    double _xStart;
    double _yStart;
    double _xEnd;
    double _yEnd;
    double _xEndOld;
    double _yEndOld;
};
#endif // MAINWINDOW_H
