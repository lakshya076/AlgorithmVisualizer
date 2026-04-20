#include "algorithmcanvas.h"
#include <QPaintEvent>
#include <QPainter>
#include <QMap>
#include <algorithm>


AlgorithmCanvas::AlgorithmCanvas(QWidget *parent)
    : QWidget(parent)
{
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
    update();
}

void AlgorithmCanvas::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_currentStep.canConvert<SortingStep>()) {
        drawSortingStep(painter, m_currentStep.value<SortingStep>());
    }
    else if (m_currentStep.canConvert<GraphStep>()) {
        drawGraphStep(painter, m_currentStep.value<GraphStep>());
    }
    else if (m_currentStep.canConvert<MazeStep>()) {
        drawMazeStep(painter, m_currentStep.value<MazeStep>());
    }
    else if (m_currentStep.canConvert<DPStep>()) {
        drawDPStep(painter, m_currentStep.value<DPStep>());
    }
}

void AlgorithmCanvas::drawDPStep(QPainter& painter, const DPStep& step)
{
    int rows = step.table.size();
    if (rows == 0) return;
    int cols = step.table[0].size();

    int margin = 60;
    int headerHeight = 100;
    int itemPanelWidth = (step.items.isEmpty()) ? 0 : 200;

    int availableWidth = width() - 2 * margin - itemPanelWidth;
    int availableHeight = height() - 2 * margin - headerHeight;

    int cellSize = qMin(availableWidth / cols, availableHeight / rows);
    cellSize = qMin(cellSize, 60); // Limit cell size

    int tableWidth = cellSize * cols;
    int tableHeight = cellSize * rows;

    int startX = margin + (availableWidth - tableWidth) / 2;
    int startY = margin + headerHeight + (availableHeight - tableHeight) / 2;

    // Draw Item List (for Knapsack)
    if (!step.items.isEmpty()) {
        int itemX = width() - margin - itemPanelWidth + 20;
        int itemY = startY;
        painter.setFont(QFont("Arial", 12, QFont::Bold));
        painter.drawText(itemX, itemY - 20, "Items (Value, Weight):");
        painter.setFont(QFont("Arial", 10));
        for (int i = 0; i < step.items.size(); ++i) {
            QRect itemRect(itemX, itemY + i * 30, itemPanelWidth - 20, 25);
            if (step.selectedItems.contains(i)) {
                painter.setBrush(QColor(100, 255, 100, 150));
            } else if (i == step.currentRow - 1) {
                painter.setBrush(QColor(255, 255, 100, 150));
            } else {
                painter.setBrush(Qt::NoBrush);
            }
            painter.setPen(Qt::black);
            painter.drawRect(itemRect);
            painter.drawText(itemRect.adjusted(5, 0, 0, 0), Qt::AlignVCenter, 
                             QString("Item %1: (%2, %3)").arg(i+1).arg(step.items[i].value).arg(step.items[i].weight));
        }

        // Draw Capacity
        painter.setFont(QFont("Arial", 12, QFont::Bold));
        painter.drawText(margin, margin + 40, "Knapsack Capacity: " + QString::number(step.capacity));
    }

    // Draw LCS Strings
    if (!step.s1.isEmpty() || !step.s2.isEmpty()) {
        painter.setFont(QFont("Arial", 12, QFont::Bold));
        painter.setPen(Qt::white);
        painter.drawText(margin, margin + 40, "LCS: " + step.s1 + " and " + step.s2);
    }

    // Draw Table Headers
    painter.setFont(QFont("Arial", 10));
    painter.setPen(Qt::white);
    // Row headers
    for (int i = 0; i < rows; ++i) {
        QString label;
        if (!step.s1.isEmpty()) {
            label = (i == 0) ? "∅" : QString(step.s1[i-1]);
        } else {
            label = (i == 0) ? "None" : QString("Item %1").arg(i);
        }
        painter.drawText(startX - 60, startY + i * cellSize, 55, cellSize, Qt::AlignCenter, label);
    }
    // Col headers
    for (int j = 0; j < cols; ++j) {
        QString label;
        if (!step.s2.isEmpty()) {
            label = (j == 0) ? "∅" : QString(step.s2[j-1]);
        } else {
            label = QString::number(j);
        }
        painter.drawText(startX + j * cellSize, startY - 25, cellSize, 20, Qt::AlignCenter, label);
    }

    // Draw Table Cells
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            QRect cellRect(startX + j * cellSize, startY + i * cellSize, cellSize, cellSize);
            
            QColor bgColor = Qt::white;

            // Highlight logic for backtracking / final result
            if (step.currentRow == -1) {
                if (step.resultCells.contains(QPoint(j, i))) {
                    bgColor = Qt::green;
                } else if (step.highlightedCells.contains(QPoint(j, i))) {
                    bgColor = QColor(200, 200, 200);
                }
            } else if (i == step.currentRow && j == step.currentCol) {
                bgColor = QColor(255, 165, 0);
            } else if (step.highlightedCells.contains(QPoint(j, i))) {
                bgColor = QColor(135, 206, 250);
            } else if (i <= step.currentRow && (i < step.currentRow || j < step.currentCol)) {
                bgColor = QColor(240, 240, 240);
            }

            painter.setBrush(bgColor);
            painter.setPen(Qt::black);
            painter.drawRect(cellRect);

            if (i < step.currentRow || (i == step.currentRow && j <= step.currentCol) || step.currentRow == -1) {
                painter.drawText(cellRect, Qt::AlignCenter, QString::number(step.table[i][j]));
            }
        }
    }
}

