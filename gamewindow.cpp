#include "gamewindow.h"
#include "replaywindow.h"
#include "ui_gamewindow.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QDebug>

Gamewindow::Gamewindow(int boardSize, int obstacleCount, const QString &firstPlayer, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Gamewindow)
    , boardSize(boardSize)
    , obstacleCount(obstacleCount)
    , firstPlayer(firstPlayer)
{
    ui->setupUi(this);

    // 보드 생성
    board = new OtheloBoard(this);
    board->setMinimumSize(400, 400);
    board->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    // 기존 layout 제거
    if (ui->centralwidget->layout()) {
        delete ui->centralwidget->layout();
    }

    // 메인 레이아웃: 수평 (좌: 보드 / 우: 상태표시 + 버튼)
    QHBoxLayout *mainLayout = new QHBoxLayout(ui->centralwidget);

    // 왼쪽: 보드 레이아웃
    QVBoxLayout *boardLayout = new QVBoxLayout();
    boardLayout->addWidget(board);
    mainLayout->addLayout(boardLayout, 3);  // 비율 조정

    // 오른쪽: 상태 표시창 + 버튼
    QVBoxLayout *sideLayout = new QVBoxLayout();
    sideLayout->addWidget(ui->turnLabel);
    sideLayout->addWidget(ui->countLabel);
    sideLayout->addStretch();  // 중간 빈 공간
    sideLayout->addWidget(ui->undoButton);
    sideLayout->addWidget(ui->mainMenuButton);
    mainLayout->addLayout(sideLayout, 1);

    // 보드 클릭 시 동작 연결
    connect(board, &OtheloBoard::cellClicked, this, &Gamewindow::handleCellClicked);
    connect(ui->undoButton, &QPushButton::clicked, this, &Gamewindow::restorePreviousState);
    connect(ui->mainMenuButton, &QPushButton::clicked, this, &Gamewindow::onMainMenuClicked);

    // 초기 보드 세팅
    initializeBoardUI();
    updateStatus();

    this->setMinimumSize(800, 700);  // 창 최소 크기 설정
}

Gamewindow::~Gamewindow() {
    delete ui;
}

void Gamewindow::initializeBoardUI() {
    boardState.resize(boardSize, QVector<int>(boardSize, EMPTY));

    int mid = boardSize / 2;
    boardState[mid - 1][mid - 1] = WHITE;
    boardState[mid - 1][mid]     = BLACK;
    boardState[mid][mid - 1]     = BLACK;
    boardState[mid][mid]         = WHITE;

    placeObstacles();

    board->setBoardSize(boardSize);
    board->setBoard(boardState);

    updateValidMoves();
    board->setValidMoves(validMoves);  // 🔸 중요: 초록 표시
}

void Gamewindow::handleCellClicked(int row, int col) {
    int currentPlayer = (firstPlayer == "흑") ? BLACK : WHITE;
    if (!isValidMove(row, col, currentPlayer)) return;

    saveCurrentState();
    placeStone(row, col, currentPlayer);
    flipStones(row, col, currentPlayer);

    updateValidMoves();
    updateBoardUI();
    updateStatus();
    switchTurn();
}

void Gamewindow::updateBoardUI() {
    board->setBoard(boardState);
    board->setValidMoves(validMoves);  // 🔸 없으면 초록 사라짐
}

bool Gamewindow::isValidMove(int row, int col, int currentPlayer) const {
    if (boardState[row][col] != EMPTY) return false;

    int opponent = (currentPlayer == BLACK) ? WHITE : BLACK;
    const int dx[] = {-1, -1, -1, 0, 1, 1, 1, 0};
    const int dy[] = {-1,  0,  1, 1, 1, 0, -1, -1};

    for (int d = 0; d < 8; ++d) {
        int x = row + dx[d];
        int y = col + dy[d];
        bool hasOpponentBetween = false;

        while (x >= 0 && x < boardSize && y >= 0 && y < boardSize) {
            if (boardState[x][y] == opponent) {
                hasOpponentBetween = true;
            } else if (boardState[x][y] == currentPlayer && hasOpponentBetween) {
                return true;
            } else {
                break;
            }
            x += dx[d];
            y += dy[d];
        }
    }
    return false;
}

void Gamewindow::placeStone(int row, int col, int currentPlayer) {
    boardState[row][col] = currentPlayer;
    updateStatus();

    ReplayState state;
    state.board = boardState;
    state.move = {row, col, currentPlayer};
    replayStates.append(state);
}

void Gamewindow::flipStones(int row, int col, int currentPlayer) {
    int opponent = (currentPlayer == BLACK) ? WHITE : BLACK;
    const int dx[] = {-1, -1, -1, 0, 1, 1, 1, 0};
    const int dy[] = {-1,  0,  1, 1, 1, 0, -1, -1};

    for (int d = 0; d < 8; ++d) {
        int x = row + dx[d];
        int y = col + dy[d];
        QVector<QPair<int, int>> toFlip;

        while (x >= 0 && x < boardSize && y >= 0 && y < boardSize) {
            if (boardState[x][y] == opponent) {
                toFlip.append({x, y});
            } else if (boardState[x][y] == currentPlayer) {
                for (auto &[fx, fy] : toFlip) {
                    boardState[fx][fy] = currentPlayer;
                }
                break;
            } else {
                break;
            }
            x += dx[d];
            y += dy[d];
        }
    }
}

