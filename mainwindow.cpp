#include "mainwindow.h"
#include "scorepage.h"
#include "publishpage.h"
#include "sidebarwidget.h"
#include "scorecardwidget.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QDir>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), publishPage(nullptr), currentPage(0)
{
    // Set up file path for scores.json
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    scoresFilePath = dataPath + "/scores.json";

    setupUi();
    createActions();
    createMenus();
    createToolbars();
    loadScoresFromLocalStorage();
    populateScoreGrid();

    setWindowTitle("DNG Music Score");
    setMinimumSize(1000, 700);

    statusBar->showMessage("Ready");
}

MainWindow::~MainWindow()
{
    saveScoresToLocalStorage();
}

void MainWindow::setupUi()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Main layout
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Create sidebar
    sidebar = new SidebarWidget(this);

    // Create stacked widget for page navigation
    stackedWidget = new QStackedWidget(this);

    // Create main page
    mainPage = new QWidget(this);
    mainPage->setStyleSheet("background-color: #181818;");

    QVBoxLayout *pageLayout = new QVBoxLayout(mainPage);
    pageLayout->setContentsMargins(30, 30, 30, 30);
    pageLayout->setSpacing(20);

    // Header with title and buttons
    QHBoxLayout *headerLayout = new QHBoxLayout();

    titleLabel = new QLabel("My Scores", this);
    titleLabel->setStyleSheet("color: white; font-size: 28px; font-weight: bold;");

    addScoreButton = new QPushButton("+ New Score", this);
    addScoreButton->setCursor(Qt::PointingHandCursor);
    addScoreButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #2a82da;"
        "   color: white;"
        "   border-radius: 6px;"
        "   padding: 8px 16px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #3a92ea;"
        "}"
        );

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(addScoreButton);

    // Divider
    QFrame *divider = new QFrame(this);
    divider->setFrameShape(QFrame::HLine);
    divider->setFrameShadow(QFrame::Sunken);
    divider->setStyleSheet("background-color: #333333;");

    // Score grid in a scroll area
    scoreScrollArea = new QScrollArea(this);
    scoreScrollArea->setWidgetResizable(true);
    scoreScrollArea->setFrameShape(QFrame::NoFrame);
    scoreScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scoreScrollArea->setStyleSheet("background-color: transparent;");

    scoreGridWidget = new QWidget(scoreScrollArea);
    scoreGridLayout = new QGridLayout(scoreGridWidget);
    scoreGridLayout->setContentsMargins(0, 0, 0, 0);
    scoreGridLayout->setSpacing(20);

    scoreScrollArea->setWidget(scoreGridWidget);

    // Add components to page layout
    pageLayout->addLayout(headerLayout);
    pageLayout->addWidget(divider);
    pageLayout->addWidget(scoreScrollArea);

    // Add main page to stacked widget
    stackedWidget->addWidget(mainPage);

    // Add sidebar and stacked widget to main layout
    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(stackedWidget);

    // Status bar
    statusBar = new QStatusBar(this);
    statusBar->setStyleSheet("background-color: #111111; color: #cccccc;");
    setStatusBar(statusBar);

    // Connect signals and slots
    connect(addScoreButton, &QPushButton::clicked, this, &MainWindow::addNewScore);
    connect(sidebar, &SidebarWidget::navigationRequested, this, &MainWindow::handleSidebarNavigation);
}