void AlgorithmCanvas::drawSortingStep(QPainter& painter, const SortingStep& step)
{
    int n = step.data.size();
    if (n == 0) {
        return;
    }

    double barWidth = (double)width() / n;

    int maxVal = 1;
    if (!step.data.isEmpty()) {
        maxVal = *std::max_element(step.data.constBegin(), step.data.constEnd());
    }

    painter.setPen(Qt::white);
    painter.drawText(10, height() - 10, step.statusMessage);

    for (int i = 0; i < n; ++i)
    {
        double barHeight = ((double)step.data[i] / maxVal) * (height() - 30);
        double x = i * barWidth;
        double y = height() - barHeight - 30;

        QRectF bar(x, y, barWidth, barHeight);

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

void AlgorithmCanvas::drawGraphStep(QPainter& painter, const GraphStep& step)
{
    painter.setPen(Qt::white);
    painter.drawText(10, height() - 10, step.statusMessage);

    QMap<int, QPointF> nodePositions;
    for (const NodeState& ns : step.nodes) {
        nodePositions[ns.id] = ns.position;
    }

    painter.setFont(QFont("Arial", 10));
    for (const EdgeState& es : step.edges)
    {
        if (nodePositions.contains(es.fromId) && nodePositions.contains(es.toId))
        {
            QPointF p1 = nodePositions[es.fromId];
            QPointF p2 = nodePositions[es.toId];

            painter.setPen(QPen(es.color, 2));
            painter.drawLine(p1, p2);

            // Draw Weight (if exists)
            if (!es.weightLabel.isEmpty()) {
                // Calculate midpoint
                QPointF mid = (p1 + p2) / 2.0;

                // Draw a small black box behind text for readability
                QRectF textRect(mid.x() - 10, mid.y() - 10, 20, 20);
                painter.setBrush(Qt::black);
                painter.setPen(Qt::NoPen);
                painter.drawRect(textRect);

                // Draw text
                painter.setPen(Qt::cyan); // Cyan color for weights
                painter.drawText(textRect, es.weightLabel, QTextOption(Qt::AlignCenter));
            }
        }
    }

    // Draw Nodes
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    const int nodeRadius = 20;

    for (const NodeState& ns : step.nodes)
    {
        painter.setBrush(ns.color);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(ns.position, nodeRadius, nodeRadius);

        painter.setPen(ns.textColor);
        painter.drawText(QRectF(ns.position.x() - nodeRadius,
                                ns.position.y() - nodeRadius,
                                nodeRadius * 2,
                                nodeRadius * 2),
                         ns.label,
                         QTextOption(Qt::AlignCenter));
    }
}

void AlgorithmCanvas::drawMazeStep(QPainter& painter, const MazeStep& step)
{
    // Draw Status Message
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 12));
    painter.drawText(10, height() - 10, step.statusMessage);

    if (step.grid.isEmpty()) return;

    int rows = step.grid.size();
    int cols = step.grid[0].size();

    // Calculate cell size to fit canvas
    // Use slightly smaller area to leave margins
    double cellW = (double)width() / cols;
    double cellH = (double)(height() - 30) / rows; // Leave room for text

    // Use the smaller dimension to keep cells square
    double cellSize = std::min(cellW, cellH);

    // Center the maze
    double startX = (width() - (cols * cellSize)) / 2.0;
    double startY = ((height() - 30) - (rows * cellSize)) / 2.0;

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            int type = step.grid[y][x];

            QRectF cellRect(startX + x * cellSize, startY + y * cellSize, cellSize, cellSize);

            // Colors
            if (type == WALL) {
                painter.setBrush(Qt::black);
                painter.setPen(Qt::NoPen);
            }
            else if (type == PATH) {
                painter.setBrush(Qt::white);
                painter.setPen(Qt::NoPen);
            }
            else if (type == VISITED) {
                painter.setBrush(Qt::red);
                painter.setPen(Qt::NoPen);
            }

            painter.drawRect(cellRect);
        }
    }
}
