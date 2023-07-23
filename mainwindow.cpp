#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //
    customPlot = new QCustomPlot(this);
    ui->verticalLayout_2->addWidget(customPlot);

    customPlot->setFocusPolicy(Qt::StrongFocus);
    //
    //customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    QCPGraph *graph = customPlot->addGraph();
    int n = 500;
    double phase = 0;
    double k = 3;
    QVector<double> x(n), y(n);
    for (int i=0; i<n; ++i)
    {
      x[i] = i/(double)(n-1)*34 - 17;
      y[i] = qExp(-x[i]*x[i]/20.0)*qSin(k*x[i]+phase);
    }
    graph->setData(x, y);
    graph->setPen(QPen(Qt::blue));
    graph->rescaleKeyAxis();
    customPlot->yAxis->setRange(-1.45, 1.65);
    customPlot->xAxis->grid()->setZeroLinePen(Qt::NoPen);


    // add the phase tracer (red circle) which sticks to the graph data (and gets updated in bracketDataSlot by timer event):
    QCPItemTracer *phaseTracer = new QCPItemTracer(customPlot);
    itemDemoPhaseTracer = phaseTracer; // so we can access it later in the bracketDataSlot for animation
    phaseTracer->setGraph(graph);
    phaseTracer->setGraphKey((M_PI*1.5-phase)/k);
    phaseTracer->setInterpolating(true);
    phaseTracer->setStyle(QCPItemTracer::tsCircle);
    phaseTracer->setPen(QPen(Qt::red));
    phaseTracer->setBrush(Qt::red);
    phaseTracer->setSize(7);

    // add label for phase tracer:
    QCPItemText *phaseTracerText = new QCPItemText(customPlot);
    phaseTracerText->position->setType(QCPItemPosition::ptAxisRectRatio);
    phaseTracerText->setPositionAlignment(Qt::AlignRight|Qt::AlignBottom);
    phaseTracerText->position->setCoords(1.0, 0.95); // lower right corner of axis rect
    phaseTracerText->setText("Points of fixed\nphase define\nphase velocity vp");
    phaseTracerText->setTextAlignment(Qt::AlignLeft);
    phaseTracerText->setFont(QFont(font().family(), 9));
    phaseTracerText->setPadding(QMargins(8, 0, 0, 0));

    // add arrow pointing at phase tracer, coming from label:
    QCPItemCurve *phaseTracerArrow = new QCPItemCurve(customPlot);
    phaseTracerArrow->start->setParentAnchor(phaseTracerText->left);
    phaseTracerArrow->startDir->setParentAnchor(phaseTracerArrow->start);
    phaseTracerArrow->startDir->setCoords(-40, 0); // direction 30 pixels to the left of parent anchor (tracerArrow->start)
    phaseTracerArrow->end->setParentAnchor(phaseTracer->position);
    phaseTracerArrow->end->setCoords(10, 10);
    phaseTracerArrow->endDir->setParentAnchor(phaseTracerArrow->end);
    phaseTracerArrow->endDir->setCoords(30, 30);
    phaseTracerArrow->setHead(QCPLineEnding::esSpikeArrow);
    phaseTracerArrow->setTail(QCPLineEnding(QCPLineEnding::esBar, (phaseTracerText->bottom->pixelPosition().y()-phaseTracerText->top->pixelPosition().y())*0.85));

    // add the group velocity tracer (green circle):
    QCPItemTracer *groupTracer = new QCPItemTracer(customPlot);
    groupTracer->setGraph(graph);
    groupTracer->setGraphKey(5.5);
    groupTracer->setInterpolating(true);
    groupTracer->setStyle(QCPItemTracer::tsCircle);
    groupTracer->setPen(QPen(Qt::black));
    groupTracer->setBrush(Qt::black);
    groupTracer->setSize(7);

    // add label for group tracer:
    QCPItemText *groupTracerText = new QCPItemText(customPlot);
    groupTracerText->position->setType(QCPItemPosition::ptAxisRectRatio);
    groupTracerText->setPositionAlignment(Qt::AlignRight|Qt::AlignTop);
    groupTracerText->position->setCoords(1.0, 0.20); // lower right corner of axis rect
    groupTracerText->setText("Fixed positions in\nwave packet define\ngroup velocity vg");
    groupTracerText->setTextAlignment(Qt::AlignLeft);
    groupTracerText->setFont(QFont(font().family(), 9));
    groupTracerText->setPadding(QMargins(8, 0, 0, 0));

    // add arrow pointing at group tracer, coming from label:
    QCPItemCurve *groupTracerArrow = new QCPItemCurve(customPlot);
    groupTracerArrow->start->setParentAnchor(groupTracerText->left);
    groupTracerArrow->startDir->setParentAnchor(groupTracerArrow->start);
    groupTracerArrow->startDir->setCoords(-40, 0); // direction 30 pixels to the left of parent anchor (tracerArrow->start)
    groupTracerArrow->end->setCoords(5.5, 0.4);
    groupTracerArrow->endDir->setParentAnchor(groupTracerArrow->end);
    groupTracerArrow->endDir->setCoords(0, -40);
    groupTracerArrow->setHead(QCPLineEnding::esSpikeArrow);
    groupTracerArrow->setTail(QCPLineEnding(QCPLineEnding::esBar, (groupTracerText->bottom->pixelPosition().y()-groupTracerText->top->pixelPosition().y())*0.85));

    // add dispersion arrow:
    QCPItemCurve *arrow = new QCPItemCurve(customPlot);
    arrow->start->setCoords(1, -1.1);
    arrow->startDir->setCoords(-1, -1.3);
    arrow->endDir->setCoords(-5, -0.3);
    arrow->end->setCoords(-10, -0.2);
    arrow->setHead(QCPLineEnding::esSpikeArrow);

    // add the dispersion arrow label:
    QCPItemText *dispersionText = new QCPItemText(customPlot);
    dispersionText->position->setCoords(-6, -0.9);
    dispersionText->setRotation(40);
    dispersionText->setText("Dispersion with\nvp < vg");
    dispersionText->setFont(QFont(font().family(), 10));

    // setup a timer that repeatedly calls MainWindow::bracketDataSlot:
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(bracketDataSlot()));
    dataTimer.start(0); // Interval 0 means to refresh as fast as possible

    QObject::connect(customPlot, &QCustomPlot::mousePress, this, &MainWindow::customPlotMousePress );
    QObject::connect(customPlot, &QCustomPlot::mouseMove, this, &MainWindow::customPlotMouseMove );
    QObject::connect(customPlot, &QCustomPlot::mouseRelease, this, &MainWindow::customPlotMouseRelease );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    //==================
    // Wave bracket 삭제
    //==================
    if(_selectedBracket and (event->key()==Qt::Key_Delete) )
    {
        _brackets.removeOne(_selectedBracket);

        customPlot->removeItem(_selectedBracket);

        _selectedBracket = nullptr;
    }
    //===========
    // arrow 삭제
    //===========
    if(_selectedArrow and (event->key()==Qt::Key_Delete) )
    {
        _arrows.removeOne(_selectedArrow);

        customPlot->removeItem(_selectedArrow);

        _selectedArrow = nullptr;
    }
    //===========
    // text 삭제
    //===========
    if(_selectedText and (event->key()==Qt::Key_Delete) )
    {
        _textItems.removeOne(_selectedText);

        customPlot->removeItem(_selectedText);

        _selectedText = nullptr;
    }

    QMainWindow::keyPressEvent(event);
}

