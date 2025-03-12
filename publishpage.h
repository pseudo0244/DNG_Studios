#ifndef PUBLISHPAGE_H
#define PUBLISHPAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QRadioButton>
#include <QGroupBox>
#include <QScrollArea>
#include "score.h"

class MainWindow;

class PublishPage : public QWidget
{
    Q_OBJECT

public:
    PublishPage(MainWindow *mainWindow, QWidget *parent = nullptr);
    void setScore(const Score &score);

private slots:
    void goBack();
    void downloadScore();

private:
    MainWindow *mainWindow;
    Score currentScore;

    QLabel *titleLabel;
    QLabel *composerLabel;
    QComboBox *formatComboBox;
    QRadioButton *highQualityRadio;
    QRadioButton *mediumQualityRadio;
    QRadioButton *lowQualityRadio;
    QPushButton *backButton;
    QPushButton *downloadButton;
};

#endif // PUBLISHPAGE_H

