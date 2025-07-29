#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "replaydata.h"
#include "gamewindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onNewGameClicked();
    void onContinueClicked();
    void onReplayClicked();

private:
    Ui::MainWindow *ui;

    QVector<ReplayState> replayStates;     // 저장된 리플레이 정보
    Gamewindow *savedGame = nullptr;       // 이어할 게임 인스턴스
    int boardSize = 8;                     // 기본 보드 크기 (설정값 반영됨)
};

#endif // MAINWINDOW_H