void MainWindow::bracketDataSlot()
{
    double secs = QCPAxisTickerDateTime::dateTimeToKey(QDateTime::currentDateTime());

    // update data to make phase move:
    int n = 500;
    double phase = secs*5;
    double k = 3;
    QVector<double> x(n), y(n);
    for (int i=0; i<n; ++i)
    {
      x[i] = i/(double)(n-1)*34 - 17;
      y[i] = qExp(-x[i]*x[i]/20.0)*qSin(k*x[i]+phase);
    }
    customPlot->graph()->setData(x, y);

    itemDemoPhaseTracer->setGraphKey((8*M_PI+fmod(M_PI*1.5-phase, 6*M_PI))/k);

    customPlot->replot();

    // calculate frames per second:
    double key = secs;
    static double lastFpsKey;
    static int frameCount;
    ++frameCount;
    if (key-lastFpsKey > 2) // average fps over 2 seconds
    {
      ui->statusbar->showMessage(
            QString("%1 FPS, Total Data points: %2")
            .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
            .arg(customPlot->graph(0)->data()->size())
            , 0);
      lastFpsKey = key;
      frameCount = 0;
    }
}

void MainWindow::on_pushButton_clicked()
{
    _advancedAxesDemo = new AdvancedAxesDemo(this);

    _advancedAxesDemo->show();
}

