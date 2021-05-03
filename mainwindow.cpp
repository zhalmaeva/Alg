#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFile>
#include <QDateTime>
#include <qcustomplot.h>

#include <iostream>
#include <datadecimator.h>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lePath->setText("../GraphDecimator/Data1.txt");
    ui->plot1->xAxis->setTicker(QSharedPointer<QCPAxisTickerDateTime>(new QCPAxisTickerDateTime));
    ui->plot2->xAxis->setTicker(QSharedPointer<QCPAxisTickerDateTime>(new QCPAxisTickerDateTime));
    connect(ui->btnSetPath, &QPushButton::clicked, this, &MainWindow::setPathClicked);
    /*connect(ui->closeButton, &QPushButton::clicked, this, &MainWindow::close);
    connect(ui->plusButton, &QPushButton::pressed, this, &MainWindow::scale_x);*/
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::read_from_file(const QString& path)
{
    FILE* file = fopen(path.toLocal8Bit().data(), "r");
    storage.clear();
    if(file == nullptr) {
        return false;
    }
    size_t l { 0 };
    char* line { nullptr };
    Point p;
    while(!feof(file)) {
        getline(&line, &l, file);
        struct tm t { };
        sscanf(line, "%d.%d.%d %d:%d:%d\t%lf", &t.tm_mday, &t.tm_mon, &t.tm_year, &t.tm_hour, &t.tm_min, &t.tm_sec, &p.y);
        p.x = mktime(&t);
        storage.push_back(p);
    }
    return true;
}

void MainWindow::setPathClicked(bool checked)
{
    storage.clear();
    if(!read_from_file(ui->lePath->text())) {
        QMessageBox mb;
        mb.setText("File doesn't exists");
        mb.show();
        mb.exec();
    }
    else {
        ui->plot1->clearGraphs();
        ui->plot1->addGraph();
        ui->plot2->clearGraphs();
        ui->plot2->addGraph();

        ui->nData1->setText(QString::number(storage.size()));
        ui->plottingTime1->setText(QString::number(timer(replot<storage_t>, ui->plot1, 0, storage)));
        bool k;
        auto range = ui->plot1->graph(0)->data()->valueRange(k);
        //auto dec_storage = DataDecimator<storage_t>::douglas_peucker(storage, (range.upper - range.lower) / 40);
        auto dec_storage = DataDecimator<storage_t>::reuman_vitkam(storage, 3);
        ui->nData2->setText(QString::number(dec_storage.size()));
        ui->plottingTime2->setText(QString::number(timer(replot<storage_t>, ui->plot2, 0, dec_storage)));
    }
}
