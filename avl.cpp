#include "avl.h"
#include <QQueue>
#include <cmath>     // For std::pow
#include <algorithm> // For std::max

// Define constants for layout
const int LEVEL_HEIGHT = 80;
const int CANVAS_WIDTH = 1200; // Assume a default width for layout

// --- Constructor & Public Methods ---

AVL::AVL() : m_root(nullptr), m_nextNodeId(0) {}

void AVL::clear()
{
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

// --- UPDATED insert ---
QList<QVariant> AVL::insert(int value)
{
    QList<QVariant> history;
    history.append(QVariant::fromValue(createSnapshot("Inserting " + QString::number(value))));

    // 1. Call recursive insert. Positions will be wrong inside here.
    m_root = insertRecursive(m_root, value, 0, history);

    // 2. NOW, recalculate all positions for the entire tree
    updatePositions();

    // 3. Add a final snapshot with the correct layout
    GraphStep finalStep = createSnapshot("Inserted " + QString::number(value) + ". Balancing complete.");

    // Find the new node to highlight it
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

QList<QVariant> AVL::remove(int value)
{
    QList<QVariant> history;
    history.append(QVariant::fromValue(createSnapshot("Attempting to remove " + QString::number(value))));
    m_root = removeRecursive(m_root, value, history);

    if (m_root) {
        // Recalculate all positions after deletion/rotation
        updatePositions();
        history.append(QVariant::fromValue(createSnapshot("Removed " + QString::number(value) + ". Final check.")));
    } else {
        history.append(QVariant::fromValue(createSnapshot("Removed " + QString::number(value) + ". Tree is empty.")));
    }
    return history;
}

// --- Recursive Algorithm Helpers ---

// --- UPDATED insertRecursive (simplified) ---
AVL::TreeNode* AVL::insertRecursive(AVL::TreeNode* node, int value, int level, QList<QVariant>& history)
{
    // --- 1. Standard BST Insert ---
    if (node == nullptr) {
        // Create new node. Note: QPointF() is just (0,0) - it's temporary.
        TreeNode* newNode = new TreeNode(value, m_nextNodeId++, level, QPointF());

        GraphStep step = createSnapshot("Found spot. Inserting " + QString::number(value));
        // We add the node/edge manually, but its position will be wrong in this frame.
        // This is OK, as the *next* step will show it corrected.
        NodeState ns;
        ns.id = newNode->id;
        ns.label = QString::number(newNode->value);
        ns.position = QPointF(50, 50); // Just put it in the corner for this one frame
        ns.color = Qt::green;
        step.nodes.append(ns);

        history.append(QVariant::fromValue(step));
        return newNode;
    }

    // Add "visiting" step (positions here might be stale, but that's ok)
    GraphStep visitStep = createSnapshot("Comparing with " + QString::number(node->value));
    highlightNode(visitStep, node->id, Qt::yellow);
    history.append(QVariant::fromValue(visitStep));

    if (value < node->value) {
        node->left = insertRecursive(node->left, value, level + 1, history);
    } else if (value > node->value) {
        node->right = insertRecursive(node->right, value, level + 1, history);
    } else {
        GraphStep existStep = createSnapshot("Value " + QString::number(value) + " already exists.");
        highlightNode(existStep, node->id, Qt::red);
        history.append(QVariant::fromValue(existStep));
        return node;
    }

    // --- 2. AVL Balancing ---
    updateHeight(node);
    // Note: We no longer pass 'value' to balance(), it's not needed
    return balance(node, history);
}

// --- UPDATED balance (simplified) ---
AVL::TreeNode* AVL::balance(AVL::TreeNode* node, QList<QVariant>& history)
{
    int balanceFactor = getBalanceFactor(node);

    GraphStep checkStep = createSnapshot("Checking balance at node " + QString::number(node->value) + ". Factor: " + QString::number(balanceFactor));
    highlightNode(checkStep, node->id, Qt::cyan);
    history.append(QVariant::fromValue(checkStep));

    // Case 1: Left Heavy
    if (balanceFactor > 1) {
        if (getBalanceFactor(node->left) < 0) { // Left-Right Case
            history.append(QVariant::fromValue(createSnapshot("Left-Right case. Performing Left rotation on " + QString::number(node->left->value))));
            node->left = leftRotate(node->left, history);
        }
        // Left-Left Case
        history.append(QVariant::fromValue(createSnapshot("Left-Left case. Performing Right rotation on " + QString::number(node->value))));
        return rightRotate(node, history);
    }

    // Case 2: Right Heavy
    if (balanceFactor < -1) {
        if (getBalanceFactor(node->right) > 0) { // Right-Left Case
            history.append(QVariant::fromValue(createSnapshot("Right-Left case. Performing Right rotation on " + QString::number(node->right->value))));
            node->right = rightRotate(node->right, history);
        }
        // Right-Right Case
        history.append(QVariant::fromValue(createSnapshot("Right-Right case. Performing Left rotation on " + QString::number(node->value))));
        return leftRotate(node, history);
    }

    // Case 3: Balanced
    return node;
}


AVL::TreeNode* AVL::removeRecursive(AVL::TreeNode* node, int value, QList<QVariant>& history)
{
    // --- 1. Standard BST Remove ---
    if (node == nullptr) {
        history.append(QVariant::fromValue(createSnapshot("Value " + QString::number(value) + " not found.")));
        return nullptr;
    }

    GraphStep visitStep = createSnapshot("Comparing with " + QString::number(node->value));
    highlightNode(visitStep, node->id, Qt::yellow);
    history.append(QVariant::fromValue(visitStep));

    if (value < node->value) {
        node->left = removeRecursive(node->left, value, history);
    } else if (value > node->value) {
        node->right = removeRecursive(node->right, value, history);
    } else {
        // Node found
        GraphStep foundStep = createSnapshot("Found node " + QString::number(node->value));
        highlightNode(foundStep, node->id, Qt::red);
        history.append(QVariant::fromValue(foundStep));

        // Case 1: Node with 0 or 1 child (This logic is correct)
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
        GraphStep succStep = createSnapshot("Node has two children. Finding inorder successor...");
        highlightNode(succStep, node->id, Qt::red);
        history.append(QVariant::fromValue(succStep));

        TreeNode* temp = findMin(node->right);

        GraphStep foundSuccStep = createSnapshot("Found successor: " + QString::number(temp->value));
        highlightNode(foundSuccStep, temp->id, Qt::green);
        history.append(QVariant::fromValue(foundSuccStep));

        node->value = temp->value; // Copy value

        GraphStep copyStep = createSnapshot("Copying value " + QString::number(temp->value) + " to node");
        highlightNode(copyStep, node->id, Qt::green);
        history.append(QVariant::fromValue(copyStep));

        // Recursively delete the successor
        node->right = removeRecursive(node->right, temp->value, history);
    }

    if (node == nullptr) {
        return nullptr; // Deletion complete
    }

    // --- 2. AVL Balancing ---
    updateHeight(node);
    return balanceForRemove(node, history);
}


AVL::TreeNode* AVL::balanceForRemove(AVL::TreeNode* node, QList<QVariant>& history)
{
    int balanceFactor = getBalanceFactor(node);

    GraphStep checkStep = createSnapshot("Checking balance at node " + QString::number(node->value) + ". Factor: " + QString::number(balanceFactor));
    highlightNode(checkStep, node->id, Qt::cyan);
    history.append(QVariant::fromValue(checkStep));

    // Case 1: Left Heavy
    if (balanceFactor > 1) {
        if (getBalanceFactor(node->left) < 0) { // Left-Right Case
            history.append(QVariant::fromValue(createSnapshot("Left-Right case. Performing Left rotation on " + QString::number(node->left->value))));
            node->left = leftRotate(node->left, history);
        }
        // Left-Left Case
        history.append(QVariant::fromValue(createSnapshot("Left-Left case. Performing Right rotation on " + QString::number(node->value))));
        return rightRotate(node, history);
    }

    // Case 2: Right Heavy
    if (balanceFactor < -1) {
        if (getBalanceFactor(node->right) > 0) { // Right-Left Case
            history.append(QVariant::fromValue(createSnapshot("Right-Left case. Performing Right rotation on " + QString::number(node->right->value))));
            node->right = rightRotate(node->right, history);
        }
        // Right-Right Case
        history.append(QVariant::fromValue(createSnapshot("Right-Right case. Performing Left rotation on " + QString::number(node->value))));
        return leftRotate(node, history);
    }

    // Case 3: Balanced
    return node;
}


AVL::TreeNode* AVL::findMin(AVL::TreeNode* node)
{
    while (node && node->left != nullptr) {
        node = node->left;
    }
    return node;
}

// --- AVL Balancing Helpers ---

int AVL::getHeight(AVL::TreeNode* node)
{
    if (node == nullptr) return 0;
    return node->height;
}

int AVL::getBalanceFactor(AVL::TreeNode* node)
{
    if (node == nullptr) return 0;
    return getHeight(node->left) - getHeight(node->right);
}

void AVL::updateHeight(AVL::TreeNode* node)
{
    if (node) {
        node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
    }
}

// --- UPDATED Rotation functions ---
// They no longer call updatePositions(). It's done at the top level.

AVL::TreeNode* AVL::rightRotate(AVL::TreeNode* y, QList<QVariant>& history)
{
    // Snapshot *before* rotation (layout is still old)
    GraphStep preStep = createSnapshot("Before Right Rotation on " + QString::number(y->value));
    highlightNode(preStep, y->id, Qt::red);
    history.append(QVariant::fromValue(preStep));

    TreeNode* x = y->left;
    TreeNode* T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    updateHeight(y);
    updateHeight(x);

    // We DON'T call updatePositions() here anymore.
    // Add a snapshot *after* rotation (layout is still old, but structure is new)
    // The *next* step (from the calling function) will have the new layout.
    GraphStep postStep = createSnapshot("After Right Rotation (re-calculating layout..._");
    highlightNode(postStep, x->id, Qt::green);
    history.append(QVariant::fromValue(postStep));

    return x; // New root of this subtree
}

AVL::TreeNode* AVL::leftRotate(AVL::TreeNode* x, QList<QVariant>& history)
{
    // Snapshot *before* rotation (layout is still old)
    GraphStep preStep = createSnapshot("Before Left Rotation on " + QString::number(x->value));
    highlightNode(preStep, x->id, Qt::red);
    history.append(QVariant::fromValue(preStep));

    TreeNode* y = x->right;
    TreeNode* T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update heights
    updateHeight(x);
    updateHeight(y);

    // We DON'T call updatePositions() here anymore.
    GraphStep postStep = createSnapshot("After Left Rotation (re-calculating layout...)");
    highlightNode(postStep, y->id, Qt::green);
    history.append(QVariant::fromValue(postStep));

    return y; // New root of this subtree
}


// --- Visualization & Layout Helpers ---

// --- THIS IS THE KEY FUNCTION ---
void AVL::updatePositions()
{
    if (!m_root) return;
    // Call the recursive helper, starting at level 0
    updatePositionsRecursive(m_root, 0, CANVAS_WIDTH / 2.0, CANVAS_WIDTH / 4.0);
}

void AVL::updatePositionsRecursive(AVL::TreeNode* node, int level, double x, double xOffset)
{
    if (node == nullptr) {
        return;
    }
    // Set the node's position based on its level and x-offset
    node->pos = QPointF(x, 50 + level * LEVEL_HEIGHT);
    node->level = level;

    // Recurse for children, halving the x-offset each time
    updatePositionsRecursive(node->left, level + 1, x - xOffset, xOffset / 2.0);
    updatePositionsRecursive(node->right, level + 1, x + xOffset, xOffset / 2.0);
}

GraphStep AVL::createSnapshot(const QString& message)
{
    GraphStep step;
    step.statusMessage = message;
    if (m_root) {
        populateSnapshot(m_root, step);
    }
    return step;
}

void AVL::populateSnapshot(AVL::TreeNode* node, GraphStep& step)
{
    if (node == nullptr) {
        return;
    }

    // 1. Add this node
    NodeState ns;
    ns.id = node->id;
    ns.label = QString::number(node->value);
    ns.position = node->pos; // Use the pre-calculated position
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

void AVL::highlightNode(GraphStep& step, int nodeId, QColor color, QColor textColor)
{
    for (NodeState& ns : step.nodes) {
        if (ns.id == nodeId) {
            ns.color = color;
            ns.textColor = textColor;
            return;
        }
    }
}
