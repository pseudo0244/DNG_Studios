import sys
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                            QHBoxLayout, QLabel, QComboBox, QAction, QToolBar, 
                            QPushButton, QScrollArea, QSplitter, QStatusBar, QSizePolicy)
from PyQt5.QtGui import QFont, QIcon
from PyQt5.QtCore import Qt, QSize

from score import ScoreView, NotationItem
from notation_panel import NotationPanel

class MusicNotationEditor(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Music Notation Editor")
        self.setMinimumSize(1280, 800)
        self.setStyleSheet(self.get_dark_theme())

        self.create_menu_bar()
        self.create_toolbar()

        self.central_widget = QWidget()
        self.setCentralWidget(self.central_widget)

        main_layout = QHBoxLayout(self.central_widget)
        main_layout.setContentsMargins(0, 0, 0, 0)
        main_layout.setSpacing(0)

        self.splitter = QSplitter(Qt.Horizontal)

        # Left panel with palettes
        self.left_panel = QWidget()
        left_layout = QVBoxLayout(self.left_panel)
        left_layout.setContentsMargins(0, 0, 0, 0)
        left_layout.setSpacing(0)
        
        # Palettes label
        palettes_label = QLabel("Palettes")
        palettes_label.setStyleSheet("background-color: #2d2d2d; color: #ddd; padding: 5px; font-weight: bold;")
        palettes_label.setAlignment(Qt.AlignLeft)
        left_layout.addWidget(palettes_label)
        
        # Add palette tabs
        self.create_palette_tabs()
        left_layout.addWidget(self.palette_tabs_widget)
        
        self.left_panel.setMinimumWidth(250)
        self.left_panel.setMaximumWidth(300)

        # Center panel with score view
        self.center_panel = QWidget()
        self.center_panel.setStyleSheet("background-color: #1e1e1e;")
        center_layout = QVBoxLayout(self.center_panel)
        center_layout.setContentsMargins(10, 10, 10, 10)

        # Add score tab header
        score_tab_layout = QHBoxLayout()
        score_tab = QLabel("*Untitled score")
        score_tab.setStyleSheet("background-color: #2d2d2d; color: #ddd; padding: 5px 10px; border-top-left-radius: 5px; border-top-right-radius: 5px;")
        close_btn = QPushButton("×")
        close_btn.setFixedSize(20, 20)
        close_btn.setStyleSheet("background-color: #2d2d2d; color: #ddd; border: none;")
        
        score_tab_layout.addWidget(score_tab)
        score_tab_layout.addWidget(close_btn)
        score_tab_layout.addStretch()
        
        center_layout.addLayout(score_tab_layout)
        
        self.score_view = ScoreView()
        center_layout.addWidget(self.score_view)

        self.splitter.addWidget(self.left_panel)
        self.splitter.addWidget(self.center_panel)

        self.splitter.setSizes([250, 1030])
        main_layout.addWidget(self.splitter)

        self.status_bar = QStatusBar()
        self.setStatusBar(self.status_bar)
        self.status_bar.showMessage("Ready")
        
        # Add workspace indicator to status bar
        workspace_label = QLabel("Workspace: Default")
        self.status_bar.addPermanentWidget(workspace_label)

        self.load_notation_elements()

    def create_menu_bar(self):
        menu_bar = self.menuBar()
        menu_bar.setStyleSheet("background-color: #2d2d2d; color: #ddd;")

        # Create menu items to match the screenshot
        home_menu = menu_bar.addMenu("Home")
        score_menu = menu_bar.addMenu("Score")
        publish_menu = menu_bar.addMenu("Publish")
        
        # Style the Score menu to look selected
        score_menu_action = score_menu.menuAction()
        score_menu_action.setText("<span style='background-color: #3d3d3d; padding: 5px;'>Score</span>")

    def create_toolbar(self):
        # Main toolbar
        self.toolbar = QToolBar("Main Toolbar")
        self.toolbar.setIconSize(QSize(24, 24))
        self.toolbar.setMovable(False)
        self.addToolBar(self.toolbar)

        # Add music notation symbols to toolbar
        notation_symbols = [
            "𝄞", "♩", "♪", "𝅗𝅥", "𝅝", "𝄽", "♯", "♭", "𝄫"
        ]
        
        for symbol in notation_symbols:
            action = QAction(symbol, self)
            action.setFont(QFont("Arial", 16))
            self.toolbar.addAction(action)

        # Add right-aligned tools
        spacer = QWidget()
        spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        self.toolbar.addWidget(spacer)
        

    def create_palette_tabs(self):
        self.palette_tabs_widget = QWidget()
        palette_layout = QVBoxLayout(self.palette_tabs_widget)
        palette_layout.setContentsMargins(0, 0, 0, 0)
        
        
        # Add search box
        search_layout = QHBoxLayout()
        search_box = QComboBox()
        search_box.setEditable(True)
        search_box.setPlaceholderText("Add palettes")
        search_box.setStyleSheet("background-color: #333; color: #ddd; padding: 5px; border: 1px solid #444;")
        
        search_btn = QPushButton("🔍")
        search_btn.setFixedSize(30, 30)
        search_btn.setStyleSheet("background-color: #333; color: #ddd; border: 1px solid #444;")
        
        search_layout.addWidget(search_box)
        search_layout.addWidget(search_btn)
        
        palette_layout.addLayout(search_layout)
        
        # Add collapsible sections
        sections = [
            "Clefs", "Key signatures", "Time signatures", "Tempo", 
            "Pitch", "Accidentals", "Dynamics", "Articulations", 
            "Text", "Keyboard", "Repeats & jumps", "Barlines", "Layout"
        ]
        
        for section in sections:
            section_btn = QPushButton("▶ " + section)
            section_btn.setStyleSheet("text-align: left; background-color: #1e1e1e; color: #ddd; border: none; padding: 5px;")
            palette_layout.addWidget(section_btn)
        
        palette_layout.addStretch()

    def load_notation_elements(self):
        # This method is kept for compatibility but we're not using it in the updated UI
        pass

    def get_dark_theme(self):
        return """
            QMainWindow { background-color: #1e1e1e; }
            QToolBar, QMenuBar, QStatusBar { background-color: #2d2d2d; color: #ddd; }
            QToolButton, QLabel, QComboBox { color: #ddd; background-color: #2d2d2d; }
            QComboBox { padding: 5px; border-radius: 4px; border: 1px solid #555; min-width: 100px; }
            QComboBox QAbstractItemView { background-color: #2d2d2d; selection-background-color: #444; }
            QLabel#titleLabel { font-size: 18px; font-weight: bold; color: #fff; }
            QPushButton { background-color: #2d2d2d; border: 1px solid #444; color: #ddd; padding: 6px 12px; }
            QPushButton:hover { background-color: #3a3a3a; }
            QSplitter::handle { background-color: #444; }
            QScrollBar:vertical {
                background: #1e1e1e;
                width: 10px;
                margin: 0px;
            }
            QScrollBar::handle:vertical {
                background: #555;
                min-height: 20px;
                border-radius: 5px;
            }
            QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
                height: 0px;
            }
            QScrollBar:horizontal {
                background: #1e1e1e;
                height: 10px;
                margin: 0px;
            }
            QScrollBar::handle:horizontal {
                background: #555;
                min-width: 20px;
                border-radius: 5px;
            }
            QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
                width: 0px;
            }
        """

# Run the application
if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MusicNotationEditor()
    window.show()
    sys.exit(app.exec_())