#include "graph.h"
#include <cmath>
#include <QQueue>
#include <QSet>
#include <QRandomGenerator>
#include <limits>
#include <queue> // std::priority_queue
#include <algorithm> // std::min, std::max

const int CANVAS_WIDTH = 1200;
const int CANVAS_HEIGHT = 700;

Graph::Graph() : m_nodeCount(0) {}

QList<QVariant> Graph::generateRandomGraph(int nodeCount)
{
    m_adjList.clear();
    m_nodePositions.clear();
    m_edgeWeights.clear();
    m_nodeCount = nodeCount;

    QList<QVariant> history;

    // 1. Position Nodes in a Grid
    double aspectRatio = (double)CANVAS_WIDTH / CANVAS_HEIGHT;
    int cols = std::ceil(std::sqrt(nodeCount * aspectRatio));
    int rows = std::ceil((double)nodeCount / cols);

    double hSpacing = CANVAS_WIDTH / (cols + 1);
    double vSpacing = CANVAS_HEIGHT / (rows + 1);

    for (int i = 0; i < nodeCount; ++i) {
        int r = i / cols;
        int c = i % cols;

        double x = hSpacing * (c + 1);
        double y = vSpacing * (r + 1);
        m_nodePositions[i] = QPointF(x, y);
    }

    // 2. Generate Edges (Locally Only)
    for (int i = 0; i < nodeCount; ++i) {
        int r1 = i / cols;
        int c1 = i % cols;

        for (int j = i + 1; j < nodeCount; ++j) {
            int r2 = j / cols;
            int c2 = j % cols;

            int distR = std::abs(r1 - r2);
            int distC = std::abs(c1 - c2);

            bool connected = false;

            if ((distR == 0 && distC == 1) || (distR == 1 && distC == 0)) {
                if (QRandomGenerator::global()->bounded(100) < 80) connected = true;
            }
            else if (distR == 1 && distC == 1) {
                if (QRandomGenerator::global()->bounded(100) < 30) connected = true;
            }

            if (connected) {
                m_adjList[i].append(j);
                m_adjList[j].append(i);
                m_edgeWeights[{std::min(i,j), std::max(i,j)}] = QRandomGenerator::global()->bounded(1, 10);
            }
        }
    }

    // 3. Ensure Connectivity
    for(int i = 0; i < nodeCount; ++i) {
        if (!m_adjList.contains(i) || m_adjList[i].isEmpty()) {
            int neighbor = (i > 0) ? i - 1 : i + 1;
            if (neighbor < nodeCount) {
                m_adjList[i].append(neighbor);
                m_adjList[neighbor].append(i);
                m_edgeWeights[{std::min(i,neighbor), std::max(i,neighbor)}] = QRandomGenerator::global()->bounded(1, 10);
            }
        }
    }

    history.append(QVariant::fromValue(createSnapshot("Generated Weighted Grid Graph with " + QString::number(nodeCount) + " nodes.", {}, {})));
    return history;
}

QList<QVariant> Graph::bfs(int startNodeId)
{
    QList<QVariant> history;
    QSet<int> visited;
    QQueue<int> queue;
    QList<int> traversalOrder;

    history.append(QVariant::fromValue(createSnapshot("Starting BFS from Node " + QString::number(startNodeId), visited, {})));

    visited.insert(startNodeId);
    queue.enqueue(startNodeId);

    while (!queue.isEmpty()) {
        int u = queue.dequeue();
        traversalOrder.append(u);

        QSet<int> queueSet;
        for(int x : queue) queueSet.insert(x);
        history.append(QVariant::fromValue(createSnapshot("Visiting Node " + QString::number(u), visited, queueSet, u)));

        const QList<int>& neighbors = m_adjList[u];
        for (int v : neighbors) {
            if (!visited.contains(v)) {
                visited.insert(v);
                queue.enqueue(v);
                queueSet.insert(v);
                history.append(QVariant::fromValue(createSnapshot("Found unvisited neighbor " + QString::number(v), visited, queueSet, u)));
            }
        }
    }

    QString pathStr;
    for (int i = 0; i < traversalOrder.size(); ++i) {
        pathStr += QString::number(traversalOrder[i]);
        if (i < traversalOrder.size() - 1) pathStr += " -> ";
    }

    history.append(QVariant::fromValue(createSnapshot("BFS Complete. Order: " + pathStr, visited, {})));
    return history;
}