void MainWindow::createActions()
{
    newAction = new QAction(QIcon::fromTheme("document-new"), "&New Score", this);
    newAction->setShortcut(QKeySequence::New);
    newAction->setStatusTip("Create a new score");
    connect(newAction, &QAction::triggered, this, &MainWindow::addNewScore);

    openAction = new QAction(QIcon::fromTheme("document-open"), "&Open Score", this);
    openAction->setShortcut(QKeySequence::Open);
    openAction->setStatusTip("Open an existing score");

    saveAction = new QAction(QIcon::fromTheme("document-save"), "&Save", this);
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setStatusTip("Save the current score");

    exitAction = new QAction(QIcon::fromTheme("application-exit"), "E&xit", this);
    exitAction->setShortcut(QKeySequence::Quit);
    exitAction->setStatusTip("Exit the application");
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    settingsAction = new QAction(QIcon::fromTheme("preferences-system"), "&Settings", this);
    settingsAction->setStatusTip("Configure application settings");

    helpAction = new QAction(QIcon::fromTheme("help-contents"), "&Help", this);
    helpAction->setStatusTip("Show help");
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    editMenu = menuBar()->addMenu("&Edit");

    viewMenu = menuBar()->addMenu("&View");

    helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction(helpAction);

    // Style the menu bar
    menuBar()->setStyleSheet(
        "QMenuBar {"
        "   background-color: #111111;"
        "   color: white;"
        "}"
        "QMenuBar::item {"
        "   background-color: transparent;"
        "   padding: 6px 10px;"
        "}"
        "QMenuBar::item:selected {"
        "   background-color: #2a82da;"
        "}"
        "QMenu {"
        "   background-color: #222222;"
        "   color: white;"
        "   border: 1px solid #333333;"
        "}"
        "QMenu::item {"
        "   padding: 6px 20px 6px 20px;"
        "}"
        "QMenu::item:selected {"
        "   background-color: #2a82da;"
        "}"
        );
}

void MainWindow::createToolbars()
{
    mainToolBar = addToolBar("Main Toolbar");
    mainToolBar->setMovable(false);
    mainToolBar->setStyleSheet(
        "QToolBar {"
        "   background-color: #111111;"
        "   border-bottom: 1px solid #333333;"
        "   spacing: 5px;"
        "}"
        "QToolButton {"
        "   background-color: transparent;"
        "   border: none;"
        "   border-radius: 4px;"
        "   padding: 6px;"
        "}"
        "QToolButton:hover {"
        "   background-color: rgba(255, 255, 255, 0.1);"
        "}"
        "QToolButton:pressed {"
        "   background-color: rgba(255, 255, 255, 0.15);"
        "}"
        );

    mainToolBar->addAction(newAction);
    mainToolBar->addAction(openAction);
    mainToolBar->addAction(saveAction);
    mainToolBar->addSeparator();
    mainToolBar->addAction(settingsAction);
    mainToolBar->addAction(helpAction);
}

