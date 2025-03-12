#include <QApplication>
#include <QSplashScreen>
#include <QTimer>
#include <QPixmap>
#include <QPainter>
#include <QFont>
#include <QMovie>
#include <QLabel>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application-wide dark theme
    app.setStyle("Fusion");
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(18, 18, 18));
    darkPalette.setColor(QPalette::AlternateBase, QColor(35, 35, 35));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(35, 35, 35));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    app.setPalette(darkPalette);

    // Create custom splash screen
    QPixmap pixmap(800, 500);

    // Create gradient background
    QLinearGradient gradient(QPointF(0, 0), QPointF(800, 500));
    gradient.setColorAt(0, QColor(35, 35, 35));
    gradient.setColorAt(0.5, QColor(25, 25, 25));
    gradient.setColorAt(1, QColor(18, 18, 18));

    QPainter painter(&pixmap);
    painter.fillRect(pixmap.rect(), gradient);

    // Add a subtle glow effect
    QRadialGradient glowGradient(QPointF(400, 200), 300);
    glowGradient.setColorAt(0, QColor(42, 130, 218, 40));
    glowGradient.setColorAt(1, QColor(42, 130, 218, 0));
    painter.fillRect(pixmap.rect(), glowGradient);

    // Draw text with modern styling
    painter.setPen(Qt::white);
    QFont titleFont("Segoe UI", 48, QFont::Bold);
    painter.setFont(titleFont);
    painter.drawText(QRect(0, 150, 800, 80), Qt::AlignCenter, "DNG Studios");

    QFont subtitleFont("Segoe UI", 16);
    painter.setFont(subtitleFont);
    painter.drawText(QRect(0, 240, 800, 30), Qt::AlignCenter, "Music Score Editor");

    QSplashScreen splash(pixmap);

    // Create loading animation
    QLabel *loadingLabel = new QLabel(&splash);
    QMovie *loadingMovie = new QMovie(":/resources/loading.gif");
    if (!loadingMovie->isValid()) {
        // If gif doesn't exist, create a text-based loading indicator
        loadingLabel->setText("Loading...");
        loadingLabel->setStyleSheet("color: white; font-family: 'Segoe UI'; font-size: 14px;");
        loadingLabel->setAlignment(Qt::AlignCenter);
        loadingLabel->setGeometry(0, 300, 800, 30);
    } else {
        loadingMovie->start();
        loadingLabel->setMovie(loadingMovie);
        loadingLabel->setGeometry(350, 300, 100, 100);
    }

    splash.show();

    // Ensure splash is shown
    app.processEvents();

    // Create main window but don't show it yet
    MainWindow mainWindow;

    // Wait for 3 seconds
    QTimer::singleShot(3000, [&]() {
        splash.close();
        mainWindow.showMaximized(); // Show maximized for better initial experience
    });

    return app.exec();
}