QList<QVariant> Graph::dfs(int startNodeId)
{
    QList<QVariant> history;
    QSet<int> visited;
    QList<int> traversalOrder;

    history.append(QVariant::fromValue(createSnapshot("Starting DFS from Node " + QString::number(startNodeId), visited, {})));
    dfsRecursive(startNodeId, visited, history, traversalOrder);

    QString pathStr;
    for (int i = 0; i < traversalOrder.size(); ++i) {
        pathStr += QString::number(traversalOrder[i]);
        if (i < traversalOrder.size() - 1) pathStr += " -> ";
    }

    history.append(QVariant::fromValue(createSnapshot("DFS Complete. Order: " + pathStr, visited, {})));
    return history;
}

void Graph::dfsRecursive(int u, QSet<int>& visited, QList<QVariant>& history, QList<int>& traversalOrder)
{
    visited.insert(u);
    traversalOrder.append(u);
    history.append(QVariant::fromValue(createSnapshot("Visiting Node " + QString::number(u), visited, {}, u)));

    const QList<int>& neighbors = m_adjList[u];
    for (int v : neighbors) {
        if (!visited.contains(v)) {
            history.append(QVariant::fromValue(createSnapshot("Moving to unvisited neighbor " + QString::number(v), visited, {}, u)));
            dfsRecursive(v, visited, history, traversalOrder);
            history.append(QVariant::fromValue(createSnapshot("Backtracked to Node " + QString::number(u), visited, {}, u)));
        }
    }
}

QList<QVariant> Graph::dijkstra(int startNodeId, int endNodeId)
{
    QList<QVariant> history;
    QSet<int> visited;
    QMap<int, int> dist;
    QMap<int, int> parent;

    for(int i=0; i<m_nodeCount; ++i) dist[i] = 9999;
    dist[startNodeId] = 0;

    std::priority_queue<QPair<int, int>> pq;
    pq.push({0, startNodeId});

    history.append(QVariant::fromValue(createSnapshot("Finding shortest path from " + QString::number(startNodeId) + " to " + QString::number(endNodeId), visited, {}, startNodeId, dist)));

    bool found = false;

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        if (visited.contains(u)) continue;
        visited.insert(u);

        if (u == endNodeId) {
            found = true;
            history.append(QVariant::fromValue(createSnapshot("Target Node " + QString::number(u) + " Reached!", visited, {}, u, dist)));
            break;
        }

        history.append(QVariant::fromValue(createSnapshot("Processing Node " + QString::number(u), visited, {}, u, dist)));

        const QList<int>& neighbors = m_adjList[u];
        for (int v : neighbors) {
            if (visited.contains(v)) continue;

            int weight = m_edgeWeights[{std::min(u,v), std::max(u,v)}];
            int newDist = dist[u] + weight;

            if (newDist < dist[v]) {
                dist[v] = newDist;
                parent[v] = u;
                pq.push({-newDist, v});

                history.append(QVariant::fromValue(createSnapshot("Relaxing Edge " + QString::number(u) + "->" + QString::number(v), visited, {}, u, dist)));
            }
        }
    }

    if (found) {
        QSet<QPair<int, int>> pathEdges;
        int curr = endNodeId;
        QString pathStr = QString::number(curr);

        while (curr != startNodeId) {
            int prev = parent[curr];
            pathEdges.insert({std::min(prev, curr), std::max(prev, curr)});
            pathStr.prepend(QString::number(prev) + " -> ");
            curr = prev;
        }

        history.append(QVariant::fromValue(createSnapshot("Shortest Path Found: " + pathStr, visited, {}, -1, dist, pathEdges)));
    } else {
        history.append(QVariant::fromValue(createSnapshot("Target Node " + QString::number(endNodeId) + " is unreachable!", visited, {}, -1, dist)));
    }

    return history;
}

