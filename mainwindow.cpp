#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QGridLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cw->setLayout(grid);
    ui->scrollArea->setWidget(cw);
    connect(ui->btnRdDt,        &QPushButton::clicked, this, &MainWindow::btnSetFileClicked);
    connect(ui->btnAddGraph,    &QPushButton::clicked, this, &MainWindow::btnAddGraphClicked);
    connect(ui->btnClose,       &QPushButton::clicked, this, &MainWindow::close);
    connect(ui->comboBox,       QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::cbIndexChanged);
    connect(ui->lePath,         &QLineEdit::textEdited, this, &MainWindow::lePathEdited);
    connect(&*browser,          &EleSyOpcUaClient::QUaBrowser::directoryChanged, this, &MainWindow::serverDirChanged);
}

MainWindow::~MainWindow()
{
    delete grid;
    delete cw;
    delete ui;
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
    graphField.push_back(newGraph);
}

MainWindow::storage_t MainWindow::readFromServer(const QString &nodeId, const QDateTime &start, const QDateTime &end)
{
    storage_t qcpData { new storage_t::value_type };
    if(start < end)
    {
        using namespace EleSyOpcUaClient;
        UaBytes cPoint, cPointNext;
        auto id = nodeId.toStdString();
        auto data = history->getHistoryRawValues(id, 1000000, UaDateTime(start.toMSecsSinceEpoch() * 1000), UaDateTime(end.toMSecsSinceEpoch() * 1000), false, cPoint);
        for(const auto& x : data) {
            qcpData->add(QCPGraphData(x.sourceTimeStamp.toUnixMicrosec() / 1000000, x.val->v_double()));
        }
        do {
            data = history->getNextHistoryRawValues(id, cPoint, cPointNext);
            for(const auto& x : data) {
                qcpData->add(QCPGraphData(x.sourceTimeStamp.toUnixMicrosec() / 1000000, x.val->v_double()));
            }
            cPoint = cPointNext;
        } while(!data.empty());
        history->releasePoint(id, cPoint);
    }
    return qcpData;
}

void MainWindow::btnSetFileClicked(bool checked)
{
    storage->clear();
    QTime timer;
    timer.start();
    bool readed = read_from_file(ui->lePath->text());
    if(!readed)
    {
        ui->lePath->setStyleSheet("color: red;");
    }
    else
    {
        ui->lblRdTm->setText(QString::number(timer.elapsed()) + "ms");
    }
    for(const auto& x : graphField)
    {
        x->setStorage(storage);
        x->replot(0);
    }
}

void MainWindow::btnSetServerClicked(bool checked)
{
    //opc.tcp://192.168.56.1:62456
    using namespace EleSyOpcUaClient;
    ConnectionParams params;
    params.serverUrl = ui->lePath->text().toStdString();
    params.connectionName = "Graph Decimator Connection";
    UaStatusCode connectionStatus = createConnection(params, true, false, connection);
    browser->setConnection(connection);
    UaStatusCode historyStatus = createHistory(connection, history);
    if(connectionStatus != uaGood || historyStatus != uaGood || !connection->isConnected() || browser->isValid())
    {
        ui->lePath->setStyleSheet("color: red;");
        return;
    }
    browser->widget()->show();
}

void MainWindow::serverDirChanged(const QString &dirname)
{
    using namespace EleSyOpcUaClient;
    UaDateTime start("2021-04-23T00:00:00.000Z");
    UaDateTime end("2021-04-23T00:05:00.000Z");
    storage = readFromServer(dirname, QDateTime::fromTime_t(start.toUnixMicrosec() / 1000000), QDateTime::fromTime_t(end.toUnixMicrosec() / 1000000));
    for(const auto& x : graphField) {
        x->setStorage(storage);
        x->replot(0);
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

void MainWindow::cbIndexChanged(bool checked)
{
    QString cbCurrentText = ui->comboBox->currentText();
    ui->btnRdDt->setText("Read data from " + cbCurrentText);
    ui->lePath->setStyleSheet("color: black");
    if(cbCurrentText == "File")
    {
        ui->lblPath->setText("Path to file:");
        connect(ui->btnRdDt,    &QPushButton::clicked, this, &MainWindow::btnSetFileClicked);
        disconnect(ui->btnRdDt, &QPushButton::clicked, this, &MainWindow::btnSetServerClicked);
    }
    else
    {
        ui->lblPath->setText("Server IP");
        disconnect(ui->btnRdDt, &QPushButton::clicked, this, &MainWindow::btnSetFileClicked);
        connect(ui->btnRdDt,    &QPushButton::clicked, this, &MainWindow::btnSetServerClicked);
    }
}

void MainWindow::lePathEdited(const QString &text)
{
    ui->lePath->setStyleSheet("color: black");
}


