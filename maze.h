#pragma once

#include "datastructures.h"
#include <QList>
#include <QVariant>
#include <QVector>

class Maze
{
public:
    Maze();
    QList<QVariant> generateRecursiveBacktracker(int width, int height);

private:
    int m_width;
    int m_height;
    QVector<QVector<int>> m_grid;

    void carve(int x, int y, QList<QVariant>& history);
    MazeStep createSnapshot(const QString& message);
};
