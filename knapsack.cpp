#include "dynamicprogramming.h"
#include <QPoint>

namespace DynamicProgramming
{
    QList<QVariant> knapsack01(int capacity, const QVector<int>& weights, const QVector<int>& values)
    {
        QList<QVariant> history;
        int n = weights.size();

        QVector<QVector<int>> dp(n + 1, QVector<int>(capacity + 1, 0));

        DPStep initialStep;
        initialStep.table = dp;
        initialStep.capacity = capacity;
        initialStep.currentRow = 0;
        initialStep.currentCol = 0;
        for (int i = 0; i < n; ++i) {
            initialStep.items.append({weights[i], values[i]});
        }
        initialStep.statusMessage = "Initialized DP table with 0s.";
        history.append(QVariant::fromValue(initialStep));

        for (int i = 1; i <= n; ++i) {
            for (int w = 0; w <= capacity; ++w) {
                DPStep step = initialStep;
                step.table = dp;
                step.currentRow = i;
                step.currentCol = w;

                if (weights[i - 1] <= w) {
                    // We can either include the item or not
                    int includeVal = values[i - 1] + dp[i - 1][w - weights[i - 1]];
                    int excludeVal = dp[i - 1][w];

                    step.highlightedCells.append(QPoint(w, i - 1)); // exclude cell
                    step.highlightedCells.append(QPoint(w - weights[i - 1], i - 1)); // cell to add to

                    if (includeVal > excludeVal) {
                        dp[i][w] = includeVal;
                        step.statusMessage = QString("Item %1 fits. Included.")
                                             .arg(i).arg(values[i - 1]).arg(includeVal).arg(excludeVal);
                    } else {
                        dp[i][w] = excludeVal;
                        step.statusMessage = QString("Item %1 fits, but excluding it gives %2, which is better than or equal to including it (%3).")
                                             .arg(i).arg(excludeVal).arg(includeVal);
                    }
                } else {
                    // Item doesn't fit
                    dp[i][w] = dp[i - 1][w];
                    step.highlightedCells.append(QPoint(w, i - 1));
                    step.statusMessage = QString("Item %1 (Weight: %2) too heavy. Taking value from above.")
                                         .arg(i).arg(weights[i - 1]).arg(w);
                }
                
                step.table[i][w] = dp[i][w]; // Update table in step
                history.append(QVariant::fromValue(step));
            }
        }

        // Backtracking to find selected items
        DPStep finalStep;
        finalStep.table = dp;
        finalStep.capacity = capacity;
        finalStep.currentRow = -1;
        finalStep.currentCol = -1;
        for (int i = 0; i < n; ++i) {
            finalStep.items.append({weights[i], values[i]});
        }

        int i = n;
        int w = capacity;
        while (i > 0 || w > 0) {
            if (i > 0 && dp[i][w] != dp[i-1][w]) {
                // Item i-1 was included
                finalStep.resultCells.append(QPoint(w, i));
                finalStep.selectedItems.append(i - 1);
                w -= weights[i-1];
                i--;
            } else {
                // Item i-1 was not included, or we reached the top row
                finalStep.highlightedCells.append(QPoint(w, i));
                if (i > 0) i--;
                else w--;
            }
        }
        finalStep.highlightedCells.append(QPoint(0, 0));

        finalStep.statusMessage = QString("Final Result: %1. Selected items are highlighted in green.").arg(dp[n][capacity]);
        history.append(QVariant::fromValue(finalStep));

        return history;
    }
}