void MainWindow::customPlotMousePress(QMouseEvent *event)
{
    if(_selectedBracket) _selectedBracket->setSelected(false);
    if(_selectedArrow) _selectedArrow->setSelected(false);
    if(_selectedText) _selectedText->setSelected(false);

    _selectedBracket = nullptr;
    _selectedArrow = nullptr;
    _selectedText = nullptr;

    QCPAbstractItem* item = customPlot->itemAt(event->pos());

    _selectedBracket = dynamic_cast<WaveBracket*>(item);
    _selectedArrow = dynamic_cast<ArrowLine*>(item);
    _selectedText = dynamic_cast<PlotTextItem*>(item);

    if(event->button()==Qt::RightButton)
    {
        _mousePressed = true;

        _bracketPosCompleted = false;
        _arrowPosCompleted = false;
        _textPosCompleted = false;

        _xStart = customPlot->xAxis->pixelToCoord(event->pos().x());
        _yStart = customPlot->yAxis->pixelToCoord(event->pos().y());

    }
    //
    //====================
    // wave bracket 인 경우
    //====================
    if(_selectedBracket and event->button()==Qt::LeftButton)
    {
        _bracketItemSelected = true;

        _mousePressPos.setX(customPlot->xAxis->pixelToCoord(event->pos().x()));
        _mousePressPos.setY(customPlot->yAxis->pixelToCoord(event->pos().y()));

        _leftPos0 = _selectedBracket->left->coords();
        _rightPos0 = _selectedBracket->right->coords();

        _selectedBracket->setSelected(true);
        //=============================
        // left and right rect item 생성
        //=============================
        _leftRect = new QCPItemRect(customPlot);
        _rightRect = new QCPItemRect(customPlot);
        // w, h 계산
        QVector<double> xTicks = customPlot->xAxis->tickVector();
        QVector<double> yTicks = customPlot->yAxis->tickVector();
        qreal w = 0.1*fabs(xTicks[1]-xTicks[0]);
        qreal h = 0.1*fabs(yTicks[1]-yTicks[0]);
        // left box
        QPointF topleft = _selectedBracket->left->coords();
        topleft.setX(topleft.x()+0.5*w);
        topleft.setY(topleft.y()-0.5*h);
        _leftRect->topLeft->setCoords(topleft);

        QPointF bottomRight;
        bottomRight.setX(topleft.x() - w);
        bottomRight.setY(topleft.y() + h);
        _leftRect->bottomRight->setCoords(bottomRight);

        QRectF leftRect;
        leftRect.setTopLeft(topleft);
        leftRect.setBottomRight(bottomRight);

        // right box
        topleft = _selectedBracket->right->coords();
        topleft.setX(topleft.x()+0.5*w);
        topleft.setY(topleft.y()-0.5*h);
        _rightRect->topLeft->setCoords(topleft);

        bottomRight.setX(topleft.x()-w);
        bottomRight.setY(topleft.y()+h);
        _rightRect->bottomRight->setCoords(bottomRight);

        QRectF rightRect;
        rightRect.setTopLeft(topleft);
        rightRect.setBottomRight(bottomRight);

        _leftRectClicked = rectContains(_mousePressPos, leftRect);
        _rightRectClicked = rectContains(_mousePressPos, rightRect);
    }
    //=============
    // arrow 인 경우
    //=============
    if(_selectedArrow and event->button()==Qt::LeftButton)
    {
        _arrowItemSelected = true;

        _mousePressPos.setX(customPlot->xAxis->pixelToCoord(event->pos().x()));
        _mousePressPos.setY(customPlot->yAxis->pixelToCoord(event->pos().y()));

        _leftPos0 = _selectedArrow->start->coords();
        _rightPos0 = _selectedArrow->end->coords();

        _selectedArrow->setSelected(true);
        //=============================
        // left and right rect item 생성
        //=============================
        _leftRect = new QCPItemRect(customPlot);
        _rightRect = new QCPItemRect(customPlot);
        // w, h 계산
        QVector<double> xTicks = customPlot->xAxis->tickVector();
        QVector<double> yTicks = customPlot->yAxis->tickVector();
        qreal w = 0.1*fabs(xTicks[1]-xTicks[0]);
        qreal h = 0.1*fabs(yTicks[1]-yTicks[0]);
        // left box
        QPointF topleft = _selectedArrow->start->coords();
        topleft.setX(topleft.x()+0.5*w);
        topleft.setY(topleft.y()-0.5*h);
        _leftRect->topLeft->setCoords(topleft);

        QPointF bottomRight;
        bottomRight.setX(topleft.x() - w);
        bottomRight.setY(topleft.y() + h);
        _leftRect->bottomRight->setCoords(bottomRight);

        QRectF leftRect;
        leftRect.setTopLeft(topleft);
        leftRect.setBottomRight(bottomRight);

        // right box
        topleft = _selectedArrow->end->coords();
        topleft.setX(topleft.x()+0.5*w);
        topleft.setY(topleft.y()-0.5*h);
        _rightRect->topLeft->setCoords(topleft);

        bottomRight.setX(topleft.x()-w);
        bottomRight.setY(topleft.y()+h);
        _rightRect->bottomRight->setCoords(bottomRight);

        QRectF rightRect;
        rightRect.setTopLeft(topleft);
        rightRect.setBottomRight(bottomRight);

        _leftRectClicked = rectContains(_mousePressPos, leftRect);
        _rightRectClicked = rectContains(_mousePressPos, rightRect);
    }
    //=============
    // Text 인 경우
    //=============
    if(_selectedText and event->button()==Qt::LeftButton)
    {

        _textItemSelected = true;

        _mousePressPos.setX(customPlot->xAxis->pixelToCoord(event->pos().x()));
        _mousePressPos.setY(customPlot->yAxis->pixelToCoord(event->pos().y()));

        _leftPos0 = _selectedText->position->coords();

        _selectedText->setSelected(true);
    }

}

