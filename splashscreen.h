#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>

class SplashScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SplashScreen(QWidget *parent = nullptr);

signals:
    void splashFinished();

private slots:
    void closeSplash();
};

#endif // SPLASHSCREEN_H
