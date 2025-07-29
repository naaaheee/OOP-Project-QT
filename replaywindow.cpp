#include "replaywindow.h"
#include "ui_replaywindow.h"
#include "otheloboard.h"
#include "gamewindow.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QPainter>
#include <QPen>

ReplayWindow::ReplayWindow(const QVector<ReplayState> &states, int boardSize, QWidget *parent)
    : QMainWindow(parent), replayStates(states), boardSize(boardSize), currentMoveIndex(0)
{
    ui = new Ui::ReplayWindow;
    ui->setupUi(this);

    setupBoard();

    connect(ui->turnSlider, &QSlider::valueChanged, this, &ReplayWindow::showTurn);
    connect(ui->prevButton, &QPushButton::clicked, this, &ReplayWindow::onPrevClicked);
    connect(ui->nextButton, &QPushButton::clicked, this, &ReplayWindow::onNextClicked);
    connect(ui->mainMenuButton, &QPushButton::clicked, this, &ReplayWindow::onMainMenuButtonClicked);

    ui->turnSlider->setMinimum(0);
    ui->turnSlider->setMaximum(replayStates.size() - 1);
    ui->turnSlider->setValue(0);

    showTurn(0);  // 초기 보드 상태
}

ReplayWindow::~ReplayWindow() {
    delete ui;
}

void ReplayWindow::setupBoard() {
    board = new OtheloBoard(this);
    board->setBoardSize(boardSize);
    board->setMinimumSize(400, 400);
    board->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    if (ui->boardWidget->layout()) {
        delete ui->boardWidget->layout();
    }

    QVBoxLayout *vLayout = new QVBoxLayout(ui->boardWidget);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addStretch();
    hLayout->addWidget(board);
    hLayout->addStretch();

    vLayout->addStretch();
    vLayout->addLayout(hLayout);
    vLayout->addStretch();

    // Move 목록 추출
    replayMoves.clear();
    for (const ReplayState &s : replayStates)
        replayMoves.append(s.move);
}

void ReplayWindow::showTurn(int index) {
    if (index < 0 || index >= replayMoves.size()) return;

    currentMoveIndex = index;

    QVector<QVector<int>> partialBoard = buildBoardStateUpTo(index);
    board->setBoard(partialBoard);
    board->setValidMoves({});              // 🔴 리플레이에선 초록색 제거
    board->setHighlight(-1, -1);           // 🔴 이전 강조 제거

    const Move &m = replayMoves[index];
    board->setHighlight(m.row, m.col);     // 🔴 현재 턴 착수 위치 강조

    // 돌 개수 세기
    int black = 0, white = 0;
    for (const auto &row : partialBoard) {
        for (int val : row) {
            if (val == 1) ++black;
            else if (val == 2) ++white;
        }
    }

    ui->turnLabel->setText(QString("현재 턴: %1").arg(m.color == 1 ? "흑" : "백"));
    ui->countLabel->setText(QString("흑: %1, 백: %2").arg(black).arg(white));
}

void ReplayWindow::onSliderChanged(int value) {
    showTurn(value);
}

void ReplayWindow::onPrevClicked() {
    if (currentMoveIndex > 0) {
        --currentMoveIndex;
        ui->turnSlider->setValue(currentMoveIndex);
        showTurn(currentMoveIndex);
    }
}

void ReplayWindow::onNextClicked() {
    if (currentMoveIndex < replayStates.size() - 1) {
        ++currentMoveIndex;
        ui->turnSlider->setValue(currentMoveIndex);
        showTurn(currentMoveIndex);
    }
}

void ReplayWindow::onMainMenuButtonClicked() {
    this->hide();
    if (parentWidget()) parentWidget()->show();
}

QVector<QVector<int>> ReplayWindow::buildBoardStateUpTo(int index) {
    // ✅ 초기 상태 복사 (장애물 + 초기 돌 포함)
    QVector<QVector<int>> board = replayStates[0].board;

    // ✅ index번째 턴까지 돌 추가
    for (int i = 0; i <= index && i < replayMoves.size(); ++i) {
        const Move &m = replayMoves[i];
        board[m.row][m.col] = m.color;
    }

    return board;
}
