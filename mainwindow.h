#pragma once
#include <QMainWindow>
#include "datadecimator.h"
#include "graph.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    using storage_t = QSharedPointer<QCPGraphDataContainer>;

private:
    Ui::MainWindow* ui;
    QWidget*        cw          { new QWidget };
    QGridLayout*    grid        { new QGridLayout };
    QLineEdit*      lePath      { new QLineEdit("../Data1.txt") };
    QPushButton*    btnPath     { new QPushButton("Set Path") };
    QPushButton*    btnAddGraph { new QPushButton("Add Graph") };
    QSize           graphSize   {10, 6};
    size_t          gridRow     { };
    QList<Graph*>   graphField  { };
    storage_t       storage     { new storage_t::value_type };

    QVector<AlgorithmComboBoxEl> comboBoxAlg {
        {
            [](const storage_t& data, double sv) { return storage_t{ new storage_t::value_type(*data) }; },
            " None" ,
            [](const QCPRange& xRange, const QCPRange& yRange) { return 0; }
        },
        {
            DataDecimator::douglas_peucker,
            "Douglas-Peucker",
            [](const QCPRange& xRange, const QCPRange& yRange) { return (yRange.upper - yRange.lower) / 30.0; }
        },
        {
            DataDecimator::reumann_witkam,
            "Reumann-Witkam",
            [](const QCPRange& xRange, const QCPRange& yRange) { return (yRange.upper - yRange.lower) / 30.0; }
        },
        {
            DataDecimator::opheim,
            "Opheim",
            [](const QCPRange& xRange, const QCPRange& yRange) { return 0; }
        },
        {
            DataDecimator::lang,
            "Lang",
            [](const QCPRange& xRange, const QCPRange& yRange) { return (yRange.upper - yRange.lower) / 40.0;}
        }
    };
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool read_from_file(const QString& path);
    void addGraph();

public slots:
    void btnPathClicked(bool checked);
    void btnAddGraphClicked(bool checked);
    void btnCloseGraphClicked(Graph* widget);
};
