#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include "glwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onRenderClicked();

private:
    Ui::MainWindow *ui;
    GLWidgetGPU *glWidget;

    QDoubleSpinBox *lightIntensitySpinBox;
    QPushButton    *renderButton;
};

#endif
