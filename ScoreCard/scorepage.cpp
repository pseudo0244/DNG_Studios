#include "scorepage.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QStatusBar>
#include <QGraphicsRectItem>

ScorePage::ScorePage(MainWindow *mainWindow, const Score &score, QWidget *parent)
    : QWidget(parent), mainWindow(mainWindow), score(score), zoomFactor(1.0), propertiesPanelVisible(true)
{
    setStyleSheet("background-color: #000000;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Header with back button and title
    QWidget *headerWidget = new QWidget(this);
    headerWidget->setStyleSheet("background-color: #111111; border-bottom: 1px solid #333333;");
    headerWidget->setFixedHeight(60);

    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(20, 10, 20, 10);
    headerLayout->setSpacing(15);

    backButton = new QPushButton("Back", this);
    backButton->setIcon(QIcon::fromTheme("go-previous"));
    backButton->setCursor(Qt::PointingHandCursor);
    backButton->setStyleSheet(
        "QPushButton {"
        "   background-color: transparent;"
        "   color: white;"
        "   border: 1px solid #444444;"
        "   border-radius: 4px;"
        "   padding: 6px 12px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(255, 255, 255, 0.1);"
        "}"
        );

    titleLabel = new QLabel(score.title, this);
    titleLabel->setStyleSheet("color: white; font-size: 20px; font-weight: bold;");

    composerLabel = new QLabel("by " + score.composer, this);
    composerLabel->setStyleSheet("color: #bbbbbb; font-size: 14px;");

    lastEditedLabel = new QLabel("Last edited: " + score.lastEdited, this);
    lastEditedLabel->setStyleSheet("color: #888888; font-size: 12px;");

    downloadButton = new QPushButton("Download", this);
    downloadButton->setIcon(QIcon::fromTheme("document-save"));
    downloadButton->setCursor(Qt::PointingHandCursor);
    downloadButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #1e90ff;"
        "   color: white;"
        "   border-radius: 4px;"
        "   padding: 6px 12px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #3aa0ff;"
        "}"
        );

    headerLayout->addWidget(backButton);
    headerLayout->addWidget(titleLabel);
    headerLayout->addWidget(composerLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(lastEditedLabel);
    headerLayout->addWidget(downloadButton);

    // Setup toolbar
    setupToolbar();

    // Main content area with score and properties panel
    mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->setHandleWidth(1);
    mainSplitter->setStyleSheet(
        "QSplitter::handle {"
        "   background-color: #333333;"
        "}"
        );

    // Setup score area
    setupScoreArea();

    // Setup properties panel
    setupPropertiesPanel();

    // Add widgets to main layout
    mainLayout->addWidget(headerWidget);
    mainLayout->addWidget(mainToolBar);
    mainLayout->addWidget(noteToolbar);
    mainLayout->addWidget(mainSplitter, 1);

    // Connect signals
    connect(backButton, &QPushButton::clicked, this, &ScorePage::goBack);

    // Fix: Explicitly capture mainWindow in lambda
    MainWindow *mainWindowPtr = mainWindow;
    connect(downloadButton, &QPushButton::clicked, [mainWindowPtr, this]() {
        mainWindowPtr->navigateToPublishPage(this->score);
    });
}

void ScorePage::setupToolbar()
{
    // Main toolbar
    mainToolBar = new QToolBar(this);
    mainToolBar->setMovable(false);
    mainToolBar->setIconSize(QSize(20, 20));
    mainToolBar->setStyleSheet(
        "QToolBar {"
        "   background-color: #222222;"
        "   border: none;"
        "   spacing: 2px;"
        "   padding: 2px;"
        "}"
        "QToolButton {"
        "   background-color: transparent;"
        "   border: none;"
        "   border-radius: 4px;"
        "   padding: 6px;"
        "   color: white;"
        "}"
        "QToolButton:hover {"
        "   background-color: rgba(255, 255, 255, 0.1);"
        "}"
        "QToolButton:pressed {"
        "   background-color: rgba(255, 255, 255, 0.15);"
        "}"
        "QToolButton:checked {"
        "   background-color: rgba(30, 144, 255, 0.3);"
        "}"
        );

    // Create file actions
    newAction = mainToolBar->addAction(QIcon::fromTheme("document-new"), "New");
    openAction = mainToolBar->addAction(QIcon::fromTheme("document-open"), "Open");
    saveAction = mainToolBar->addAction(QIcon::fromTheme("document-save"), "Save");
    exportAction = mainToolBar->addAction(QIcon::fromTheme("document-export"), "Export");

    mainToolBar->addSeparator();

    // Create edit actions
    undoAction = mainToolBar->addAction(QIcon::fromTheme("edit-undo"), "Undo");
    redoAction = mainToolBar->addAction(QIcon::fromTheme("edit-redo"), "Redo");

    mainToolBar->addSeparator();

    // Create playback controls
    createPlaybackControls();

    mainToolBar->addSeparator();

    // Create zoom controls
    zoomInAction = mainToolBar->addAction(QIcon::fromTheme("zoom-in"), "Zoom In");
    zoomOutAction = mainToolBar->addAction(QIcon::fromTheme("zoom-out"), "Zoom Out");
    zoomResetAction = mainToolBar->addAction(QIcon::fromTheme("zoom-original"), "Reset Zoom");

    // Connect toolbar actions
    connect(zoomInAction, &QAction::triggered, this, &ScorePage::zoomIn);
    connect(zoomOutAction, &QAction::triggered, this, &ScorePage::zoomOut);
    connect(zoomResetAction, &QAction::triggered, this, &ScorePage::resetZoom);

    // Create note toolbar
    createNoteToolbar();
}

void ScorePage::createPlaybackControls()
{
    playAction = mainToolBar->addAction(QIcon::fromTheme("media-playback-start"), "Play");
    pauseAction = mainToolBar->addAction(QIcon::fromTheme("media-playback-pause"), "Pause");
    stopAction = mainToolBar->addAction(QIcon::fromTheme("media-playback-stop"), "Stop");
    metronomeAction = mainToolBar->addAction(QIcon::fromTheme("chronometer"), "Metronome");
    metronomeAction->setCheckable(true);

    // Connect playback actions
    connect(playAction, &QAction::triggered, this, &ScorePage::playScore);
    connect(pauseAction, &QAction::triggered, this, &ScorePage::pauseScore);
    connect(stopAction, &QAction::triggered, this, &ScorePage::stopScore);
    connect(metronomeAction, &QAction::triggered, this, &ScorePage::toggleMetronome);
}

void ScorePage::createNoteToolbar()
{
    noteToolbar = new QToolBar(this);
    noteToolbar->setMovable(false);
    noteToolbar->setIconSize(QSize(20, 20));
    noteToolbar->setStyleSheet(
        "QToolBar {"
        "   background-color: #1a1a1a;"
        "   border: none;"
        "   spacing: 2px;"
        "   padding: 2px 10px;"
        "}"
        "QToolButton {"
        "   background-color: transparent;"
        "   border: none;"
        "   border-radius: 4px;"
        "   padding: 6px;"
        "   color: white;"
        "}"
        "QToolButton:hover {"
        "   background-color: rgba(255, 255, 255, 0.1);"
        "}"
        "QToolButton:pressed {"
        "   background-color: rgba(255, 255, 255, 0.15);"
        "}"
        "QToolButton:checked {"
        "   background-color: rgba(30, 144, 255, 0.3);"
        "}"
        "QComboBox {"
        "   background-color: #333333;"
        "   color: white;"
        "   border: 1px solid #444444;"
        "   border-radius: 4px;"
        "   padding: 4px 8px;"
        "   min-width: 100px;"
        "}"
        "QComboBox::drop-down {"
        "   border: none;"
        "   width: 20px;"
        "}"
        "QComboBox QAbstractItemView {"
        "   background-color: #333333;"
        "   color: white;"
        "   selection-background-color: #1e90ff;"
        "}"
        );

    // Note duration selector
    QLabel *durationLabel = new QLabel("Duration:", this);
    durationLabel->setStyleSheet("color: white;");
    noteToolbar->addWidget(durationLabel);

    noteDurationCombo = new QComboBox(this);
    noteDurationCombo->addItem("Whole Note");
    noteDurationCombo->addItem("Half Note");
    noteDurationCombo->addItem("Quarter Note");
    noteDurationCombo->addItem("Eighth Note");
    noteDurationCombo->addItem("Sixteenth Note");
    noteToolbar->addWidget(noteDurationCombo);

    noteToolbar->addSeparator();

    // Clef selector
    QLabel *clefLabel = new QLabel("Clef:", this);
    clefLabel->setStyleSheet("color: white;");
    noteToolbar->addWidget(clefLabel);

    clefCombo = new QComboBox(this);
    clefCombo->addItem("Treble");
    clefCombo->addItem("Bass");
    clefCombo->addItem("Alto");
    clefCombo->addItem("Tenor");
    noteToolbar->addWidget(clefCombo);

    noteToolbar->addSeparator();

    // Key signature selector
    QLabel *keyLabel = new QLabel("Key:", this);
    keyLabel->setStyleSheet("color: white;");
    noteToolbar->addWidget(keyLabel);

    keySignatureCombo = new QComboBox(this);
    keySignatureCombo->addItem("C Major");
    keySignatureCombo->addItem("G Major");
    keySignatureCombo->addItem("D Major");
    keySignatureCombo->addItem("A Major");
    keySignatureCombo->addItem("E Major");
    keySignatureCombo->addItem("B Major");
    keySignatureCombo->addItem("F# Major");
    keySignatureCombo->addItem("C# Major");
    keySignatureCombo->addItem("F Major");
    keySignatureCombo->addItem("Bb Major");
    keySignatureCombo->addItem("Eb Major");
    keySignatureCombo->addItem("Ab Major");
    keySignatureCombo->addItem("Db Major");
    keySignatureCombo->addItem("Gb Major");
    keySignatureCombo->addItem("Cb Major");
    noteToolbar->addWidget(keySignatureCombo);

    // Connect note toolbar actions
    connect(noteDurationCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ScorePage::changeNoteDuration);
    connect(clefCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ScorePage::changeClef);
    connect(keySignatureCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ScorePage::changeKeySignature);
}

void ScorePage::setupScoreArea()
{
    // Create score view
    scoreView = new QGraphicsView(this);
    scoreView->setRenderHint(QPainter::Antialiasing);
    scoreView->setRenderHint(QPainter::TextAntialiasing);
    scoreView->setRenderHint(QPainter::SmoothPixmapTransform);
    scoreView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    scoreView->setDragMode(QGraphicsView::NoDrag);
    scoreView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    scoreView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scoreView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scoreView->setStyleSheet(
        "QGraphicsView {"
        "   background-color: #111111;"
        "   border: none;"
        "}"
        "QScrollBar:horizontal {"
        "   background-color: #222222;"
        "   height: 12px;"
        "}"
        "QScrollBar:vertical {"
        "   background-color: #222222;"
        "   width: 12px;"
        "}"
        "QScrollBar::handle {"
        "   background-color: #444444;"
        "   border-radius: 6px;"
        "}"
        "QScrollBar::handle:hover {"
        "   background-color: #555555;"
        "}"
        );

    // Create score scene
    scoreScene = new QGraphicsScene(this);
    scoreScene->setBackgroundBrush(QColor("#111111"));
    scoreView->setScene(scoreScene);

    // Fix: Create a white sheet for the score - properly allocate and add to scene
    QGraphicsRectItem *sheet = scoreScene->addRect(0, 0, 800, 1100, QPen(QColor("#dddddd"), 1), QBrush(QColor("#ffffff")));

    // Add shadow effect to the sheet
    QGraphicsDropShadowEffect *sheetShadow = new QGraphicsDropShadowEffect();
    sheetShadow->setBlurRadius(20);
    sheetShadow->setColor(QColor(0, 0, 0, 100));
    sheetShadow->setOffset(5, 5);
    sheet->setGraphicsEffect(sheetShadow);

    // Add staff lines (5 lines per staff, 3 staves for example)
    QPen staffPen(QColor("#000000"), 1);
    for (int staff = 0; staff < 3; staff++) {
        int staffY = 100 + (staff * 150);

        // Draw staff name
        QGraphicsTextItem *staffName = scoreScene->addText("Staff " + QString::number(staff + 1));
        staffName->setPos(50, staffY - 30);
        staffName->setFont(QFont("Arial", 12, QFont::Bold));

        // Draw treble clef
        QGraphicsTextItem *clef = scoreScene->addText("𝄞");
        clef->setPos(50, staffY - 20);
        clef->setFont(QFont("Arial", 40));

        // Draw staff lines
        for (int line = 0; line < 5; line++) {
            scoreScene->addLine(100, staffY + (line * 10), 750, staffY + (line * 10), staffPen);
        }

        // Draw bar lines
        for (int bar = 0; bar <= 4; bar++) {
            int barX = 100 + (bar * 150);
            scoreScene->addLine(barX, staffY, barX, staffY + 40, staffPen);
        }

        // Add some example notes
        if (staff == 0) {
            // Quarter notes
            for (int i = 0; i < 4; i++) {
                QGraphicsEllipseItem *noteHead = scoreScene->addEllipse(150 + (i * 50), staffY + 20, 10, 8, QPen(Qt::NoPen), QBrush(QColor("#000000")));
                scoreScene->addLine(160 + (i * 50), staffY + 20, 160 + (i * 50), staffY - 20, staffPen);
            }
        } else if (staff == 1) {
            // Half notes
            for (int i = 0; i < 2; i++) {
                QGraphicsEllipseItem *noteHead = scoreScene->addEllipse(150 + (i * 100), staffY + 15, 10, 8, staffPen, QBrush(Qt::transparent));
                scoreScene->addLine(160 + (i * 100), staffY + 15, 160 + (i * 100), staffY - 25, staffPen);
            }
        } else if (staff == 2) {
            // Whole note
            scoreScene->addEllipse(200, staffY + 10, 12, 10, staffPen, QBrush(Qt::transparent));
        }
    }

    // Center the sheet in the view
    scoreView->centerOn(sheet);

    // Add to splitter
    mainSplitter->addWidget(scoreView);
}

void ScorePage::setupPropertiesPanel()
{
    // Create properties panel
    propertiesPanel = new QWidget(this);
    propertiesPanel->setMinimumWidth(250);
    propertiesPanel->setMaximumWidth(300);
    propertiesPanel->setStyleSheet(
        "background-color: #181818;"
        "border-left: 1px solid #333333;"
        );

    QVBoxLayout *propertiesLayout = new QVBoxLayout(propertiesPanel);
    propertiesLayout->setContentsMargins(10, 10, 10, 10);
    propertiesLayout->setSpacing(10);

    // Properties header
    QHBoxLayout *propertiesHeaderLayout = new QHBoxLayout();
    QLabel *propertiesTitle = new QLabel("Properties", propertiesPanel);
    propertiesTitle->setStyleSheet("color: white; font-size: 16px; font-weight: bold;");

    togglePropertiesPanelButton = new QPushButton(propertiesPanel);
    togglePropertiesPanelButton->setIcon(QIcon::fromTheme("go-next"));
    togglePropertiesPanelButton->setFixedSize(24, 24);
    togglePropertiesPanelButton->setCursor(Qt::PointingHandCursor);
    togglePropertiesPanelButton->setStyleSheet(
        "QPushButton {"
        "   background-color: transparent;"
        "   border: none;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(255, 255, 255, 0.1);"
        "   border-radius: 12px;"
        "}"
        );

    propertiesHeaderLayout->addWidget(propertiesTitle);
    propertiesHeaderLayout->addStretch();
    propertiesHeaderLayout->addWidget(togglePropertiesPanelButton);

    // Properties tabs
    propertiesTabs = new QTabWidget(propertiesPanel);
    propertiesTabs->setStyleSheet(
        "QTabWidget::pane {"
        "   border: 1px solid #333333;"
        "   background-color: #222222;"
        "   border-radius: 4px;"
        "}"
        "QTabBar::tab {"
        "   background-color: #181818;"
        "   color: #bbbbbb;"
        "   border: 1px solid #333333;"
        "   border-bottom: none;"
        "   border-top-left-radius: 4px;"
        "   border-top-right-radius: 4px;"
        "   padding: 6px 10px;"
        "   margin-right: 2px;"
        "}"
        "QTabBar::tab:selected {"
        "   background-color: #222222;"
        "   color: white;"
        "}"
        "QTabBar::tab:hover:!selected {"
        "   background-color: #1a1a1a;"
        "}"
        );

    // Note properties tab
    QWidget *noteTab = new QWidget(propertiesTabs);
    QVBoxLayout *noteLayout = new QVBoxLayout(noteTab);

    QGroupBox *notePropertiesGroup = new QGroupBox("Note Properties", noteTab);
    notePropertiesGroup->setStyleSheet(
        "QGroupBox {"
        "   color: white;"
        "   border: 1px solid #333333;"
        "   border-radius: 4px;"
        "   margin-top: 12px;"
        "   padding-top: 16px;"
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   subcontrol-position: top left;"
        "   padding: 0 5px;"
        "}"
        );

    QFormLayout *notePropertiesLayout = new QFormLayout(notePropertiesGroup);
    notePropertiesLayout->setLabelAlignment(Qt::AlignRight);
    notePropertiesLayout->setFormAlignment(Qt::AlignLeft);
    notePropertiesLayout->setSpacing(10);

    QComboBox *pitchCombo = new QComboBox(notePropertiesGroup);
    pitchCombo->addItems(QStringList() << "C" << "D" << "E" << "F" << "G" << "A" << "B");
    pitchCombo->setStyleSheet(
        "QComboBox {"
        "   background-color: #333333;"
        "   color: white;"
        "   border: 1px solid #444444;"
        "   border-radius: 4px;"
        "   padding: 4px 8px;"
        "}"
        "QComboBox::drop-down {"
        "   border: none;"
        "   width: 20px;"
        "}"
        "QComboBox QAbstractItemView {"
        "   background-color: #333333;"
        "   color: white;"
        "   selection-background-color: #1e90ff;"
        "}"
        );

    QComboBox *accidentalCombo = new QComboBox(notePropertiesGroup);
    accidentalCombo->addItems(QStringList() << "None" << "Sharp" << "Flat" << "Natural");
    accidentalCombo->setStyleSheet(pitchCombo->styleSheet());

    QComboBox *octaveCombo = new QComboBox(notePropertiesGroup);
    octaveCombo->addItems(QStringList() << "1" << "2" << "3" << "4" << "5" << "6" << "7");
    octaveCombo->setCurrentIndex(3); // Default to octave 4
    octaveCombo->setStyleSheet(pitchCombo->styleSheet());

    QLabel *pitchLabel = new QLabel("Pitch:", notePropertiesGroup);
    pitchLabel->setStyleSheet("color: white;");
    QLabel *accidentalLabel = new QLabel("Accidental:", notePropertiesGroup);
    accidentalLabel->setStyleSheet("color: white;");
    QLabel *octaveLabel = new QLabel("Octave:", notePropertiesGroup);
    octaveLabel->setStyleSheet("color: white;");

    notePropertiesLayout->addRow(pitchLabel, pitchCombo);
    notePropertiesLayout->addRow(accidentalLabel, accidentalCombo);
    notePropertiesLayout->addRow(octaveLabel, octaveCombo);

    noteLayout->addWidget(notePropertiesGroup);

    // Playback properties tab
    QWidget *playbackTab = new QWidget(propertiesTabs);
    QVBoxLayout *playbackLayout = new QVBoxLayout(playbackTab);

    QGroupBox *tempoGroup = new QGroupBox("Tempo", playbackTab);
    tempoGroup->setStyleSheet(notePropertiesGroup->styleSheet());

    QVBoxLayout *tempoLayout = new QVBoxLayout(tempoGroup);

    tempoSlider = new QSlider(Qt::Horizontal, tempoGroup);
    tempoSlider->setRange(40, 240);
    tempoSlider->setValue(120);
    tempoSlider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "   background-color: #333333;"
        "   height: 8px;"
        "   border-radius: 4px;"
        "}"
        "QSlider::handle:horizontal {"
        "   background-color: #1e90ff;"
        "   width: 16px;"
        "   margin: -4px 0;"
        "   border-radius: 8px;"
        "}"
        "QSlider::handle:horizontal:hover {"
        "   background-color: #3aa0ff;"
        "}"
        );

    QHBoxLayout *tempoValueLayout = new QHBoxLayout();
    QLabel *tempoLabel = new QLabel("BPM:", tempoGroup);
    tempoLabel->setStyleSheet("color: white;");

    tempoSpinBox = new QSpinBox(tempoGroup);
    tempoSpinBox->setRange(40, 240);
    tempoSpinBox->setValue(120);
    tempoSpinBox->setStyleSheet(
        "QSpinBox {"
        "   background-color: #333333;"
        "   color: white;"
        "   border: 1px solid #444444;"
        "   border-radius: 4px;"
        "   padding: 4px 8px;"
        "}"
        "QSpinBox::up-button, QSpinBox::down-button {"
        "   background-color: #444444;"
        "   width: 16px;"
        "}"
        "QSpinBox::up-button:hover, QSpinBox::down-button:hover {"
        "   background-color: #555555;"
        "}"
        );

    tempoValueLayout->addWidget(tempoLabel);
    tempoValueLayout->addWidget(tempoSpinBox);

    tempoLayout->addLayout(tempoValueLayout);
    tempoLayout->addWidget(tempoSlider);

    // Connect tempo controls
    connect(tempoSlider, &QSlider::valueChanged, tempoSpinBox, &QSpinBox::setValue);
    connect(tempoSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), tempoSlider, &QSlider::setValue);
    connect(tempoSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ScorePage::updateTempo);

    playbackLayout->addWidget(tempoGroup);

    // Add metronome settings
    QGroupBox *metronomeGroup = new QGroupBox("Metronome", playbackTab);
    metronomeGroup->setStyleSheet(notePropertiesGroup->styleSheet());

    QFormLayout *metronomeLayout = new QFormLayout(metronomeGroup);

    QComboBox *timeSignatureCombo = new QComboBox(metronomeGroup);
    timeSignatureCombo->addItems(QStringList() << "4/4" << "3/4" << "2/4" << "6/8" << "9/8" << "12/8");
    timeSignatureCombo->setStyleSheet(pitchCombo->styleSheet());

    QComboBox *accentCombo = new QComboBox(metronomeGroup);
    accentCombo->addItems(QStringList() << "First Beat" << "All Beats" << "None");
    accentCombo->setStyleSheet(pitchCombo->styleSheet());

    QLabel *timeSignatureLabel = new QLabel("Time Signature:", metronomeGroup);
    timeSignatureLabel->setStyleSheet("color: white;");
    QLabel *accentLabel = new QLabel("Accent:", metronomeGroup);
    accentLabel->setStyleSheet("color: white;");

    metronomeLayout->addRow(timeSignatureLabel, timeSignatureCombo);
    metronomeLayout->addRow(accentLabel, accentCombo);

    playbackLayout->addWidget(metronomeGroup);
    playbackLayout->addStretch();

    // Layout properties tab
    QWidget *layoutTab = new QWidget(propertiesTabs);
    QVBoxLayout *layoutTabLayout = new QVBoxLayout(layoutTab);

    QGroupBox *pageGroup = new QGroupBox("Page Layout", layoutTab);
    pageGroup->setStyleSheet(notePropertiesGroup->styleSheet());

    QFormLayout *pageLayout = new QFormLayout(pageGroup);

    QComboBox *pageSizeCombo = new QComboBox(pageGroup);
    pageSizeCombo->addItems(QStringList() << "A4" << "Letter" << "Legal" << "Tabloid");
    pageSizeCombo->setStyleSheet(pitchCombo->styleSheet());

    QComboBox *orientationCombo = new QComboBox(pageGroup);
    orientationCombo->addItems(QStringList() << "Portrait" << "Landscape");
    orientationCombo->setStyleSheet(pitchCombo->styleSheet());

    QSpinBox *marginSpinBox = new QSpinBox(pageGroup);
    marginSpinBox->setRange(10, 50);
    marginSpinBox->setValue(20);
    marginSpinBox->setSuffix(" mm");
    marginSpinBox->setStyleSheet(tempoSpinBox->styleSheet());

    QLabel *pageSizeLabel = new QLabel("Page Size:", pageGroup);
    pageSizeLabel->setStyleSheet("color: white;");
    QLabel *orientationLabel = new QLabel("Orientation:", pageGroup);
    orientationLabel->setStyleSheet("color: white;");
    QLabel *marginLabel = new QLabel("Margins:", pageGroup);
    marginLabel->setStyleSheet("color: white;");

    pageLayout->addRow(pageSizeLabel, pageSizeCombo);
    pageLayout->addRow(orientationLabel, orientationCombo);
    pageLayout->addRow(marginLabel, marginSpinBox);

    layoutTabLayout->addWidget(pageGroup);

    QGroupBox *spacingGroup = new QGroupBox("Spacing", layoutTab);
    spacingGroup->setStyleSheet(notePropertiesGroup->styleSheet());

    QFormLayout *spacingLayout = new QFormLayout(spacingGroup);

    QSpinBox *staffSpacingSpinBox = new QSpinBox(spacingGroup);
    staffSpacingSpinBox->setRange(10, 50);
    staffSpacingSpinBox->setValue(20);
    staffSpacingSpinBox->setSuffix(" mm");
    staffSpacingSpinBox->setStyleSheet(tempoSpinBox->styleSheet());

    QSpinBox *systemSpacingSpinBox = new QSpinBox(spacingGroup);
    systemSpacingSpinBox->setRange(10, 50);
    systemSpacingSpinBox->setValue(15);
    systemSpacingSpinBox->setSuffix(" mm");
    systemSpacingSpinBox->setStyleSheet(tempoSpinBox->styleSheet());

    QLabel *staffSpacingLabel = new QLabel("Staff Spacing:", spacingGroup);
    staffSpacingLabel->setStyleSheet("color: white;");
    QLabel *systemSpacingLabel = new QLabel("System Spacing:", spacingGroup);
    systemSpacingLabel->setStyleSheet("color: white;");

    spacingLayout->addRow(staffSpacingLabel, staffSpacingSpinBox);
    spacingLayout->addRow(systemSpacingLabel, systemSpacingSpinBox);

    layoutTabLayout->addWidget(spacingGroup);
    layoutTabLayout->addStretch();

    // Add tabs to tab widget
    propertiesTabs->addTab(noteTab, "Note");
    propertiesTabs->addTab(playbackTab, "Playback");
    propertiesTabs->addTab(layoutTab, "Layout");

    // Add to properties layout
    propertiesLayout->addLayout(propertiesHeaderLayout);
    propertiesLayout->addWidget(propertiesTabs);

    // Add to splitter
    mainSplitter->addWidget(propertiesPanel);

    // Set initial splitter sizes
    QList<int> sizes;
    sizes << 700 << 300;
    mainSplitter->setSizes(sizes);

    // Connect toggle button
    connect(togglePropertiesPanelButton, &QPushButton::clicked, this, &ScorePage::togglePropertiesPanel);
}

