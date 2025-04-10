import sys
import json
import os
from datetime import datetime
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout, 
    QPushButton, QLabel, QScrollArea, QFrame, QDialog, QLineEdit, 
    QFormLayout, QMessageBox, QSplitter, QListWidget, QListWidgetItem, 
    QGridLayout, QToolButton, QSpacerItem, QAction, QMenu
)
from PyQt5.QtCore import Qt, QSize, QRect, pyqtSignal
from PyQt5.QtGui import QIcon, QFont, QColor, QPalette, QPixmap, QCursor

if not os.path.exists("scores"):
    os.makedirs("scores")

def load_scores():
    try:
        if os.path.exists("scores/scores.json"):
            with open("scores/scores.json", "r") as f:
                return json.load(f)
        return []
    except Exception as e:
        print(f"Error loading scores: {e}")
        return []

def save_scores(scores):
    try:
        with open("scores/scores.json", "w") as f:
            json.dump(scores, f, indent=4)
    except Exception as e:
        print(f"Error saving scores: {e}")

class NewScoreDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("Create New Score")
        self.setMinimumWidth(400)
        self.setObjectName("newScoreDialog")
        
        layout = QFormLayout()
        
        self.title_input = QLineEdit()
        self.title_input.setPlaceholderText("Enter score title")
        layout.addRow("Title:", self.title_input)
        
        self.composer_input = QLineEdit()
        self.composer_input.setPlaceholderText("Enter composer name")
        layout.addRow("Composer:", self.composer_input)
        
        create_button = QPushButton("Create Score")
        create_button.clicked.connect(self.accept)
        create_button.setObjectName("createScoreButton")
        
        cancel_button = QPushButton("Cancel")
        cancel_button.clicked.connect(self.reject)
        
        button_layout = QHBoxLayout()
        button_layout.addWidget(create_button)
        button_layout.addWidget(cancel_button)
        
        layout.addRow("", button_layout)
        self.setLayout(layout)

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
        
        self.thumbnail = QLabel()
        self.thumbnail.setFixedSize(220, 220)
        self.thumbnail.setObjectName("scoreThumbnail")
        self.thumbnail.setStyleSheet("background-color: #f5f5f5; border-radius: 4px;")
        
        if score.get("is_new_score", False):
            plus_label = QLabel("+")
            plus_font = QFont()
            plus_font.setPointSize(40)
            plus_label.setFont(plus_font)
            plus_label.setAlignment(Qt.AlignCenter)
            plus_label.setStyleSheet("color: #333;")
            
            thumbnail_layout = QVBoxLayout(self.thumbnail)
            thumbnail_layout.addWidget(plus_label)
        else:
            self.setContextMenuPolicy(Qt.CustomContextMenu)
            self.customContextMenuRequested.connect(self.show_context_menu)
            
            cloud_icon = QLabel()
            cloud_icon.setFixedSize(24, 24)
            cloud_icon.setStyleSheet("color: white; background-color: #7c4dff; border-radius: 12px;")
            cloud_icon.setAlignment(Qt.AlignCenter)
            cloud_icon.setText("↑")
            cloud_icon.setParent(self.thumbnail)
            cloud_icon.move(185, 10)
        
        self.title_label = QLabel(score["title"])
        self.title_label.setObjectName("scoreTitle")
        title_font = QFont()
        title_font.setPointSize(12)
        title_font.setBold(True)
        self.title_label.setFont(title_font)
        
        self.status_label = QLabel("TODAY" if not score.get("is_new_score", False) else "New score")
        self.status_label.setObjectName("scoreStatus")
        
        layout.addWidget(self.thumbnail)
        layout.addWidget(self.title_label)
        layout.addWidget(self.status_label)
        
        self.setLayout(layout)
    
    def show_context_menu(self, pos):
        menu = QMenu()
        delete_action = menu.addAction("Delete")
        action = menu.exec_(self.mapToGlobal(pos))
        if action == delete_action:
            self.delete_score()
    
    def delete_score(self):
        parent = self.find_main_window()
        if parent is None:
            return
            
        reply = QMessageBox.question(
            self, 'Delete Score', 
            f"Are you sure you want to delete '{self.score['title']}'?",
            QMessageBox.Yes | QMessageBox.No, QMessageBox.No
        )
        
        if reply == QMessageBox.Yes:
            parent.scores = [s for s in parent.scores if s['id'] != self.score['id']]
            save_scores(parent.scores)
            parent.show_home()
        def open_score_editor(self, score):
        # Clear current content
            for i in reversed(range(self.content_layout.count())):
                widget = self.content_layout.itemAt(i).widget()
            if widget:
                widget.setParent(None)

        # Create and display the editor
        editor = ScoreEditorWindow(score, self)
        editor.closed.connect(self.show_home)
        self.current_editors.append(editor)
        self.content_layout.addWidget(editor)

    
    def find_main_window(self):
        parent = self.parent()
        while parent is not None and not isinstance(parent, MainWindow):
            parent = parent.parent()
        return parent
    
    def mousePressEvent(self, event):
        parent = self.find_main_window()
        if parent is None:
            return
            
        if self.score.get("is_new_score", False):
            dialog = NewScoreDialog(self)
            if dialog.exec_() == QDialog.Accepted:
                title = dialog.title_input.text().strip()
                composer = dialog.composer_input.text().strip()
                
                if not title:
                    QMessageBox.warning(self, "Missing Information", "Please enter a title for your score.")
                    return
                
                new_score = {
                    "id": len(parent.scores) + 1,
                    "title": title,
                    "composer": composer if composer else "Unknown",
                    "date": datetime.now().strftime("%Y-%m-%d %H:%M")
                }
                
                parent.scores.append(new_score)
                save_scores(parent.scores)
                parent.open_score_editor(new_score)
        else:
            parent.open_score_editor(self.score)

