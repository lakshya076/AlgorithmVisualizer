#pragma once

#include <QWidget>
#include <QPainter>
#include "datastructures.h"

class AlgorithmCanvas : public QWidget
{
    Q_OBJECT

public:
    explicit AlgorithmCanvas(QWidget *parent = nullptr);

public slots:
    void drawStep(const QVariant& step);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QVariant m_currentStep;
    void drawSortingStep(QPainter& painter, const SortingStep& step);
    void drawGraphStep(QPainter& painter, const GraphStep& step);
};
