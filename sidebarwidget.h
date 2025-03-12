#ifndef SIDEBARWIDGET_H
#define SIDEBARWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QIcon>
#include <QVector>

class SidebarButton : public QPushButton
{
    Q_OBJECT

public:
    SidebarButton(const QString &text, const QIcon &icon, QWidget *parent = nullptr);
    void setActive(bool active);

private:
    QLabel *iconLabel;
    QLabel *textLabel;
    bool isActive;
};

class SidebarWidget : public QWidget
{
    Q_OBJECT

public:
    SidebarWidget(QWidget *parent = nullptr);
    void setActiveButton(int index);

signals:
    void navigationRequested(int index);

private:
    QVBoxLayout *mainLayout;
    QVector<SidebarButton*> buttons;
    int activeButtonIndex;
};

#endif // SIDEBARWIDGET_H

