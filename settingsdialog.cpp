#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QRandomGenerator>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    // 기본값 설정
    ui->boardSizeComboBox->addItems({"6", "8", "10", "12"});
    ui->obstacleSpinBox->setRange(0, 16);
    ui->firstPlayerComboBox->addItems({"흑", "백", "무작위"});
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

int SettingsDialog::getBoardSize() const {
    return ui->boardSizeComboBox->currentText().toInt();
}

int SettingsDialog::getObstacleCount() const {
    return ui->obstacleSpinBox->value();
}

QString SettingsDialog::getFirstPlayer() const {
    QString first = ui->firstPlayerComboBox->currentText();
    if (first == "무작위") {
        return (QRandomGenerator::global()->bounded(2) == 0) ? "흑" : "백";
    }
    return first;
}
