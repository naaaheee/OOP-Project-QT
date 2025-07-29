#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    int getBoardSize() const;
    int getObstacleCount() const;
    QString getFirstPlayer() const;

private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
