#pragma once

#include <QObject>
#include <QVector>
#include <QList>
#include <QString>
#include <QVariant>
#include <QColor>
#include <QPointF>

// Step definition for Sorting
struct SortingStep {
    QVector<int> data;
    QString statusMessage;

    QList<int> compareIndices;
    QList<int> pivotIndex;
    QList<int> swapIndices;
    QList<int> sortedIndices;
};

struct NodeState {
    int id;
    QString label;
    QPointF position;
    QColor color;
    QColor textColor;

    NodeState() : id(-1), label(""), position(0,0), color(Qt::white), textColor(Qt::black) {}
};

struct EdgeState {
    int fromId;
    int toId;
    QColor color;

    EdgeState() : fromId(-1), toId(-1), color(Qt::white) {}
};

struct GraphStep {
    QList<NodeState> nodes;
    QList<EdgeState> edges;
    QString statusMessage;
};

Q_DECLARE_METATYPE(SortingStep)
Q_DECLARE_METATYPE(GraphStep)

// We will add GraphStep and MazeStep here later