void MainWindow::customPlotMouseMove(QMouseEvent *event)
{
    QPoint mousePos = event->pos();

    if(_mousePressed)
    {
        _xEnd = customPlot->xAxis->pixelToCoord(mousePos.x());
        _yEnd = customPlot->yAxis->pixelToCoord(mousePos.y());

        _bracketPosCompleted = false;
        _arrowPosCompleted = false;
        _textPosCompleted = false;

        if(_addBracket) drawItemBracket();
        if(_addArrow) drawItemArrow();
        if(_addText) drawItemText();
    }
    //=============
    // Wave bracket
    //=============
    if(_selectedBracket and _bracketItemSelected)
    {

        _mouseMovePos.setX(customPlot->xAxis->pixelToCoord(event->pos().x()));
        _mouseMovePos.setY(customPlot->yAxis->pixelToCoord(event->pos().y()));

        _deltaPos = _mouseMovePos - _mousePressPos;

        QPointF currentLeftPos = _leftPos0+_deltaPos;
        QPointF currentRightPos = _rightPos0+_deltaPos;

        if(_leftRectClicked)
        {
            _selectedBracket->left->setCoords(currentLeftPos);
        }
        else if(_rightRectClicked)
        {
            _selectedBracket->right->setCoords(currentRightPos);
        }
        else
        {
            _selectedBracket->left->setCoords(currentLeftPos);
            _selectedBracket->right->setCoords(currentRightPos);
        }
        //===================================
        // left and right rect items updating
        //===================================
        if(_leftRect) customPlot->removeItem(_leftRect);
        if(_rightRect) customPlot->removeItem(_rightRect);

        _leftRect= nullptr;
        _rightRect = nullptr;

        _leftRect = new QCPItemRect(customPlot);
        _rightRect = new QCPItemRect(customPlot);
        // w, h 계산
        QVector<double> xTicks = customPlot->xAxis->tickVector();
        QVector<double> yTicks = customPlot->yAxis->tickVector();
        qreal w = 0.1*fabs(xTicks[1]-xTicks[0]);
        qreal h = 0.1*fabs(yTicks[1]-yTicks[0]);
        // left box
        QPointF topleft = _selectedBracket->left->coords();
        topleft.setX(topleft.x()+0.5*w);
        topleft.setY(topleft.y()-0.5*h);
        _leftRect->topLeft->setCoords(topleft);

        QPointF bottomRight;
        bottomRight.setX(topleft.x() - w);
        bottomRight.setY(topleft.y() + h);
        _leftRect->bottomRight->setCoords(bottomRight);
        // right box
        topleft = _selectedBracket->right->coords();
        topleft.setX(topleft.x()+0.5*w);
        topleft.setY(topleft.y()-0.5*h);
        _rightRect->topLeft->setCoords(topleft);

        bottomRight.setX(topleft.x()-w);
        bottomRight.setY(topleft.y()+h);
        _rightRect->bottomRight->setCoords(bottomRight);
    }
    //======
    // arrow
    //======
    if(_selectedArrow and _arrowItemSelected)
    {

        _mouseMovePos.setX(customPlot->xAxis->pixelToCoord(event->pos().x()));
        _mouseMovePos.setY(customPlot->yAxis->pixelToCoord(event->pos().y()));

        _deltaPos = _mouseMovePos - _mousePressPos;

        QPointF currentLeftPos = _leftPos0+_deltaPos;
        QPointF currentRightPos = _rightPos0+_deltaPos;

        if(_leftRectClicked)
        {
            _selectedArrow->start->setCoords(currentLeftPos);
        }
        else if(_rightRectClicked)
        {
            _selectedArrow->end->setCoords(currentRightPos);
        }
        else
        {
            _selectedArrow->start->setCoords(currentLeftPos);
            _selectedArrow->end->setCoords(currentRightPos);
        }
        //===================================
        // left and right rect items updating
        //===================================
        if(_leftRect) customPlot->removeItem(_leftRect);
        if(_rightRect) customPlot->removeItem(_rightRect);

        _leftRect= nullptr;
        _rightRect = nullptr;

        _leftRect = new QCPItemRect(customPlot);
        _rightRect = new QCPItemRect(customPlot);
        // w, h 계산
        QVector<double> xTicks = customPlot->xAxis->tickVector();
        QVector<double> yTicks = customPlot->yAxis->tickVector();
        qreal w = 0.1*fabs(xTicks[1]-xTicks[0]);
        qreal h = 0.1*fabs(yTicks[1]-yTicks[0]);
        // left box
        QPointF topleft = _selectedArrow->start->coords();
        topleft.setX(topleft.x()+0.5*w);
        topleft.setY(topleft.y()-0.5*h);
        _leftRect->topLeft->setCoords(topleft);

        QPointF bottomRight;
        bottomRight.setX(topleft.x() - w);
        bottomRight.setY(topleft.y() + h);
        _leftRect->bottomRight->setCoords(bottomRight);
        // right box
        topleft = _selectedArrow->end->coords();
        topleft.setX(topleft.x()+0.5*w);
        topleft.setY(topleft.y()-0.5*h);
        _rightRect->topLeft->setCoords(topleft);

        bottomRight.setX(topleft.x()-w);
        bottomRight.setY(topleft.y()+h);
        _rightRect->bottomRight->setCoords(bottomRight);
    }
    //==========
    // Text item
    //==========
    if(_selectedText and _textItemSelected)
    {

        _mouseMovePos.setX(customPlot->xAxis->pixelToCoord(event->pos().x()));
        _mouseMovePos.setY(customPlot->yAxis->pixelToCoord(event->pos().y()));

        _deltaPos = _mouseMovePos - _mousePressPos;

        QPointF currentLeftPos = _leftPos0+_deltaPos;

        _selectedText->position->setCoords(currentLeftPos);
    }
}

