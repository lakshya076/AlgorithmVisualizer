#include "mainwindow.h"
#include "visualizercontroller.h"
#include "algorithmcanvas.h"

// Include all necessary widget headers
#include <QComboBox>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_controller = new VisualizerController(this);
    m_canvas = new AlgorithmCanvas(this);

    setupUI();
    connectSignals();

    // Trigger the controller to load the default algorithm's steps
    m_controller->onAlgorithmSelected(m_algBox->currentText());

    resize(1200, 800);
    setWindowTitle("Algorithm Visualizer");
}

MainWindow::~MainWindow()
{
    // QObject parent-child relationships handle memory deletion
}

void MainWindow::setupUI()
{
    // 1. Create all widgets
    m_algBox = new QComboBox;

    // --- UPDATE THIS BLOCK ---
    m_algBox->addItems({
        "Bubble Sort",
        "Insertion Sort",
        "Selection Sort",
        "Quick Sort",
        "Merge Sort",
        "BST Insert",
        "BST Remove",
        "AVL Insert", // <-- ADD THIS
        "AVL Remove"  // <-- ADD THIS
    });
    // -------------------------

    m_shuffleButton = new QPushButton("Shuffle / New Data");
    m_startButton = new QPushButton("Start");
    m_pauseButton = new QPushButton("Pause");
    m_stopButton = new QPushButton("Stop");
    m_prevButton = new QPushButton("<< Prev");
    m_nextButton = new QPushButton("Next >>");

    m_speedLabel = new QLabel("Speed:");
    m_speedSlider = new QSlider(Qt::Horizontal);
    m_speedSlider->setRange(1, 100); // 1 (slow) to 100 (fast)
    m_speedSlider->setValue(80); // Default speed

    // 2. Create control panel layout
    QHBoxLayout* controlLayout = new QHBoxLayout;
    controlLayout->addWidget(m_algBox);
    controlLayout->addWidget(m_shuffleButton);
    controlLayout->addSpacing(20);
    controlLayout->addWidget(m_startButton);
    controlLayout->addWidget(m_pauseButton);
    controlLayout->addWidget(m_stopButton);
    controlLayout->addWidget(m_prevButton);
    controlLayout->addWidget(m_nextButton);
    controlLayout->addStretch(); // Pushes speed slider to the right
    controlLayout->addWidget(m_speedLabel);
    controlLayout->addWidget(m_speedSlider);

    QWidget* controlPanel = new QWidget;
    controlPanel->setLayout(controlLayout);
    controlPanel->setFixedHeight(50); // Give the control panel a fixed height

    // 3. Create main layout
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(controlPanel);
    mainLayout->addWidget(m_canvas);
    mainLayout->setStretchFactor(m_canvas, 1); // Makes canvas fill space

    // 4. Set central widget
    QWidget* centralWidget = new QWidget;
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);
}

void MainWindow::connectSignals()
{
    // Connect UI controls to the Controller's slots
    connect(m_algBox, &QComboBox::currentTextChanged, m_controller, &VisualizerController::onAlgorithmSelected);
    connect(m_shuffleButton, &QPushButton::clicked, m_controller, &VisualizerController::onShuffle);
    connect(m_startButton, &QPushButton::clicked, m_controller, &VisualizerController::onStart);
    connect(m_pauseButton, &QPushButton::clicked, m_controller, &VisualizerController::onPause);
    connect(m_stopButton, &QPushButton::clicked, m_controller, &VisualizerController::onStop);
    connect(m_prevButton, &QPushButton::clicked, m_controller, &VisualizerController::onPrevious);
    connect(m_nextButton, &QPushButton::clicked, m_controller, &VisualizerController::onNext);
    connect(m_speedSlider, &QSlider::valueChanged, m_controller, &VisualizerController::onSpeedChanged);

    // Special case: When shuffle is clicked, it must re-trigger the algorithm selection
    connect(m_shuffleButton, &QPushButton::clicked, this, [this](){
        m_controller->onAlgorithmSelected(m_algBox->currentText());
    });

    // Connect the Controller's signal to the Canvas's slot
    connect(m_controller, &VisualizerController::requestRedraw, m_canvas, &AlgorithmCanvas::drawStep);

    // Trigger the controller to update the speed on startup
    m_controller->onSpeedChanged(m_speedSlider->value());
}
