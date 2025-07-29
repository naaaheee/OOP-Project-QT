#include "otheloboard.h"
#include <QPainter>
#include <QMouseEvent>

OtheloBoard::OtheloBoard(QWidget *parent) : QWidget(parent) {
    setMinimumSize(400, 400);
    setMouseTracking(true);
}

void OtheloBoard::setBoard(const QVector<QVector<int>> &state) {
    boardState = state;
    update();
}

void OtheloBoard::setValidMoves(const QVector<QPair<int, int>> &moves) {
    validMoves = moves;
    update();
}

void OtheloBoard::setBoardSize(int size) {
    boardSize = size;
    update();
}

void OtheloBoard::setHighlight(int row, int col) {
    highlightCell = {row, col};
    update();
}

void OtheloBoard::clearHighlight() {
    highlightCell = {-1, -1};
    update();
}

void OtheloBoard::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    if (boardSize <= 0 || boardState.isEmpty()) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int cellSize = qMin(width(), height()) / boardSize;

    for (int row = 0; row < boardSize; ++row) {
        for (int col = 0; col < boardSize; ++col) {
            QRect cell(col * cellSize, row * cellSize, cellSize, cellSize);

            // 배경과 격자 그리기
            painter.setPen(QPen(Qt::gray, 1));
            painter.setBrush(Qt::white);
            painter.drawRect(cell);

            int val = boardState[row][col];

            if (val == 1) {
                painter.setBrush(Qt::black);
                painter.setPen(Qt::black);
                painter.drawEllipse(cell.adjusted(8, 8, -8, -8));
            } else if (val == 2) {
                painter.setBrush(Qt::white);
                painter.setPen(Qt::black);
                painter.drawEllipse(cell.adjusted(8, 8, -8, -8));
            } else if (val == -1) {
                painter.fillRect(cell, QColor("navy"));  // 장애물
            }
        }
    }

    // 🔶 착수 가능한 위치(초록색) 칠하기
    painter.setBrush(QColor(144, 238, 144)); // light green
    painter.setPen(Qt::NoPen);
    for (const auto &pos : validMoves) {
        QRect hint(pos.second * cellSize, pos.first * cellSize, cellSize, cellSize);
        painter.drawRect(hint);
    }

    // 🔴 현재 착수 위치 강조 (빨간 테두리)
    if (highlightCell.first != -1 && highlightCell.second != -1) {
        QRect hrect(highlightCell.second * cellSize, highlightCell.first * cellSize, cellSize, cellSize);
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(Qt::red, 3));
        painter.drawRect(hrect);
    }
}

void OtheloBoard::mousePressEvent(QMouseEvent *event) {
    if (boardSize <= 0) return;
    int cellSize = qMin(width(), height()) / boardSize;
    int col = event->pos().x() / cellSize;
    int row = event->pos().y() / cellSize;

    if (row >= 0 && row < boardSize && col >= 0 && col < boardSize) {
        emit cellClicked(row, col);
    }
}
