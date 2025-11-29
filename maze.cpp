#include "maze.h"
#include <QRandomGenerator>
#include <algorithm> // std::shuffle

Maze::Maze() {}

QList<QVariant> Maze::generateRecursiveBacktracker(int width, int height)
{
    m_width = width;
    m_height = height;

    // Initialize grid with WALLS
    m_grid.clear();
    m_grid.resize(height);
    for(int y=0; y<height; ++y) {
        m_grid[y].resize(width);
        m_grid[y].fill(CellType::WALL);
    }

    QList<QVariant> history;
    history.append(QVariant::fromValue(createSnapshot("Initializing Maze Grid...")));

    // Start carving from (1, 1)
    // We use odd coordinates for cells to ensure walls exist between them
    carve(1, 1, history);

    // Final cleanup: Remove the "Head" markers (VISITED) and make them normal PATHS
    // (In this implementation, carve sets them to PATH immediately after recursion,
    // so we just need a final 'Done' snapshot)
    history.append(QVariant::fromValue(createSnapshot("Maze Generation Complete!")));

    return history;
}

void Maze::carve(int x, int y, QList<QVariant>& history)
{
    m_grid[y][x] = CellType::VISITED; // Mark as "Head" (Processing)
    history.append(QVariant::fromValue(createSnapshot("Carving at (" + QString::number(x) + "," + QString::number(y) + ")")));

    // Directions: Up, Down, Left, Right
    // dx, dy pairs
    QVector<QPair<int, int>> dirs = { {0, -2}, {0, 2}, {-2, 0}, {2, 0} };

    // Randomize directions
    for (int i = 0; i < dirs.size(); ++i) {
        int j = QRandomGenerator::global()->bounded(dirs.size());
        qSwap(dirs[i], dirs[j]);
    }

    for (const auto& dir : dirs) {
        int dx = dir.first;
        int dy = dir.second;

        int nx = x + dx;
        int ny = y + dy;

        // Check bounds and if unvisited (WALL)
        // We check 1 step away (wall) and 2 steps away (target cell)
        if (nx > 0 && nx < m_width - 1 && ny > 0 && ny < m_height - 1 && m_grid[ny][nx] == CellType::WALL) {

            // Remove wall between current and next
            m_grid[y + dy/2][x + dx/2] = CellType::PATH;

            // Mark next as visited (Head)
            // Note: We temporarily mark the 'wall' we just broke as VISITED for visual flair
            m_grid[y + dy/2][x + dx/2] = CellType::VISITED;

            carve(nx, ny, history);

            // Backtracking: Set the path behind us to normal PATH (clearing the red head)
            m_grid[y + dy/2][x + dx/2] = CellType::PATH;
        }
    }

    // We are done with this cell, set it to normal PATH
    m_grid[y][x] = CellType::PATH;
    history.append(QVariant::fromValue(createSnapshot("Backtracking from (" + QString::number(x) + "," + QString::number(y) + ")")));
}

MazeStep Maze::createSnapshot(const QString& message)
{
    MazeStep step;
    step.grid = m_grid; // Deep copy the grid
    step.statusMessage = message;
    return step;
}
