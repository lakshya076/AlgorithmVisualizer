#pragma once

#include <QList>
#include <QVariant>
#include <QVector>
#include <QString>
#include "datastructures.h"

namespace DynamicProgramming
{
    // 0/1 Knapsack
    QList<QVariant> knapsack01(int capacity, const QVector<int>& weights, const QVector<int>& values);
    
    // Longest Common Subsequence
    QList<QVariant> lcs(const QString& s1, const QString& s2);
}
