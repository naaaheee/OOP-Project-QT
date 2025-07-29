#ifndef REPLAYWINDOW_H
#define REPLAYWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QTimer>
#include "replaydata.h"
#include "otheloboard.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ReplayWindow; }
QT_END_NAMESPACE

class ReplayWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ReplayWindow(const QVector<ReplayState> &states, int boardSize, QWidget *parent = nullptr);
    ~ReplayWindow();

private slots:
    void onSliderChanged(int value);     // 슬라이더 변경 시 턴 표시
    void showTurn(int index);            // 특정 턴 상태를 보드에 출력
    void onPrevClicked();                // 이전 턴으로 이동
    void onNextClicked();                // 다음 턴으로 이동
    void onMainMenuButtonClicked();      // 메인 화면 복귀

private:
    Ui::ReplayWindow *ui;

    int boardSize = 0;
    int currentMoveIndex = 0;

    QVector<ReplayState> replayStates;   // 전체 리플레이 상태
    QVector<Move> replayMoves;           // 착수 좌표만 분리 저장
    OtheloBoard *board = nullptr;        // 보드 위젯

    void setupBoard();                   // UI에 보드 배치
    QVector<QVector<int>> buildBoardStateUpTo(int index);  // index 턴까지 돌 배치
};

#endif // REPLAYWINDOW_H
