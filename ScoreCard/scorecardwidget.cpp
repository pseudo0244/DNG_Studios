#include "scorecardwidget.h"
#include <QEvent>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

ScoreCardWidget::ScoreCardWidget(const Score &score, int index, QWidget *parent)
    : QWidget(parent), score(score), index(index)
{
    setFixedSize(250, 320);
    setCursor(Qt::PointingHandCursor);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    cardContent = new QWidget(this);
    cardContent->setObjectName("cardContent");
    cardContent->setStyleSheet(
        "#cardContent {"
        "   background-color: #1a1a1a;"
        "   border-radius: 12px;"
        "   border: 1px solid #333333;"
        "}"
        );

    QVBoxLayout *contentLayout = new QVBoxLayout(cardContent);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    // Score preview area with gradient background
    QWidget *previewArea = new QWidget(cardContent);
    previewArea->setFixedHeight(180);
    previewArea->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #1e3c72, stop:1 #2a5298);"
        "border-top-left-radius: 12px;"
        "border-top-right-radius: 12px;"
        );

    QVBoxLayout *previewLayout = new QVBoxLayout(previewArea);
    previewLayout->setAlignment(Qt::AlignCenter);

    QLabel *iconLabel = new QLabel(previewArea);
    iconLabel->setPixmap(QIcon::fromTheme("audio-x-generic").pixmap(80, 80));
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet("background-color: transparent;");

    previewLayout->addWidget(iconLabel);

    // Score details area with modern design
    QWidget *detailsArea = new QWidget(cardContent);
    detailsArea->setStyleSheet(
        "background-color: #1a1a1a;"
        "border-bottom-left-radius: 12px;"
        "border-bottom-right-radius: 12px;"
        "padding: 15px;"
        );

    QVBoxLayout *detailsLayout = new QVBoxLayout(detailsArea);
    detailsLayout->setContentsMargins(15, 15, 15, 15);
    detailsLayout->setSpacing(8);

    titleLabel = new QLabel(score.title, detailsArea);
    titleLabel->setStyleSheet("color: white; font-size: 18px; font-weight: bold;");
    titleLabel->setWordWrap(true);

    composerLabel = new QLabel(score.composer, detailsArea);
    composerLabel->setStyleSheet("color: #bbbbbb; font-size: 14px;");

    lastEditedLabel = new QLabel("Last edited: " + score.lastEdited, detailsArea);
    lastEditedLabel->setStyleSheet("color: #888888; font-size: 12px;");

    // Add action buttons
    QHBoxLayout *actionLayout = new QHBoxLayout();
    actionLayout->setContentsMargins(0, 10, 0, 0);
    actionLayout->setSpacing(10);

    QPushButton *editButton = new QPushButton(detailsArea);
    editButton->setIcon(QIcon::fromTheme("document-edit"));
    editButton->setToolTip("Edit Score");
    editButton->setCursor(Qt::PointingHandCursor);
    editButton->setFixedSize(36, 36);
    editButton->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(255, 255, 255, 0.1);"
        "   border-radius: 18px;"
        "   border: none;"
        "   padding: 8px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(255, 255, 255, 0.2);"
        "}"
        );

    downloadButton = new QPushButton(detailsArea);
    downloadButton->setIcon(QIcon::fromTheme("document-save"));
    downloadButton->setToolTip("Download Score");
    downloadButton->setCursor(Qt::PointingHandCursor);
    downloadButton->setFixedSize(36, 36);
    downloadButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #1e90ff;"
        "   border-radius: 18px;"
        "   border: none;"
        "   padding: 8px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #3aa0ff;"
        "}"
        );

    actionLayout->addStretch();
    actionLayout->addWidget(editButton);
    actionLayout->addWidget(downloadButton);

    detailsLayout->addWidget(titleLabel);
    detailsLayout->addWidget(composerLabel);
    detailsLayout->addWidget(lastEditedLabel);
    detailsLayout->addLayout(actionLayout);

    // Add components to layouts
    contentLayout->addWidget(previewArea);
    contentLayout->addWidget(detailsArea);

    mainLayout->addWidget(cardContent);

    // Connect signals - Fix: Explicitly capture index in lambda
    int cardIndex = index;
    connect(downloadButton, &QPushButton::clicked, [this, cardIndex]() {
        emit downloadClicked(cardIndex);
    });

    connect(editButton, &QPushButton::clicked, [this, cardIndex]() {
        emit scoreClicked(cardIndex);
    });
}

// Fix: Update enterEvent to handle both Qt5 and Qt6
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void ScoreCardWidget::enterEvent(QEnterEvent *event)
#else
void ScoreCardWidget::enterEvent(QEvent *event)
#endif
{
    // Create a parallel animation group for multiple animations
    QParallelAnimationGroup *animGroup = new QParallelAnimationGroup(this);

    // Scale up animation
    QPropertyAnimation *scaleAnim = new QPropertyAnimation(cardContent, "geometry");
    scaleAnim->setDuration(200);
    QRect startRect = cardContent->geometry();
    QRect endRect = startRect.adjusted(-5, -5, 5, 5);
    scaleAnim->setStartValue(startRect);
    scaleAnim->setEndValue(endRect);
    scaleAnim->setEasingCurve(QEasingCurve::OutCubic);
    animGroup->addAnimation(scaleAnim);

    // Move up animation
    QPropertyAnimation *moveAnim = new QPropertyAnimation(cardContent, "pos");
    moveAnim->setDuration(200);
    moveAnim->setStartValue(cardContent->pos());
    moveAnim->setEndValue(QPoint(cardContent->pos().x(), cardContent->pos().y() - 5));
    moveAnim->setEasingCurve(QEasingCurve::OutCubic);
    animGroup->addAnimation(moveAnim);

    // Start the animation group
    animGroup->start(QAbstractAnimation::DeleteWhenStopped);

    QWidget::enterEvent(event);
}

void ScoreCardWidget::leaveEvent(QEvent *event)
{
    // Create a parallel animation group for multiple animations
    QParallelAnimationGroup *animGroup = new QParallelAnimationGroup(this);

    // Scale down animation
    QPropertyAnimation *scaleAnim = new QPropertyAnimation(cardContent, "geometry");
    scaleAnim->setDuration(200);
    QRect endRect = cardContent->geometry().adjusted(5, 5, -5, -5);
    scaleAnim->setStartValue(cardContent->geometry());
    scaleAnim->setEndValue(endRect);
    scaleAnim->setEasingCurve(QEasingCurve::OutCubic);
    animGroup->addAnimation(scaleAnim);

    // Move down animation
    QPropertyAnimation *moveAnim = new QPropertyAnimation(cardContent, "pos");
    moveAnim->setDuration(200);
    moveAnim->setStartValue(cardContent->pos());
    moveAnim->setEndValue(QPoint(cardContent->pos().x(), cardContent->pos().y() + 5));
    moveAnim->setEasingCurve(QEasingCurve::OutCubic);
    animGroup->addAnimation(moveAnim);

    // Start the animation group
    animGroup->start(QAbstractAnimation::DeleteWhenStopped);

    QWidget::leaveEvent(event);
}

void ScoreCardWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit scoreClicked(index);
    }

    QWidget::mousePressEvent(event);
}

