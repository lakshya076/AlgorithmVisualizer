#pragma once

#include <QObject>
#include <QTimer>
#include <QList>
#include <QVariant>
#include <QVector>
#include "tree.h"
#include "avl.h"
#include "graph.h"
#include "maze.h"

class VisualizerController : public QObject
{
    Q_OBJECT

public:
    explicit VisualizerController(QObject *parent = nullptr);

signals:
    void requestRedraw(const QVariant& step);
    void logMessage(const QString& message);

public slots:
    void onStart();
    void onPause();
    void onStop();
    void onNext();
    void onPrevious();

    void onShuffle();

    void onAlgorithmSelected(const QString& algName);
    void onSpeedChanged(int value);

private slots:
    void onTimerTick();

private:
    QTimer* m_timer;
    QList<QVariant> m_stepHistory;
    int m_currentStep;
    int m_timerInterval;

    QVector<int> m_randomData; // Shared by Sorting and Trees

    BST m_bst;
    AVL m_avl;
    Graph m_graph;
    Maze m_maze;

    bool m_graphGenerated;

    void generateRandomData();
};