void MainWindow::customPlotMouseRelease(QMouseEvent *event)
{
    QPoint mousePos = event->pos();

    if(_mousePressed)
    {
        _xEnd = customPlot->xAxis->pixelToCoord(mousePos.x());
        _yEnd = customPlot->yAxis->pixelToCoord(mousePos.y());

        _mousePressed = false;

        if(_addBracket)
        {
            _bracketPosCompleted = true;
            drawItemBracket();
        }
        //
        if(_addArrow)
        {
            _arrowPosCompleted = true;
            drawItemArrow();
        }
        //
        if(_addText)
        {
            _textPosCompleted = true;
            drawItemText();
        }
    }
    //=============
    // Wave bracket
    //=============
    if(_selectedBracket)
    {
        _bracketItemSelected = false;

        if(_leftRect) customPlot->removeItem(_leftRect);
        if(_rightRect) customPlot->removeItem(_rightRect);

        _leftRect= nullptr;
        _rightRect = nullptr;
    }
    //======
    // arrow
    //======
    if(_selectedArrow)
    {
        _arrowItemSelected = false;

        if(_leftRect) customPlot->removeItem(_leftRect);
        if(_rightRect) customPlot->removeItem(_rightRect);

        _leftRect= nullptr;
        _rightRect = nullptr;
    }
    //==========
    // text item
    //==========
    if(_selectedText)
    {
        _textItemSelected = false;
    }
}

