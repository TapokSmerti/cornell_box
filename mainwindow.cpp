#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *layout = new QHBoxLayout(centralWidget);

    // Левая панель (пока пустая)
    QWidget *controlPanel = new QWidget(centralWidget);
    controlPanel->setMinimumWidth(250);

    // OpenGL виджет
    glWidget = new GLWidgetGPU(centralWidget);

    layout->addWidget(controlPanel);
    layout->addWidget(glWidget, 1); // растягивается
}

MainWindow::~MainWindow()
{
    delete ui;
}
