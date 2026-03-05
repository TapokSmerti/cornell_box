#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QGroupBox>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setStyleSheet("background-color: #606f7d;");
    setCentralWidget(centralWidget);

    QHBoxLayout *layout = new QHBoxLayout(centralWidget);

    // Левая панель
    QWidget *controlPanel = new QWidget(centralWidget);
    controlPanel->setStyleSheet("background-color: #8e9dab;");
    controlPanel->setMinimumWidth(250);

    QVBoxLayout *controlLayout = new QVBoxLayout(controlPanel);

    // Группа света
    QGroupBox *lightGroup = new QGroupBox("Источник света");
    QVBoxLayout *lightLayout = new QVBoxLayout(lightGroup);

    lightLayout->addWidget(new QLabel("Интенсивность (0.0 — 20.0):"));

    lightIntensitySpinBox = new QDoubleSpinBox();
    lightIntensitySpinBox->setRange(0.0, 20.0);
    lightIntensitySpinBox->setSingleStep(0.1);
    lightIntensitySpinBox->setDecimals(1);
    lightIntensitySpinBox->setValue(5.0);
    lightIntensitySpinBox->setSpecialValueText(""); // пустое поле недопустимо
    lightLayout->addWidget(lightIntensitySpinBox);

    renderButton = new QPushButton("Рендерить");
    lightLayout->addWidget(renderButton);

    connect(renderButton, &QPushButton::clicked,
            this, &MainWindow::onRenderClicked);

    controlLayout->addWidget(lightGroup);
    controlLayout->addStretch();

    // OpenGL виджет
    glWidget = new GLWidgetGPU(centralWidget);

    layout->addWidget(controlPanel);
    layout->addWidget(glWidget, 1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onRenderClicked()
{
    // Проверка что поле не пустое и значение в допустимом диапазоне
    QString text = lightIntensitySpinBox->cleanText();
    if (text.isEmpty()) {
        renderButton->setText("Введите значение!");
        return;
    }

    float intensity = lightIntensitySpinBox->value();
    glWidget->setLightIntensity(intensity);
    glWidget->update();

    renderButton->setText("Рендерить");
}
