#include "scorepage.h"
#include "mainwindow.h"

ScorePage::ScorePage(MainWindow *mainWindow, const Score &score, QWidget *parent)
    : QWidget(parent), mainWindow(mainWindow), score(score)
{
    setStyleSheet("background-color: #181818;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    // Header with back button and title
    QHBoxLayout *headerLayout = new QHBoxLayout();

    backButton = new QPushButton("Back", this);
    backButton->setIcon(QIcon::fromTheme("go-previous"));
    backButton->setCursor(Qt::PointingHandCursor);
    backButton->setStyleSheet(
        "QPushButton {"
        "   background-color: transparent;"
        "   color: white;"
        "   border: 1px solid #444444;"
        "   border-radius: 4px;"
        "   padding: 6px 12px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(255, 255, 255, 0.1);"
        "}"
        );

    titleLabel = new QLabel(score.title, this);
    titleLabel->setStyleSheet("color: white; font-size: 28px; font-weight: bold;");

    downloadButton = new QPushButton("Download", this);
    downloadButton->setIcon(QIcon::fromTheme("document-save"));
    downloadButton->setCursor(Qt::PointingHandCursor);
    downloadButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #2a82da;"
        "   color: white;"
        "   border-radius: 4px;"
        "   padding: 6px 12px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #3a92ea;"
        "}"
        );

    headerLayout->addWidget(backButton);
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(downloadButton);

    // Divider
    QFrame *divider = new QFrame(this);
    divider->setFrameShape(QFrame::HLine);
    divider->setFrameShadow(QFrame::Sunken);
    divider->setStyleSheet("background-color: #333333;");

    // Score details
    QHBoxLayout *detailsLayout = new QHBoxLayout();

    composerLabel = new QLabel("Composer: " + score.composer, this);
    composerLabel->setStyleSheet("color: #bbbbbb; font-size: 16px;");

    lastEditedLabel = new QLabel("Last edited: " + score.lastEdited, this);
    lastEditedLabel->setStyleSheet("color: #888888; font-size: 14px;");

    detailsLayout->addWidget(composerLabel);
    detailsLayout->addStretch();
    detailsLayout->addWidget(lastEditedLabel);

    // Score content area
    QScrollArea *scoreScrollArea = new QScrollArea(this);
    scoreScrollArea->setWidgetResizable(true);
    scoreScrollArea->setFrameShape(QFrame::NoFrame);
    scoreScrollArea->setStyleSheet("background-color: #222222; border-radius: 8px;");

    QWidget *scoreContent = new QWidget(scoreScrollArea);
    QVBoxLayout *contentLayout = new QVBoxLayout(scoreContent);

    // Placeholder for score content
    QLabel *placeholderLabel = new QLabel("Score content will be displayed here", scoreContent);
    placeholderLabel->setAlignment(Qt::AlignCenter);
    placeholderLabel->setStyleSheet("color: #888888; font-size: 18px; padding: 100px;");

    contentLayout->addWidget(placeholderLabel);
    scoreScrollArea->setWidget(scoreContent);

    // Edit button
    editButton = new QPushButton("Edit Score", this);
    editButton->setIcon(QIcon::fromTheme("document-edit"));
    editButton->setCursor(Qt::PointingHandCursor);
    editButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #2a82da;"
        "   color: white;"
        "   border-radius: 4px;"
        "   padding: 8px 16px;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #3a92ea;"
        "}"
        );

    // Add all layouts to main layout
    mainLayout->addLayout(headerLayout);
    mainLayout->addWidget(divider);
    mainLayout->addLayout(detailsLayout);
    mainLayout->addWidget(scoreScrollArea, 1);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(editButton);

    mainLayout->addLayout(buttonLayout);

    // Connect signals
    connect(backButton, &QPushButton::clicked, this, &ScorePage::goBack);
    connect(editButton, &QPushButton::clicked, this, &ScorePage::editScore);

    // Fix: Explicitly capture mainWindow in lambda
    MainWindow *mainWindowPtr = mainWindow;
    connect(downloadButton, &QPushButton::clicked, [mainWindowPtr, this]() {
        mainWindowPtr->navigateToPublishPage(this->score);
    });
}

void ScorePage::goBack()
{
    mainWindow->navigateToMainPage();
}

void ScorePage::editScore()
{
    // Placeholder for edit functionality
    // In a real app, this would open the score editor
}

