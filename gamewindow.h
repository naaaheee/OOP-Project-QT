#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include "replaydata.h"
#include "otheloboard.h"
#include <QMainWindow>
#include <QVector>
#include <QStack>

#define EMPTY 0
#define BLACK 1
#define WHITE 2
#define OBSTACLE -1

namespace Ui {
class Gamewindow;
}

struct GameState {
    QVector<QVector<int>> board;
    QString turn;
};

class Gamewindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit Gamewindow(int boardSize, int obstacleCount, const QString &firstPlayer, QWidget *parent = nullptr);

    bool isGameEnded() const;
    ~Gamewindow();

signals:
        void gameEnded(const QVector<ReplayState> &states, int boardSize, bool saved);
    void requestSave(Gamewindow *game);

private:
    Ui::Gamewindow *ui;

    int boardSize;
    int obstacleCount;
    QString firstPlayer;

    QVector<QVector<int>> boardState;
    QVector<QPair<int, int>> validMoves;
    QStack<GameState> undoStack;
    QVector<ReplayState> replayStates;

    OtheloBoard *board;  // 오델로 보드 위젯

    // 새 UI용 함수
    void initializeBoardUI();           // drawBoard 대신
    void updateBoardUI();               // 돌 + 착수 위치 갱신
    bool isValidMove(int row, int col, int currentPlayer) const;
    void placeStone(int row, int col, int currentPlayer);
    void flipStones(int row, int col, int currentPlayer);
    void switchTurn();
    void updateValidMoves();
    void updateStatus();
    void checkGameOver();
    bool hasValidMove(int playerColor) const;
    void placeObstacles();
    void saveCurrentState();
    void restorePreviousState();
    bool gameEndedFlag = false;


private slots:
    void handleCellClicked(int row, int col);  // 🔄 파라미터 추가
    void onMainMenuClicked();
};

#endif // GAMEWINDOW_H
