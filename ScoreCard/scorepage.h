#ifndef SCOREPAGE_H
#define SCOREPAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QToolBar>
#include <QAction>
#include <QComboBox>
#include <QSlider>
#include <QSpinBox>
#include <QGroupBox>
#include <QTabWidget>
#include <QSplitter>
#include <QGraphicsView>
#include <QGraphicsScene>
#include "score.h"

class MainWindow;

class ScorePage : public QWidget
{
    Q_OBJECT

public:
    ScorePage(MainWindow *mainWindow, const Score &score, QWidget *parent = nullptr);

private slots:
    void goBack();
    void editScore();
    void playScore();
    void pauseScore();
    void stopScore();
    void toggleMetronome();
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void togglePropertiesPanel();
    void changeNoteDuration(int index);
    void changeClef(int index);
    void changeKeySignature(int index);
    void updateTempo(int tempo);

private:
    void setupToolbar();
    void setupScoreArea();
    void setupPropertiesPanel();
    void createNoteToolbar();
    void createPlaybackControls();
    void createEditingTools();

    MainWindow *mainWindow;
    Score score;

    // Header components
    QLabel *titleLabel;
    QLabel *composerLabel;
    QLabel *lastEditedLabel;
    QPushButton *backButton;
    QPushButton *editButton;
    QPushButton *downloadButton;

    // Toolbar components
    QToolBar *mainToolBar;
    QToolBar *noteToolbar;
    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *exportAction;
    QAction *undoAction;
    QAction *redoAction;
    QAction *playAction;
    QAction *pauseAction;
    QAction *stopAction;
    QAction *metronomeAction;
    QAction *zoomInAction;
    QAction *zoomOutAction;
    QAction *zoomResetAction;
    QComboBox *noteDurationCombo;
    QComboBox *clefCombo;
    QComboBox *keySignatureCombo;

    // Score display area
    QSplitter *mainSplitter;
    QGraphicsView *scoreView;
    QGraphicsScene *scoreScene;
    double zoomFactor;

    // Properties panel
    QWidget *propertiesPanel;
    QPushButton *togglePropertiesPanelButton;
    QTabWidget *propertiesTabs;
    QSpinBox *tempoSpinBox;
    QSlider *tempoSlider;
    bool propertiesPanelVisible;
};

#endif // SCOREPAGE_H

