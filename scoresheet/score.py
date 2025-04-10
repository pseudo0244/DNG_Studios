from PyQt5.QtWidgets import (
    QWidget, QVBoxLayout, QHBoxLayout, QLabel, QScrollArea,
    QFrame, QPushButton, QApplication, QLineEdit
)
from PyQt5.QtGui import QPainter, QPen, QColor, QFont, QDrag, QTransform
from PyQt5.QtCore import Qt, QMimeData, QPoint, QRectF
import sys

class NotationItem(QWidget):
    def __init__(self, name, symbol):
        super().__init__()
        self.name = name
        self.symbol = symbol
        self.init_ui()
    
    def init_ui(self):
        layout = QVBoxLayout(self)
        layout.setContentsMargins(5, 5, 5, 5)
        layout.setSpacing(2)

        self.symbol_label = QLabel(self.symbol)
        self.symbol_label.setAlignment(Qt.AlignCenter)
        self.symbol_label.setStyleSheet("font-size: 24px; color: #000000;")

        self.name_label = QLabel(self.name)
        self.name_label.setAlignment(Qt.AlignCenter)
        self.name_label.setWordWrap(True)
        self.name_label.setStyleSheet("font-size: 10px; color: #333333;")

        layout.addWidget(self.symbol_label)
        layout.addWidget(self.name_label)

        self.setFixedSize(70, 70)
        self.setStyleSheet("""
            QWidget {
                background-color: #f0f0f0;
                border: 1px solid #ccc;
                border-radius: 5px;
            }
            QWidget:hover {
                background-color: #e0e0e0;
                border: 1px solid #999;
            }
        """)

    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.drag_start_position = event.pos()

    def mouseMoveEvent(self, event):
        if not (event.buttons() & Qt.LeftButton):
            return

        if (event.pos() - self.drag_start_position).manhattanLength() < QApplication.startDragDistance():
            return

        drag = QDrag(self)
        mime_data = QMimeData()
        mime_data.setText(self.name)
        drag.setMimeData(mime_data)

        drag.setHotSpot(event.pos())
        drag.exec_(Qt.CopyAction | Qt.MoveAction)


class StaffWidget(QWidget):
    def __init__(self, measure_count=12, start_measure=1):
        super().__init__()
        self.measure_count = measure_count
        self.start_measure = start_measure
        self.setMinimumHeight(120)
        self.setMaximumHeight(120)

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)

        pen = QPen(QColor("#000000"))  # Black lines for white background
        pen.setWidth(1)
        painter.setPen(pen)

        width = self.width()
        height = self.height()
        line_spacing = height / 6
        
        # Draw staff lines
        for i in range(5):
            y_pos = int(line_spacing + i * line_spacing)
            painter.drawLine(0, y_pos, int(width), y_pos)

        # Draw clef and time signature for first staff only
        if self.start_measure == 1:
            painter.setFont(QFont("Arial", 30))
            painter.setPen(QColor("#000000"))  # Black clef
            painter.drawText(10, 50, "𝄞")  # Treble clef
            
            painter.drawText(10, 105, "𝄢")  # Bass clef
            
            painter.setFont(QFont("Arial", 20))
            painter.drawText(50, 40, "4")  # Time signature
            painter.drawText(50, 65, "4")
        
        # Draw measure number if it's the first measure of the system
        if self.start_measure > 1:
            painter.setFont(QFont("Arial", 10))
            painter.drawText(5, 15, str(self.start_measure))

        # Draw bar lines
        measure_width = (width - 60) / self.measure_count
        for i in range(self.measure_count + 1):
            x_pos = int(60 + i * measure_width)
            if i == 0 and self.start_measure == 1:
                # First bar line is thicker for the first system
                pen.setWidth(2)
                painter.setPen(pen)
            else:
                pen.setWidth(1)
                painter.setPen(pen)
            
            # Draw bar line for both staves
            painter.drawLine(x_pos, int(line_spacing), x_pos, int(line_spacing * 5))
            painter.drawLine(x_pos, int(line_spacing * 8), x_pos, int(line_spacing * 12))
            
        # Draw bass staff lines
        for i in range(5):
            y_pos = int(line_spacing * 8 + i * line_spacing)
            painter.drawLine(0, y_pos, int(width), y_pos)


