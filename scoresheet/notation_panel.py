from PyQt5.QtWidgets import (QWidget, QVBoxLayout, QLabel, QScrollArea, 
                             QFrame, QGridLayout)
from PyQt5.QtCore import Qt

class NotationPanel(QWidget):
    def __init__(self, title):
        super().__init__()
        self.title = title
        self.init_ui()
        self.notation_items = []
    
    def init_ui(self):
        # Main layout
        main_layout = QVBoxLayout(self)
        main_layout.setContentsMargins(0, 0, 0, 0)
        main_layout.setSpacing(0)
        
        # Panel title
        title_label = QLabel(self.title)
        title_label.setAlignment(Qt.AlignCenter)
        title_label.setStyleSheet("""
    background-color: #1e1e1e;  /* Dark gray instead of blue */
    color: #dddddd;
    padding: 10px;
    font-size: 16px;
    font-weight: bold;
    border-bottom: 1px solid #444;
""")

        
        # Scroll area for notation items
        scroll_area = QScrollArea()
        scroll_area.setWidgetResizable(True)
        scroll_area.setFrameShape(QFrame.NoFrame)
        scroll_area.setStyleSheet("""
    QScrollArea {
        background-color: #1e1e1e;
        border: none;
    }
    QScrollBar:vertical {
        background: #121212;
        width: 8px;
        margin: 0px 0px 0px 0px;
    }
    QScrollBar::handle:vertical {
        background: #333333;
        border-radius: 4px;
        min-height: 20px;
    }
    QScrollBar::handle:vertical:hover {
        background: #555555;
    }
    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
        height: 0px;
    }
    QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
        background: none;
    }
""")

        
        # Content widget inside scroll area
        self.content_widget = QWidget()
        self.content_widget.setStyleSheet("background-color: #00000;")
        
        # Grid layout for notation items
        self.grid_layout = QGridLayout(self.content_widget)
        self.grid_layout.setContentsMargins(10, 10, 10, 10)
        self.grid_layout.setSpacing(10)
        
        scroll_area.setWidget(self.content_widget)
        
        # Add widgets to main layout
        main_layout.addWidget(title_label)
        main_layout.addWidget(scroll_area)
    
    def add_notation_item(self, item):
        self.notation_items.append(item)
        count = len(self.notation_items) - 1
        row = count // 2
        col = count % 2
        self.grid_layout.addWidget(item, row, col)