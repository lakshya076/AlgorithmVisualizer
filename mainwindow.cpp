#include "mainwindow.h"
#include "visualizercontroller.h"
#include "algorithmcanvas.h"
#include "datastructures.h"

#include <QComboBox>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QGroupBox>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    qRegisterMetaType<GraphStep>("GraphStep");
    qRegisterMetaType<SortingStep>("SortingStep");

    m_controller = new VisualizerController(this);
    m_canvas = new AlgorithmCanvas(this);

    setupUI();
    connectSignals();

    onAlgorithmChanged(m_algBox->currentText());

    showMaximized();
    setWindowTitle("Algorithm Visualizer");
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI()
{
    m_algBox = new QComboBox;
    m_algBox->addItems({
        "Bubble Sort", "Insertion Sort", "Selection Sort", "Quick Sort", "Merge Sort",
        "BST Insert", "BST Remove", "AVL Insert", "AVL Remove",
        "Graph Generate", "Graph BFS", "Graph DFS", "Graph Dijkstra", "Graph Prim's MST",
        "Maze Generate"
    });

    m_shuffleButton = new QPushButton("Shuffle Data");
    m_startButton = new QPushButton("Start");
    m_pauseButton = new QPushButton("Pause");
    m_stopButton = new QPushButton("Stop");
    m_prevButton = new QPushButton("<< Prev");
    m_nextButton = new QPushButton("Next >>");

    m_speedLabel = new QLabel("Speed:");
    m_speedSlider = new QSlider(Qt::Horizontal);
    m_speedSlider->setRange(1, 100);
    m_speedSlider->setValue(80);

    QHBoxLayout* controlLayout = new QHBoxLayout;
    controlLayout->addWidget(m_algBox);
    controlLayout->addWidget(m_shuffleButton);
    controlLayout->addSpacing(20);
    controlLayout->addWidget(m_startButton);
    controlLayout->addWidget(m_pauseButton);
    controlLayout->addWidget(m_stopButton);
    controlLayout->addWidget(m_prevButton);
    controlLayout->addWidget(m_nextButton);
    controlLayout->addStretch();

    controlLayout->addWidget(m_speedLabel);
    controlLayout->addWidget(m_speedSlider);

    QWidget* controlPanel = new QWidget;
    controlPanel->setLayout(controlLayout);
    controlPanel->setFixedHeight(60);

    m_logDisplay = new QTextEdit;
    m_logDisplay->setReadOnly(true);
    m_logDisplay->setStyleSheet("font-family: Consolas, Monospace; font-size: 14px;");

    QGroupBox* logGroup = new QGroupBox("Algorithm Log");
    QVBoxLayout* logLayout = new QVBoxLayout;
    logLayout->addWidget(m_logDisplay);
    logGroup->setLayout(logLayout);
    logGroup->setFixedWidth(350);

    QHBoxLayout* middleLayout = new QHBoxLayout;
    middleLayout->addWidget(m_canvas, 1);
    middleLayout->addWidget(logGroup, 0);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(controlPanel);
    mainLayout->addLayout(middleLayout);

    QWidget* centralWidget = new QWidget;
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);
}

void MainWindow::connectSignals()
{
    connect(m_algBox, &QComboBox::currentTextChanged, this, &MainWindow::onAlgorithmChanged);

    connect(m_startButton, &QPushButton::clicked, m_controller, &VisualizerController::onStart);
    connect(m_pauseButton, &QPushButton::clicked, m_controller, &VisualizerController::onPause);
    connect(m_stopButton, &QPushButton::clicked, m_controller, &VisualizerController::onStop);
    connect(m_prevButton, &QPushButton::clicked, m_controller, &VisualizerController::onPrevious);
    connect(m_nextButton, &QPushButton::clicked, m_controller, &VisualizerController::onNext);
    connect(m_speedSlider, &QSlider::valueChanged, m_controller, &VisualizerController::onSpeedChanged);

    // --- UPDATED LOGIC: Just Generate, Don't Start ---
    connect(m_shuffleButton, &QPushButton::clicked, this, [this](){
        m_controller->onShuffle();
        m_logDisplay->clear();
        // This reloads the algorithm (generating new graph/array) and draws Step 0
        m_controller->onAlgorithmSelected(m_algBox->currentText());
        // m_controller->onStart(); <--- REMOVED THIS
    });

    // Clear logs on stop
    connect(m_stopButton, &QPushButton::clicked, m_logDisplay, &QTextEdit::clear);

    connect(m_controller, &VisualizerController::requestRedraw, m_canvas, &AlgorithmCanvas::drawStep);
    connect(m_controller, &VisualizerController::requestRedraw, this, &MainWindow::updateLogFromStep);
    connect(m_controller, &VisualizerController::logMessage, this, &MainWindow::appendLog);

    m_controller->onSpeedChanged(m_speedSlider->value());
}

void MainWindow::onAlgorithmChanged(const QString& algName)
{
    if (algName.contains("Graph")) {
        m_shuffleButton->setText("New Random Graph");
    } else {
        m_shuffleButton->setText("Shuffle Data");
    }

    m_logDisplay->clear();
    m_controller->onAlgorithmSelected(algName);
}

void MainWindow::appendLog(const QString& message)
{
    m_logDisplay->append(message);
    m_logDisplay->verticalScrollBar()->setValue(m_logDisplay->verticalScrollBar()->maximum());
}

void MainWindow::updateLogFromStep(const QVariant& step)
{
    QString message;
    if (step.canConvert<SortingStep>()) {
        message = step.value<SortingStep>().statusMessage;
    }
    else if (step.canConvert<GraphStep>()) {
        message = step.value<GraphStep>().statusMessage;
    }

    if (!message.isEmpty()) {
        m_logDisplay->append(message);
        m_logDisplay->verticalScrollBar()->setValue(m_logDisplay->verticalScrollBar()->maximum());
    }
}