QList<QVariant> Graph::primMST(int startNodeId)
{
    QList<QVariant> history;
    QSet<int> visited;
    QMap<int, int> minEdgeWeight;
    QMap<int, int> parent;
    QSet<QPair<int, int>> mstEdges;

    for(int i=0; i<m_nodeCount; ++i) minEdgeWeight[i] = 9999;
    minEdgeWeight[startNodeId] = 0;

    std::priority_queue<QPair<int, int>> pq;
    pq.push({0, startNodeId});

    history.append(QVariant::fromValue(createSnapshot("Starting Prim's MST from Node " + QString::number(startNodeId), visited, {}, startNodeId)));

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        if (visited.contains(u)) continue;
        visited.insert(u);

        if (parent.contains(u)) {
            int p = parent[u];
            mstEdges.insert({std::min(p, u), std::max(p, u)});
            history.append(QVariant::fromValue(createSnapshot("Added Edge " + QString::number(p) + "-" + QString::number(u) + " to MST.", visited, {}, u, {}, mstEdges)));
        } else {
            history.append(QVariant::fromValue(createSnapshot("Processing Node " + QString::number(u), visited, {}, u, {}, mstEdges)));
        }

        const QList<int>& neighbors = m_adjList[u];
        for (int v : neighbors) {
            if (visited.contains(v)) continue;

            int weight = m_edgeWeights[{std::min(u,v), std::max(u,v)}];

            if (weight < minEdgeWeight[v]) {
                minEdgeWeight[v] = weight;
                parent[v] = u;
                pq.push({-weight, v});
            }
        }
    }

    int totalWeight = 0;
    for(int i=0; i<m_nodeCount; ++i) if(minEdgeWeight[i] != 9999) totalWeight += minEdgeWeight[i];

    history.append(QVariant::fromValue(createSnapshot("MST Complete. Total Weight: " + QString::number(totalWeight), visited, {}, -1, {}, mstEdges)));
    return history;
}

GraphStep Graph::createSnapshot(const QString& message, const QSet<int>& visited,
                                const QSet<int>& currentQueueStack, int currentNode,
                                const QMap<int, int>& distances,
                                const QSet<QPair<int, int>>& pathEdges)
{
    GraphStep step;
    step.statusMessage = message;

    // Create Nodes
    for (int i = 0; i < m_nodeCount; ++i) {
        NodeState ns;
        ns.id = i;
        ns.position = m_nodePositions[i];

        // --- FIXED: Always show only the ID ---
        ns.label = QString::number(i);
        // --------------------------------------

        if (i == currentNode) {
            ns.color = Qt::red;
        } else if (currentQueueStack.contains(i)) {
            ns.color = Qt::yellow;
        } else if (visited.contains(i)) {
            ns.color = Qt::green;
        } else {
            ns.color = Qt::white;
        }

        ns.textColor = Qt::black;
        step.nodes.append(ns);
    }

    // Create Edges
    for (auto it = m_adjList.begin(); it != m_adjList.end(); ++it) {
        int u = it.key();
        const QList<int>& neighbors = it.value();
        for (int v : neighbors) {
            if (u < v) {
                EdgeState es;
                es.fromId = u;
                es.toId = v;

                int w = m_edgeWeights[{u, v}];
                es.weightLabel = QString::number(w);

                // Check for Path/MST edges
                if (pathEdges.contains({u, v})) {
                    // Use Magenta for Dijkstra path, Orange for MST
                    // Since we pass the same set, we can differentiate by context
                    // or just use one highlight color. Let's use Orange if it looks like MST (many edges)
                    // or Magenta if it looks like a Path.
                    // Simpler: Just use Magenta for both, or Orange.
                    // Let's use Orange as it stands out well on black.
                    es.color = QColor(255, 165, 0);
                }
                else if (visited.contains(u) && visited.contains(v)) {
                    es.color = Qt::lightGray;
                } else {
                    es.color = Qt::white;
                }
                step.edges.append(es);
            }
        }
    }

    return step;
}
