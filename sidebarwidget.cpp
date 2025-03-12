#include "sidebarwidget.h"
#include <QStyle>
#include <QApplication>

SidebarButton::SidebarButton(const QString &text, const QIcon &icon, QWidget *parent)
    : QPushButton(parent), isActive(false)
{
    setFlat(true);
    setCursor(Qt::PointingHandCursor);
    setCheckable(true);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(12, 8, 12, 8);
    layout->setSpacing(12);

    iconLabel = new QLabel(this);
    iconLabel->setFixedSize(24, 24);
    QPixmap pixmap = icon.pixmap(24, 24);
    iconLabel->setPixmap(pixmap);

    textLabel = new QLabel(text, this);
    textLabel->setStyleSheet("color: #cccccc; font-size: 14px;");

    layout->addWidget(iconLabel);
    layout->addWidget(textLabel);
    layout->addStretch();

    setStyleSheet(
        "SidebarButton {"
        "   background-color: transparent;"
        "   border-radius: 6px;"
        "   text-align: left;"
        "   padding: 8px 12px;"
        "   margin: 2px 8px;"
        "}"
        "SidebarButton:hover {"
        "   background-color: rgba(255, 255, 255, 0.1);"
        "}"
        "SidebarButton:pressed {"
        "   background-color: rgba(255, 255, 255, 0.15);"
        "}"
        );
}

void SidebarButton::setActive(bool active)
{
    isActive = active;

    if (active) {
        setStyleSheet(
            "SidebarButton {"
            "   background-color: rgba(42, 130, 218, 0.2);"
            "   border-radius: 6px;"
            "   text-align: left;"
            "   padding: 8px 12px;"
            "   margin: 2px 8px;"
            "}"
            "SidebarButton:hover {"
            "   background-color: rgba(42, 130, 218, 0.3);"
            "}"
            );
        textLabel->setStyleSheet("color: #ffffff; font-size: 14px; font-weight: bold;");
    } else {
        setStyleSheet(
            "SidebarButton {"
            "   background-color: transparent;"
            "   border-radius: 6px;"
            "   text-align: left;"
            "   padding: 8px 12px;"
            "   margin: 2px 8px;"
            "}"
            "SidebarButton:hover {"
            "   background-color: rgba(255, 255, 255, 0.1);"
            "}"
            "SidebarButton:pressed {"
            "   background-color: rgba(255, 255, 255, 0.15);"
            "}"
            );
        textLabel->setStyleSheet("color: #cccccc; font-size: 14px;");
    }
}

SidebarWidget::SidebarWidget(QWidget *parent)
    : QWidget(parent), activeButtonIndex(0)
{
    setFixedWidth(200);
    setStyleSheet("background-color: #111111; border-right: 1px solid #333333;");

    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 20, 0, 20);
    mainLayout->setSpacing(4);

    // Logo and title
    QHBoxLayout *logoLayout = new QHBoxLayout();
    logoLayout->setContentsMargins(16, 0, 16, 20);

    QLabel *logoLabel = new QLabel(this);
    logoLabel->setFixedSize(32, 32);
    // You can set an actual logo here
    logoLabel->setStyleSheet("background-color: #2a82da; border-radius: 8px;");

    QLabel *titleLabel = new QLabel("DNG Studios", this);
    titleLabel->setStyleSheet("color: white; font-size: 18px; font-weight: bold;");

    logoLayout->addWidget(logoLabel);
    logoLayout->addWidget(titleLabel);
    logoLayout->addStretch();

    mainLayout->addLayout(logoLayout);

    // Navigation buttons
    SidebarButton *homeButton = new SidebarButton("Home", QIcon::fromTheme("go-home"), this);
    SidebarButton *scoresButton = new SidebarButton("Scores", QIcon::fromTheme("folder-music"), this);
    SidebarButton *publishButton = new SidebarButton("Publish", QIcon::fromTheme("document-send"), this);

    buttons.append(homeButton);
    buttons.append(scoresButton);
    buttons.append(publishButton);

    mainLayout->addWidget(homeButton);
    mainLayout->addWidget(scoresButton);
    mainLayout->addWidget(publishButton);
    mainLayout->addStretch();

    // Set initial active button
    setActiveButton(0);

    // Connect signals
    connect(homeButton, &QPushButton::clicked, [this]() {
        setActiveButton(0);
        emit navigationRequested(0);
    });

    connect(scoresButton, &QPushButton::clicked, [this]() {
        setActiveButton(1);
        emit navigationRequested(1);
    });

    connect(publishButton, &QPushButton::clicked, [this]() {
        setActiveButton(2);
        emit navigationRequested(2);
    });
}

void SidebarWidget::setActiveButton(int index)
{
    if (index >= 0 && index < buttons.size()) {
        if (activeButtonIndex >= 0 && activeButtonIndex < buttons.size()) {
            buttons[activeButtonIndex]->setActive(false);
        }

        buttons[index]->setActive(true);
        activeButtonIndex = index;
    }
}

