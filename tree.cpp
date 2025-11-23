#include "tree.h"
#include <QQueue>
#include <cmath>

const int LEVEL_HEIGHT = 80;
const int CANVAS_WIDTH = 1200;

// --- Constructor & Public Methods ---

BST::BST() : m_root(nullptr), m_nextNodeId(0) {}

void BST::clear()
{
    // A simple non-recursive way to delete all nodes
    if (!m_root) return;
    QQueue<TreeNode*> queue;
    queue.enqueue(m_root);
    while(!queue.isEmpty()) {
        TreeNode* node = queue.dequeue();
        if (node->left) queue.enqueue(node->left);
        if (node->right) queue.enqueue(node->right);
        delete node;
    }
    m_root = nullptr;
    m_nextNodeId = 0;
}

QList<QVariant> BST::insert(int value)
{
    QList<QVariant> history;

    // Add initial step
    history.append(QVariant::fromValue(createSnapshot("Inserting " + QString::number(value))));

    m_root = insertRecursive(m_root, value, 0, QPointF(CANVAS_WIDTH / 2.0, 50), nullptr, history);

    // After insertion, positions might be wrong, so update them all
    updatePositions();

    // Add a final step showing the new layout and highlighting the new node
    GraphStep finalStep = createSnapshot("Inserted " + QString::number(value));
    // We need to find the node we just inserted to highlight it.
    // (This is a simpler approach than passing the ID back up)
    QQueue<TreeNode*> queue;
    if(m_root) queue.enqueue(m_root);
    while(!queue.isEmpty()) {
        TreeNode* node = queue.dequeue();
        if(node->value == value) {
            highlightNode(finalStep, node->id, Qt::green);
            break;
        }
        if(node->left) queue.enqueue(node->left);
        if(node->right) queue.enqueue(node->right);
    }

    history.append(QVariant::fromValue(finalStep));
    return history;
}

QList<QVariant> BST::remove(int value)
{
    QList<QVariant> history;

    // Add initial step
    history.append(QVariant::fromValue(createSnapshot("Attempting to remove " + QString::number(value))));

    m_root = removeRecursive(m_root, value, history);

    // Update positions in case the structure changed significantly
    if (m_root) {
        updatePositions();
    }

    // Add final step
    history.append(QVariant::fromValue(createSnapshot("Removed " + QString::number(value))));
    return history;
}

// --- Recursive Algorithm Helpers ---

BST::TreeNode* BST::insertRecursive(TreeNode* node, int value, int level, QPointF pos, TreeNode* parent, QList<QVariant>& history)
{
    // --- 1. Base Case: Found insertion spot ---
    if (node == nullptr) {
        TreeNode* newNode = new TreeNode(value, m_nextNodeId++, level, pos);

        // Add a step showing the new node being added
        GraphStep step = createSnapshot("Found spot. Inserting " + QString::number(value));
        // Manually add the new node/edge to the snapshot for this frame
        NodeState ns;
        ns.id = newNode->id;
        ns.label = QString::number(newNode->value);
        ns.position = newNode->pos;
        ns.color = Qt::green; // Highlight new node
        step.nodes.append(ns);

        if (parent) {
            EdgeState es;
            es.fromId = parent->id;
            es.toId = newNode->id;
            es.color = Qt::green; // Highlight new edge
            step.edges.append(es);
        }
        history.append(QVariant::fromValue(step));
        return newNode;
    }

    // --- 2. Recursive Step: Traverse down ---

    // Add "visiting" step
    GraphStep visitStep = createSnapshot("Comparing with " + QString::number(node->value));
    highlightNode(visitStep, node->id, Qt::yellow);
    history.append(QVariant::fromValue(visitStep));

    if (value < node->value) {
        double xOffset = CANVAS_WIDTH / std::pow(2, level + 2);
        node->left = insertRecursive(node->left, value, level + 1, QPointF(node->pos.x() - xOffset, node->pos.y() + LEVEL_HEIGHT), node, history);
    } else if (value > node->value) {
        double xOffset = CANVAS_WIDTH / std::pow(2, level + 2);
        node->right = insertRecursive(node->right, value, level + 1, QPointF(node->pos.x() + xOffset, node->pos.y() + LEVEL_HEIGHT), node, history);
    } else {
        // Value already exists
        GraphStep existStep = createSnapshot("Value " + QString::number(value) + " already exists.");
        highlightNode(existStep, node->id, Qt::red);
        history.append(QVariant::fromValue(existStep));
        return node;
    }

    return node;
}