void MainWindow::populateScoreGrid()
{
    // Clear existing items
    QLayoutItem *item;
    while ((item = scoreGridLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // Display scores in the grid layout
    int row = 0;
    int col = 0;
    int maxCols = 4; // Adjust based on window width

    for (int i = 0; i < scores.size(); ++i) {
        ScoreCardWidget *card = new ScoreCardWidget(scores[i], i, scoreGridWidget);

        connect(card, &ScoreCardWidget::scoreClicked, this, &MainWindow::openScore);
        connect(card, &ScoreCardWidget::downloadClicked, this, &MainWindow::downloadScore);

        scoreGridLayout->addWidget(card, row, col);

        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }

    // Add empty widget to fill remaining space
    if (scores.isEmpty()) {
        QLabel *emptyLabel = new QLabel("No scores yet. Click '+ New Score' to create one.", scoreGridWidget);
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet("color: #888888; font-size: 16px; padding: 40px;");
        scoreGridLayout->addWidget(emptyLabel, 0, 0, 1, maxCols);
    }
}

void MainWindow::createScoreCard(const Score &score, int index)
{
    ScoreCardWidget *card = new ScoreCardWidget(score, index, scoreGridWidget);

    connect(card, &ScoreCardWidget::scoreClicked, this, &MainWindow::openScore);
    connect(card, &ScoreCardWidget::downloadClicked, this, &MainWindow::downloadScore);

    // Calculate position in grid
    int maxCols = 4;
    int row = index / maxCols;
    int col = index % maxCols;

    scoreGridLayout->addWidget(card, row, col);
}

// Remove this method as we're not using scoreListWidget anymore
// void MainWindow::populateScoreList() { ... }

// Remove this method as we're not using scoreListWidget anymore
// void MainWindow::createScoreListItem(const Score &score, int index) { ... }

void MainWindow::addNewScore()
{
    // Create a dialog to get score details
    QDialog dialog(this);
    dialog.setWindowTitle("New Score");
    dialog.setStyleSheet(
        "QDialog {"
        "   background-color: #222222;"
        "}"
        "QLabel {"
        "   color: white;"
        "}"
        "QLineEdit {"
        "   background-color: #333333;"
        "   color: white;"
        "   border: 1px solid #444444;"
        "   border-radius: 4px;"
        "   padding: 6px;"
        "}"
        "QPushButton {"
        "   background-color: #2a82da;"
        "   color: white;"
        "   border-radius: 4px;"
        "   padding: 6px 12px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #3a92ea;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #1a72ca;"
        "}"
        );

    QVBoxLayout *dialogLayout = new QVBoxLayout(&dialog);
    QFormLayout *form = new QFormLayout();
    form->setSpacing(15);
    form->setContentsMargins(20, 20, 20, 20);

    QLineEdit *titleEdit = new QLineEdit(&dialog);
    titleEdit->setText("New Score");
    titleEdit->setMinimumWidth(300);
    form->addRow("Title:", titleEdit);

    QLineEdit *composerEdit = new QLineEdit(&dialog);
    composerEdit->setText("Unknown");
    form->addRow("Composer:", composerEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal, &dialog);

    dialogLayout->addLayout(form);
    dialogLayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        // Add the new score
        QDateTime currentTime = QDateTime::currentDateTime();
        QString lastEdited = "Just now";

        Score newScore(titleEdit->text(), composerEdit->text(), lastEdited);
        scores.append(newScore);

        // Create a new score page for this score
        ScorePage *scorePage = new ScorePage(this, newScore);
        scorePages.append(scorePage);
        stackedWidget->addWidget(scorePage);

        // Add to grid
        createScoreCard(newScore, scores.size() - 1);

        // Save to local storage
        saveScoresToLocalStorage();

        statusBar->showMessage("New score created", 3000);
    }
}

// Remove this method as we're not using scoreListWidget anymore
// void MainWindow::openScore(QListWidgetItem *item) { ... }

void MainWindow::openScore(int index)
{
    if (index >= 0 && index < scores.size()) {
        navigateToScorePage(scores[index]);
    }
}

void MainWindow::downloadScore(int index)
{
    if (index >= 0 && index < scores.size()) {
        navigateToPublishPage(scores[index]);
    }
}

void MainWindow::handleSidebarNavigation(int index)
{
    switch (index) {
    case 0: // Home
        navigateToMainPage();
        break;
    case 1: // Scores
        navigateToMainPage();
        break;
    case 2: // Publish
        if (!scores.isEmpty()) {
            navigateToPublishPage(scores[0]);
        } else {
            QMessageBox::information(this, "No Scores", "Create a score first before publishing.");
            navigateToMainPage();
        }
        break;
    }
}

void MainWindow::navigateToScorePage(const Score &score)
{
    // Find or create the score page
    int index = -1;
    for (int i = 0; i < scores.size(); ++i) {
        if (scores[i].title == score.title && scores[i].composer == score.composer) {
            index = i;
            break;
        }
    }

    if (index >= 0) {
        // Ensure we have a page for this score
        if (index >= scorePages.size()) {
            ScorePage *scorePage = new ScorePage(this, score);
            scorePages.append(scorePage);
            stackedWidget->addWidget(scorePage);
        }

        animatePageTransition(scorePages[index]);
        sidebar->setActiveButton(1); // Set Scores as active
        statusBar->showMessage("Viewing score: " + score.title);
    }
}

void MainWindow::navigateToPublishPage(const Score &score)
{
    // Create publish page if it doesn't exist
    if (!publishPage) {
        publishPage = new PublishPage(this);
        stackedWidget->addWidget(publishPage);
    }

    // Set the current score and show the page
    publishPage->setScore(score);
    animatePageTransition(publishPage);
    sidebar->setActiveButton(2); // Set Publish as active
    statusBar->showMessage("Publishing score: " + score.title);
}

void MainWindow::navigateToMainPage()
{
    animatePageTransition(mainPage);
    sidebar->setActiveButton(1); // Set Scores as active
    statusBar->showMessage("Ready");
}

void MainWindow::animatePageTransition(QWidget *nextPage)
{
    QWidget *currentWidget = stackedWidget->currentWidget();

    if (currentWidget == nextPage) {
        return;
    }

    // Create fade out effect for current widget
    QGraphicsOpacityEffect *currentEffect = new QGraphicsOpacityEffect(currentWidget);
    currentWidget->setGraphicsEffect(currentEffect);

    // Create fade in effect for next widget
    QGraphicsOpacityEffect *nextEffect = new QGraphicsOpacityEffect(nextPage);
    nextPage->setGraphicsEffect(nextEffect);
    nextEffect->setOpacity(0);

    // Show the next widget (with 0 opacity)
    stackedWidget->setCurrentWidget(nextPage);

    // Create animations
    QPropertyAnimation *fadeOutAnimation = new QPropertyAnimation(currentEffect, "opacity");
    fadeOutAnimation->setDuration(150);
    fadeOutAnimation->setStartValue(1.0);
    fadeOutAnimation->setEndValue(0.0);
    fadeOutAnimation->setEasingCurve(QEasingCurve::OutCubic);

    QPropertyAnimation *fadeInAnimation = new QPropertyAnimation(nextEffect, "opacity");
    fadeInAnimation->setDuration(150);
    fadeInAnimation->setStartValue(0.0);
    fadeInAnimation->setEndValue(1.0);
    fadeInAnimation->setEasingCurve(QEasingCurve::InCubic);

    // Create animation group
    QParallelAnimationGroup *animationGroup = new QParallelAnimationGroup(this);
    animationGroup->addAnimation(fadeOutAnimation);
    animationGroup->addAnimation(fadeInAnimation);

    // Connect finished signal to cleanup
    connect(animationGroup, &QParallelAnimationGroup::finished, [=]() {
        currentWidget->setGraphicsEffect(nullptr);
        nextPage->setGraphicsEffect(nullptr);
        delete currentEffect;
        delete nextEffect;
        delete animationGroup;
    });

    // Start animations
    animationGroup->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::saveScoresToLocalStorage()
{
    QJsonArray scoresArray;

    for (const Score &score : scores) {
        QJsonObject scoreObject;
        scoreObject["title"] = score.title;
        scoreObject["composer"] = score.composer;
        scoreObject["lastEdited"] = score.lastEdited;
        scoresArray.append(scoreObject);
    }

    QJsonDocument document(scoresArray);
    QFile file(scoresFilePath);

    if (file.open(QIODevice::WriteOnly)) {
        file.write(document.toJson());
        file.close();
    } else {
        qWarning("Could not save scores to file.");
    }
}

void MainWindow::loadScoresFromLocalStorage()
{
    scores.clear();
    scorePages.clear();

    QFile file(scoresFilePath);

    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();

        QJsonDocument document = QJsonDocument::fromJson(data);
        QJsonArray scoresArray = document.array();

        for (int i = 0; i < scoresArray.size(); ++i) {
            QJsonObject scoreObject = scoresArray[i].toObject();

            QString title = scoreObject["title"].toString();
            QString composer = scoreObject["composer"].toString();
            QString lastEdited = scoreObject["lastEdited"].toString();

            Score score(title, composer, lastEdited);
            scores.append(score);

            // Create a score page for this score
            ScorePage *scorePage = new ScorePage(this, score);
            scorePages.append(scorePage);
            stackedWidget->addWidget(scorePage);
        }
    }
}

