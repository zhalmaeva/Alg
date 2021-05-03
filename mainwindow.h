#pragma once

#include <QMainWindow>

#include "func.h"
#include "datadecimator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    using storage_t = std::vector<Point>;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    storage_t storage;

    bool read_from_file(const QString&);

public slots:
    void setPathClicked(bool checked = false);
};
