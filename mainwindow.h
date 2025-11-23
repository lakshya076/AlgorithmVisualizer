#pragma once

#include <QMainWindow>

// Forward declarations to avoid including full headers
class QComboBox;
class QPushButton;
class QSlider;
class QLabel;
class AlgorithmCanvas;
class VisualizerController;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setupUI();
    void connectSignals();

    // Controller
    VisualizerController* m_controller;

    // Drawing Widget
    AlgorithmCanvas* m_canvas;

    // UI Control Widgets
    QComboBox* m_algBox;
    QPushButton* m_shuffleButton;
    QPushButton* m_startButton;
    QPushButton* m_pauseButton;
    QPushButton* m_stopButton;
    QPushButton* m_prevButton;
    QPushButton* m_nextButton;
    QSlider* m_speedSlider;
    QLabel* m_speedLabel;
};