void ScorePage::goBack()
{
    mainWindow->navigateToMainPage();
}

void ScorePage::editScore()
{
    // Placeholder for edit functionality
    QMessageBox::information(this, "Edit Score", "Score editing functionality would be implemented here.");
}

// Fix: Use mainWindow->statusBar() to access the status bar
void ScorePage::playScore()
{
    // Placeholder for playback functionality
    mainWindow->statusBar()->showMessage("Playing score...", 3000);
}

void ScorePage::pauseScore()
{
    // Placeholder for pause functionality
    mainWindow->statusBar()->showMessage("Playback paused", 3000);
}

void ScorePage::stopScore()
{
    // Placeholder for stop functionality
    mainWindow->statusBar()->showMessage("Playback stopped", 3000);
}

void ScorePage::toggleMetronome()
{
    // Placeholder for metronome functionality
    bool isChecked = metronomeAction->isChecked();
    mainWindow->statusBar()->showMessage(isChecked ? "Metronome enabled" : "Metronome disabled", 3000);
}

void ScorePage::zoomIn()
{
    zoomFactor *= 1.2;
    scoreView->scale(1.2, 1.2);
    mainWindow->statusBar()->showMessage(QString("Zoom: %1%").arg(int(zoomFactor * 100)), 3000);
}

