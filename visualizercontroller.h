#pragma once

#include <QObject>
#include <QTimer>
#include <QList>
#include <QVariant>
#include <QVector>
#include "tree.h" // Our BST
#include "avl.h"  // <-- 1. INCLUDE THE AVL HEADER

class VisualizerController : public QObject
{
    Q_OBJECT

public:
    explicit VisualizerController(QObject *parent = nullptr);

signals:
    // Signal sent to the canvas to draw a new step
    void requestRedraw(const QVariant& step);

public slots:
    // Slots connected to UI buttons
    void onStart();
    void onPause();
    void onStop();
    void onNext();
    void onPrevious();
    void onShuffle();

    // Slot connected to the algorithm dropdown
    void onAlgorithmSelected(const QString& algName);

    // Slot connected to the speed slider
    void onSpeedChanged(int value);

private slots:
    // Slot connected to the timer's timeout
    void onTimerTick();

private:
    QTimer* m_timer;
    QList<QVariant> m_stepHistory;
    int m_currentStep;
    int m_timerInterval;
    QVector<int> m_initialData; // Stores the shuffled data

    BST m_bst; // Our BST object
    AVL m_avl; // <-- 2. ADD THE AVL OBJECT

    void generateInitialData();
};
