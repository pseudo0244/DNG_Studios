from PyQt5.QtWidgets import QWidget
from PyQt5.QtGui import QPainter, QPen, QBrush
from PyQt5.QtCore import Qt
from models.note import Note

class StaffWidget(QWidget):
    def __init__(self):
        super().__init__()
        self.notes = []

    def mousePressEvent(self, event):
        self.notes.append(Note(event.x(), event.y()))
        self.update()

    def paintEvent(self, event):
        painter = QPainter(self)
        pen = QPen(Qt.black, 2)
        painter.setPen(pen)

        # Draw staff lines
        spacing = 20
        top = 100
        for i in range(5):
            y = top + i * spacing
            painter.drawLine(50, y, self.width() - 50, y)

        # Draw notes
        for note in self.notes:
            painter.setBrush(QBrush(Qt.black))
            painter.drawEllipse(note.x - 7, note.y - 5, 14, 10)