void Gamewindow::switchTurn() {
    firstPlayer = (firstPlayer == "흑") ? "백" : "흑";
    updateValidMoves();

    if (validMoves.isEmpty()) {
        qDebug() << firstPlayer << "은 착수할 수 없어 패스됩니다.";
        firstPlayer = (firstPlayer == "흑") ? "백" : "흑";
        updateValidMoves();

        if (validMoves.isEmpty()) {
            checkGameOver();
            return;
        } else {
            QMessageBox::information(this, "패스", "착수 가능한 위치가 없어 패스됩니다.");
        }
    }

    updateBoardUI();
    updateStatus();
    checkGameOver();
    qDebug() << "턴 전환 → 현재 턴:" << firstPlayer;
}

void Gamewindow::updateValidMoves() {
    validMoves.clear();
    int currentPlayer = (firstPlayer == "흑") ? BLACK : WHITE;

    for (int row = 0; row < boardSize; ++row) {
        for (int col = 0; col < boardSize; ++col) {
            if (isValidMove(row, col, currentPlayer)) {
                validMoves.append({row, col});
            }
        }
    }

    if (board) board->setValidMoves(validMoves);  // 🔸 중요
}

void Gamewindow::updateStatus() {
    if (!ui || !ui->turnLabel || !ui->countLabel) return;

    int blackCount = 0, whiteCount = 0;
    for (const auto &row : boardState) {
        for (int cell : row) {
            if (cell == BLACK) ++blackCount;
            else if (cell == WHITE) ++whiteCount;
        }
    }

    ui->turnLabel->setText(QString("현재 턴: %1").arg(firstPlayer));

    // 🔶 현재 턴 배경색 강조
    if (firstPlayer == "흑") {
        ui->turnLabel->setStyleSheet("background-color: black; color: white; font-weight: bold; font-size: 16px;");
    } else {
        ui->turnLabel->setStyleSheet("background-color: white; color: black; font-weight: bold; font-size: 16px;");
    }

    ui->countLabel->setText(QString("흑: %1, 백: %2").arg(blackCount).arg(whiteCount));
}

void Gamewindow::checkGameOver() {
    int blackCount = 0, whiteCount = 0;
    for (const auto &row : boardState) {
        for (int cell : row) {
            if (cell == BLACK) ++blackCount;
            else if (cell == WHITE) ++whiteCount;
        }
    }

    if (hasValidMove(BLACK) || hasValidMove(WHITE)) return;

    // 게임 결과 알림
    QString result;
    if (blackCount > whiteCount) result = "흑 승리!";
    else if (whiteCount > blackCount) result = "백 승리!";
    else result = "무승부입니다!";
    QMessageBox::information(this, "게임 종료", result);

    // 리플레이 저장 여부 확인
    bool saved = false;
    auto reply = QMessageBox::question(this, "리플레이 저장", "리플레이를 저장하시겠습니까?");
    if (reply == QMessageBox::Yes) {
        saved = true;
    }
    gameEndedFlag = true;
    emit gameEnded(replayStates, boardSize, saved);  // ✅ 저장 여부 전달

    this->hide();
    parentWidget()->show();
}


bool Gamewindow::hasValidMove(int playerColor) const {
    for (int row = 0; row < boardSize; ++row) {
        for (int col = 0; col < boardSize; ++col) {
            if (isValidMove(row, col, playerColor)) {
                return true;
            }
        }
    }
    return false;
}

void Gamewindow::placeObstacles() {
    int placed = 0;
    int pairsToPlace = obstacleCount / 2;
    int mid = boardSize / 2;

    QSet<QPair<int, int>> forbidden = {
        {mid - 1, mid - 1}, {mid - 1, mid},
        {mid,     mid - 1}, {mid,     mid}
    };
    QSet<QPair<int, int>> used;

    int attempts = 0;
    const int maxAttempts = 1000;

    while (placed < pairsToPlace && attempts < maxAttempts) {
        int row = QRandomGenerator::global()->bounded(boardSize);
        int col = QRandomGenerator::global()->bounded(boardSize);
        int symRow = row;
        int symCol = boardSize - 1 - col;
        attempts++;

        QPair<int, int> p1 = {row, col};
        QPair<int, int> p2 = {symRow, symCol};

        if (forbidden.contains(p1) || forbidden.contains(p2) ||
            used.contains(p1) || used.contains(p2) ||
            boardState[row][col] != EMPTY || boardState[symRow][symCol] != EMPTY)
            continue;

        boardState[row][col] = OBSTACLE;
        boardState[symRow][symCol] = OBSTACLE;
        used.insert(p1);
        used.insert(p2);
        placed++;
    }

    if (placed < pairsToPlace) {
        qDebug() << "⚠️ 장애물 일부만 배치됨. 요청:" << obstacleCount << " 실제:" << placed * 2;
    }
}

void Gamewindow::saveCurrentState() {
    GameState state;
    state.board = boardState;
    state.turn = firstPlayer;
    undoStack.push(state);
}

void Gamewindow::restorePreviousState() {
    if (undoStack.isEmpty()) return;

    GameState state = undoStack.pop();
    boardState = state.board;
    firstPlayer = state.turn;

    updateValidMoves();
    updateBoardUI();
    updateStatus();

    // ✅ 되돌리기 시 리플레이도 한 턴 제거
    if (!replayStates.isEmpty()) {
        replayStates.removeLast();
    }
}


void Gamewindow::onMainMenuClicked() {
    if (!gameEndedFlag) {
        emit requestSave(this);  // 게임 중일 때만 이어하기 저장
    }
    this->hide();
    parentWidget()->show();
}
bool Gamewindow::isGameEnded() const {
    return gameEndedFlag;
}
