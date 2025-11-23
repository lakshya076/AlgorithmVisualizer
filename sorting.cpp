#include "sorting.h"

// --- Private Helper Prototypes for Recursive Algos ---
namespace Sorting
{
// Helpers for QuickSort
void quickSortRecursive(QVector<int>& data, int low, int high, QList<QVariant>& history);
int partition(QVector<int>& data, int low, int high, QList<QVariant>& history);

// Helpers for MergeSort
void mergeSortRecursive(QVector<int>& data, int left, int right, QList<QVariant>& history);
void merge(QVector<int>& data, int left, int mid, int right, QList<QVariant>& history);
}

// --- Public Functions ---

QList<QVariant> Sorting::bubbleSort(QVector<int> data)
{
    QList<QVariant> history;
    int n = data.size();

    // Add Initial Step
    history.append(QVariant::fromValue(SortingStep{data, "Initial array"}));

    for (int i = 0; i < n - 1; ++i) {
        bool swapped = false;
        for (int j = 0; j < n - i - 1; ++j) {
            // 1. Comparison Step
            history.append(QVariant::fromValue(SortingStep{data,
                "Comparing " + QString::number(data[j]) + " and " + QString::number(data[j+1]),
                {j, j+1} // compareIndices
            }));

            if (data[j] > data[j + 1]) {
                qSwap(data[j], data[j + 1]);
                swapped = true;
                // 2. Swap Step
                history.append(QVariant::fromValue(SortingStep{data,
                    "Swapping", {}, {}, {j, j+1} // swapIndices
                }));
            }
        }
        // 3. Mark last element as sorted
        SortingStep sortedMarkStep = history.last().value<SortingStep>();
        sortedMarkStep.sortedIndices.append(n - 1 - i);
        sortedMarkStep.compareIndices.clear();
        history.append(QVariant::fromValue(sortedMarkStep));

        if (!swapped) break;
    }

    // Final Step
    SortingStep finalStep;
    finalStep.data = data;
    finalStep.statusMessage = "Sorted!";
    for(int i=0; i < n; ++i) finalStep.sortedIndices.append(i);
    history.append(QVariant::fromValue(finalStep));
    return history;
}

QList<QVariant> Sorting::insertionSort(QVector<int> data)
{
    QList<QVariant> history;
    int n = data.size();
    history.append(QVariant::fromValue(SortingStep{data, "Initial array"}));

    for (int i = 1; i < n; ++i) {
        int key = data[i];
        int j = i - 1;

        // 1. Select key to insert
        history.append(QVariant::fromValue(SortingStep{data,
            "Selecting key: " + QString::number(key), {i}, {}, {}, {0, i} // sortedIndices up to i
        }));

        while (j >= 0 && data[j] > key) {
            // 2. Compare key with element
            history.append(QVariant::fromValue(SortingStep{data,
                "Comparing " + QString::number(key) + " and " + QString::number(data[j]),
                {i, j}, {}, {}, {0, i}
            }));

            // 3. Shift element
            data[j + 1] = data[j];
            history.append(QVariant::fromValue(SortingStep{data,
                "Shifting " + QString::number(data[j]), {}, {}, {j+1, j}, {0, i}
            }));
            j = j - 1;
        }
        // 4. Insert key
        data[j + 1] = key;
        history.append(QVariant::fromValue(SortingStep{data,
            "Inserting " + QString::number(key), {}, {}, {j+1}, {0, i+1}
        }));
    }

    history.append(QVariant::fromValue(SortingStep{data, "Sorted!", {}, {}, {}, {0, n}}));
    return history;
}

QList<QVariant> Sorting::selectionSort(QVector<int> data)
{
    QList<QVariant> history;
    int n = data.size();
    history.append(QVariant::fromValue(SortingStep{data, "Initial array"}));

    for (int i = 0; i < n - 1; ++i) {
        int min_idx = i;

        // 1. Mark current minimum
        history.append(QVariant::fromValue(SortingStep{data,
            "Finding minimum for pass " + QString::number(i+1), {min_idx}, {}, {}, {0, i}
        }));

        for (int j = i + 1; j < n; ++j) {
            // 2. Compare
            history.append(QVariant::fromValue(SortingStep{data,
                "Comparing " + QString::number(data[j]) + " and " + QString::number(data[min_idx]),
                {j, min_idx}, {}, {}, {0, i}
            }));
            if (data[j] < data[min_idx]) {
                min_idx = j;
                // 3. Found new minimum
                history.append(QVariant::fromValue(SortingStep{data,
                    "Found new minimum: " + QString::number(data[min_idx]), {min_idx}, {}, {}, {0, i}
                }));
            }
        }
        // 4. Swap
        qSwap(data[i], data[min_idx]);
        history.append(QVariant::fromValue(SortingStep{data,
            "Swapping with position " + QString::number(i), {}, {}, {i, min_idx}, {0, i+1}
        }));
    }

    history.append(QVariant::fromValue(SortingStep{data, "Sorted!", {}, {}, {}, {0, n}}));
    return history;
}

