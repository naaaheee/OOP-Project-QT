#ifndef REPLAYDATA_H
#define REPLAYDATA_H

#include <QVector>

// 한 번의 착수 정보를 저장
struct Move {
    int row;
    int col;
    int color;  // BLACK 또는 WHITE
};

// 하나의 턴에 대한 전체 상태 저장 (보드 + 착수 정보)
struct ReplayState {
    QVector<QVector<int>> board;
    Move move;
};

#endif // REPLAYDATA_H