bool MainWindow::rectContains(QPointF p, QRectF r)
{
    if(r.contains(p)) return true;
    else return false;
}

void MainWindow::drawItemBracket()
{
    WaveBracket* bracket = new WaveBracket(customPlot);
    // add the bracket at the top:
    bracket->left->setCoords(_xStart, _yStart);
    bracket->right->setCoords(_xEnd, _yEnd);
    bracket->setLength(13);

    customPlot->replot();

    _xEndOld = _xEnd;
    _yEndOld = _yEnd;

    if(!_bracketPosCompleted)
    {
        customPlot->removeItem(bracket);
    }
    else
    {
        _bracketPosCompleted = false;
        ui->addBracketRadioButton->setChecked(_bracketPosCompleted);
        _brackets.append(bracket);
    }
}

void MainWindow::drawItemArrow()
{
    // add line arrow item
    ArrowLine* arrow = new ArrowLine(customPlot);

    arrow->start->setCoords(_xStart, _yStart);
    arrow->end->setCoords(_xEnd, _yEnd);

    customPlot->replot();

    _xEndOld = _xEnd;
    _yEndOld = _yEnd;

    if(!_arrowPosCompleted)
    {
        customPlot->removeItem(arrow);
    }
    else
    {
        _arrowPosCompleted = false;
        _arrows.append(arrow);
    }
}

void MainWindow::drawItemText()
{
    PlotTextItem* textItem = new PlotTextItem(customPlot);

    //textItem->position->setType(QCPItemPosition::ptAxisRectRatio);
    //textItem->setPositionAlignment(Qt::AlignRight|Qt::AlignBottom);
    textItem->position->setCoords(_xStart, _yStart); // lower right corner of axis rect
    textItem->setText("Double click text");
    //textItem->setTextAlignment(Qt::AlignLeft);
    //textItem->setFont(QFont(font().family(), 9));
    //textItem->setPadding(QMargins(8, 0, 0, 0));

    customPlot->replot();

    _xEndOld = _xEnd;
    _yEndOld = _yEnd;

    if(!_textPosCompleted)
    {
        customPlot->removeItem(textItem);
    }
    else
    {
        _textPosCompleted = false;
        _textItems.append(textItem);
    }
}

void MainWindow::on_addBracketRadioButton_toggled(bool checked)
{
    if(checked)
    {
        _addBracket = true;
    }
    else
    {
        _addBracket = false;
    }
}

void MainWindow::on_addArrowRadioButton_toggled(bool checked)
{
    if(checked)
    {
        _addArrow = true;
    }
    else
    {
        _addArrow = false;
    }
}

void MainWindow::on_addTextRadioButton_toggled(bool checked)
{
    if(checked)
    {
        _addText = true;
    }
    else
    {
        _addText = false;
    }
}
