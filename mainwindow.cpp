#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "settingsdialog.h"
#include "gamewindow.h"
#include "replaywindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->replayButton->setEnabled(false);
    ui->continueButton->setEnabled(false);

    connect(ui->newGameButton, &QPushButton::clicked, this, &MainWindow::onNewGameClicked);
    connect(ui->continueButton, &QPushButton::clicked, this, &MainWindow::onContinueClicked);
    connect(ui->replayButton, &QPushButton::clicked, this, &MainWindow::onReplayClicked);
    connect(ui->exitButton, &QPushButton::clicked, this, &MainWindow::close);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onNewGameClicked() {
    SettingsDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        int size = dialog.getBoardSize();
        int obs = dialog.getObstacleCount();
        QString first = dialog.getFirstPlayer();

        Gamewindow *game = new Gamewindow(size, obs, first, this);

        // ✅ 리플레이용 정보 저장
        connect(game, &Gamewindow::gameEnded, this,
                [=](const QVector<ReplayState> &states, int s, bool saved) {
                    if (saved) {
                        this->replayStates = states;
                        this->boardSize = s;
                        ui->replayButton->setEnabled(true);  // ✅ 리플레이 활성화
                    }
                });
        connect(game, &Gamewindow::requestSave, this, [=](Gamewindow *g) {
            if (!g->isGameEnded()) {  // ✅ gameEndedFlag를 확인
                this->savedGame = g;
                ui->continueButton->setEnabled(true);
            }
        });
        game->show();
        this->hide();
    }
}

void MainWindow::onContinueClicked() {
    if (savedGame) {
        savedGame->show();
        this->hide();
    } else {
        QMessageBox::information(this, "안내", "이어할 게임이 없습니다.");
    }
}

void MainWindow::onReplayClicked() {

    ReplayWindow *replay = new ReplayWindow(replayStates, boardSize, this);
    replay->show();
    this->hide();
}
