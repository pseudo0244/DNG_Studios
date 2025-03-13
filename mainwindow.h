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

// Forward declarations
class ScorePage;
class PublishPage;
class SidebarWidget;
class ScoreCardWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Navigation methods
    void navigateToScorePage(const Score &score);
    void navigateToPublishPage(const Score &score);
    void navigateToMainPage();

    // Data persistence methods
    void saveScoresToLocalStorage();
    void loadScoresFromLocalStorage();

    // Public method to set status bar messages
    void setStatusMessage(const QString &message, int timeout = 0);

private slots:
    // Slots for handling UI interactions
    void addNewScore();
    void openScore(int scoreIndex);
    void downloadScore(int scoreIndex);
    void handleSidebarNavigation(int index);
    void filterScores(const QString &filter); // Added filterScores slot

private:
    // UI setup methods
    void setupUi();
    void createActions();
    void createMenus();
    void createToolbars();
    void populateScoreGrid();
    void createScoreCard(const Score &score, int index);
    void animatePageTransition(QWidget *nextPage);

    // UI components
    QWidget *centralWidget;
    QStackedWidget *stackedWidget;
    QWidget *mainPage;
    QScrollArea *scoreScrollArea;
    QWidget *scoreGridWidget;
    QGridLayout *scoreGridLayout;
    QPushButton *addScoreButton;
    QLabel *titleLabel;
    SidebarWidget *sidebar;

    // Actions for menus and toolbars
    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *exitAction;
    QAction *settingsAction;
    QAction *helpAction;

    // Menus
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;

    // Toolbars
    QToolBar *mainToolBar;

    // Data storage
    QVector<Score> scores;
    QVector<ScorePage*> scorePages;
    PublishPage *publishPage;

    // File path for storing scores
    QString scoresFilePath;

    // Current page index
    int currentPage;
};

#endif // MAINWINDOW_H
