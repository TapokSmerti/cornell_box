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

    // Изменение положения камеры

    QGroupBox *cameraGroup = new QGroupBox("Камера");
    QVBoxLayout *cameraLayout = new QVBoxLayout(cameraGroup);

    // Проекция
    cameraLayout->addWidget(new QLabel("Проекция:"));
    projectionCombo = new QComboBox();
    projectionCombo->addItem("Перспективная");
    projectionCombo->addItem("Ортографическая");
    cameraLayout->addWidget(projectionCombo);

    cameraLayout->addWidget(new QLabel("Позиция (X, Y, Z):"));
    QHBoxLayout *posLayout = new QHBoxLayout();
    camPosX = new QDoubleSpinBox(); camPosX->setRange(-10, 10); camPosX->setValue(0.0);  camPosX->setSingleStep(0.1); camPosX->setDecimals(1);
    camPosY = new QDoubleSpinBox(); camPosY->setRange(-5,  10); camPosY->setValue(1.5);  camPosY->setSingleStep(0.1); camPosY->setDecimals(1);
    camPosZ = new QDoubleSpinBox(); camPosZ->setRange(-5,  20); camPosZ->setValue(8.0);  camPosZ->setSingleStep(0.1); camPosZ->setDecimals(1);
    posLayout->addWidget(new QLabel("X")); posLayout->addWidget(camPosX);
    posLayout->addWidget(new QLabel("Y")); posLayout->addWidget(camPosY);
    posLayout->addWidget(new QLabel("Z")); posLayout->addWidget(camPosZ);
    cameraLayout->addLayout(posLayout);

    cameraLayout->addWidget(new QLabel("Цель (X, Y, Z):"));
    QHBoxLayout *targetLayout = new QHBoxLayout();
    camTargetX = new QDoubleSpinBox(); camTargetX->setRange(-10, 10); camTargetX->setValue(0.0); camTargetX->setSingleStep(0.1); camTargetX->setDecimals(1);
    camTargetY = new QDoubleSpinBox(); camTargetY->setRange(-5,  10); camTargetY->setValue(1.0); camTargetY->setSingleStep(0.1); camTargetY->setDecimals(1);
    camTargetZ = new QDoubleSpinBox(); camTargetZ->setRange(-10,  5); camTargetZ->setValue(0.0); camTargetZ->setSingleStep(0.1); camTargetZ->setDecimals(1);
    targetLayout->addWidget(new QLabel("X")); targetLayout->addWidget(camTargetX);
    targetLayout->addWidget(new QLabel("Y")); targetLayout->addWidget(camTargetY);
    targetLayout->addWidget(new QLabel("Z")); targetLayout->addWidget(camTargetZ);
    cameraLayout->addLayout(targetLayout);

    cameraLayout->addWidget(new QLabel("FOV (10 — 120):"));
    camFov = new QDoubleSpinBox(); camFov->setRange(10, 120); camFov->setValue(60.0); camFov->setSingleStep(1.0); camFov->setDecimals(1);
    cameraLayout->addWidget(camFov);

    controlLayout->addWidget(lightGroup);
    controlLayout->addWidget(cameraGroup);





    // Две кнопки рендера:
    QHBoxLayout *buttonsLayout = new QHBoxLayout();

    renderButton = new QPushButton("Рендерить");
    renderButton->setMinimumHeight(35);

    QPushButton *resetButton = new QPushButton("Сброс");
    resetButton->setMinimumHeight(35);

    buttonsLayout->addWidget(resetButton);
    buttonsLayout->addWidget(renderButton);
    controlLayout->addLayout(buttonsLayout);
    controlLayout->addStretch();

    connect(renderButton, &QPushButton::clicked, this, &MainWindow::onRenderClicked);
    connect(resetButton,  &QPushButton::clicked, this, &MainWindow::onResetClicked);
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
    QString text = lightIntensitySpinBox->cleanText();
    if (text.isEmpty()) {
        renderButton->setText("Введите значение!");
        return;
    }

    // Свет
    glWidget->setLightIntensity(lightIntensitySpinBox->value());

    // Камера
    glWidget->setCameraPosition(QVector3D(camPosX->value(), camPosY->value(), camPosZ->value()));
    glWidget->setCameraTarget(QVector3D(camTargetX->value(), camTargetY->value(), camTargetZ->value()));
    glWidget->setCameraFov(camFov->value());
    // Проекция камеры
    glWidget->setProjectionType(projectionCombo->currentIndex());


    glWidget->update();
    renderButton->setText("Рендерить");
}

void MainWindow::onResetClicked()
{
    // Свет
    lightIntensitySpinBox->setValue(5.0);

    // Камера
    camPosX->setValue(0.0);
    camPosY->setValue(1.5);
    camPosZ->setValue(8.0);
    camTargetX->setValue(0.0);
    camTargetY->setValue(1.0);
    camTargetZ->setValue(0.0);
    camFov->setValue(60.0);
    projectionCombo->setCurrentIndex(0); // перспективная
}