class ScoreView(QWidget):
    def __init__(self):
        super().__init__()
        self.zoom_factor = 1.0
        self.init_ui()

    def init_ui(self):
        main_layout = QVBoxLayout(self)
        main_layout.setContentsMargins(0, 0, 0, 0)

        # Create zoom controls
        zoom_layout = QHBoxLayout()
        zoom_out_btn = QPushButton("-")
        zoom_out_btn.setFixedSize(30, 30)
        zoom_out_btn.clicked.connect(self.zoom_out)
        
        self.zoom_label = QLabel("100%")
        self.zoom_label.setAlignment(Qt.AlignCenter)
        self.zoom_label.setFixedWidth(60)
        
        zoom_in_btn = QPushButton("+")
        zoom_in_btn.setFixedSize(30, 30)
        zoom_in_btn.clicked.connect(self.zoom_in)
        
        zoom_layout.addStretch()
        zoom_layout.addWidget(zoom_out_btn)
        zoom_layout.addWidget(self.zoom_label)
        zoom_layout.addWidget(zoom_in_btn)
        zoom_layout.addStretch()

        # Create scroll area for score
        self.scroll_area = QScrollArea()
        self.scroll_area.setWidgetResizable(True)
        self.scroll_area.setFrameShape(QFrame.NoFrame)
        self.scroll_area.setHorizontalScrollBarPolicy(Qt.ScrollBarAsNeeded)
        self.scroll_area.setVerticalScrollBarPolicy(Qt.ScrollBarAsNeeded)

        self.score_content = QWidget()
        self.score_content.setStyleSheet("background-color: #ffffff;")  # White background

        self.score_layout = QVBoxLayout(self.score_content)
        self.score_layout.setContentsMargins(40, 40, 40, 40)
        self.score_layout.setSpacing(20)

        # Create editable title and composer
        title_layout = QVBoxLayout()
        title_layout.setSpacing(5)
        
        self.title_edit = QLineEdit("Untitled score")
        self.title_edit.setAlignment(Qt.AlignCenter)
        self.title_edit.setStyleSheet("font-size: 24px; font-weight: bold; border: none; color: #000000;")
        
        self.subtitle_edit = QLineEdit("Subtitle")
        self.subtitle_edit.setAlignment(Qt.AlignCenter)
        self.subtitle_edit.setStyleSheet("font-size: 16px; border: none; color: #000000;")
        
        self.composer_edit = QLineEdit("Composer / arranger")
        self.composer_edit.setAlignment(Qt.AlignRight)
        self.composer_edit.setStyleSheet("font-size: 14px; border: none; color: #000000;")
        
        title_layout.addWidget(self.title_edit)
        title_layout.addWidget(self.subtitle_edit)
        title_layout.addWidget(self.composer_edit)
        
        self.score_layout.addLayout(title_layout)

        # Add staff systems
        self.add_staff_system(1)
        self.add_staff_system(13)

        self.score_layout.addStretch()
        self.scroll_area.setWidget(self.score_content)
        
        main_layout.addLayout(zoom_layout)
        main_layout.addWidget(self.scroll_area)

    def add_staff_system(self, start_measure):
        staff = StaffWidget(measure_count=12, start_measure=start_measure)
        self.score_layout.addWidget(staff)

    def zoom_in(self):
        if self.zoom_factor < 2.0:
            self.zoom_factor += 0.1
            self.apply_zoom()

    def zoom_out(self):
        if self.zoom_factor > 0.5:
            self.zoom_factor -= 0.1
            self.apply_zoom()

    def apply_zoom(self):
        # Update zoom label
        zoom_percentage = int(self.zoom_factor * 100)
        self.zoom_label.setText(f"{zoom_percentage}%")
        
        # Apply transform to score content
        self.score_content.setFixedWidth(int(self.width() * self.zoom_factor))
        
        # Update font sizes based on zoom
        base_title_size = 24
        base_subtitle_size = 16
        base_composer_size = 14
        
        self.title_edit.setStyleSheet(f"font-size: {int(base_title_size * self.zoom_factor)}px; font-weight: bold; border: none; color: #000000;")
        self.subtitle_edit.setStyleSheet(f"font-size: {int(base_subtitle_size * self.zoom_factor)}px; border: none; color: #000000;")
        self.composer_edit.setStyleSheet(f"font-size: {int(base_composer_size * self.zoom_factor)}px; border: none; color: #000000;")

    def dragEnterEvent(self, event):
        if event.mimeData().hasText():
            event.acceptProposedAction()

    def dropEvent(self, event):
        position = event.pos()
        notation_name = event.mimeData().text()
        print(f"Dropped {notation_name} at position {position}")
        event.acceptProposedAction()

    def resizeEvent(self, event):
        super().resizeEvent(event)
        self.apply_zoom()