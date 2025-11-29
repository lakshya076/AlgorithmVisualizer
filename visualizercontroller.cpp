#include "visualizercontroller.h"
#include "sorting.h"
#include "tree.h"
#include "avl.h"
#include "graph.h"
#include <QRandomGenerator>
#include <algorithm>
#include <QStringList>
#include <random>

VisualizerController::VisualizerController(QObject *parent)
    : QObject(parent),
    m_currentStep(0),
    m_timerInterval(200),
    m_graphGenerated(false)
{
    m_timer = new QTimer(this);
    m_timer->setInterval(m_timerInterval);
    connect(m_timer, &QTimer::timeout, this, &VisualizerController::onTimerTick);
    generateRandomData();
}

void VisualizerController::generateRandomData()
{
    m_randomData.clear();
    for(int i = 0; i < 50; ++i) {
        m_randomData.append(QRandomGenerator::global()->bounded(1, 100));
    }
}

void VisualizerController::onShuffle()
{
    m_timer->stop();
    generateRandomData();
    m_graphGenerated = false;
}

void VisualizerController::onStart() { if (!m_stepHistory.isEmpty()) m_timer->start(); }
void VisualizerController::onPause() { m_timer->stop(); }
void VisualizerController::onStop() {
    m_timer->stop();
    m_currentStep = 0;
    if (!m_stepHistory.isEmpty()) emit requestRedraw(m_stepHistory[0]);
}
void VisualizerController::onNext() {
    m_timer->stop();
    if (m_currentStep < m_stepHistory.size() - 1) {
        m_currentStep++;
        emit requestRedraw(m_stepHistory[m_currentStep]);
    }
}
void VisualizerController::onPrevious() {
    m_timer->stop();
    if (m_currentStep > 0) {
        m_currentStep--;
        emit requestRedraw(m_stepHistory[m_currentStep]);
    }
}

void VisualizerController::onAlgorithmSelected(const QString& algName)
{
    m_timer->stop();
    m_stepHistory.clear();
    m_currentStep = 0;

    // --- SORTING ---
    if (algName.contains("Sort"))
    {
        QVector<int> dataCopy = m_randomData;
        if (algName == "Bubble Sort") m_stepHistory = Sorting::bubbleSort(dataCopy);
        else if (algName == "Insertion Sort") m_stepHistory = Sorting::insertionSort(dataCopy);
        else if (algName == "Selection Sort") m_stepHistory = Sorting::selectionSort(dataCopy);
        else if (algName == "Quick Sort") m_stepHistory = Sorting::quickSort(dataCopy);
        else if (algName == "Merge Sort") m_stepHistory = Sorting::mergeSort(dataCopy);
    }
    // --- TREES ---
    else if (algName.contains("BST") || algName.contains("AVL"))
    {
        int treeSize = qMin(15, m_randomData.size());
        QVector<int> treeData = m_randomData.mid(0, treeSize);

        // Prepare a random engine for shuffling
        std::random_device rd;
        std::mt19937 g(rd());

        if (algName == "BST Insert") {
            m_bst.clear();
            for(int v : treeData) m_stepHistory.append(m_bst.insert(v));
        }
        else if (algName == "BST Remove") {
            m_bst.clear();
            for(int v : treeData) m_bst.insert(v);
            QVector<int> removalOrder = treeData;

            std::shuffle(removalOrder.begin(), removalOrder.end(), g);

            for (int v : removalOrder) m_stepHistory.append(m_bst.remove(v));
        }
        else if (algName == "AVL Insert") {
            m_avl.clear();
            for(int v : treeData) m_stepHistory.append(m_avl.insert(v));
        }
        else if (algName == "AVL Remove") {
            m_avl.clear();
            for(int v : treeData) m_avl.insert(v);
            QVector<int> removalOrder = treeData;

            std::shuffle(removalOrder.begin(), removalOrder.end(), g);

            for (int v : removalOrder) m_stepHistory.append(m_avl.remove(v));
        }
    }

    // --- GRAPHS ---
    else if (algName.contains("Graph"))
    {
        int nodeCount = 50;

        if (algName == "Graph Generate") {
            m_stepHistory = m_graph.generateRandomGraph(nodeCount);
            m_graphGenerated = true;
        }
        else {
            if (!m_graphGenerated) {
                m_graph.generateRandomGraph(nodeCount);
                m_graphGenerated = true;
            }

            int startNode = QRandomGenerator::global()->bounded(0, nodeCount);

            if (algName == "Graph BFS") {
                m_stepHistory = m_graph.bfs(startNode);
            }
            else if (algName == "Graph DFS") {
                m_stepHistory = m_graph.dfs(startNode);
            }
            else if (algName == "Graph Dijkstra") {
                int endNode = startNode;
                while(endNode == startNode) {
                    endNode = QRandomGenerator::global()->bounded(0, nodeCount);
                }

                emit logMessage("--------------------------------");
                emit logMessage("Goal: Dijkstra's Pathfinding");
                emit logMessage("Start Node: " + QString::number(startNode));
                emit logMessage("Target Node: " + QString::number(endNode));
                emit logMessage("--------------------------------");

                m_stepHistory = m_graph.dijkstra(startNode, endNode);
            }
            else if (algName == "Graph Prim's MST") {
                emit logMessage("--------------------------------");
                emit logMessage("Goal: Prim's Minimum Spanning Tree");
                emit logMessage("Start Node: " + QString::number(startNode));
                emit logMessage("--------------------------------");

                m_stepHistory = m_graph.primMST(startNode);
            }
        }
    }
    // --- MAZE ---
    else if (algName == "Maze Generate") {
        emit logMessage("--------------------------------");
        emit logMessage("Goal: Recursive Backtracker Maze");
        emit logMessage("Size: 41 x 25");
        emit logMessage("--------------------------------");

        m_stepHistory = m_maze.generateRecursiveBacktracker(41, 25);
    }

    if (!m_stepHistory.isEmpty()) {
        emit requestRedraw(m_stepHistory[0]);
    }
}

void VisualizerController::onSpeedChanged(int value) {
    m_timerInterval = 1005 - (value * 10);
    m_timer->setInterval(m_timerInterval);
}
void VisualizerController::onTimerTick() {
    if (m_currentStep < m_stepHistory.size() - 1) {
        m_currentStep++;
        emit requestRedraw(m_stepHistory[m_currentStep]);
    } else {
        m_timer->stop();
    }
}
