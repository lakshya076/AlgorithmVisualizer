#include "dynamicprogramming.h"
#include <QPoint>

namespace DynamicProgramming
{
    QList<QVariant> lcs(const QString& s1, const QString& s2)
    {
        QList<QVariant> history;
        int m = s1.length();
        int n = s2.length();

        QVector<QVector<int>> dp(m + 1, QVector<int>(n + 1, 0));

        DPStep initialStep;
        initialStep.table = dp;
        initialStep.s1 = s1;
        initialStep.s2 = s2;
        initialStep.currentRow = 0;
        initialStep.currentCol = 0;
        initialStep.statusMessage = "Initialized DP table for LCS.";
        history.append(QVariant::fromValue(initialStep));

        for (int i = 1; i <= m; ++i) {
            for (int j = 1; j <= n; ++j) {
                DPStep step = initialStep;
                step.table = dp;
                step.currentRow = i;
                step.currentCol = j;

                if (s1[i - 1] == s2[j - 1]) {
                    dp[i][j] = 1 + dp[i - 1][j - 1];
                    step.highlightedCells.append(QPoint(j - 1, i - 1));
                    step.statusMessage = QString("Characters match: '%1'. LCS length = 1 + dp[%2][%3] = %4.")
                                         .arg(s1[i-1]).arg(i-1).arg(j-1).arg(dp[i][j]);
                } else {
                    dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
                    step.highlightedCells.append(QPoint(j, i - 1));
                    step.highlightedCells.append(QPoint(j - 1, i));
                    step.statusMessage = QString("Characters '%1' and '%2' don't match. Taking max(dp[%3][%4], dp[%5][%6]) = %7.")
                                         .arg(s1[i-1]).arg(s2[j-1]).arg(i-1).arg(j).arg(i).arg(j-1).arg(dp[i][j]);
                }

                step.table[i][j] = dp[i][j];
                history.append(QVariant::fromValue(step));
            }
        }

        // Backtracking
        DPStep finalStep = initialStep;
        finalStep.table = dp;
        finalStep.currentRow = -1;
        finalStep.currentCol = -1;

        QString result = "";
        int i = m, j = n;
        while (i > 0 && j > 0) {
            if (s1[i - 1] == s2[j - 1]) {
                finalStep.resultCells.append(QPoint(j, i)); // Match!
                result = s1[i - 1] + result;
                i--;
                j--;
            } else {
                finalStep.highlightedCells.append(QPoint(j, i)); // Just part of the path
                if (dp[i - 1][j] > dp[i][j - 1]) {
                    i--;
                } else {
                    j--;
                }
            }
        }
        // Add the remaining path to (0,0) if any
        while (i >= 0 && j >= 0) {
            finalStep.highlightedCells.append(QPoint(j, i));
            if (i == 0 && j == 0) break;
            if (i > 0) i--;
            else j--;
        }

        finalStep.statusMessage = QString("LCS is '%1' with length %2.").arg(result).arg(dp[m][n]);
        history.append(QVariant::fromValue(finalStep));

        return history;
    }
}