void ScorePage::zoomOut()
{
    zoomFactor /= 1.2;
    scoreView->scale(1/1.2, 1/1.2);
    mainWindow->statusBar()->showMessage(QString("Zoom: %1%").arg(int(zoomFactor * 100)), 3000);
}

void ScorePage::resetZoom()
{
    scoreView->resetTransform();
    zoomFactor = 1.0;
    mainWindow->statusBar()->showMessage("Zoom reset to 100%", 3000);
}

void ScorePage::togglePropertiesPanel()
{
    if (propertiesPanelVisible) {
        // Hide properties panel
        QList<int> sizes = mainSplitter->sizes();
        QList<int> newSizes;
        newSizes << sizes[0] + sizes[1] << 0;
        mainSplitter->setSizes(newSizes);
        togglePropertiesPanelButton->setIcon(QIcon::fromTheme("go-previous"));
        propertiesPanelVisible = false;
    } else {
        // Show properties panel
        QList<int> sizes = mainSplitter->sizes();
        QList<int> newSizes;
        newSizes << sizes[0] - 300 << 300;
        mainSplitter->setSizes(newSizes);
        togglePropertiesPanelButton->setIcon(QIcon::fromTheme("go-next"));
        propertiesPanelVisible = true;
    }
}

void ScorePage::changeNoteDuration(int index)
{
    // Placeholder for note duration change
    QString duration = noteDurationCombo->itemText(index);
    mainWindow->statusBar()->showMessage("Note duration changed to " + duration, 3000);
}

void ScorePage::changeClef(int index)
{
    // Placeholder for clef change
    QString clef = clefCombo->itemText(index);
    mainWindow->statusBar()->showMessage("Clef changed to " + clef, 3000);
}

void ScorePage::changeKeySignature(int index)
{
    // Placeholder for key signature change
    QString key = keySignatureCombo->itemText(index);
    mainWindow->statusBar()->showMessage("Key signature changed to " + key, 3000);
}

void ScorePage::updateTempo(int tempo)
{
    // Placeholder for tempo update
    mainWindow->statusBar()->showMessage(QString("Tempo set to %1 BPM").arg(tempo), 3000);
}

