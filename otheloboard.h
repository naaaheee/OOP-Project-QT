#ifndef OTHELOBOARD_H
#define OTHELOBOARD_H

#include <QWidget>
#include <QVector>
#include <QPair>

class OtheloBoard : public QWidget {
    Q_OBJECT

public:
    explicit OtheloBoard(QWidget *parent = nullptr);

    // 상태 설정 함수
    void setBoard(const QVector<QVector<int>> &state);
    void setValidMoves(const QVector<QPair<int, int>> &moves);
    void setBoardSize(int size);
    void setHighlight(int row, int col);
    void clearHighlight();  // 🔸 강조 초기화

signals:
    void cellClicked(int row, int col);  // 셀 클릭 시그널

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QVector<QVector<int>> boardState;         // 현재 보드 상태 (0, 1, 2, -1)
    QVector<QPair<int, int>> validMoves;      // 현재 착수 가능한 위치 리스트
    int boardSize = 0;                        // 보드 크기 (기본값 0으로 명시)
    QPair<int, int> highlightCell = {-1, -1}; // 현재 강조 셀 (-1이면 없음)
};

#endif // OTHELOBOARD_H