BST::TreeNode* BST::removeRecursive(TreeNode* node, int value, QList<QVariant>& history)
{
    // --- 1. Base Case: Node not found ---
    if (node == nullptr) {
        history.append(QVariant::fromValue(createSnapshot("Value " + QString::number(value) + " not found.")));
        return nullptr;
    }

    // --- 2. Recursive Step: Traverse down ---
    GraphStep visitStep = createSnapshot("Comparing with " + QString::number(node->value));
    highlightNode(visitStep, node->id, Qt::yellow);
    history.append(QVariant::fromValue(visitStep));

    if (value < node->value) {
        node->left = removeRecursive(node->left, value, history);
    } else if (value > node->value) {
        node->right = removeRecursive(node->right, value, history);

        // --- 3. Found Node to Delete ---
    } else {
        GraphStep foundStep = createSnapshot("Found node " + QString::number(node->value));
        highlightNode(foundStep, node->id, Qt::red);
        history.append(QVariant::fromValue(foundStep));

        // Case 1: Node with 0 or 1 child
        if (node->left == nullptr) {
            TreeNode* temp = node->right;
            delete node;
            return temp;
        } else if (node->right == nullptr) {
            TreeNode* temp = node->left;
            delete node;
            return temp;
        }

        // Case 2: Node with 2 children
        // Find inorder successor (smallest in the right subtree)
        GraphStep succStep = createSnapshot("Node has two children. Finding inorder successor...");
        highlightNode(succStep, node->id, Qt::red);
        history.append(QVariant::fromValue(succStep));

        TreeNode* temp = findMin(node->right);

        // Add step showing successor
        GraphStep foundSuccStep = createSnapshot("Found successor: " + QString::number(temp->value));
        highlightNode(foundSuccStep, temp->id, Qt::green);
        history.append(QVariant::fromValue(foundSuccStep));

        // Copy successor's value to this node
        node->value = temp->value;

        // Add step showing value copy
        GraphStep copyStep = createSnapshot("Copying value " + QString::number(temp->value) + " to node");
        highlightNode(copyStep, node->id, Qt::green);
        highlightNode(copyStep, temp->id, Qt::yellow);
        history.append(QVariant::fromValue(copyStep));

        // Recursively delete the successor
        node->right = removeRecursive(node->right, temp->value, history);
    }
    return node;
}

BST::TreeNode* BST::findMin(TreeNode* node)
{
    while (node && node->left != nullptr) {
        node = node->left;
    }
    return node;
}


// --- Visualization & Layout Helpers ---

void BST::updatePositions()
{
    if (!m_root) return;
    updatePositionsRecursive(m_root, 0, CANVAS_WIDTH / 2.0, CANVAS_WIDTH / 4.0);
}

void BST::updatePositionsRecursive(TreeNode* node, int level, double x, double xOffset)
{
    if (node == nullptr) {
        return;
    }
    node->pos = QPointF(x, 50 + level * LEVEL_HEIGHT);
    node->level = level;

    updatePositionsRecursive(node->left, level + 1, x - xOffset, xOffset / 2.0);
    updatePositionsRecursive(node->right, level + 1, x + xOffset, xOffset / 2.0);
}

GraphStep BST::createSnapshot(const QString& message)
{
    GraphStep step;
    step.statusMessage = message;
    if (m_root) {
        populateSnapshot(m_root, step);
    }
    return step;
}

void BST::populateSnapshot(TreeNode* node, GraphStep& step)
{
    if (node == nullptr) {
        return;
    }

    // 1. Add this node
    NodeState ns;
    ns.id = node->id;
    ns.label = QString::number(node->value);
    ns.position = node->pos;
    step.nodes.append(ns);

    // 2. Add edge to left child
    if (node->left) {
        EdgeState es;
        es.fromId = node->id;
        es.toId = node->left->id;
        step.edges.append(es);
        populateSnapshot(node->left, step); // Recurse
    }

    // 3. Add edge to right child
    if (node->right) {
        EdgeState es;
        es.fromId = node->id;
        es.toId = node->right->id;
        step.edges.append(es);
        populateSnapshot(node->right, step); // Recurse
    }
}

void BST::highlightNode(GraphStep& step, int nodeId, QColor color)
{
    for (NodeState& ns : step.nodes) {
        if (ns.id == nodeId) {
            ns.color = color;
            return;
        }
    }
}
