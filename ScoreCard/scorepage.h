#ifndef SCOREPAGE_H
#define SCOREPAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include "score.h"

class MainWindow;

class ScorePage : public QWidget
{
    Q_OBJECT

public:
    ScorePage(MainWindow *mainWindow, const Score &score, QWidget *parent = nullptr);

private slots:
    void goBack();
    void editScore();

private:
    MainWindow *mainWindow;
    Score score;

    QLabel *titleLabel;
    QLabel *composerLabel;
    QLabel *lastEditedLabel;
    QPushButton *backButton;
    QPushButton *editButton;
    QPushButton *downloadButton;
};

#endif // SCOREPAGE_H