// --- QuickSort Implementation ---

QList<QVariant> Sorting::quickSort(QVector<int> data)
{
    QList<QVariant> history;
    history.append(QVariant::fromValue(SortingStep{data, "Initial array"}));

    quickSortRecursive(data, 0, data.size() - 1, history);

    SortingStep finalStep;
    finalStep.data = data;
    finalStep.statusMessage = "Sorted!";
    for(int i=0; i < data.size(); ++i) finalStep.sortedIndices.append(i);
    history.append(QVariant::fromValue(finalStep));
    return history;
}

void Sorting::quickSortRecursive(QVector<int>& data, int low, int high, QList<QVariant>& history)
{
    if (low < high) {
        int pi = partition(data, low, high, history);

        // Mark pivot as sorted
        SortingStep pivotSortedStep = history.last().value<SortingStep>();
        pivotSortedStep.sortedIndices.append(pi);
        history.append(QVariant::fromValue(pivotSortedStep));

        quickSortRecursive(data, low, pi - 1, history);
        quickSortRecursive(data, pi + 1, high, history);
    }
}

int Sorting::partition(QVector<int>& data, int low, int high, QList<QVariant>& history)
{
    int pivot = data[high];
    int i = (low - 1);

    // 1. Select pivot
    history.append(QVariant::fromValue(SortingStep{data,
        "Partitioning. Pivot: " + QString::number(pivot), {}, {high} // pivotIndex
    }));

    for (int j = low; j < high; ++j) {
        // 2. Compare with pivot
        history.append(QVariant::fromValue(SortingStep{data,
            "Comparing " + QString::number(data[j]) + " and " + QString::number(pivot),
            {j, high}
        }));

        if (data[j] < pivot) {
            i++;
            qSwap(data[i], data[j]);
            // 3. Swap element
            history.append(QVariant::fromValue(SortingStep{data,
                "Swapping " + QString::number(data[i]) + " and " + QString::number(data[j]),
                {}, {high}, {i, j} // swapIndices
            }));
        }
    }
    // 4. Swap pivot to final place
    qSwap(data[i + 1], data[high]);
    history.append(QVariant::fromValue(SortingStep{data,
        "Placing pivot", {}, {}, {i+1, high}
    }));
    return (i + 1);
}

// --- MergeSort Implementation ---

QList<QVariant> Sorting::mergeSort(QVector<int> data)
{
    QList<QVariant> history;
    history.append(QVariant::fromValue(SortingStep{data, "Initial array"}));

    mergeSortRecursive(data, 0, data.size() - 1, history);

    SortingStep finalStep;
    finalStep.data = data;
    finalStep.statusMessage = "Sorted!";
    for(int i=0; i < data.size(); ++i) finalStep.sortedIndices.append(i);
    history.append(QVariant::fromValue(finalStep));
    return history;
}

void Sorting::mergeSortRecursive(QVector<int>& data, int left, int right, QList<QVariant>& history)
{
    if (left >= right) {
        return;
    }
    int mid = left + (right - left) / 2;
    mergeSortRecursive(data, left, mid, history);
    mergeSortRecursive(data, mid + 1, right, history);
    merge(data, left, mid, right, history);
}

void Sorting::merge(QVector<int>& data, int left, int mid, int right, QList<QVariant>& history)
{
    int n1 = mid - left + 1;
    int n2 = right - mid;

    QVector<int> L(n1), R(n2);
    for (int i = 0; i < n1; ++i) L[i] = data[left + i];
    for (int j = 0; j < n2; ++j) R[j] = data[mid + 1 + j];

    // 1. Announce merge
    QList<int> mergeRange;
    for(int i = left; i <= right; ++i) mergeRange.append(i);
    history.append(QVariant::fromValue(SortingStep{data,
        "Merging sub-arrays", mergeRange
    }));

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        // 2. Compare elements from L and R
        history.append(QVariant::fromValue(SortingStep{data,
            "Comparing " + QString::number(L[i]) + " and " + QString::number(R[j]),
            {left + i, mid + 1 + j}
        }));

        if (L[i] <= R[j]) {
            data[k] = L[i];
            i++;
        } else {
            data[k] = R[j];
            j++;
        }
        // 3. Place element back into main array
        history.append(QVariant::fromValue(SortingStep{data,
            "Placing " + QString::number(data[k]), {}, {}, {k}
        }));
        k++;
    }

    while (i < n1) {
        data[k] = L[i];
        history.append(QVariant::fromValue(SortingStep{data,
            "Placing remaining " + QString::number(data[k]), {}, {}, {k}
        }));
        i++;
        k++;
    }
    while (j < n2) {
        data[k] = R[j];
        history.append(QVariant::fromValue(SortingStep{data,
            "Placing remaining " + QString::number(data[k]), {}, {}, {k}
        }));
        j++;
        k++;
    }

    // 4. Sub-array merged
    history.append(QVariant::fromValue(SortingStep{data,
        "Sub-array merge complete", {}, {}, {}, mergeRange
    }));
}
