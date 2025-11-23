#pragma once

#include <QMainWindow>
#include "datastructures.h"

class QComboBox;
class QPushButton;
class QSlider;
class QLabel;
class QTextEdit;
class AlgorithmCanvas;
class VisualizerController;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void appendLog(const QString& message);

private:
    void setupUI();
    void connectSignals();
    void updateLogFromStep(const QVariant& step);
    void onAlgorithmChanged(const QString& algName);

    VisualizerController* m_controller;
    AlgorithmCanvas* m_canvas;

    QComboBox* m_algBox;
    QPushButton* m_shuffleButton;
    QPushButton* m_startButton;
    QPushButton* m_pauseButton;
    QPushButton* m_stopButton;
    QPushButton* m_prevButton;
    QPushButton* m_nextButton;

    QSlider* m_speedSlider;
    QLabel* m_speedLabel;

    QTextEdit* m_logDisplay;
};
