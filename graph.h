#pragma once

#include "datastructures.h"
#include <QList>
#include <QVariant>
#include <QVector>
#include <QPair>
#include <QMap>

class Graph
{
public:
    Graph();

    QList<QVariant> generateRandomGraph(int nodeCount = 10);

    QList<QVariant> bfs(int startNodeId);
    QList<QVariant> dfs(int startNodeId);
    QList<QVariant> dijkstra(int startNodeId, int endNodeId);

    QList<QVariant> primMST(int startNodeId);

private:
    QMap<int, QList<int>> m_adjList;
    QMap<QPair<int, int>, int> m_edgeWeights;
    QMap<int, QPointF> m_nodePositions;
    int m_nodeCount;

    void dfsRecursive(int node, QSet<int>& visited, QList<QVariant>& history, QList<int>& traversalOrder);

    GraphStep createSnapshot(const QString& message, const QSet<int>& visited,
                             const QSet<int>& currentQueueStack, int currentNode = -1,
                             const QMap<int, int>& distances = {},
                             const QSet<QPair<int, int>>& pathEdges = {});
};
