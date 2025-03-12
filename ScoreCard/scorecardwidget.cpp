#include "scorecardwidget.h"
#include <QEvent>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>

ScoreCardWidget::ScoreCardWidget(const Score &score, int index, QWidget *parent)
    : QWidget(parent), score(score), index(index)
{
    setFixedSize(220, 280);
    setCursor(Qt::PointingHandCursor);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    cardContent = new QWidget(this);
    cardContent->setObjectName("cardContent");
    cardContent->setStyleSheet(
        "#cardContent {"
        "   background-color: #222222;"
        "   border-radius: 8px;"
        "   border: 1px solid #333333;"
        "}"
        );

    QVBoxLayout *contentLayout = new QVBoxLayout(cardContent);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    // Score preview area (placeholder)
    QWidget *previewArea = new QWidget(cardContent);
    previewArea->setFixedHeight(160);
    previewArea->setStyleSheet(
        "background-color: #1a1a1a;"
        "border-top-left-radius: 8px;"
        "border-top-right-radius: 8px;"
        );

    QVBoxLayout *previewLayout = new QVBoxLayout(previewArea);
    previewLayout->setAlignment(Qt::AlignCenter);

    QLabel *iconLabel = new QLabel(previewArea);
    iconLabel->setPixmap(QIcon::fromTheme("audio-x-generic").pixmap(64, 64));
    iconLabel->setAlignment(Qt::AlignCenter);

    previewLayout->addWidget(iconLabel);

    // Score details area
    QWidget *detailsArea = new QWidget(cardContent);
    detailsArea->setStyleSheet(
        "background-color: #222222;"
        "border-bottom-left-radius: 8px;"
        "border-bottom-right-radius: 8px;"
        "padding: 12px;"
        );

    QVBoxLayout *detailsLayout = new QVBoxLayout(detailsArea);
    detailsLayout->setContentsMargins(12, 12, 12, 12);
    detailsLayout->setSpacing(4);

    titleLabel = new QLabel(score.title, detailsArea);
    titleLabel->setStyleSheet("color: white; font-size: 16px; font-weight: bold;");
    titleLabel->setWordWrap(true);

    composerLabel = new QLabel(score.composer, detailsArea);
    composerLabel->setStyleSheet("color: #bbbbbb; font-size: 13px;");

    lastEditedLabel = new QLabel("Last edited: " + score.lastEdited, detailsArea);
    lastEditedLabel->setStyleSheet("color: #888888; font-size: 11px;");

    detailsLayout->addWidget(titleLabel);
    detailsLayout->addWidget(composerLabel);
    detailsLayout->addWidget(lastEditedLabel);

    // Download button (initially hidden)
    downloadButton = new QPushButton(detailsArea);
    downloadButton->setIcon(QIcon::fromTheme("document-save"));
    downloadButton->setToolTip("Download Score");
    downloadButton->setCursor(Qt::PointingHandCursor);
    downloadButton->setFixedSize(32, 32);
    downloadButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #2a82da;"
        "   border-radius: 16px;"
        "   border: none;"
        "   padding: 4px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #3a92ea;"
        "}"
        );
    downloadButton->setVisible(false);
    downloadButton->raise();
    downloadButton->move(detailsArea->width() - 40, 10);

    // Add components to layouts
    contentLayout->addWidget(previewArea);
    contentLayout->addWidget(detailsArea);

    mainLayout->addWidget(cardContent);

    // Add shadow effect
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(15);
    shadowEffect->setColor(QColor(0, 0, 0, 80));
    shadowEffect->setOffset(0, 2);
    cardContent->setGraphicsEffect(shadowEffect);

    // Connect signals - Fix: Explicitly capture index in lambda
    int cardIndex = index;
    connect(downloadButton, &QPushButton::clicked, [this, cardIndex]() {
        emit downloadClicked(cardIndex);
    });
}

// Fix: Update enterEvent to handle both Qt5 and Qt6
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void ScoreCardWidget::enterEvent(QEnterEvent *event)
#else
void ScoreCardWidget::enterEvent(QEvent *event)
#endif
{
    QPropertyAnimation *animation = new QPropertyAnimation(cardContent, "pos");
    animation->setDuration(150);
    animation->setStartValue(cardContent->pos());
    animation->setEndValue(QPoint(cardContent->pos().x(), cardContent->pos().y() - 5));
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->start(QAbstractAnimation::DeleteWhenStopped);

    downloadButton->setVisible(true);

    QWidget::enterEvent(event);
}

void ScoreCardWidget::leaveEvent(QEvent *event)
{
    QPropertyAnimation *animation = new QPropertyAnimation(cardContent, "pos");
    animation->setDuration(150);
    animation->setStartValue(cardContent->pos());
    animation->setEndValue(QPoint(cardContent->pos().x(), cardContent->pos().y() + 5));
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->start(QAbstractAnimation::DeleteWhenStopped);

    downloadButton->setVisible(false);

    QWidget::leaveEvent(event);
}

void ScoreCardWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit scoreClicked(index);
    }

    QWidget::mousePressEvent(event);
}

