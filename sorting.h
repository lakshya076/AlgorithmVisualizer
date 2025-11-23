#pragma once

#include "datastructures.h"
#include <QList>
#include <QVariant>
#include <QVector>

// We use a namespace to group all our sorting algorithms
namespace Sorting
{
QList<QVariant> bubbleSort(QVector<int> data);
QList<QVariant> insertionSort(QVector<int> data);
QList<QVariant> selectionSort(QVector<int> data);
QList<QVariant> quickSort(QVector<int> data);
QList<QVariant> mergeSort(QVector<int> data);
}
