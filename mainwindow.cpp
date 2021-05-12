#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QGridLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    grid->addWidget(&*lePath, gridRow, 0, 1, 3);
    grid->addWidget(&*btnPath, gridRow++, 3, 1, 1);
    grid->addWidget(&*btnAddGraph, gridRow++, 0);
    cw->setLayout(grid);
    setCentralWidget(cw);
    connect(btnPath,      &QPushButton::clicked, this, &MainWindow::btnPathClicked);
    connect(btnAddGraph,  &QPushButton::clicked, this, &MainWindow::btnAddGraphClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete cw;
    delete grid;
    delete lePath;
    delete btnPath;
    delete btnAddGraph;
    for(auto& graph : graphField) {
        delete graph;
    }
}

bool MainWindow::read_from_file(const QString &path)
{
    FILE* file = fopen(path.toLocal8Bit().data(), "r");
    if(!file) {
        return false;
    }
    size_t  l;
    char*   line { nullptr };
    double  value;
    while(!feof(file)) {
        getline(&line, &l, file);
        struct tm t { };
        sscanf(line, "%d.%d.%d %d:%d:%d\t%lf", &t.tm_mday, &t.tm_mon, &t.tm_year, &t.tm_hour, &t.tm_min, &t.tm_sec, &value);
        t.tm_year -= 1900;
        --t.tm_mon;
        storage->add({ static_cast<double>(mktime(&t)), value });
    }
    return true;
}

void MainWindow::addGraph()
{
    Graph* newGraph { new Graph };
    newGraph->customPlot()->addGraph();
    newGraph->customPlot()->xAxis->setTicker(QSharedPointer<QCPAxisTickerDateTime>(new QCPAxisTickerDateTime));
    newGraph->setField(comboBoxAlg);
    connect(newGraph, &Graph::close, this, &MainWindow::btnCloseGraphClicked);
    grid->addWidget(newGraph->widget(), gridRow++, 0, 1, 4);
    grid->addWidget(btnAddGraph, gridRow++, 0);
    graphField.push_back(newGraph);
}

void MainWindow::btnPathClicked(bool checked)
{
    storage->clear();
    if(read_from_file(lePath->text())) {
        for(const auto& x : graphField) {
            x->setStorage(storage);
            x->replot(0);
        }
    }
}

void MainWindow::btnAddGraphClicked(bool checked)
{
    addGraph();
    auto graph = graphField.back();
    graph->setStorage(storage);
    graph->replot(0);
}

void MainWindow::btnCloseGraphClicked(Graph* graph)
{
    graphField.removeOne(graph);
    grid->removeWidget(graph->widget());
}



