#pragma once

#include "datastructures.h"
#include <QList>
#include <QVariant>
#include <QMap>

class AVL
{
public:
    AVL();

    // Public methods that generate and return a step-by-step history
    QList<QVariant> insert(int value);
    QList<QVariant> remove(int value);
    void clear(); // Resets the tree

private:
    // --- Core Tree Structure ---
    struct TreeNode {
        int value;
        TreeNode *left;
        TreeNode *right;
        int height;     // <-- AVL addition

        int id;         // Unique ID for visualization
        QPointF pos;    // Position on canvas
        int level;      // Tree level (0 for root)

        TreeNode(int v, int i, int lvl, QPointF p)
            : value(v), left(nullptr), right(nullptr), height(1), // Height of a new leaf is 1
            id(i), pos(p), level(lvl) {}
    };

    TreeNode* m_root;
    int m_nextNodeId;

    // --- Recursive Algorithm Helpers ---

    // *** CORRECTED SIGNATURE ***: Removed pos and parent
    TreeNode* insertRecursive(TreeNode* node, int value, int level, QList<QVariant>& history);

    TreeNode* removeRecursive(TreeNode* node, int value, QList<QVariant>& history);
    TreeNode* findMin(TreeNode* node);

    // --- AVL Balancing Helpers ---
    int getHeight(TreeNode* node);
    int getBalanceFactor(TreeNode* node);
    void updateHeight(TreeNode* node);
    TreeNode* rightRotate(TreeNode* y, QList<QVariant>& history);
    TreeNode* leftRotate(TreeNode* x, QList<QVariant>& history);

    // *** CORRECTED SIGNATURE ***: Removed value
    TreeNode* balance(TreeNode* node, QList<QVariant>& history);

    TreeNode* balanceForRemove(TreeNode* node, QList<QVariant>& history); // Helper for balancing on remove


    // --- Visualization & Layout Helpers ---
    GraphStep createSnapshot(const QString& message);
    void populateSnapshot(TreeNode* node, GraphStep& step);
    void updatePositions();
    void updatePositionsRecursive(TreeNode* node, int level, double x, double xOffset);
    void highlightNode(GraphStep& step, int nodeId, QColor color, QColor textColor = Qt::black);
};
