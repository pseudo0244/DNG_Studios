#ifndef SCORECARDWIDGET_H
#define SCORECARDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "score.h"

class ScoreCardWidget : public QWidget
{
    Q_OBJECT

public:
    ScoreCardWidget(const Score &score, int index, QWidget *parent = nullptr);

signals:
    void scoreClicked(int index);
    void downloadClicked(int index);

protected:
           // Handle both Qt5 and Qt6
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent *event) override;
#else
    void enterEvent(QEvent *event) override;
#endif
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    Score score;
    int index;
    QLabel *titleLabel;
    QLabel *composerLabel;
    QLabel *lastEditedLabel;
    QPushButton *downloadButton;
    QWidget *cardContent;
};

#endif // SCORECARDWIDGET_H

