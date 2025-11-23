#pragma once

#include "datastructures.h"
#include <QList>
#include <QVariant>
#include <QMap>

class BST
{
public:
    BST();

    QList<QVariant> insert(int value);
    QList<QVariant> remove(int value);
    void clear();

private:
    // --- Core Tree Structure ---
    struct TreeNode {
        int value;
        TreeNode *left;
        TreeNode *right;

        int id;
        QPointF pos;
        int level;

        TreeNode(int v, int i, int lvl, QPointF p)
            : value(v), left(nullptr), right(nullptr), id(i), pos(p), level(lvl) {}
    };

    TreeNode* m_root;
    int m_nextNodeId;

    // --- Recursive Algorithm Helpers ---

    TreeNode* insertRecursive(TreeNode* node, int value, int level, QPointF pos, TreeNode* parent, QList<QVariant>& history);
    TreeNode* removeRecursive(TreeNode* node, int value, QList<QVariant>& history);
    TreeNode* findMin(TreeNode* node);

    // --- Visualization & Layout Helpers ---

    GraphStep createSnapshot(const QString& message);
    void populateSnapshot(TreeNode* node, GraphStep& step);

    void updatePositions();
    void updatePositionsRecursive(TreeNode* node, int level, double x, double xOffset);

    // Highlights a single node in a snapshot
    void highlightNode(GraphStep& step, int nodeId, QColor color);

    // Highlights a node and its connection to its parent
    void highlightEdge(GraphStep& step, TreeNode* node, TreeNode* parent, QColor color);
};
