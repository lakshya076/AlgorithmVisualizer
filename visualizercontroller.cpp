#include "visualizercontroller.h"
#include "sorting.h"
#include "tree.h"
#include "avl.h"   // <-- 1. INCLUDE AVL HEADER
#include <QRandomGenerator>
#include <algorithm>          // For std::random_shuffle

VisualizerController::VisualizerController(QObject *parent)
    : QObject(parent),
    m_currentStep(0),
    m_timerInterval(200)
{
    m_timer = new QTimer(this);
    m_timer->setInterval(m_timerInterval);

    connect(m_timer, &QTimer::timeout, this, &VisualizerController::onTimerTick);

    generateInitialData();
}

void VisualizerController::generateInitialData()
{
    m_initialData.clear();

    // Generate a larger set (e.g., 50) for sorting algorithms
    for(int i = 0; i < 50; ++i) {
        m_initialData.append(QRandomGenerator::global()->bounded(1, 100));
    }
}

void VisualizerController::onStart()
{
    if (!m_stepHistory.isEmpty()) {
        m_timer->start();
    }
}

void VisualizerController::onPause()
{
    m_timer->stop();
}

void VisualizerController::onStop()
{
    m_timer->stop();
    m_currentStep = 0;
    if (!m_stepHistory.isEmpty()) {
        emit requestRedraw(m_stepHistory[0]);
    }
}

void VisualizerController::onNext()
{
    m_timer->stop();
    if (m_currentStep < m_stepHistory.size() - 1) {
        m_currentStep++;
        emit requestRedraw(m_stepHistory[m_currentStep]);
    }
}

void VisualizerController::onPrevious()
{
    m_timer->stop();
    if (m_currentStep > 0) {
        m_currentStep--;
        emit requestRedraw(m_stepHistory[m_currentStep]);
    }
}

void VisualizerController::onShuffle()
{
    m_timer->stop();
    generateInitialData();
    // The main window's connection will now call
    // onAlgorithmSelected() automatically after this.
}


void VisualizerController::onAlgorithmSelected(const QString& algName)
{
    m_timer->stop();
    m_stepHistory.clear();
    m_currentStep = 0;

    // --- Sorting Algorithms ---
    if (algName.contains("Sort"))
    {
        // Use the FULL m_initialData vector
        QVector<int> dataCopy = m_initialData;

        if (algName == "Bubble Sort") {
            m_stepHistory = Sorting::bubbleSort(dataCopy);
        } else if (algName == "Insertion Sort") {
            m_stepHistory = Sorting::insertionSort(dataCopy);
        } else if (algName == "Selection Sort") {
            m_stepHistory = Sorting::selectionSort(dataCopy);
        } else if (algName == "Quick Sort") {
            m_stepHistory = Sorting::quickSort(dataCopy);
        } else if (algName == "Merge Sort") {
            m_stepHistory = Sorting::mergeSort(dataCopy);
        }
    }
    // --- Tree Algorithms ---
    else if (algName.contains("BST") || algName.contains("AVL"))
    {
        // Use a SMALLER SUBSET of the data for trees (e.g., first 15)
        int treeSize = qMin(15, m_initialData.size());
        QVector<int> treeData = m_initialData.mid(0, treeSize);

        if (algName == "BST Insert")
        {
            m_bst.clear();
            for (int value : treeData)
            {
                m_stepHistory.append(m_bst.insert(value));
            }
        }
        else if (algName == "BST Remove")
        {
            m_bst.clear();
            for (int value : treeData) {
                m_bst.insert(value);
            }

            QVector<int> removalOrder = treeData;
            std::random_shuffle(removalOrder.begin(), removalOrder.end());

            for (int value : removalOrder)
            {
                m_stepHistory.append(m_bst.remove(value));
            }
        }
        // --- 2. ADD THIS NEW BLOCK for AVL ---
        else if (algName == "AVL Insert")
        {
            m_avl.clear(); // Use the m_avl object
            for (int value : treeData)
            {
                m_stepHistory.append(m_avl.insert(value));
            }
        }
        else if (algName == "AVL Remove")
        {
            m_avl.clear(); // Use the m_avl object
            for (int value : treeData) {
                m_avl.insert(value);
            }

            QVector<int> removalOrder = treeData;
            std::random_shuffle(removalOrder.begin(), removalOrder.end());

            for (int value : removalOrder)
            {
                m_stepHistory.append(m_avl.remove(value));
            }
        }
    }

    // Show the first step
    if (!m_stepHistory.isEmpty()) {
        emit requestRedraw(m_stepHistory[0]);
    }
}

void VisualizerController::onSpeedChanged(int value)
{
    // Maps slider value (1-100) to an interval (e.g., 1000ms down to 5ms)
    m_timerInterval = 1005 - (value * 10);
    m_timer->setInterval(m_timerInterval);
}

void VisualizerController::onTimerTick()
{
    if (m_currentStep < m_stepHistory.size() - 1) {
        m_currentStep++;
        emit requestRedraw(m_stepHistory[m_currentStep]);
    } else {
        m_timer->stop(); // We're at the end
    }
}
