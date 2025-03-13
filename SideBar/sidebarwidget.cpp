#include "sidebarwidget.h"
#include <QStyle>
#include <QApplication>
#include <QGraphicsDropShadowEffect>

SidebarButton::SidebarButton(const QString &text, const QIcon &icon, QWidget *parent)
    : QPushButton(parent), isActive(false)
{
    setFlat(true);
    setCursor(Qt::PointingHandCursor);
    setCheckable(true);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(15, 10, 15, 10);
    layout->setSpacing(15);

    iconLabel = new QLabel(this);
    iconLabel->setFixedSize(24, 24);
    QPixmap pixmap = icon.pixmap(24, 24);
    iconLabel->setPixmap(pixmap);

    textLabel = new QLabel(text, this);
    textLabel->setStyleSheet("color: #cccccc; font-size: 15px;");

    layout->addWidget(iconLabel);
    layout->addWidget(textLabel);
    layout->addStretch();

    setStyleSheet(
        "SidebarButton {"
        "   background-color: transparent;"
        "   border-radius: 8px;"
        "   text-align: left;"
        "   padding: 10px 15px;"
        "   margin: 3px 10px;"
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
            "   background-color: rgba(30, 144, 255, 0.2);"
            "   border-radius: 8px;"
            "   text-align: left;"
            "   padding: 10px 15px;"
            "   margin: 3px 10px;"
            "}"
            "SidebarButton:hover {"
            "   background-color: rgba(30, 144, 255, 0.3);"
            "}"
            );
        textLabel->setStyleSheet("color: #ffffff; font-size: 15px; font-weight: bold;");
    } else {
        setStyleSheet(
            "SidebarButton {"
            "   background-color: transparent;"
            "   border-radius: 8px;"
            "   text-align: left;"
            "   padding: 10px 15px;"
            "   margin: 3px 10px;"
            "}"
            "SidebarButton:hover {"
            "   background-color: rgba(255, 255, 255, 0.1);"
            "}"
            "SidebarButton:pressed {"
            "   background-color: rgba(255, 255, 255, 0.15);"
            "}"
            );
        textLabel->setStyleSheet("color: #cccccc; font-size: 15px;");
    }
}

SidebarWidget::SidebarWidget(QWidget *parent)
    : QWidget(parent), activeButtonIndex(0)
{
    setFixedWidth(220);
    setStyleSheet("background-color: #0a0a0a; border-right: 1px solid #333333;");

    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 25, 0, 25);
    mainLayout->setSpacing(5);

    // Logo and title
    QHBoxLayout *logoLayout = new QHBoxLayout();
    logoLayout->setContentsMargins(20, 0, 20, 25);

    QLabel *logoLabel = new QLabel(this);
    logoLabel->setFixedSize(36, 36);
    // You can set an actual logo here
    logoLabel->setStyleSheet("background-color: #1e90ff; border-radius: 10px;");

    QLabel *titleLabel = new QLabel("DNG Studios", this);
    titleLabel->setStyleSheet("color: white; font-size: 20px; font-weight: bold;");

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

    // Add a divider
    QFrame *divider = new QFrame(this);
    divider->setFrameShape(QFrame::HLine);
    divider->setFrameShadow(QFrame::Sunken);
    divider->setStyleSheet("background-color: #333333; margin: 20px 20px;");

    mainLayout->addWidget(divider);

    // Add settings button
    SidebarButton *settingsButton = new SidebarButton("Settings", QIcon::fromTheme("preferences-system"), this);
    mainLayout->addWidget(settingsButton);

    // Add help button
    SidebarButton *helpButton = new SidebarButton("Help", QIcon::fromTheme("help-contents"), this);
    mainLayout->addWidget(helpButton);

    mainLayout->addStretch();

    // Add user profile at bottom
    QWidget *userWidget = new QWidget(this);
    userWidget->setStyleSheet(
        "background-color: #111111;"
        "border-radius: 8px;"
        "margin: 10px 10px;"
        "padding: 10px;"
        );

    QHBoxLayout *userLayout = new QHBoxLayout(userWidget);

    QLabel *avatarLabel = new QLabel(userWidget);
    avatarLabel->setFixedSize(32, 32);
    avatarLabel->setStyleSheet("background-color: #1e90ff; border-radius: 16px;");

    QVBoxLayout *userInfoLayout = new QVBoxLayout();
    QLabel *nameLabel = new QLabel("User Name", userWidget);
    nameLabel->setStyleSheet("color: white; font-weight: bold;");
    QLabel *statusLabel = new QLabel("Online", userWidget);
    statusLabel->setStyleSheet("color: #00cc00; font-size: 12px;");

    userInfoLayout->addWidget(nameLabel);
    userInfoLayout->addWidget(statusLabel);

    userLayout->addWidget(avatarLabel);
    userLayout->addLayout(userInfoLayout);
    userLayout->addStretch();

    mainLayout->addWidget(userWidget);

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

