#pragma once

#include "qcustomplot.h"

#include <QMessageBox>
#include <QLineEdit>
#include "point.h"

template <typename Func, typename... Args>
int timer(Func&& f, Args&&... args)
{
    QTime timer;
    timer.start();
    f(args...);
    return timer.elapsed();
}

template<typename storage_t = QVector<Point>>
void replot(QCustomPlot* customPlot, unsigned id, const storage_t& storage)
{
    double yMax = std::numeric_limits<double>::min();
    double yMin = std::numeric_limits<double>::max();
    for(const auto& point : storage) {
        customPlot->graph(id)->addData(point.x, point.y);
        if(point.y > yMax) {
            yMax = point.y;
        }
        else if(point.y < yMin) {
            yMin = point.y;
        }
    }
    customPlot->yAxis->setRange(yMin, yMax);
    bool k;
    customPlot->xAxis->setRange(customPlot->graph(id)->data()->keyRange(k));
    customPlot->replot();
}
