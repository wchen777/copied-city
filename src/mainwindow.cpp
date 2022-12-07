#include "mainwindow.h"
#include "settings.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QSettings>
#include <QLabel>
#include <QWidget>
#include <QGroupBox>
#include <iostream>

void MainWindow::initialize() {
    city = new CopiedCity;
    QHBoxLayout *hLayout = new QHBoxLayout; // horizontal alignment
    QVBoxLayout *vLayout = new QVBoxLayout(); // vertical alignment
    vLayout->setAlignment(Qt::AlignTop);
    hLayout->addLayout(vLayout);
    hLayout->addWidget(city, 1);
    this->setLayout(hLayout);
}

void MainWindow::finish() {
    city->finish();
    delete(city);
}
