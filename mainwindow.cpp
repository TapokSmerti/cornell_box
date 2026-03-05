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
    controlPanel->setMinimumWidth(280);
    controlPanel->setMaximumWidth(280);
    QVBoxLayout *controlLayout = new QVBoxLayout(controlPanel);

    // Вкладки
    tabWidget = new QTabWidget();
    controlLayout->addWidget(tabWidget);

    // ======= Вкладка "Камера и Свет" =======
    QWidget *camLightTab = new QWidget();
    QVBoxLayout *camLightLayout = new QVBoxLayout(camLightTab);

    // Группа света
    QGroupBox *lightGroup = new QGroupBox("Источник света");
    QVBoxLayout *lightLayout = new QVBoxLayout(lightGroup);
    lightLayout->addWidget(new QLabel("Интенсивность (0.0 — 20.0):"));
    lightIntensitySpinBox = new QDoubleSpinBox();
    lightIntensitySpinBox->setRange(0.0, 20.0);
    lightIntensitySpinBox->setSingleStep(0.1);
    lightIntensitySpinBox->setDecimals(1);
    lightIntensitySpinBox->setValue(5.0);
    lightLayout->addWidget(lightIntensitySpinBox);
    camLightLayout->addWidget(lightGroup);

    // Группа камеры
    QGroupBox *cameraGroup = new QGroupBox("Камера");
    QVBoxLayout *cameraLayout = new QVBoxLayout(cameraGroup);

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

    camLightLayout->addWidget(cameraGroup);
    camLightLayout->addStretch();
    tabWidget->addTab(camLightTab, "Камера и Свет");

    // ======= Вкладки материалов =======
    QString sphereNames[] = {"Сфера 1", "Сфера 2", "Сфера 3"};
    double albedoDef[3][3] = {{0.2, 0.8, 0.8}, {0.9, 0.8, 0.2}, {0.3, 0.2, 0.4}};
    double specDef[3][3]   = {{0.9, 0.9, 0.9}, {0.9, 0.9, 0.9}, {0.8, 0.8, 0.8}};
    double shinDef[]       = {64.0, 32.0, 16.0};
    double transDef[]      = {0.0,  0.0,  0.0};

    for (int i = 0; i < 3; i++) {
        QWidget *matTab = new QWidget();
        QVBoxLayout *matLayout = new QVBoxLayout(matTab);

        // Albedo
        matLayout->addWidget(new QLabel("Albedo (RGB):"));
        QHBoxLayout *albedoLayout = new QHBoxLayout();
        matAlbedoR[i] = new QDoubleSpinBox(); matAlbedoR[i]->setRange(0, 1); matAlbedoR[i]->setValue(albedoDef[i][0]); matAlbedoR[i]->setSingleStep(0.05); matAlbedoR[i]->setDecimals(2);
        matAlbedoG[i] = new QDoubleSpinBox(); matAlbedoG[i]->setRange(0, 1); matAlbedoG[i]->setValue(albedoDef[i][1]); matAlbedoG[i]->setSingleStep(0.05); matAlbedoG[i]->setDecimals(2);
        matAlbedoB[i] = new QDoubleSpinBox(); matAlbedoB[i]->setRange(0, 1); matAlbedoB[i]->setValue(albedoDef[i][2]); matAlbedoB[i]->setSingleStep(0.05); matAlbedoB[i]->setDecimals(2);
        albedoLayout->addWidget(new QLabel("R")); albedoLayout->addWidget(matAlbedoR[i]);
        albedoLayout->addWidget(new QLabel("G")); albedoLayout->addWidget(matAlbedoG[i]);
        albedoLayout->addWidget(new QLabel("B")); albedoLayout->addWidget(matAlbedoB[i]);
        matLayout->addLayout(albedoLayout);

        // Specular
        matLayout->addWidget(new QLabel("Specular (RGB):"));
        QHBoxLayout *specLayout = new QHBoxLayout();
        matSpecR[i] = new QDoubleSpinBox(); matSpecR[i]->setRange(0, 1); matSpecR[i]->setValue(specDef[i][0]); matSpecR[i]->setSingleStep(0.05); matSpecR[i]->setDecimals(2);
        matSpecG[i] = new QDoubleSpinBox(); matSpecG[i]->setRange(0, 1); matSpecG[i]->setValue(specDef[i][1]); matSpecG[i]->setSingleStep(0.05); matSpecG[i]->setDecimals(2);
        matSpecB[i] = new QDoubleSpinBox(); matSpecB[i]->setRange(0, 1); matSpecB[i]->setValue(specDef[i][2]); matSpecB[i]->setSingleStep(0.05); matSpecB[i]->setDecimals(2);
        specLayout->addWidget(new QLabel("R")); specLayout->addWidget(matSpecR[i]);
        specLayout->addWidget(new QLabel("G")); specLayout->addWidget(matSpecG[i]);
        specLayout->addWidget(new QLabel("B")); specLayout->addWidget(matSpecB[i]);
        matLayout->addLayout(specLayout);

        // Shininess
        matLayout->addWidget(new QLabel("Shininess (1 — 256):"));
        matShininess[i] = new QDoubleSpinBox(); matShininess[i]->setRange(1, 256); matShininess[i]->setValue(shinDef[i]); matShininess[i]->setSingleStep(1.0); matShininess[i]->setDecimals(0);
        matLayout->addWidget(matShininess[i]);

        // Transparency
        matLayout->addWidget(new QLabel("Transparency (0.0 — 1.0):"));
        matTransparency[i] = new QDoubleSpinBox(); matTransparency[i]->setRange(0, 1); matTransparency[i]->setValue(transDef[i]); matTransparency[i]->setSingleStep(0.05); matTransparency[i]->setDecimals(2);
        matLayout->addWidget(matTransparency[i]);

        matLayout->addStretch();
        tabWidget->addTab(matTab, sphereNames[i]);
    }

    // Кнопки внизу
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    QPushButton *resetButton = new QPushButton("Сброс");
    resetButton->setMinimumHeight(35);
    renderButton = new QPushButton("Рендерить");
    renderButton->setMinimumHeight(35);
    buttonsLayout->addWidget(resetButton);
    buttonsLayout->addWidget(renderButton);
    controlLayout->addLayout(buttonsLayout);

    connect(renderButton, &QPushButton::clicked, this, &MainWindow::onRenderClicked);
    connect(resetButton,  &QPushButton::clicked, this, &MainWindow::onResetClicked);

    // OpenGL виджет
    glWidget = new GLWidgetGPU(centralWidget);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidget(controlPanel);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFixedWidth(300);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    layout->addWidget(scrollArea);
    layout->addWidget(glWidget, 1);
}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onRenderClicked()
{
    glWidget->setLightIntensity(lightIntensitySpinBox->value());
    glWidget->setCameraPosition(QVector3D(camPosX->value(), camPosY->value(), camPosZ->value()));
    glWidget->setCameraTarget(QVector3D(camTargetX->value(), camTargetY->value(), camTargetZ->value()));
    glWidget->setCameraFov(camFov->value());
    glWidget->setProjectionType(projectionCombo->currentIndex());

    // Материалы
    for (int i = 0; i < 3; i++) {
        QString prefix = QString("u_material%1_").arg(i + 1);
        glWidget->setMaterialAlbedo(i + 1,       QVector3D(matAlbedoR[i]->value(), matAlbedoG[i]->value(), matAlbedoB[i]->value()));
        glWidget->setMaterialSpecular(i + 1,     QVector3D(matSpecR[i]->value(),   matSpecG[i]->value(),   matSpecB[i]->value()));
        glWidget->setMaterialShininess(i + 1,    matShininess[i]->value());
        glWidget->setMaterialTransparency(i + 1, matTransparency[i]->value());
    }

    glWidget->update();
}

