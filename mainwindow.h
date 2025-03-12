#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QIcon>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDialog>
#include <QLineEdit>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QVector>
#include <QStackedWidget>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QScrollArea>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>
#include "score.h"

class ScorePage;
class PublishPage;
class SidebarWidget;
class ScoreCardWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void navigateToScorePage(const Score &score);
    void navigateToPublishPage(const Score &score);
    void navigateToMainPage();
    void saveScoresToLocalStorage();
    void loadScoresFromLocalStorage();

private slots:
    void addNewScore();
    void openScore(int scoreIndex);
    void downloadScore(int scoreIndex);
    void handleSidebarNavigation(int index);

private:
    void setupUi();
    void createActions();
    void createMenus();
    void createToolbars();
    void populateScoreGrid();
    void createScoreCard(const Score &score, int index);
    void animatePageTransition(QWidget *nextPage);

    QWidget *centralWidget;
    QStackedWidget *stackedWidget;
    QWidget *mainPage;
    QScrollArea *scoreScrollArea;
    QWidget *scoreGridWidget;
    QGridLayout *scoreGridLayout;
    QPushButton *addScoreButton;
    QLabel *titleLabel;
    SidebarWidget *sidebar;

    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *exitAction;
    QAction *settingsAction;
    QAction *helpAction;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;

    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    QVector<Score> scores;
    QVector<ScorePage*> scorePages;
    PublishPage *publishPage;

    QString scoresFilePath;
    int currentPage;
};

#endif // MAINWINDOW_H

