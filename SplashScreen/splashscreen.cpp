#include "splashscreen.h"

SplashScreen::SplashScreen(QWidget *parent) : QWidget(parent)
{
    setFixedSize(500, 300);
    setStyleSheet("background-color: #121212; border-radius: 10px;");

    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *titleLabel = new QLabel("DNG Music Score", this);
    titleLabel->setStyleSheet("font-size: 24px; color: white; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);

    QLabel *loadingLabel = new QLabel("Loading...", this);
    loadingLabel->setStyleSheet("font-size: 14px; color: gray;");
    loadingLabel->setAlignment(Qt::AlignCenter);

    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addWidget(loadingLabel);
    layout->addStretch();

    QTimer::singleShot(5000, this, &SplashScreen::closeSplash);
}

void SplashScreen::closeSplash()
{
    emit splashFinished();
    close();
}