void MainWindow::onResetClicked()
{
    lightIntensitySpinBox->setValue(5.0);
    camPosX->setValue(0.0); camPosY->setValue(1.5); camPosZ->setValue(8.0);
    camTargetX->setValue(0.0); camTargetY->setValue(1.0); camTargetZ->setValue(0.0);
    camFov->setValue(60.0);
    projectionCombo->setCurrentIndex(0);

    // Сброс материалов
    double albedoDef[3][3] = {{0.2, 0.8, 0.8}, {0.9, 0.8, 0.2}, {0.3, 0.2, 0.4}};
    double specDef[3][3]   = {{0.9, 0.9, 0.9}, {0.9, 0.9, 0.9}, {0.8, 0.8, 0.8}};
    double shinDef[]       = {64.0, 32.0, 16.0};

    for (int i = 0; i < 3; i++) {
        matAlbedoR[i]->setValue(albedoDef[i][0]);
        matAlbedoG[i]->setValue(albedoDef[i][1]);
        matAlbedoB[i]->setValue(albedoDef[i][2]);
        matSpecR[i]->setValue(specDef[i][0]);
        matSpecG[i]->setValue(specDef[i][1]);
        matSpecB[i]->setValue(specDef[i][2]);
        matShininess[i]->setValue(shinDef[i]);
        matTransparency[i]->setValue(0.0);
    }
}
