#include "algorithmcanvas.h"
#include <QPaintEvent>
#include <QPainter>
#include <QMap>
#include <algorithm> // For std::max_element

// Constructor
AlgorithmCanvas::AlgorithmCanvas(QWidget *parent)
    : QWidget(parent)
{
    // Set a solid black background
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::black);
    setPalette(pal);
}

// Public Slot: Receives the step from the controller
void AlgorithmCanvas::drawStep(const QVariant& step)
{
    // Store the step data
    m_currentStep = step;

    // Trigger a repaint. This will automatically call paintEvent().
    update();
}

// Protected: The main paint event handler
void AlgorithmCanvas::paintEvent(QPaintEvent* event)
{
    // First, call the base class paintEvent
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Check what type of step we have and call the correct helper
    if (m_currentStep.canConvert<SortingStep>())
    {
        drawSortingStep(painter, m_currentStep.value<SortingStep>());
    }
    else if (m_currentStep.canConvert<GraphStep>())
    {
        drawGraphStep(painter, m_currentStep.value<GraphStep>());
    }
    // else if (m_currentStep.canConvert<MazeStep>()) { ... }
}

// Private Helper: Does the actual drawing for sorting bars
void AlgorithmCanvas::drawSortingStep(QPainter& painter, const SortingStep& step)
{
    int n = step.data.size();
    if (n == 0) {
        return; // Nothing to draw
    }

    // --- 1. Calculate Bar Dimensions ---
    double barWidth = (double)width() / n;

    int maxVal = 1; // Default to 1 to avoid division by zero
    if (!step.data.isEmpty()) {
        maxVal = *std::max_element(step.data.constBegin(), step.data.constEnd());
    }

    // --- 2. Draw Status Message ---
    painter.setPen(Qt::white);
    painter.drawText(10, height() - 10, step.statusMessage);

    // --- 3. Draw Bars ---
    for (int i = 0; i < n; ++i)
    {
        double barHeight = ((double)step.data[i] / maxVal) * (height() - 30); // Leave 30px for text
        double x = i * barWidth;
        double y = height() - barHeight - 30; // 30px offset from bottom

        QRectF bar(x, y, barWidth, barHeight);

        // --- 4. Set Bar Color ---
        if (step.sortedIndices.contains(i)) {
            painter.setBrush(Qt::green);
        } else if (step.swapIndices.contains(i)) {
            painter.setBrush(Qt::red);
        } else if (step.pivotIndex.contains(i)) {
            painter.setBrush(Qt::blue);
        } else if (step.compareIndices.contains(i)) {
            painter.setBrush(Qt::yellow);
        } else {
            painter.setBrush(Qt::gray);
        }

        painter.setPen(Qt::NoPen);
        painter.drawRect(bar);
    }
}

// Private Helper: Does the actual drawing for trees/graphs
void AlgorithmCanvas::drawGraphStep(QPainter& painter, const GraphStep& step)
{
    // 1. Draw Status Message
    painter.setPen(Qt::white);
    painter.drawText(10, height() - 10, step.statusMessage);

    // Create a map of node IDs to their positions for easy edge drawing
    QMap<int, QPointF> nodePositions;
    for (const NodeState& ns : step.nodes) {
        nodePositions[ns.id] = ns.position;
    }

    // 2. Draw Edges (drawn first, so they appear "behind" the nodes)
    painter.setPen(QPen(Qt::white, 2)); // Default edge color and thickness
    for (const EdgeState& es : step.edges)
    {
        if (nodePositions.contains(es.fromId) && nodePositions.contains(es.toId))
        {
            painter.setPen(QPen(es.color, 2));
            painter.drawLine(nodePositions[es.fromId], nodePositions[es.toId]);
        }
    }

    // 3. Draw Nodes and Labels (drawn last, on top of edges)
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    const int nodeRadius = 20; // Draw nodes as 40x40 circles

    for (const NodeState& ns : step.nodes)
    {
        // Draw the circle
        painter.setBrush(ns.color);
        painter.setPen(Qt::NoPen); // No outline for the circle
        painter.drawEllipse(ns.position, nodeRadius, nodeRadius);

        // Draw the text label
        painter.setPen(ns.textColor);
        painter.drawText(QRectF(ns.position.x() - nodeRadius,
                                ns.position.y() - nodeRadius,
                                nodeRadius * 2,
                                nodeRadius * 2),
                         ns.label,
                         QTextOption(Qt::AlignCenter));
    }
}
