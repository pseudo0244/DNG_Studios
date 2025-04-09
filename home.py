import sys
import json
import os
from datetime import datetime
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout, 
    QPushButton, QLabel, QScrollArea, QFrame, QDialog, QLineEdit, 
    QFormLayout, QMessageBox, QSplitter, QListWidget, QListWidgetItem, 
    QGridLayout, QToolButton, QSpacerItem
)
from PyQt5.QtCore import Qt, QSize, QRect
from PyQt5.QtGui import QIcon, QFont, QColor, QPalette, QPixmap, QCursor


# Ensure scores directory exists
if not os.path.exists("scores"):
    os.makedirs("scores")

# Load scores from JSON file or create empty list
def load_scores():
    try:
        if os.path.exists("scores/scores.json"):
            with open("scores/scores.json", "r") as f:
                return json.load(f)
        return []
    except Exception as e:
        print(f"Error loading scores: {e}")
        return []

# Save scores to JSON file
def save_scores(scores):
    try:
        with open("scores/scores.json", "w") as f:
            json.dump(scores, f, indent=4)
    except Exception as e:
        print(f"Error saving scores: {e}")

class ScoreCard(QFrame):
    def __init__(self, score, parent=None):
        super().__init__(parent)
        self.score = score
        self.setObjectName("scoreCard")
        self.setFixedSize(220, 280)
        self.setCursor(QCursor(Qt.PointingHandCursor))
        
        layout = QVBoxLayout()
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(4)
        
        # Score thumbnail (placeholder)
        thumbnail = QLabel()
        thumbnail.setFixedSize(220, 220)
        thumbnail.setObjectName("scoreThumbnail")
        thumbnail.setStyleSheet("background-color: #f5f5f5; border-radius: 4px;")
        
        # If it's a new score card, show plus icon
        if score.get("is_new_score", False):
            plus_label = QLabel("+")
            plus_font = QFont()
            plus_font.setPointSize(40)
            plus_label.setFont(plus_font)
            plus_label.setAlignment(Qt.AlignCenter)
            plus_label.setStyleSheet("color: #333;")
            
            thumbnail_layout = QVBoxLayout(thumbnail)
            thumbnail_layout.addWidget(plus_label)
        else:
            # Cloud icon for synced scores
            cloud_icon = QLabel()
            cloud_icon.setFixedSize(24, 24)
            cloud_icon.setStyleSheet("background-color: #4285f4; border-radius: 12px;")
            cloud_icon.setAlignment(Qt.AlignCenter)
            cloud_icon.setText("↑")
            cloud_icon.setStyleSheet("color: white; background-color: #7c4dff; border-radius: 12px;")
            
            # Position cloud icon at top-right of thumbnail
            cloud_icon.setParent(thumbnail)
            cloud_icon.move(185, 10)
        
        # Title
        title_label = QLabel(score["title"])
        title_label.setObjectName("scoreTitle")
        title_font = QFont()
        title_font.setPointSize(12)
        title_font.setBold(True)
        title_label.setFont(title_font)
        title_label.setAlignment(Qt.AlignLeft)
        
        # Date or status
        if score.get("is_new_score", False):
            status_label = QLabel("New score")
        else:
            status_label = QLabel("TODAY")
        status_label.setObjectName("scoreStatus")
        status_label.setAlignment(Qt.AlignLeft)
        
        layout.addWidget(thumbnail)
        layout.addWidget(title_label)
        layout.addWidget(status_label)
        
        self.setLayout(layout)
        
    def mousePressEvent(self, event):
        if self.score.get("is_new_score", False):
            # Create new score
            dialog = NewScoreDialog(self.parent())
            if dialog.exec_() == QDialog.Accepted:
                title = dialog.title_input.text().strip()
                composer = dialog.composer_input.text().strip()
                
                if not title:
                    QMessageBox.warning(self, "Missing Information", "Please enter a title for your score.")
                    return
                
                # Create new score
                new_score = {
                    "id": len(self.parent().parent().parent().scores) + 1,
                    "title": title,
                    "composer": composer if composer else "Unknown",
                    "date": datetime.now().strftime("%Y-%m-%d %H:%M")
                }
                
                # Add to scores list
                self.parent().parent().parent().scores.append(new_score)
                save_scores(self.parent().parent().parent().scores)
                
                # Open the score editor
                score_window = ScoreEditorWindow(new_score)
                score_window.show()
                
                # Refresh home screen
                self.parent().parent().parent().show_home()
        else:
            # Open existing score
            score_window = ScoreEditorWindow(self.score)
            score_window.show()

class NewScoreDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("Create New Score")
        self.setMinimumWidth(400)
        self.setObjectName("newScoreDialog")
        
        layout = QFormLayout()
        
        # Title input
        self.title_input = QLineEdit()
        self.title_input.setPlaceholderText("Enter score title")
        layout.addRow("Title:", self.title_input)
        
        # Composer input
        self.composer_input = QLineEdit()
        self.composer_input.setPlaceholderText("Enter composer name")
        layout.addRow("Composer:", self.composer_input)
        
        # Create button
        create_button = QPushButton("Create Score")
        create_button.clicked.connect(self.accept)
        create_button.setObjectName("createScoreButton")
        
        # Cancel button
        cancel_button = QPushButton("Cancel")
        cancel_button.clicked.connect(self.reject)
        
        # Button layout
        button_layout = QHBoxLayout()
        button_layout.addWidget(create_button)
        button_layout.addWidget(cancel_button)
        
        layout.addRow("", button_layout)
        self.setLayout(layout)

class ScoreEditorWindow(QMainWindow):
    def __init__(self, score):
        super().__init__()
        self.score = score
        self.setWindowTitle(f"{score['title']} - DNG Studios Music Notation")
        self.setMinimumSize(1200, 800)
        
        central_widget = QWidget()
        layout = QVBoxLayout()
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(0)
        
        # Top toolbar
        toolbar = QWidget()
        toolbar.setObjectName("editorToolbar")
        toolbar.setFixedHeight(50)
        
        toolbar_layout = QHBoxLayout()
        toolbar_layout.setContentsMargins(10, 0, 10, 0)
        
        title_label = QLabel(score["title"])
        title_font = QFont()
        title_font.setPointSize(14)
        title_font.setBold(True)
        title_label.setFont(title_font)
        
        toolbar_layout.addWidget(title_label)
        toolbar_layout.addStretch()
        
        save_button = QPushButton("Save")
        save_button.setObjectName("toolbarButton")
        
        export_button = QPushButton("Export")
        export_button.setObjectName("toolbarButton")
        
        toolbar_layout.addWidget(save_button)
        toolbar_layout.addWidget(export_button)
        
        toolbar.setLayout(toolbar_layout)
        
        # Score editor area (placeholder)
        editor_area = QWidget()
        editor_area.setObjectName("scoreEditorArea")
        
        editor_layout = QVBoxLayout()
        editor_layout.setContentsMargins(0, 0, 0, 0)
        
        # Create a placeholder for the score sheet
        score_sheet = QLabel("Score Editor - Staff lines would appear here")
        score_sheet.setAlignment(Qt.AlignCenter)
        score_sheet.setStyleSheet("background-color: #f5f5f5; color: #333; padding: 20px;")
        score_sheet.setMinimumHeight(700)
        
        editor_layout.addWidget(score_sheet)
        editor_area.setLayout(editor_layout)
        
        # Add to main layout
        layout.addWidget(toolbar)
        layout.addWidget(editor_area)
        
        central_widget.setLayout(layout)
        self.setCentralWidget(central_widget)

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("DNG Studios Music Notation")
        self.setMinimumSize(1200, 800)
        
        # Load scores
        self.scores = load_scores()
        
        # Main layout with splitter
        self.central_widget = QWidget()
        main_layout = QHBoxLayout()
        main_layout.setContentsMargins(0, 0, 0, 0)
        main_layout.setSpacing(0)
        self.central_widget.setLayout(main_layout)
        
        # Create sidebar
        self.sidebar = self.create_sidebar()
        
        # Create main content area
        self.content_area = QWidget()
        self.content_layout = QVBoxLayout()
        self.content_layout.setContentsMargins(30, 30, 30, 30)
        self.content_area.setLayout(self.content_layout)
        
        # Add sidebar and content to main layout
        main_layout.addWidget(self.sidebar)
        main_layout.addWidget(self.content_area)
        
        self.setCentralWidget(self.central_widget)
        
        # Show home screen by default
        self.show_home()
    
    def create_sidebar(self):
        sidebar = QWidget()
        sidebar.setObjectName("sidebar")
        sidebar.setFixedWidth(250)
        
        layout = QVBoxLayout()
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(0)
        
        # User profile section
        profile_section = QWidget()
        profile_section.setObjectName("profileSection")
        profile_section.setFixedHeight(100)
        
        profile_layout = QHBoxLayout()
        
        # User avatar
        avatar_label = QLabel()
        avatar_label.setFixedSize(40, 40)
        avatar_label.setObjectName("userAvatar")
        avatar_label.setStyleSheet("background-color: #555; border-radius: 20px;")
        
        # Username
        username_label = QLabel("DNG Studios")
        username_label.setObjectName("username")
        
        profile_layout.addWidget(avatar_label)
        profile_layout.addWidget(username_label)
        profile_layout.addStretch()
        
        profile_section.setLayout(profile_layout)
        
        # Navigation menu
        nav_menu = QWidget()
        nav_menu.setObjectName("navMenu")
        
        nav_layout = QVBoxLayout()
        nav_layout.setContentsMargins(0, 0, 0, 0)
        nav_layout.setSpacing(0)
        
        # Navigation buttons
        home_btn = QPushButton("Home")
        home_btn.setObjectName("navButton")
        home_btn.setProperty("active", True)
        home_btn.setIcon(QIcon.fromTheme("go-home"))
        
        scores_btn = QPushButton("Scores")
        scores_btn.setObjectName("navButton")
        scores_btn.setIcon(QIcon.fromTheme("document-music"))
        
        plugins_btn = QPushButton("Contact")
        plugins_btn.setObjectName("navButton")
        plugins_btn.setIcon(QIcon.fromTheme("preferences-contact"))
        
        learn_btn = QPushButton("Settings")
        learn_btn.setObjectName("navButton")
        learn_btn.setIcon(QIcon.fromTheme("help-settings"))
        
        # Connect signals
        home_btn.clicked.connect(self.show_home)
        
        # Add buttons to layout
        nav_layout.addWidget(home_btn)
        nav_layout.addWidget(scores_btn)
        nav_layout.addWidget(plugins_btn)
        nav_layout.addWidget(learn_btn)
        nav_layout.addStretch()
        
        nav_menu.setLayout(nav_layout)
        
        # Add all sections to sidebar
        layout.addWidget(profile_section)
        layout.addWidget(nav_menu)
        
        sidebar.setLayout(layout)
        return sidebar
    
    def show_home(self):
        # Clear current content
        self.clear_content()
        
        # Header with search
        header = QWidget()
        header_layout = QHBoxLayout()
        header_layout.setContentsMargins(0, 0, 0, 20)
        
        scores_label = QLabel("Scores")
        scores_font = QFont()
        scores_font.setPointSize(24)
        scores_font.setBold(True)
        scores_label.setFont(scores_font)
        
        search_box = QLineEdit()
        search_box.setPlaceholderText("Search")
        search_box.setObjectName("searchBox")
        search_box.setFixedWidth(250)
        
        header_layout.addWidget(scores_label)
        header_layout.addStretch()
        header_layout.addWidget(search_box)
        
        header.setLayout(header_layout)
        
        # Recent scores section
        recent_section = QWidget()
        recent_layout = QVBoxLayout()
        recent_layout.setContentsMargins(0, 0, 0, 30)
        
        recent_label = QLabel("New & recent")
        recent_font = QFont()
        recent_font.setPointSize(16)
        recent_label.setFont(recent_font)
        
        # Grid for score cards
        scores_grid = QGridLayout()
        scores_grid.setContentsMargins(0, 20, 0, 0)
        scores_grid.setSpacing(20)
        
        # Add "New score" card first
        new_score = {"title": "New score", "is_new_score": True}
        new_score_card = ScoreCard(new_score, self)
        scores_grid.addWidget(new_score_card, 0, 0)
        
        # Add existing scores
        col = 1
        for i, score in enumerate(self.scores[:5]):  # Show only first 5 scores
            score_card = ScoreCard(score, self)
            scores_grid.addWidget(score_card, 0, col)
            col += 1
        
        # Add grid to recent section
        recent_layout.addWidget(recent_label)
        recent_layout.addLayout(scores_grid)
        
        recent_section.setLayout(recent_layout)
        
        # Bottom buttons
        bottom_section = QWidget()
        bottom_layout = QHBoxLayout()
        
        
        new_btn = QPushButton("New")
        new_btn.setObjectName("bottomButton")
        new_btn.clicked.connect(self.create_new_score)
        
        open_btn = QPushButton("Open other...")
        open_btn.setObjectName("bottomButton")
        bottom_layout.addStretch()
        bottom_layout.addWidget(new_btn)
        bottom_layout.addWidget(open_btn)
        
        bottom_section.setLayout(bottom_layout)
        
        # Add all sections to main content
        self.content_layout.addWidget(header)
        self.content_layout.addWidget(recent_section)
        self.content_layout.addStretch()
        self.content_layout.addWidget(bottom_section)
    
    def clear_content(self):
        # Clear all widgets from content layout
        while self.content_layout.count():
            item = self.content_layout.takeAt(0)
            if item.widget():
                item.widget().deleteLater()
    
    def create_new_score(self):
        dialog = NewScoreDialog(self)
        result = dialog.exec_()
        
        if result == QDialog.Accepted:
            title = dialog.title_input.text().strip()
            composer = dialog.composer_input.text().strip()
            
            if not title:
                QMessageBox.warning(self, "Missing Information", "Please enter a title for your score.")
                return
            
            # Create new score
            new_score = {
                "id": len(self.scores) + 1,
                "title": title,
                "composer": composer if composer else "Unknown",
                "date": datetime.now().strftime("%Y-%m-%d %H:%M")
            }
            
            # Add to scores list
            self.scores.append(new_score)
            save_scores(self.scores)
            
            # Open the score editor
            score_window = ScoreEditorWindow(new_score)
            score_window.show()
            
            # Refresh home screen
            self.show_home()
