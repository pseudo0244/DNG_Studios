#include "publishpage.h"
#include "mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>

PublishPage::PublishPage(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent), mainWindow(mainWindow)
{
    setStyleSheet("background-color: #181818;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    // Header with back button and title
    QHBoxLayout *headerLayout = new QHBoxLayout();

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

    titleLabel = new QLabel("Publish Score", this);
    titleLabel->setStyleSheet("color: white; font-size: 28px; font-weight: bold;");

    headerLayout->addWidget(backButton);
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();

    // Divider
    QFrame *divider = new QFrame(this);
    divider->setFrameShape(QFrame::HLine);
    divider->setFrameShadow(QFrame::Sunken);
    divider->setStyleSheet("background-color: #333333;");

    // Score details
    QHBoxLayout *detailsLayout = new QHBoxLayout();

    composerLabel = new QLabel("", this);
    composerLabel->setStyleSheet("color: #bbbbbb; font-size: 16px;");

    detailsLayout->addWidget(composerLabel);
    detailsLayout->addStretch();

    // Publish options in a scroll area
    QScrollArea *optionsScrollArea = new QScrollArea(this);
    optionsScrollArea->setWidgetResizable(true);
    optionsScrollArea->setFrameShape(QFrame::NoFrame);
    optionsScrollArea->setStyleSheet("background-color: transparent;");

    QWidget *optionsWidget = new QWidget(optionsScrollArea);
    QVBoxLayout *optionsLayout = new QVBoxLayout(optionsWidget);
    optionsLayout->setContentsMargins(0, 0, 0, 0);
    optionsLayout->setSpacing(20);

    // Format selection
    QGroupBox *formatGroup = new QGroupBox("Export Format", optionsWidget);
    formatGroup->setStyleSheet(
        "QGroupBox {"
        "   color: white;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "   border: 1px solid #333333;"
        "   border-radius: 6px;"
        "   margin-top: 16px;"
        "   padding: 20px;"
        "   background-color: #222222;"
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   subcontrol-position: top left;"
        "   padding: 0 10px;"
        "   left: 10px;"
        "}"
        );

    QVBoxLayout *formatLayout = new QVBoxLayout(formatGroup);

    formatComboBox = new QComboBox(formatGroup);
    formatComboBox->addItem("PDF");
    formatComboBox->addItem("MIDI");
    formatComboBox->addItem("MusicXML");
    formatComboBox->addItem("MP3 (Audio)");
    formatComboBox->setStyleSheet(
        "QComboBox {"
        "   background-color: #333333;"
        "   color: white;"
        "   border: 1px solid #444444;"
        "   border-radius: 4px;"
        "   padding: 6px 12px;"
        "   min-width: 200px;"
        "}"
        "QComboBox::drop-down {"
        "   border: none;"
        "   width: 24px;"
        "}"
        "QComboBox QAbstractItemView {"
        "   background-color: #333333;"
        "   color: white;"
        "   selection-background-color: #2a82da;"
        "}"
        );

    formatLayout->addWidget(formatComboBox);

    // Quality options
    QGroupBox *qualityGroup = new QGroupBox("Quality Settings", optionsWidget);
    qualityGroup->setStyleSheet(
        "QGroupBox {"
        "   color: white;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "   border: 1px solid #333333;"
        "   border-radius: 6px;"
        "   margin-top: 16px;"
        "   padding: 20px;"
        "   background-color: #222222;"
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   subcontrol-position: top left;"
        "   padding: 0 10px;"
        "   left: 10px;"
        "}"
        );

    QVBoxLayout *qualityLayout = new QVBoxLayout(qualityGroup);

    highQualityRadio = new QRadioButton("High Quality", qualityGroup);
    highQualityRadio->setChecked(true);
    mediumQualityRadio = new QRadioButton("Medium Quality", qualityGroup);
    lowQualityRadio = new QRadioButton("Low Quality", qualityGroup);

    QString radioStyle =
        "QRadioButton {"
        "   color: white;"
        "   spacing: 8px;"
        "}"
        "QRadioButton::indicator {"
        "   width: 16px;"
        "   height: 16px;"
        "}"
        "QRadioButton::indicator:checked {"
        "   background-color: #2a82da;"
        "   border: 2px solid white;"
        "   border-radius: 8px;"
        "}"
        "QRadioButton::indicator:unchecked {"
        "   border: 2px solid #888888;"
        "   border-radius: 8px;"
        "}";

    highQualityRadio->setStyleSheet(radioStyle);
    mediumQualityRadio->setStyleSheet(radioStyle);
    lowQualityRadio->setStyleSheet(radioStyle);

    qualityLayout->addWidget(highQualityRadio);
    qualityLayout->addWidget(mediumQualityRadio);
    qualityLayout->addWidget(lowQualityRadio);

    // Add groups to options layout
    optionsLayout->addWidget(formatGroup);
    optionsLayout->addWidget(qualityGroup);
    optionsLayout->addStretch();

    optionsScrollArea->setWidget(optionsWidget);

    // Download button
    downloadButton = new QPushButton("Download", this);
    downloadButton->setIcon(QIcon::fromTheme("document-save"));
    downloadButton->setCursor(Qt::PointingHandCursor);
    downloadButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #2a82da;"
        "   color: white;"
        "   border-radius: 4px;"
        "   padding: 10px 20px;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #3a92ea;"
        "}"
        );

    // Add all layouts to main layout
    mainLayout->addLayout(headerLayout);
    mainLayout->addWidget(divider);
    mainLayout->addLayout(detailsLayout);
    mainLayout->addWidget(optionsScrollArea, 1);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(downloadButton);

    mainLayout->addLayout(buttonLayout);

    // Connect signals
    connect(backButton, &QPushButton::clicked, this, &PublishPage::goBack);
    connect(downloadButton, &QPushButton::clicked, this, &PublishPage::downloadScore);
}

void PublishPage::setScore(const Score &score)
{
    currentScore = score;
    titleLabel->setText("Publish: " + score.title);
    composerLabel->setText("Composer: " + score.composer);
}

void PublishPage::goBack()
{
    mainWindow->navigateToMainPage();
}

void PublishPage::downloadScore()
{
    QString format = formatComboBox->currentText();
    QString extension;

    if (format == "PDF") {
        extension = "pdf";
    } else if (format == "MIDI") {
        extension = "mid";
    } else if (format == "MusicXML") {
        extension = "musicxml";
    } else if (format == "MP3 (Audio)") {
        extension = "mp3";
    }

    QString quality;
    if (highQualityRadio->isChecked()) {
        quality = "High Quality";
    } else if (mediumQualityRadio->isChecked()) {
        quality = "Medium Quality";
    } else {
        quality = "Low Quality";
    }

    QString fileName = QFileDialog::getSaveFileName(this, "Save Score",
                                                    currentScore.title + "." + extension,
                                                    format + " Files (*." + extension + ")");

    if (!fileName.isEmpty()) {
        // Here you would actually save the file
        // For now, just show a message
        QMessageBox::information(this, "Download Complete",
                                 "Score has been downloaded to:\n" + fileName +
                                     "\nFormat: " + format +
                                     "\nQuality: " + quality);
    }
}

