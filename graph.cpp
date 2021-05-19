#include "graph.h"

Graph::Graph()
    : QObject(nullptr)
{
    connect(m_cbAlgorithm,  QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Graph::currentAlgChanged);
    connect(m_buttonClose,  &QPushButton::clicked, this, &Graph::btnCloseClicked);
    connect(m_dteStart,     &QDateTimeEdit::dateTimeChanged, this, &Graph::dteStartChanged);
    connect(m_dteEnd,       &QDateTimeEdit::dateTimeChanged, this, &Graph::dteEndChanged);
    m_buttonClose->setMinimumSize(25, 25);
    m_buttonClose->setMaximumSize(25, 25);
    m_dteEnd->setMaximumWidth(180);
    m_dteStart->setMaximumWidth(180);
    m_dteEnd->setMinimumWidth(180);
    m_dteStart->setMinimumWidth(180);
    m_customPlot->setMinimumHeight(300);
    m_dteStart->setCalendarPopup(true);
    m_dteEnd->setCalendarPopup(true);
    m_grid->addWidget(m_customPlot,  0, 0, 8, 15 );
    m_grid->addWidget(m_buttonClose, 0, 18 );
    m_grid->addWidget(m_lReplotTime, 2, 15, 1, 2 );
    m_grid->addWidget(m_nData,       3, 15, 1, 2 );
    m_grid->addWidget(m_dteStart,    4, 15, 1, 2 );
    m_grid->addWidget(m_dteEnd,      4, 17, 1, 2 );
    m_grid->addWidget(m_cbAlgorithm, 5, 15 );
    m_widget->setLayout(m_grid);
}

Graph::~Graph()
{
    delete m_cbAlgorithm;
    delete m_lReplotTime;
    delete m_nData;
    delete m_dteEnd;
    delete m_dteStart;
    delete m_customPlot;
    delete m_buttonClose;
    delete m_grid;
    delete m_widget;
}


void Graph::setField(const QVector<AlgorithmComboBoxEl> &el)
{
    comboBoxEl = &el;
    for(const auto& x : *comboBoxEl) {
        m_cbAlgorithm->addItem(x.text);
    }
}

void Graph::replot(int id)
{
    if(!comboBoxEl || m_algIndex >= comboBoxEl->size()) {
        return;
    }
    auto graph = m_customPlot->graph(id);
    bool xEx, yEx;
    auto yr = m_storage->valueRange(yEx);
    auto xr = m_storage->keyRange(xEx);
    if(!xEx || !yEx) {
        graph->data()->clear();
    }
    else {
        xRange = xr;
        yRange = yr;
        auto alg = (*comboBoxEl)[m_algIndex];
        graph->setData(alg.function(m_storage, alg.kFunc(xRange, yRange)));
        auto lower = QDateTime::fromTime_t(xRange.lower);
        auto upper = QDateTime::fromTime_t(xRange.upper);
        m_dteStart->setDateTimeRange(lower, upper);
        m_dteEnd->setDateTimeRange(lower, upper);
        m_dteStart->setDateTime(lower);
        m_dteEnd->setDateTime(upper);
    }
    QTime timer;
    timer.start();
    m_customPlot->xAxis->setRange(xRange);
    m_customPlot->yAxis->setRange(yRange);
    m_customPlot->replot();
    m_lReplotTime->setText("Replot time:\t" + QString::number(timer.elapsed()) + "ms");
    m_nData->setText("Data count:\t" + QString::number(graph->data()->size()));
}

void Graph::replot(int id, const storage_t& data)
{
    if(!comboBoxEl || m_algIndex >= comboBoxEl->size()) {
        return;
    }
    auto graph = m_customPlot->graph(id);
    bool xEx, yEx;
    auto yr = data->valueRange(yEx);
    auto xr = data->keyRange(xEx);
    if(!xEx || !yEx) {
        graph->data()->clear();
    }
    else {
        xRange.expand(xr);
        yRange.expand(yr);
        auto alg = (*comboBoxEl)[m_algIndex];
        graph->setData(alg.function(data, alg.kFunc(xRange, yRange)));
    }
    QTime timer;
    timer.start();
    m_customPlot->xAxis->setRange(xr);
    m_customPlot->yAxis->setRange(yr);
    m_customPlot->replot();
    m_lReplotTime->setText("Replot time:\t" + QString::number(timer.elapsed()) + "ms");
    m_nData->setText("Data count:\t" + QString::number(graph->data()->size()));
}

void Graph::setStorage(const storage_t& data)
{
    m_storage = data;
}

//                  Slots

void Graph::currentAlgChanged(int index)
{
    m_algIndex = index;
    replot(0);
}

void Graph::dteStartChanged(const QDateTime& dateTime)
{
    auto s2 = storage_t(new storage_t::value_type(*m_storage));
    s2->removeBefore(static_cast<double>(m_dteStart->dateTime().toTime_t()));
    s2->removeAfter(static_cast<double>(m_dteEnd->dateTime().toTime_t()));
    m_dteEnd->setMinimumDateTime(m_dteStart->dateTime());
    replot(0, s2);
}

void Graph::dteEndChanged(const QDateTime& dateTime)
{
    auto s2 = storage_t(new storage_t::value_type(*m_storage));
    s2->removeBefore(static_cast<double>(m_dteStart->dateTime().toTime_t()));
    s2->removeAfter(static_cast<double>(m_dteEnd->dateTime().toTime_t()));
    m_dteStart->setMaximumDateTime(m_dteEnd->dateTime());
    replot(0, s2);
}

void Graph::btnCloseClicked(bool checked)
{
    emit close(this);
    this->~Graph();
}