class ScoreEditorWindow(QWidget):
    closed = pyqtSignal()
    
    def __init__(self, score, parent=None):
        super().__init__(parent)
        self.score = score
        self.parent_window = parent
        
        layout = QVBoxLayout()
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(0)
        
        toolbar = QWidget()
        toolbar.setObjectName("editorToolbar")
        toolbar.setFixedHeight(50)
        
        toolbar_layout = QHBoxLayout()
        toolbar_layout.setContentsMargins(10, 0, 10, 0)
        
        self.title_label = QLabel(score["title"])
        title_font = QFont()
        title_font.setPointSize(14)
        title_font.setBold(True)
        self.title_label.setFont(title_font)
        
        toolbar_layout.addWidget(self.title_label)
        toolbar_layout.addStretch()
        
        save_button = QPushButton("Save")
        save_button.setObjectName("toolbarButton")
        
        export_button = QPushButton("Export")
        export_button.setObjectName("toolbarButton")
        
        back_button = QPushButton("Back to Home")
        back_button.setObjectName("toolbarButton")
        back_button.clicked.connect(self.close_editor)
        
        toolbar_layout.addWidget(save_button)
        toolbar_layout.addWidget(export_button)
        toolbar_layout.addWidget(back_button)
        
        toolbar.setLayout(toolbar_layout)
        
        editor_area = QWidget()
        editor_area.setObjectName("scoreEditorArea")
        
        editor_layout = QVBoxLayout()
        editor_layout.setContentsMargins(0, 0, 0, 0)
        
        score_sheet = QLabel(f"Score Editor - {score['title']}\n(Staff lines would appear here)")
        score_sheet.setAlignment(Qt.AlignCenter)
        score_sheet.setStyleSheet("background-color: #f5f5f5; color: #333; padding: 20px;")
        score_sheet.setMinimumHeight(700)
        
        editor_layout.addWidget(score_sheet)
        editor_area.setLayout(editor_layout)
        
        layout.addWidget(toolbar)
        layout.addWidget(editor_area)
        
        self.setLayout(layout)
    
    def close_editor(self):
        self.closed.emit()
        self.deleteLater()

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("DNG Studios Music Notation")
        self.setMinimumSize(1200, 800)
        self.scores = load_scores()
        self.current_editors = []
        
        self.init_ui()
        self.show_home()
    
    def init_ui(self):
        self.central_widget = QWidget()
        self.main_layout = QHBoxLayout()
        self.main_layout.setContentsMargins(0, 0, 0, 0)
        self.main_layout.setSpacing(0)
        self.central_widget.setLayout(self.main_layout)
        
        self.sidebar = self.create_sidebar()
        self.content_area = QWidget()
        self.content_layout = QVBoxLayout()
        self.content_layout.setContentsMargins(30, 30, 30, 30)
        self.content_area.setLayout(self.content_layout)
        
        self.main_layout.addWidget(self.sidebar)
        self.main_layout.addWidget(self.content_area)
        self.setCentralWidget(self.central_widget)
    
    def create_sidebar(self):
        sidebar = QWidget()
        sidebar.setObjectName("sidebar")
        sidebar.setFixedWidth(250)
        
        layout = QVBoxLayout()
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(0)
        
        profile_section = QWidget()
        profile_section.setObjectName("profileSection")
        profile_section.setFixedHeight(100)
        
        profile_layout = QHBoxLayout()
        
        avatar_label = QLabel()
        avatar_label.setFixedSize(40, 40)
        avatar_label.setObjectName("userAvatar")
        avatar_label.setStyleSheet("background-color: #555; border-radius: 20px;")
        
        username_label = QLabel("DNG Studios")
        username_label.setObjectName("username")
        
        profile_layout.addWidget(avatar_label)
        profile_layout.addWidget(username_label)
        profile_layout.addStretch()
        
        profile_section.setLayout(profile_layout)
        
        nav_menu = QWidget()
        nav_menu.setObjectName("navMenu")
        
        nav_layout = QVBoxLayout()
        nav_layout.setContentsMargins(0, 0, 0, 0)
        nav_layout.setSpacing(0)
        
        home_btn = QPushButton("Home")
        home_btn.setObjectName("navButton")
        home_btn.setProperty("active", True)
        home_btn.setIcon(QIcon.fromTheme("go-home"))
        home_btn.clicked.connect(self.show_home)
        
        scores_btn = QPushButton("Scores")
        scores_btn.setObjectName("navButton")
        scores_btn.setIcon(QIcon.fromTheme("document-music"))
        
        plugins_btn = QPushButton("Contact")
        plugins_btn.setObjectName("navButton")
        plugins_btn.setIcon(QIcon.fromTheme("preferences-contact"))
        
        learn_btn = QPushButton("Settings")
        learn_btn.setObjectName("navButton")
        learn_btn.setIcon(QIcon.fromTheme("help-settings"))
        
        nav_layout.addWidget(home_btn)
        nav_layout.addWidget(scores_btn)
        nav_layout.addWidget(plugins_btn)
        nav_layout.addWidget(learn_btn)
        nav_layout.addStretch()
        
        nav_menu.setLayout(nav_layout)
        
        layout.addWidget(profile_section)
        layout.addWidget(nav_menu)
        
        sidebar.setLayout(layout)
        return sidebar
    
    def show_home(self):
        self.clear_content()
        self.sidebar.show()
        
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
        
        recent_section = QWidget()
        recent_layout = QVBoxLayout()
        recent_layout.setContentsMargins(0, 0, 0, 30)
        
        recent_label = QLabel("New & recent")
        recent_font = QFont()
        recent_font.setPointSize(16)
        recent_label.setFont(recent_font)
        
        scores_grid = QGridLayout()
        scores_grid.setContentsMargins(0, 20, 0, 0)
        scores_grid.setSpacing(20)
        
        new_score = {"title": "New score", "is_new_score": True}
        new_score_card = ScoreCard(new_score, self)
        scores_grid.addWidget(new_score_card, 0, 0)
        
        col = 1
        for i, score in enumerate(self.scores[:5]):
            score_card = ScoreCard(score, self)
            scores_grid.addWidget(score_card, 0, col)
            col += 1
        
        recent_layout.addWidget(recent_label)
        recent_layout.addLayout(scores_grid)
        
        recent_section.setLayout(recent_layout)
        
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
        
        self.content_layout.addWidget(header)
        self.content_layout.addWidget(recent_section)
        self.content_layout.addStretch()
        self.content_layout.addWidget(bottom_section)
    
    def open_score_editor(self, score):
        self.sidebar.hide()
        self.clear_content()
        
        editor = ScoreEditorWindow(score, self)
        editor.closed.connect(self.show_home)
        self.content_layout.addWidget(editor)
        self.current_editors.append(editor)
    
    def clear_content(self):
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
            
            new_score = {
                "id": len(self.scores) + 1,
                "title": title,
                "composer": composer if composer else "Unknown",
                "date": datetime.now().strftime("%Y-%m-%d %H:%M")
            }
            
            self.scores.append(new_score)
            save_scores(self.scores)
            self.open_score_editor(new_score)