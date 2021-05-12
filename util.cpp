#include "util.h"

void messageBox(const QString& text)
{
    QMessageBox mb;
    mb.setText(text);
    mb.show();
    mb.exec();
}

QSharedPointer<QCPGraphDataContainer> getDataByKeyRange(const QSharedPointer<QCPGraphDataContainer>& storage, const QCPRange& range)
{
    QSharedPointer<QCPGraphDataContainer> rangedStorage { new QCPGraphDataContainer(*storage) };
    rangedStorage->removeBefore(range.lower);
    rangedStorage->removeAfter(range.upper);
    return rangedStorage;
}

void setRanges(QCustomPlot* plot, const QCPRange& xRange, const QCPRange& yRange)
{
    plot->xAxis->setRange(xRange);
    plot->yAxis->setRange(yRange);
}

bool replotGraph(QCustomPlot* plot, int id, const QSharedPointer<QCPGraphDataContainer>& data)
{
    auto graph = plot->graph(id);
    graph->setData(data);
    bool xValid, yValid;
    auto xRange = graph->getKeyRange(xValid);
    auto yRange = graph->getValueRange(yValid);
    bool response = false;
    if( !xValid || !yValid ) {
        graph->data().clear();
    }
    else {
        setRanges(plot, xRange, yRange);
        response = true;
    }
    plot->replot();
    return response;
}
